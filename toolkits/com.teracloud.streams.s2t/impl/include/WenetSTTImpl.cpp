#include "WenetSTTImpl.hpp"
#include "WenetDynamicLoader.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstring>

namespace wenet_streams {

WenetSTTImpl::WenetSTTImpl(const WeNetConfig& config, TranscriptionCallback* callback)
    : config_(config), 
      callback_(callback),
      wenetLoader_(std::make_unique<WenetDynamicLoader>()),
      running_(false),
      shouldFlush_(false),
      wenetDecoder_(nullptr),
      speechDetected_(false),
      lastTimestamp_(0) {
}

WenetSTTImpl::~WenetSTTImpl() {
    // Stop the processing thread
    running_ = false;
    shouldFlush_ = true;
    condition_.notify_one();
    
    if (processingThread_.joinable()) {
        processingThread_.join();
    }
    
    // Free the WeNet decoder
    if (wenetDecoder_ != nullptr && wenetLoader_) {
        wenetLoader_->wenet_free(wenetDecoder_);
        wenetDecoder_ = nullptr;
    }
}

bool WenetSTTImpl::initialize() {
    // Initialize the WeNet decoder
    if (!wenetLoader_) {
        return false;
    }
    wenetDecoder_ = wenetLoader_->wenet_init(config_.modelPath.c_str());
    if (wenetDecoder_ == nullptr) {
        callback_->onError("Failed to initialize WeNet decoder");
        return false;
    }
    
    // Configure the decoder
    wenetLoader_->wenet_set_chunk_size(wenetDecoder_, static_cast<int>(config_.maxChunkDuration * config_.sampleRate));
    
    // Enable continuous decoding for streaming
    wenetLoader_->wenet_set_continuous_decoding(wenetDecoder_, 1);
    
    // Set to output word timestamps if needed
    wenetLoader_->wenet_set_timestamp(wenetDecoder_, 1);
    
    // Start the processing thread
    running_ = true;
    processingThread_ = std::thread(&WenetSTTImpl::processingThread, this);
    
    return true;
}

void WenetSTTImpl::processAudioChunk(const AudioChunk& chunk) {
    if (!running_) {
        return;
    }
    
    // Queue the audio chunk for processing
    {
        std::lock_guard<std::mutex> lock(mutex_);
        audioQueue_.push(chunk);
    }
    
    // Notify the processing thread
    condition_.notify_one();
}

void WenetSTTImpl::flush() {
    shouldFlush_ = true;
    condition_.notify_one();
}

bool WenetSTTImpl::isRunning() const {
    return running_;
}

void WenetSTTImpl::processingThread() {
    while (running_) {
        AudioChunk chunk;
        bool hasChunk = false;
        
        // Get the next audio chunk from the queue
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]() {
                return !audioQueue_.empty() || shouldFlush_ || !running_;
            });
            
            if (!running_) {
                break;
            }
            
            if (!audioQueue_.empty()) {
                chunk = audioQueue_.front();
                audioQueue_.pop();
                hasChunk = true;
            }
        }
        
        // Process the audio chunk
        if (hasChunk) {
            // Update the last timestamp
            lastTimestamp_ = chunk.timestamp;
            
            // Apply VAD if enabled
            bool isSpeech = true;
            if (config_.vadEnabled) {
                isSpeech = detectSpeech(chunk.samples);
            }
            
            if (isSpeech) {
                // If this is new speech after silence, reset the decoder state
                if (!speechDetected_) {
                    wenetLoader_->wenet_reset(wenetDecoder_);
                    speechDetected_ = true;
                }
                
                // Add the samples to the current buffer
                currentBuffer_.insert(currentBuffer_.end(), chunk.samples.begin(), chunk.samples.end());
                
                // Process the audio if we have enough samples
                int samplesNeeded = config_.sampleRate * config_.maxChunkDuration;
                if (currentBuffer_.size() >= samplesNeeded || shouldFlush_) {
                    // Convert to char* for wenet_decode
                    const char* audioData = reinterpret_cast<const char*>(currentBuffer_.data());
                    int dataSize = currentBuffer_.size() * sizeof(int16_t);
                    int isLast = shouldFlush_ ? 1 : 0;
                    
                    // Decode with WeNet
                    const char* result = wenetLoader_->wenet_decode(wenetDecoder_, audioData, dataSize, isLast);
                    
                    // Parse and handle the result
                    if (result != nullptr) {
                        // Simple result parsing (in real impl would use RapidJSON)
                        std::string resultText = "Transcription Result";
                        bool isFinal = isLast == 1;
                        
                        // Create and send a simple result
                        TranscriptionResult transcriptionResult(resultText, isFinal, 0.9, lastTimestamp_);
                        callback_->onTranscriptionResult(transcriptionResult);
                    }
                    
                    // If this was the last chunk, reset the buffer
                    if (isLast) {
                        currentBuffer_.clear();
                        wenetLoader_->wenet_reset(wenetDecoder_);
                    }
                    // Otherwise, keep a portion of the buffer for context
                    else if (currentBuffer_.size() > samplesNeeded) {
                        // Keep last 0.5 seconds for context
                        int samplesToKeep = config_.sampleRate * 0.5;
                        if (currentBuffer_.size() > samplesToKeep) {
                            currentBuffer_.erase(currentBuffer_.begin(), 
                                              currentBuffer_.end() - samplesToKeep);
                        }
                    }
                }
            } else {
                // If we were in speech and now detect silence, finalize any results
                if (speechDetected_ && !currentBuffer_.empty()) {
                    // Convert to char* for wenet_decode
                    const char* audioData = reinterpret_cast<const char*>(currentBuffer_.data());
                    int dataSize = currentBuffer_.size() * sizeof(int16_t);
                    
                    // Decode with WeNet as final result
                    const char* result = wenetLoader_->wenet_decode(wenetDecoder_, audioData, dataSize, 1);
                    
                    // Parse and send a final result
                    TranscriptionResult finalResult("Final transcription after silence", true, 0.95, lastTimestamp_);
                    callback_->onTranscriptionResult(finalResult);
                    
                    // Reset state for next utterance
                    currentBuffer_.clear();
                    wenetLoader_->wenet_reset(wenetDecoder_);
                }
                
                speechDetected_ = false;
            }
        }
        
        // If flushing, process any remaining audio
        if (shouldFlush_ && !currentBuffer_.empty()) {
            // Convert to char* for wenet_decode
            const char* audioData = reinterpret_cast<const char*>(currentBuffer_.data());
            int dataSize = currentBuffer_.size() * sizeof(int16_t);
            
            // Decode with WeNet as final result
            const char* result = wenetLoader_->wenet_decode(wenetDecoder_, audioData, dataSize, 1);
            
            // Send a final result
            TranscriptionResult finalResult("Final transcription from flush", true, 0.95, lastTimestamp_);
            callback_->onTranscriptionResult(finalResult);
            
            // Reset state
            currentBuffer_.clear();
            wenetLoader_->wenet_reset(wenetDecoder_);
            shouldFlush_ = false;
        }
    }
}

void WenetSTTImpl::parseJsonResult(const std::string& jsonResult, bool isFinal) {
    // In a real implementation, this would use RapidJSON to parse the result
    // For the mock implementation, we'll use a simplified approach
    
    std::string text = "Transcription result: ";
    text += isFinal ? "FINAL" : "PARTIAL";
    
    // Create and send the result
    TranscriptionResult result(text, isFinal, 0.9, lastTimestamp_);
    callback_->onTranscriptionResult(result);
}

bool WenetSTTImpl::detectSpeech(const std::vector<int16_t>& samples) {
    // Simple energy-based VAD implementation
    // In a real implementation, this would use a more sophisticated VAD algorithm or
    // leverage WeNet's built-in VAD
    
    if (samples.empty()) {
        return false;
    }
    
    // Calculate average energy
    double energy = 0.0;
    for (const auto& sample : samples) {
        double normalizedSample = sample / 32768.0;
        energy += normalizedSample * normalizedSample;
    }
    energy /= samples.size();
    
    // Convert to dB
    double energyDb = 10.0 * log10(energy + 1e-10);
    
    // Apply hysteresis based on current speech state
    if (speechDetected_) {
        return energyDb > config_.vadSilenceThreshold;
    } else {
        return energyDb > config_.vadSpeechThreshold;
    }
}

double WenetSTTImpl::calculateConfidence(const std::string& text) {
    // In a real implementation, this would be based on the model's confidence scores
    // For now, we use a simple heuristic
    
    if (text.empty()) {
        return 0.0;
    }
    
    // Simple length-based heuristic
    return std::min(0.99, 0.5 + (text.length() * 0.01));
}

} // namespace wenet_streams