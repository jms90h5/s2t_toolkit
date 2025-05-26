#ifndef ONNX_STT_WRAPPER_HPP
#define ONNX_STT_WRAPPER_HPP

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace onnx_stt {

// Forward declaration - no ONNX headers here
class OnnxSTTImpl;

/**
 * Wrapper class that provides interface without exposing ONNX Runtime headers
 * This prevents macro conflicts with SPL
 */
class OnnxSTTWrapper {
public:
    struct Config {
        std::string encoder_onnx_path;
        std::string vocab_path;
        std::string cmvn_stats_path;
        int sample_rate = 16000;
        int chunk_size_ms = 100;
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
    
    struct Stats {
        uint64_t total_samples;
        uint64_t total_chunks;
        double real_time_factor;
        double avg_latency_ms;
    };
    
    OnnxSTTWrapper(const Config& config);
    ~OnnxSTTWrapper();
    
    bool initialize();
    TranscriptionResult processAudioChunk(const int16_t* samples, size_t num_samples, uint64_t timestamp_ms);
    void reset();
    Stats getStats() const;
    
private:
    // Use pimpl pattern to hide ONNX Runtime
    std::unique_ptr<OnnxSTTImpl> pImpl;
};

} // namespace onnx_stt

#endif // ONNX_STT_WRAPPER_HPP