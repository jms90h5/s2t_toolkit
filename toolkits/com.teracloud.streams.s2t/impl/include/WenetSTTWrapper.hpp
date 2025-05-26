#ifndef WENET_STT_WRAPPER_HPP
#define WENET_STT_WRAPPER_HPP

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace wenet_streams {

// Forward declaration - no WeNet headers here
class WenetSTTImpl;

/**
 * Wrapper class that provides interface without exposing WeNet headers
 * This prevents any potential conflicts with SPL
 */
class WenetSTTWrapper {
public:
    struct Config {
        std::string model_dir;
        int sample_rate = 16000;
        int nbest = 1;
        int chunk_size = 16;
        bool enable_timestamp = false;
        int num_threads = 1;
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
    };
    
    WenetSTTWrapper(const Config& config);
    ~WenetSTTWrapper();
    
    bool initialize();
    TranscriptionResult processAudioChunk(const int16_t* samples, size_t num_samples, uint64_t timestamp_ms);
    void reset();
    Stats getStats() const;
    
private:
    // Use pimpl pattern to hide WeNet implementation
    std::unique_ptr<WenetSTTImpl> pImpl;
    
    // Internal callback handler
    class WrapperCallback;
    std::unique_ptr<WrapperCallback> callback_;
};

} // namespace wenet_streams

#endif // WENET_STT_WRAPPER_HPP