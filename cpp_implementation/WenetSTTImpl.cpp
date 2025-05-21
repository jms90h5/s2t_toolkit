#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <cmath>
#include <memory>
#include <random>
#include <fstream>
#include <sstream>

// For direct WeNet C++ API
extern "C" {
#include "wenet_api.h"
}

#include "shared_include/WenetSTTImpl.hpp"

// Implementation that directly uses the WeNet C++ API
namespace wenet_streams {

// Constructor
WenetSTTImpl::WenetSTTImpl(const WeNetConfig& config, TranscriptionCallback* callback)
    : config_(config), 
      callback_(callback), 
      running_(false), 
      shouldFlush_(false), 
      wenetDecoder_(nullptr),
      speechDetected_(false),
      lastTimestamp_(0) {
}

// Destructor
WenetSTTImpl::~WenetSTTImpl() {
    // Stop the processing thread if running
    if (running_) {
        running_ = false;
        shouldFlush_ = true;
        condition_.notify_one();
        if (processingThread_.joinable()) {
            processingThread_.join();
        }
    }
    
    // Free WeNet decoder if allocated
    if (wenetDecoder_) {
        wenet_free(wenetDecoder_);
        wenetDecoder_ = nullptr;
    }
}

// Initialize the engine
bool WenetSTTImpl::initialize() {
    try {
        std::cout << "Initializing WeNet C++ implementation from: " << config_.modelPath << std::endl;
        
        // Initialize WeNet decoder with the model path
        wenetDecoder_ = wenet_init(config_.modelPath.c_str());
        if (!wenetDecoder_) {
            if (callback_) {
                callback_->onError("Failed to initialize WeNet decoder");
            }
            return false;
        }
        
        // Configure WeNet decoder
        int chunkSizeInSamples = static_cast<int>(config_.maxChunkDuration * config_.sampleRate);
        wenet_set_chunk_size(wenetDecoder_, chunkSizeInSamples);
        wenet_set_continuous_decoding(wenetDecoder_, 1);
        wenet_set_timestamp(wenetDecoder_, 1);
        
        std::cout << "WeNet C++ implementation initialized successfully" << std::endl;
        
        // Start the processing thread
        running_ = true;
        processingThread_ = std::thread(&WenetSTTImpl::processingThread, this);
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        if (callback_) {
            callback_->onError(std::string("Initialization error: ") + e.what());
        }
        return false;
    }
}

// Process an audio chunk
void WenetSTTImpl::processAudioChunk(const AudioChunk& chunk) {
    if (!running_ || !wenetDecoder_) {
        return;
    }
    
    // Add the chunk to the queue
    std::lock_guard<std::mutex> lock(mutex_);
    audioQueue_.push(chunk);
    condition_.notify_one();
}

// Signal that no more audio is coming (flush any pending results)
void WenetSTTImpl::flush() {
    if (!running_ || !wenetDecoder_) {
        return;
    }
    
    shouldFlush_ = true;
    condition_.notify_one();
}

// Check if the engine is running
bool WenetSTTImpl::isRunning() const {
    return running_;
}

// Internal processing thread function
void WenetSTTImpl::processingThread() {
    std::vector<int16_t> buffer;
    bool flushPending = false;
    
    while (running_) {
        AudioChunk chunk;
        bool hasChunk = false;
        
        // Get a chunk from the queue
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (audioQueue_.empty() && !shouldFlush_) {
                // Wait for a new chunk or flush signal
                condition_.wait(lock, [this]() {
                    return !audioQueue_.empty() || shouldFlush_ || !running_;
                });
            }
            
            if (!running_) {
                break;
            }
            
            flushPending = shouldFlush_;
            
            if (!audioQueue_.empty()) {
                chunk = audioQueue_.front();
                audioQueue_.pop();
                hasChunk = true;
            }
        }
        
        // Process the chunk
        if (hasChunk) {
            lastTimestamp_ = chunk.timestamp;
            
            // Apply VAD if enabled
            bool hasSpeech = true;
            if (config_.vadEnabled) {
                hasSpeech = detectSpeech(chunk.samples);
            }
            
            // If speech detected, add samples to buffer
            if (hasSpeech) {
                speechDetected_ = true;
                buffer.insert(buffer.end(), chunk.samples.begin(), chunk.samples.end());
            } else if (speechDetected_ && buffer.empty()) {
                // End of speech detected
                speechDetected_ = false;
            }
            
            // Process buffer if it's large enough or end of speech detected
            if (!buffer.empty() && (buffer.size() >= config_.sampleRate * config_.maxLatency || 
                                  (!hasSpeech && speechDetected_))) {
                // Process audio buffer using WeNet C++ API
                const char* audioData = reinterpret_cast<const char*>(buffer.data());
                int audioSize = buffer.size() * sizeof(int16_t);
                
                // Call the WeNet decode function with last=0 for partial results
                const char* result = wenet_decode(wenetDecoder_, audioData, audioSize, 0);
                
                // Parse result
                if (result && config_.partialResultsEnabled) {
                    parseJsonResult(result, false);
                }
                
                // Keep remainder of buffer if needed
                buffer.clear();
            }
        }
        
        // Handle flush
        if (flushPending && !buffer.empty()) {
            // Process any remaining audio with flush flag
            const char* audioData = reinterpret_cast<const char*>(buffer.data());
            int audioSize = buffer.size() * sizeof(int16_t);
            
            // Call the WeNet decode function with last=1 for final results
            const char* result = wenet_decode(wenetDecoder_, audioData, audioSize, 1);
            
            if (result) {
                parseJsonResult(result, true);
            }
            
            buffer.clear();
            shouldFlush_ = false;
            speechDetected_ = false;
            
            // Reset decoder for next stream
            wenet_reset(wenetDecoder_);
        }
    }
}

// Apply voice activity detection
bool WenetSTTImpl::detectSpeech(const std::vector<int16_t>& samples) {
    // Simple energy-based VAD (in real implementation, use a proper VAD algorithm)
    if (samples.empty()) {
        return false;
    }
    
    // Calculate RMS energy
    double sum = 0.0;
    for (int16_t sample : samples) {
        sum += static_cast<double>(sample) * static_cast<double>(sample);
    }
    double rms = std::sqrt(sum / samples.size());
    
    // Convert to dB
    double db = 20.0 * std::log10(rms / 32768.0);
    
    // Check thresholds
    if (speechDetected_) {
        // More permissive threshold once speech has started
        return db > config_.vadSilenceThreshold;
    } else {
        // Stricter threshold to detect start of speech
        return db > config_.vadSpeechThreshold;
    }
}

// Calculate confidence score for transcription
double WenetSTTImpl::calculateConfidence(const std::string& text) {
    // Simple implementation - in reality, we would parse the confidence from the WeNet result
    if (text.empty()) {
        return 0.0;
    }
    
    // For now, return a fixed high confidence
    return 0.95;
}

// Parse WeNet JSON result
void WenetSTTImpl::parseJsonResult(const std::string& jsonResult, bool isFinal) {
    if (jsonResult.empty()) {
        return;
    }
    
    try {
        // Very simple JSON parsing without using a library
        std::string text;
        double confidence = 0.9;
        
        // Find the sentence field in the JSON
        size_t sentencePos = jsonResult.find("\"sentence\":");
        if (sentencePos != std::string::npos) {
            size_t startQuote = jsonResult.find("\"", sentencePos + 12);
            if (startQuote != std::string::npos) {
                size_t endQuote = jsonResult.find("\"", startQuote + 1);
                if (endQuote != std::string::npos) {
                    text = jsonResult.substr(startQuote + 1, endQuote - startQuote - 1);
                }
            }
        }
        
        if (text.empty()) {
            std::cerr << "Failed to extract sentence from JSON" << std::endl;
            text = isFinal ? "Final result (error parsing)" : "Partial result (error parsing)";
        }
        
        // Create and return the result
        if (callback_) {
            TranscriptionResult result(text, isFinal, confidence, lastTimestamp_);
            callback_->onTranscriptionResult(result);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while parsing JSON: " << e.what() << std::endl;
    }
}

} // namespace wenet_streams
