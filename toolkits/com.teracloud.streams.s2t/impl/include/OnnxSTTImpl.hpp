#ifndef ONNX_STT_IMPL_HPP
#define ONNX_STT_IMPL_HPP

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <chrono>

// Prevent macro conflicts with Streams
#ifdef NO_EXCEPTION
#undef NO_EXCEPTION
#endif

#include "onnxruntime_cxx_api.h"

namespace onnx_stt {

/**
 * ONNX-based Speech-to-Text implementation
 * Uses ONNX Runtime for maximum portability with any ONNX speech model
 */
class OnnxSTTImpl {
public:
    struct Config {
        // Model paths
        std::string encoder_onnx_path;
        std::string vocab_path;
        std::string cmvn_stats_path;
        
        // Audio parameters
        int sample_rate = 16000;
        int chunk_size_ms = 100;
        
        // Feature parameters
        int num_mel_bins = 80;
        int frame_length_ms = 25;
        int frame_shift_ms = 10;
        
        // Decoding parameters
        int beam_size = 10;
        int blank_id = 0;
        
        // Performance tuning
        int num_threads = 4;
        bool use_gpu = false;
    };
    
    struct TranscriptionResult {
        std::string text;
        bool is_final;
        double confidence;
        uint64_t timestamp_ms;
        uint64_t latency_ms;
    };
    
    explicit OnnxSTTImpl(const Config& config);
    ~OnnxSTTImpl();
    
    // Initialize ONNX runtime and load models
    bool initialize();
    
    // Process audio chunk
    TranscriptionResult processAudioChunk(const int16_t* samples, 
                                         size_t num_samples, 
                                         uint64_t timestamp_ms);
    
    // Reset decoder state
    void reset();
    
    // Get performance stats
    struct Stats {
        uint64_t total_audio_ms = 0;
        uint64_t total_processing_ms = 0;
        double real_time_factor = 0.0;
    };
    Stats getStats() const { return stats_; }
    
private:
    Config config_;
    
    // ONNX Runtime
    Ort::Env env_;
    Ort::SessionOptions session_options_;
    std::unique_ptr<Ort::Session> encoder_session_;
    Ort::MemoryInfo memory_info_;
    
    // Model info
    std::vector<std::string> vocab_;
    std::vector<float> cmvn_mean_;
    std::vector<float> cmvn_var_;
    
    // Feature extraction state
    std::vector<float> feature_buffer_;
    size_t feature_frame_shift_samples_;
    size_t feature_frame_length_samples_;
    
    // Decoder state
    std::string previous_text_;
    std::vector<std::vector<float>> hypothesis_scores_;
    
    // Performance tracking
    mutable Stats stats_;
    
    // Internal methods
    std::vector<float> extractFbankFeatures(const int16_t* samples, size_t num_samples);
    void applyCMVN(std::vector<float>& features);
    std::string ctcDecode(const std::vector<float>& logits, int num_frames, double& confidence);
    bool loadVocabulary(const std::string& path);
    bool loadCMVNStats(const std::string& path);
};

} // namespace onnx_stt

#endif // ONNX_STT_IMPL_HPP