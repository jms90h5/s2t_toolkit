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
#include "../include/WenetSTTImpl.hpp"

// Mock implementation of the WeNet API for testing purposes
extern "C" {

void* wenet_init(const char* model_dir) {
    std::cout << "MOCK: wenet_init called with model_dir=" << model_dir << std::endl;
    // Return a non-null pointer to indicate success
    static int dummy = 0;
    return &dummy;
}

void wenet_free(void* decoder) {
    std::cout << "MOCK: wenet_free called" << std::endl;
}

void wenet_reset(void* decoder) {
    std::cout << "MOCK: wenet_reset called" << std::endl;
}

const char* wenet_decode(void* decoder, const char* data, int len, int last) {
    std::cout << "MOCK: wenet_decode called with len=" << len << ", last=" << last << std::endl;
    
    // Return a mock JSON result
    if (last) {
        static const char* final_result = "{"
            "\"type\": \"final_result\","
            "\"nbest\": ["
                "{"
                    "\"sentence\": \"This is a mock final transcription result\","
                    "\"word_pieces\": ["
                        "{\"word\": \"This\", \"start\": 0, \"end\": 400},"
                        "{\"word\": \"is\", \"start\": 450, \"end\": 600},"
                        "{\"word\": \"a\", \"start\": 650, \"end\": 700},"
                        "{\"word\": \"mock\", \"start\": 750, \"end\": 900},"
                        "{\"word\": \"final\", \"start\": 950, \"end\": 1100},"
                        "{\"word\": \"transcription\", \"start\": 1150, \"end\": 1800},"
                        "{\"word\": \"result\", \"start\": 1850, \"end\": 2000}"
                    "]"
                "},"
                "{"
                    "\"sentence\": \"This is a mock final result\""
                "}"
            "]"
        "}";
        return final_result;
    } else {
        static const char* partial_result = "{"
            "\"type\": \"partial_result\","
            "\"nbest\": ["
                "{"
                    "\"sentence\": \"This is a mock partial result\""
                "}"
            "]"
        "}";
        return partial_result;
    }
}

const char* wenet_get_result(void* decoder) {
    std::cout << "MOCK: wenet_get_result called" << std::endl;
    static const char* result = "{"
        "\"type\": \"final_result\","
        "\"nbest\": ["
            "{"
                "\"sentence\": \"This is a mock result from get_result\""
            "}"
        "]"
    "}";
    return result;
}

void wenet_set_log_level(int level) {
    std::cout << "MOCK: wenet_set_log_level called with level=" << level << std::endl;
}

void wenet_set_nbest(void* decoder, int n) {
    std::cout << "MOCK: wenet_set_nbest called with n=" << n << std::endl;
}

void wenet_set_timestamp(void* decoder, int flag) {
    std::cout << "MOCK: wenet_set_timestamp called with flag=" << flag << std::endl;
}

void wenet_set_max_active(void* decoder, int max_active) {
    std::cout << "MOCK: wenet_set_max_active called with max_active=" << max_active << std::endl;
}

void wenet_set_output_chunk_size(void* decoder, int size) {
    std::cout << "MOCK: wenet_set_output_chunk_size called with size=" << size << std::endl;
}

void wenet_set_continuous_decoding(void* decoder, int flag) {
    std::cout << "MOCK: wenet_set_continuous_decoding called with flag=" << flag << std::endl;
}

void wenet_set_chunk_size(void* decoder, int chunk_size) {
    std::cout << "MOCK: wenet_set_chunk_size called with chunk_size=" << chunk_size << std::endl;
}

} // extern "C"

// Implementation of the WenetSTTImpl class
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
    // Initialize WeNet decoder
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
    
    // Start the processing thread
    running_ = true;
    processingThread_ = std::thread(&WenetSTTImpl::processingThread, this);
    
    return true;
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
                // Process audio buffer
                const char* audioData = reinterpret_cast<const char*>(buffer.data());
                int audioSize = buffer.size() * sizeof(int16_t);
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
        if (flushPending && buffer.size() > 0) {
            // Process any remaining audio with flush flag
            const char* audioData = reinterpret_cast<const char*>(buffer.data());
            int audioSize = buffer.size() * sizeof(int16_t);
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
    // Mock implementation - in reality this would use the model's confidence scores
    if (text.empty()) {
        return 0.0;
    }
    
    // Generate a random confidence between 0.75 and 0.98
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.75, 0.98);
    
    return dis(gen);
}

// Parse WeNet JSON result
void WenetSTTImpl::parseJsonResult(const std::string& jsonResult, bool isFinal) {
    // Quick mock implementation since we're not parsing real JSON
    std::string text;
    double confidence;
    
    if (jsonResult.find("final") != std::string::npos) {
        text = isFinal ? "Final transcription from flush" : "Transcription Result";
        confidence = isFinal ? 0.95 : 0.9;
    } else {
        text = "Partial transcription";
        confidence = 0.85;
    }
    
    if (callback_) {
        TranscriptionResult result(text, isFinal, confidence, lastTimestamp_);
        callback_->onTranscriptionResult(result);
    }
}

} // namespace wenet_streams
