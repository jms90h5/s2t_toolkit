#ifndef WENET_STT_IMPL_IMPROVED_HPP
#define WENET_STT_IMPL_IMPROVED_HPP

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <optional>

// Check if we're using the mock implementation
#ifdef MOCK_WENET_IMPLEMENTATION
// Mock forward declarations for WeNet API
extern "C" {
void* wenet_init(const char* model_dir);
void wenet_free(void* decoder);
void wenet_reset(void* decoder);
const char* wenet_decode(void* decoder, const char* data, int len, int last);
const char* wenet_get_result(void* decoder);
void wenet_set_nbest(void* decoder, int n);
void wenet_set_timestamp(void* decoder, int flag);
void wenet_set_continuous_decoding(void* decoder, int flag);
void wenet_set_chunk_size(void* decoder, int chunk_size);
}
#else
// Real WeNet includes
#include "wenet/api/wenet_api.h"
#include "wenet/decoder/asr_decoder.h"
#include "wenet/frontend/feature_pipeline.h"
#endif

// Include RapidJSON for proper JSON parsing
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace wenet_streams {

/**
 * Thread-safe circular buffer for efficient audio processing
 */
template<typename T>
class CircularBuffer {
public:
    explicit CircularBuffer(size_t capacity)
        : buffer_(capacity), write_pos_(0), read_pos_(0), size_(0) {}

    bool push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ >= buffer_.size()) {
            return false;  // Buffer full
        }
        buffer_[write_pos_] = item;
        write_pos_ = (write_pos_ + 1) % buffer_.size();
        ++size_;
        return true;
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ == 0) {
            return std::nullopt;
        }
        T item = std::move(buffer_[read_pos_]);
        read_pos_ = (read_pos_ + 1) % buffer_.size();
        --size_;
        return item;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_ == 0;
    }

private:
    mutable std::mutex mutex_;
    std::vector<T> buffer_;
    size_t write_pos_;
    size_t read_pos_;
    size_t size_;
};

/**
 * RAII wrapper for WeNet decoder
 */
class WenetDecoderWrapper {
public:
    explicit WenetDecoderWrapper(const std::string& model_path) 
        : decoder_(nullptr) {
        decoder_ = wenet_init(model_path.c_str());
        if (!decoder_) {
            throw std::runtime_error("Failed to initialize WeNet decoder with model: " + model_path);
        }
    }

    ~WenetDecoderWrapper() {
        if (decoder_) {
            wenet_free(decoder_);
        }
    }

    // Delete copy constructor and assignment operator
    WenetDecoderWrapper(const WenetDecoderWrapper&) = delete;
    WenetDecoderWrapper& operator=(const WenetDecoderWrapper&) = delete;

    // Move constructor and assignment
    WenetDecoderWrapper(WenetDecoderWrapper&& other) noexcept 
        : decoder_(other.decoder_) {
        other.decoder_ = nullptr;
    }

    WenetDecoderWrapper& operator=(WenetDecoderWrapper&& other) noexcept {
        if (this != &other) {
            if (decoder_) {
                wenet_free(decoder_);
            }
            decoder_ = other.decoder_;
            other.decoder_ = nullptr;
        }
        return *this;
    }

    void* get() const { return decoder_; }
    explicit operator bool() const { return decoder_ != nullptr; }

    void reset() {
        if (decoder_) {
            wenet_reset(decoder_);
        }
    }

    void setNBest(int n) {
        if (decoder_) {
            wenet_set_nbest(decoder_, n);
        }
    }

    void setTimestamp(bool enabled) {
        if (decoder_) {
            wenet_set_timestamp(decoder_, enabled ? 1 : 0);
        }
    }

    void setContinuousDecoding(bool enabled) {
        if (decoder_) {
            wenet_set_continuous_decoding(decoder_, enabled ? 1 : 0);
        }
    }

    void setChunkSize(int chunk_size) {
        if (decoder_) {
            wenet_set_chunk_size(decoder_, chunk_size);
        }
    }

private:
    void* decoder_;
};

/**
 * Structure to represent a chunk of audio data
 */
struct AudioChunk {
    std::vector<int16_t> samples;
    uint64_t timestamp;
    
    AudioChunk() : timestamp(0) {}
    AudioChunk(const std::vector<int16_t>& s, uint64_t ts) : samples(s), timestamp(ts) {}
    AudioChunk(std::vector<int16_t>&& s, uint64_t ts) : samples(std::move(s)), timestamp(ts) {}
};

/**
 * Structure to represent a transcription result
 */
struct TranscriptionResult {
    std::string text;
    bool isFinal;
    double confidence;
    uint64_t timestamp;
    std::vector<std::pair<std::string, double>> alternatives;  // N-best alternatives
    
    TranscriptionResult() : isFinal(false), confidence(0.0), timestamp(0) {}
    TranscriptionResult(const std::string& t, bool f, double c, uint64_t ts) 
        : text(t), isFinal(f), confidence(c), timestamp(ts) {}
};

/**
 * Enhanced configuration parameters for the WeNet STT engine
 */
struct WeNetConfig {
    std::string modelPath;
    int sampleRate;
    bool partialResultsEnabled;
    double maxLatency;
    bool vadEnabled;
    double vadSilenceThreshold;
    double vadSpeechThreshold;
    double maxChunkDuration;
    int nBest;                      // Number of alternatives to return
    bool timestampEnabled;          // Enable word-level timestamps
    int bufferCapacity;             // Circular buffer capacity
    int chunkSize;                  // Processing chunk size
    double beamSize;                // Decoding beam size
    double languageModelWeight;     // LM weight
    
    WeNetConfig() 
        : sampleRate(16000), 
          partialResultsEnabled(true), 
          maxLatency(0.3),
          vadEnabled(true),
          vadSilenceThreshold(-40.0),
          vadSpeechThreshold(-10.0),
          maxChunkDuration(3.0),
          nBest(1),
          timestampEnabled(false),
          bufferCapacity(100),
          chunkSize(16),
          beamSize(10.0),
          languageModelWeight(0.5) {}
};

/**
 * Callback interface for receiving transcription results
 */
class TranscriptionCallback {
public:
    virtual ~TranscriptionCallback() {}
    virtual void onTranscriptionResult(const TranscriptionResult& result) = 0;
    virtual void onError(const std::string& errorMessage) = 0;
};

/**
 * Performance metrics tracking
 */
struct PerformanceMetrics {
    std::atomic<uint64_t> totalSamplesProcessed{0};
    std::atomic<uint64_t> totalProcessingTimeMs{0};
    std::atomic<uint64_t> queueDepth{0};
    std::atomic<uint64_t> errorCount{0};
    std::atomic<double> averageLatencyMs{0.0};
};

/**
 * Improved implementation class for WeNet speech-to-text functionality
 */
class WenetSTTImplImproved {
public:
    WenetSTTImplImproved(const WeNetConfig& config, TranscriptionCallback* callback);
    ~WenetSTTImplImproved();
    
    // Initialize the engine
    bool initialize();
    
    // Process an audio chunk (thread-safe)
    void processAudioChunk(const AudioChunk& chunk);
    void processAudioChunk(AudioChunk&& chunk);  // Move version
    
    // Signal that no more audio is coming (flush any pending results)
    void flush();
    
    // Check if the engine is running
    bool isRunning() const;
    
    // Get performance metrics
    PerformanceMetrics getMetrics() const;
    
private:
    // Internal processing thread function
    void processingThread();
    
    // Apply voice activity detection
    bool detectSpeech(const std::vector<int16_t>& samples);
    
    // Calculate confidence score for transcription
    double calculateConfidence(const std::string& text);
    
    // Parse WeNet JSON result
    void parseJsonResult(const std::string& jsonResult, bool isFinal);
    
    // Process audio batch
    void processBatch(const std::vector<AudioChunk>& batch);
    
    // Configuration
    WeNetConfig config_;
    
    // Callback for results
    TranscriptionCallback* callback_;
    
    // Processing thread and synchronization
    std::thread processingThread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    CircularBuffer<AudioChunk> audioQueue_;
    std::atomic<bool> running_;
    std::atomic<bool> shouldFlush_;
    
    // WeNet decoder with RAII
    std::unique_ptr<WenetDecoderWrapper> wenetDecoder_;
    
    // Internal state tracking
    std::atomic<bool> speechDetected_;
    std::vector<int16_t> currentBuffer_;
    std::atomic<uint64_t> lastTimestamp_;
    
    // Performance metrics
    mutable PerformanceMetrics metrics_;
    
    // Error handling
    void handleError(const std::string& context, const std::exception& e);
};

} // namespace wenet_streams

#endif // WENET_STT_IMPL_IMPROVED_HPP