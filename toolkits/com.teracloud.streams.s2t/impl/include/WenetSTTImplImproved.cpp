#include "WenetSTTImplImproved.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <sstream>

namespace wenet_streams {

WenetSTTImplImproved::WenetSTTImplImproved(const WeNetConfig& config, TranscriptionCallback* callback)
    : config_(config), 
      callback_(callback),
      audioQueue_(config.bufferCapacity),
      running_(false),
      shouldFlush_(false),
      speechDetected_(false),
      lastTimestamp_(0) {
    if (!callback) {
        throw std::invalid_argument("TranscriptionCallback cannot be null");
    }
}

WenetSTTImplImproved::~WenetSTTImplImproved() {
    try {
        // Stop the processing thread gracefully
        running_ = false;
        shouldFlush_ = true;
        condition_.notify_all();
        
        if (processingThread_.joinable()) {
            processingThread_.join();
        }
    } catch (const std::exception& e) {
        // Log error but don't throw from destructor
        std::cerr << "Error in destructor: " << e.what() << std::endl;
    }
}

bool WenetSTTImplImproved::initialize() {
    try {
        // Initialize the WeNet decoder with RAII wrapper
        wenetDecoder_ = std::make_unique<WenetDecoderWrapper>(config_.modelPath);
        
        // Configure the decoder
        wenetDecoder_->setChunkSize(static_cast<int>(config_.maxChunkDuration * config_.sampleRate));
        wenetDecoder_->setContinuousDecoding(true);
        wenetDecoder_->setTimestamp(config_.timestampEnabled);
        wenetDecoder_->setNBest(config_.nBest);
        
        // Start the processing thread
        running_ = true;
        processingThread_ = std::thread(&WenetSTTImplImproved::processingThread, this);
        
        return true;
    } catch (const std::exception& e) {
        handleError("initialize", e);
        return false;
    }
}

void WenetSTTImplImproved::processAudioChunk(const AudioChunk& chunk) {
    if (!running_) {
        return;
    }
    
    auto start = std::chrono::steady_clock::now();
    
    // Try to queue the audio chunk
    if (!audioQueue_.push(chunk)) {
        metrics_.errorCount++;
        callback_->onError("Audio queue full - dropping chunk");
    } else {
        metrics_.queueDepth = audioQueue_.size();
    }
    
    // Notify the processing thread
    condition_.notify_one();
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    metrics_.totalProcessingTimeMs += duration;
}

void WenetSTTImplImproved::processAudioChunk(AudioChunk&& chunk) {
    if (!running_) {
        return;
    }
    
    auto start = std::chrono::steady_clock::now();
    
    // Try to queue the audio chunk (move version)
    if (!audioQueue_.push(std::move(chunk))) {
        metrics_.errorCount++;
        callback_->onError("Audio queue full - dropping chunk");
    } else {
        metrics_.queueDepth = audioQueue_.size();
    }
    
    // Notify the processing thread
    condition_.notify_one();
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    metrics_.totalProcessingTimeMs += duration;
}

void WenetSTTImplImproved::flush() {
    shouldFlush_ = true;
    condition_.notify_all();
}

bool WenetSTTImplImproved::isRunning() const {
    return running_;
}

PerformanceMetrics WenetSTTImplImproved::getMetrics() const {
    return metrics_;
}

void WenetSTTImplImproved::processingThread() {
    std::vector<AudioChunk> batch;
    batch.reserve(config_.chunkSize);
    
    while (running_) {
        try {
            // Wait for audio chunks or flush signal
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this]() {
                    return !audioQueue_.empty() || shouldFlush_ || !running_;
                });
            }
            
            if (!running_) {
                break;
            }
            
            // Collect a batch of audio chunks
            batch.clear();
            while (!audioQueue_.empty() && batch.size() < config_.chunkSize) {
                auto chunk = audioQueue_.pop();
                if (chunk.has_value()) {
                    batch.push_back(std::move(chunk.value()));
                }
            }
            
            // Process the batch
            if (!batch.empty()) {
                processBatch(batch);
            }
            
            // Handle flush request
            if (shouldFlush_ && !currentBuffer_.empty()) {
                auto start = std::chrono::steady_clock::now();
                
                // Convert to char* for wenet_decode
                const char* audioData = reinterpret_cast<const char*>(currentBuffer_.data());
                int dataSize = currentBuffer_.size() * sizeof(int16_t);
                
                // Decode with WeNet as final result
                const char* result = wenet_decode(wenetDecoder_->get(), audioData, dataSize, 1);
                
                if (result != nullptr) {
                    parseJsonResult(result, true);
                }
                
                // Reset state
                currentBuffer_.clear();
                wenetDecoder_->reset();
                shouldFlush_ = false;
                speechDetected_ = false;
                
                auto end = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                metrics_.totalProcessingTimeMs += duration;
            }
            
        } catch (const std::exception& e) {
            handleError("processingThread", e);
        }
    }
}

void WenetSTTImplImproved::processBatch(const std::vector<AudioChunk>& batch) {
    auto start = std::chrono::steady_clock::now();
    
    for (const auto& chunk : batch) {
        // Update metrics
        metrics_.totalSamplesProcessed += chunk.samples.size();
        lastTimestamp_ = chunk.timestamp;
        
        // Apply VAD if enabled
        bool isSpeech = true;
        if (config_.vadEnabled) {
            isSpeech = detectSpeech(chunk.samples);
        }
        
        if (isSpeech) {
            // If this is new speech after silence, reset the decoder state
            if (!speechDetected_) {
                wenetDecoder_->reset();
                speechDetected_ = true;
            }
            
            // Add the samples to the current buffer
            currentBuffer_.insert(currentBuffer_.end(), chunk.samples.begin(), chunk.samples.end());
            
            // Process the audio if we have enough samples
            int samplesNeeded = config_.sampleRate * config_.maxChunkDuration;
            if (currentBuffer_.size() >= samplesNeeded) {
                // Convert to char* for wenet_decode
                const char* audioData = reinterpret_cast<const char*>(currentBuffer_.data());
                int dataSize = currentBuffer_.size() * sizeof(int16_t);
                
                // Decode with WeNet
                const char* result = wenet_decode(wenetDecoder_->get(), audioData, dataSize, 0);
                
                // Parse and handle the result
                if (result != nullptr) {
                    parseJsonResult(result, false);
                }
                
                // Keep last portion of buffer for context
                int samplesToKeep = config_.sampleRate * 0.5;
                if (currentBuffer_.size() > samplesToKeep) {
                    currentBuffer_.erase(currentBuffer_.begin(), 
                                      currentBuffer_.end() - samplesToKeep);
                }
            }
        } else {
            // If we were in speech and now detect silence, finalize any results
            if (speechDetected_ && !currentBuffer_.empty()) {
                // Convert to char* for wenet_decode
                const char* audioData = reinterpret_cast<const char*>(currentBuffer_.data());
                int dataSize = currentBuffer_.size() * sizeof(int16_t);
                
                // Decode with WeNet as final result
                const char* result = wenet_decode(wenetDecoder_->get(), audioData, dataSize, 1);
                
                if (result != nullptr) {
                    parseJsonResult(result, true);
                }
                
                // Reset state for next utterance
                currentBuffer_.clear();
                wenetDecoder_->reset();
            }
            
            speechDetected_ = false;
        }
    }
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    metrics_.totalProcessingTimeMs += duration;
    
    // Update average latency
    if (metrics_.totalSamplesProcessed > 0) {
        double avgLatency = static_cast<double>(metrics_.totalProcessingTimeMs) / 
                           (metrics_.totalSamplesProcessed / config_.sampleRate / 1000.0);
        metrics_.averageLatencyMs = avgLatency;
    }
}

void WenetSTTImplImproved::parseJsonResult(const std::string& jsonResult, bool isFinal) {
    try {
        rapidjson::Document doc;
        doc.Parse(jsonResult.c_str());
        
        if (doc.HasParseError()) {
            throw std::runtime_error("JSON parse error at offset " + 
                                   std::to_string(doc.GetErrorOffset()));
        }
        
        TranscriptionResult result;
        result.isFinal = isFinal;
        result.timestamp = lastTimestamp_;
        
        // Extract main text
        if (doc.HasMember("text") && doc["text"].IsString()) {
            result.text = doc["text"].GetString();
        }
        
        // Extract confidence
        if (doc.HasMember("confidence") && doc["confidence"].IsNumber()) {
            result.confidence = doc["confidence"].GetDouble();
        } else {
            result.confidence = calculateConfidence(result.text);
        }
        
        // Extract alternatives (N-best)
        if (doc.HasMember("nbest") && doc["nbest"].IsArray()) {
            const auto& nbest = doc["nbest"];
            for (rapidjson::SizeType i = 0; i < nbest.Size(); ++i) {
                if (nbest[i].IsObject()) {
                    std::string altText;
                    double altConf = 0.0;
                    
                    if (nbest[i].HasMember("sentence") && nbest[i]["sentence"].IsString()) {
                        altText = nbest[i]["sentence"].GetString();
                    }
                    if (nbest[i].HasMember("confidence") && nbest[i]["confidence"].IsNumber()) {
                        altConf = nbest[i]["confidence"].GetDouble();
                    }
                    
                    result.alternatives.emplace_back(altText, altConf);
                }
            }
        }
        
        // Send the result
        callback_->onTranscriptionResult(result);
        
    } catch (const std::exception& e) {
        handleError("parseJsonResult", e);
        
        // Fall back to simple result
        TranscriptionResult fallbackResult;
        fallbackResult.text = "Transcription result";
        fallbackResult.isFinal = isFinal;
        fallbackResult.confidence = 0.5;
        fallbackResult.timestamp = lastTimestamp_;
        callback_->onTranscriptionResult(fallbackResult);
    }
}

bool WenetSTTImplImproved::detectSpeech(const std::vector<int16_t>& samples) {
    if (samples.empty()) {
        return false;
    }
    
    // Calculate RMS energy
    double sumSquares = 0.0;
    for (const auto& sample : samples) {
        double normalizedSample = sample / 32768.0;
        sumSquares += normalizedSample * normalizedSample;
    }
    double rms = std::sqrt(sumSquares / samples.size());
    
    // Convert to dB
    double energyDb = 20.0 * std::log10(rms + 1e-10);
    
    // Apply hysteresis based on current speech state
    if (speechDetected_) {
        return energyDb > config_.vadSilenceThreshold;
    } else {
        return energyDb > config_.vadSpeechThreshold;
    }
}

double WenetSTTImplImproved::calculateConfidence(const std::string& text) {
    if (text.empty()) {
        return 0.0;
    }
    
    // More sophisticated confidence calculation
    double lengthFactor = std::min(1.0, text.length() / 100.0);
    double wordCountFactor = std::min(1.0, std::count(text.begin(), text.end(), ' ') / 20.0);
    
    return 0.5 + (lengthFactor * 0.3) + (wordCountFactor * 0.2);
}

void WenetSTTImplImproved::handleError(const std::string& context, const std::exception& e) {
    metrics_.errorCount++;
    
    std::stringstream ss;
    ss << "Error in " << context << ": " << e.what();
    std::string errorMsg = ss.str();
    
    // Log to stderr
    std::cerr << errorMsg << std::endl;
    
    // Notify callback
    try {
        callback_->onError(errorMsg);
    } catch (...) {
        // Ignore errors in error callback
    }
}

} // namespace wenet_streams