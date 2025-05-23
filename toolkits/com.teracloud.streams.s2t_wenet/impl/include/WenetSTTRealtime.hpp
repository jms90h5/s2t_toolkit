#ifndef WENET_STT_REALTIME_HPP
#define WENET_STT_REALTIME_HPP

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

// Check if we're using the mock implementation
#ifdef MOCK_WENET_IMPLEMENTATION
extern "C" {
void* wenet_init(const char* model_dir);
void wenet_free(void* decoder);
void wenet_reset(void* decoder);
const char* wenet_decode(void* decoder, const char* data, int len, int last);
void wenet_set_nbest(void* decoder, int n);
void wenet_set_timestamp(void* decoder, int flag);
void wenet_set_continuous_decoding(void* decoder, int flag);
}
#else
#include "wenet/api/wenet_api.h"
#endif

#include "rapidjson/document.h"

namespace wenet_streams {

/**
 * RAII wrapper for WeNet decoder - focused on safety
 */
class WenetDecoder {
public:
    explicit WenetDecoder(const std::string& model_path) {
        decoder_ = wenet_init(model_path.c_str());
        if (!decoder_) {
            throw std::runtime_error("Failed to initialize WeNet decoder");
        }
    }
    
    ~WenetDecoder() {
        if (decoder_) {
            wenet_free(decoder_);
        }
    }
    
    // Delete copy operations
    WenetDecoder(const WenetDecoder&) = delete;
    WenetDecoder& operator=(const WenetDecoder&) = delete;
    
    // Move operations
    WenetDecoder(WenetDecoder&& other) noexcept : decoder_(other.decoder_) {
        other.decoder_ = nullptr;
    }
    
    WenetDecoder& operator=(WenetDecoder&& other) noexcept {
        if (this != &other) {
            if (decoder_) wenet_free(decoder_);
            decoder_ = other.decoder_;
            other.decoder_ = nullptr;
        }
        return *this;
    }
    
    void* get() const { return decoder_; }
    
private:
    void* decoder_;
};

/**
 * Real-time transcription result
 */
struct RealtimeResult {
    std::string text;
    bool is_final;
    double confidence;
    uint64_t audio_timestamp_ms;
    uint64_t processing_timestamp_ms;
    
    // For real-time, latency is critical
    uint64_t latency_ms() const {
        return processing_timestamp_ms - audio_timestamp_ms;
    }
};

/**
 * Real-time configuration - focused on low latency
 */
struct RealtimeConfig {
    std::string model_path;
    int sample_rate = 16000;
    
    // Real-time specific parameters
    int chunk_ms = 100;              // Process every 100ms of audio
    bool enable_partial = true;      // Stream partial results
    bool enable_vad = true;          // Voice activity detection
    int vad_silence_ms = 500;        // Silence duration to end utterance
    
    // Advanced tuning
    int n_best = 1;                  // Keep it low for speed
    bool word_timestamps = false;    // Disabled by default for speed
    
    // Compute chunk size in samples
    int chunk_samples() const {
        return (sample_rate * chunk_ms) / 1000;
    }
};

/**
 * Callback for real-time results
 */
using ResultCallback = std::function<void(const RealtimeResult&)>;
using ErrorCallback = std::function<void(const std::string&)>;

/**
 * Real-time STT implementation - optimized for minimal latency
 */
class WenetSTTRealtime {
public:
    WenetSTTRealtime(const RealtimeConfig& config, 
                     ResultCallback result_cb,
                     ErrorCallback error_cb = nullptr);
    ~WenetSTTRealtime();
    
    // Initialize the decoder
    bool initialize();
    
    // Process audio in real-time - NO BATCHING
    void processAudio(const int16_t* samples, size_t num_samples, uint64_t timestamp_ms);
    
    // Signal end of stream
    void endStream();
    
    // Reset for new utterance
    void reset();
    
    // Check if active
    bool isActive() const { return active_; }
    
    // Get current latency stats
    struct LatencyStats {
        double avg_latency_ms;
        double min_latency_ms;
        double max_latency_ms;
        uint64_t sample_count;
    };
    LatencyStats getLatencyStats() const;
    
private:
    // Process accumulated audio
    void processAccumulatedAudio(bool force_final);
    
    // Parse WeNet result
    void parseResult(const char* json_result, bool is_final, uint64_t audio_ts);
    
    // Simple VAD based on energy
    bool detectSpeech(const int16_t* samples, size_t num_samples);
    
    // Configuration
    RealtimeConfig config_;
    ResultCallback result_callback_;
    ErrorCallback error_callback_;
    
    // Decoder
    std::unique_ptr<WenetDecoder> decoder_;
    
    // State
    std::atomic<bool> active_{false};
    std::mutex mutex_;
    
    // Audio buffer - only for accumulating to chunk size
    std::vector<int16_t> audio_buffer_;
    uint64_t buffer_timestamp_ms_{0};
    
    // VAD state
    bool in_speech_{false};
    uint64_t last_speech_ms_{0};
    std::chrono::steady_clock::time_point last_speech_time_;
    
    // Latency tracking
    mutable std::mutex stats_mutex_;
    double total_latency_ms_{0};
    double min_latency_ms_{std::numeric_limits<double>::max()};
    double max_latency_ms_{0};
    uint64_t latency_samples_{0};
};

// Inline implementation of key methods for header-only use

inline WenetSTTRealtime::WenetSTTRealtime(const RealtimeConfig& config,
                                          ResultCallback result_cb,
                                          ErrorCallback error_cb)
    : config_(config), 
      result_callback_(std::move(result_cb)),
      error_callback_(std::move(error_cb)) {
    if (!result_callback_) {
        throw std::invalid_argument("Result callback is required");
    }
}

inline WenetSTTRealtime::~WenetSTTRealtime() {
    active_ = false;
}

inline bool WenetSTTRealtime::initialize() {
    try {
        std::lock_guard<std::mutex> lock(mutex_);
        
        decoder_ = std::make_unique<WenetDecoder>(config_.model_path);
        
        // Configure for real-time streaming
        wenet_set_continuous_decoding(decoder_->get(), 1);
        wenet_set_nbest(decoder_->get(), config_.n_best);
        wenet_set_timestamp(decoder_->get(), config_.word_timestamps ? 1 : 0);
        
        active_ = true;
        return true;
        
    } catch (const std::exception& e) {
        if (error_callback_) {
            error_callback_("Initialization failed: " + std::string(e.what()));
        }
        return false;
    }
}

inline void WenetSTTRealtime::processAudio(const int16_t* samples, 
                                           size_t num_samples, 
                                           uint64_t timestamp_ms) {
    if (!active_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Track when we first receive audio for this buffer
    if (audio_buffer_.empty()) {
        buffer_timestamp_ms_ = timestamp_ms;
    }
    
    // Append to buffer
    audio_buffer_.insert(audio_buffer_.end(), samples, samples + num_samples);
    
    // Process immediately if we have a chunk worth of audio
    if (audio_buffer_.size() >= config_.chunk_samples()) {
        processAccumulatedAudio(false);
    }
}

inline void WenetSTTRealtime::processAccumulatedAudio(bool force_final) {
    if (audio_buffer_.empty()) return;
    
    auto process_start = std::chrono::steady_clock::now();
    
    // VAD check if enabled
    bool is_speech = true;
    if (config_.enable_vad) {
        is_speech = detectSpeech(audio_buffer_.data(), audio_buffer_.size());
    }
    
    if (is_speech) {
        in_speech_ = true;
        last_speech_ms_ = buffer_timestamp_ms_;
        last_speech_time_ = std::chrono::steady_clock::now();
        
        // Decode audio
        const char* audio_data = reinterpret_cast<const char*>(audio_buffer_.data());
        int data_size = audio_buffer_.size() * sizeof(int16_t);
        
        const char* result = wenet_decode(decoder_->get(), audio_data, data_size, 
                                         force_final ? 1 : 0);
        
        if (result) {
            parseResult(result, force_final, buffer_timestamp_ms_);
        }
        
    } else if (in_speech_) {
        // Check if silence duration exceeds threshold
        auto silence_duration = std::chrono::steady_clock::now() - last_speech_time_;
        auto silence_ms = std::chrono::duration_cast<std::chrono::milliseconds>(silence_duration).count();
        
        if (silence_ms >= config_.vad_silence_ms || force_final) {
            // End of utterance - get final result
            const char* audio_data = reinterpret_cast<const char*>(audio_buffer_.data());
            int data_size = audio_buffer_.size() * sizeof(int16_t);
            
            const char* result = wenet_decode(decoder_->get(), audio_data, data_size, 1);
            if (result) {
                parseResult(result, true, buffer_timestamp_ms_);
            }
            
            // Reset decoder for next utterance
            wenet_reset(decoder_->get());
            in_speech_ = false;
        }
    }
    
    // Clear processed audio
    audio_buffer_.clear();
}

inline void WenetSTTRealtime::parseResult(const char* json_result, 
                                          bool is_final, 
                                          uint64_t audio_ts) {
    auto now = std::chrono::steady_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    try {
        rapidjson::Document doc;
        doc.Parse(json_result);
        
        if (!doc.HasParseError() && doc.HasMember("text")) {
            RealtimeResult result;
            result.text = doc["text"].GetString();
            result.is_final = is_final;
            result.confidence = doc.HasMember("confidence") ? 
                               doc["confidence"].GetDouble() : 0.9;
            result.audio_timestamp_ms = audio_ts;
            result.processing_timestamp_ms = now_ms;
            
            // Update latency stats
            double latency = result.latency_ms();
            {
                std::lock_guard<std::mutex> lock(stats_mutex_);
                total_latency_ms_ += latency;
                min_latency_ms_ = std::min(min_latency_ms_, latency);
                max_latency_ms_ = std::max(max_latency_ms_, latency);
                latency_samples_++;
            }
            
            // Deliver result
            result_callback_(result);
        }
    } catch (const std::exception& e) {
        if (error_callback_) {
            error_callback_("Parse error: " + std::string(e.what()));
        }
    }
}

inline bool WenetSTTRealtime::detectSpeech(const int16_t* samples, size_t num_samples) {
    if (num_samples == 0) return false;
    
    // Simple energy-based VAD
    double energy = 0.0;
    for (size_t i = 0; i < num_samples; ++i) {
        double normalized = samples[i] / 32768.0;
        energy += normalized * normalized;
    }
    energy = std::sqrt(energy / num_samples);
    
    // Convert to dB
    double energy_db = 20.0 * std::log10(energy + 1e-10);
    
    // Simple threshold
    return energy_db > -30.0;
}

inline void WenetSTTRealtime::endStream() {
    std::lock_guard<std::mutex> lock(mutex_);
    processAccumulatedAudio(true);
}

inline void WenetSTTRealtime::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    audio_buffer_.clear();
    wenet_reset(decoder_->get());
    in_speech_ = false;
}

inline WenetSTTRealtime::LatencyStats WenetSTTRealtime::getLatencyStats() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    LatencyStats stats;
    stats.sample_count = latency_samples_;
    stats.avg_latency_ms = latency_samples_ > 0 ? 
                           total_latency_ms_ / latency_samples_ : 0.0;
    stats.min_latency_ms = latency_samples_ > 0 ? min_latency_ms_ : 0.0;
    stats.max_latency_ms = max_latency_ms_;
    return stats;
}

} // namespace wenet_streams

#endif // WENET_STT_REALTIME_HPP