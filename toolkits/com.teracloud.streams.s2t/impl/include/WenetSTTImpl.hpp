#ifndef WENET_STT_IMPL_HPP
#define WENET_STT_IMPL_HPP

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

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
// Real WeNet includes - use dynamic loading instead
// #include "wenet/api/wenet_api.h"
// #include "wenet/decoder/asr_decoder.h"
// #include "wenet/frontend/feature_pipeline.h"
#endif

// Forward declaration for dynamic loader
namespace wenet_streams {
    class WenetDynamicLoader;
}

namespace wenet_streams {

/**
 * Structure to represent a chunk of audio data
 */
struct AudioChunk {
    std::vector<int16_t> samples;
    uint64_t timestamp;
    
    AudioChunk() : timestamp(0) {}
    AudioChunk(const std::vector<int16_t>& s, uint64_t ts) : samples(s), timestamp(ts) {}
};

/**
 * Structure to represent a transcription result
 */
struct TranscriptionResult {
    std::string text;
    bool isFinal;
    double confidence;
    uint64_t timestamp;
    
    TranscriptionResult() : isFinal(false), confidence(0.0), timestamp(0) {}
    TranscriptionResult(const std::string& t, bool f, double c, uint64_t ts) 
        : text(t), isFinal(f), confidence(c), timestamp(ts) {}
};

/**
 * Configuration parameters for the WeNet STT engine
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
    
    WeNetConfig() 
        : sampleRate(16000), 
          partialResultsEnabled(true), 
          maxLatency(0.3),
          vadEnabled(true),
          vadSilenceThreshold(-40.0),
          vadSpeechThreshold(-10.0),
          maxChunkDuration(3.0) {}
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
 * Main implementation class for WeNet speech-to-text functionality
 */
class WenetSTTImpl {
public:
    WenetSTTImpl(const WeNetConfig& config, TranscriptionCallback* callback);
    ~WenetSTTImpl();
    
    // Initialize the engine
    bool initialize();
    
    // Process an audio chunk
    void processAudioChunk(const AudioChunk& chunk);
    
    // Signal that no more audio is coming (flush any pending results)
    void flush();
    
    // Check if the engine is running
    bool isRunning() const;
    
private:
    // Internal processing thread function
    void processingThread();
    
    // Apply voice activity detection
    bool detectSpeech(const std::vector<int16_t>& samples);
    
    // Calculate confidence score for transcription
    double calculateConfidence(const std::string& text);
    
    // Parse WeNet JSON result
    void parseJsonResult(const std::string& jsonResult, bool isFinal);
    
    // Configuration
    WeNetConfig config_;
    
    // Callback for results
    TranscriptionCallback* callback_;
    
    // Dynamic loader for WeNet functions
    std::unique_ptr<WenetDynamicLoader> wenetLoader_;
    
    // Processing thread and synchronization
    std::thread processingThread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<AudioChunk> audioQueue_;
    std::atomic<bool> running_;
    std::atomic<bool> shouldFlush_;
    
    // WeNet decoder
    void* wenetDecoder_;
    
    // Internal state tracking
    bool speechDetected_;
    std::vector<int16_t> currentBuffer_;
    uint64_t lastTimestamp_;
};

} // namespace wenet_streams

#endif // WENET_STT_IMPL_HPP