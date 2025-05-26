#include "WenetSTTWrapper.hpp"
#include "WenetSTTImpl.hpp"

namespace wenet_streams {

// Internal callback handler - must be fully defined before use
class WenetSTTWrapper::WrapperCallback : public TranscriptionCallback {
public:
    WenetSTTWrapper::TranscriptionResult lastResult;
    bool hasResult = false;
    
    void onTranscriptionResult(const wenet_streams::TranscriptionResult& result) override {
        lastResult.text = result.text;
        lastResult.is_final = result.isFinal;
        lastResult.confidence = result.confidence;
        lastResult.timestamp_ms = result.timestamp;
        lastResult.latency_ms = 0; // Not provided by original
        hasResult = true;
    }
    
    void onError(const std::string& errorMessage) override {
        // Log error but don't propagate for now
        (void)errorMessage; // Suppress unused warning
    }
};

WenetSTTWrapper::WenetSTTWrapper(const Config& config) {
    WeNetConfig implConfig;
    implConfig.modelPath = config.model_dir;
    implConfig.sampleRate = config.sample_rate;
    
    // Set reasonable defaults for fields not in our config
    implConfig.partialResultsEnabled = true;
    implConfig.maxLatency = 0.3;
    implConfig.vadEnabled = false;
    implConfig.vadSilenceThreshold = -40.0;
    implConfig.vadSpeechThreshold = -10.0;
    implConfig.maxChunkDuration = 3.0;
    
    callback_ = std::make_unique<WrapperCallback>();
    pImpl = std::make_unique<WenetSTTImpl>(implConfig, callback_.get());
}

WenetSTTWrapper::~WenetSTTWrapper() = default;

bool WenetSTTWrapper::initialize() {
    return pImpl->initialize();
}

WenetSTTWrapper::TranscriptionResult WenetSTTWrapper::processAudioChunk(
    const int16_t* samples, size_t num_samples, uint64_t timestamp_ms) {
    
    // Convert to AudioChunk
    std::vector<int16_t> sampleVec(samples, samples + num_samples);
    AudioChunk chunk(sampleVec, timestamp_ms);
    
    // Clear previous result
    callback_->hasResult = false;
    
    // Process
    pImpl->processAudioChunk(chunk);
    
    // Return result if available
    if (callback_->hasResult) {
        return callback_->lastResult;
    }
    
    // Return empty result
    TranscriptionResult empty;
    empty.text = "";
    empty.is_final = false;
    empty.confidence = 0.0;
    empty.timestamp_ms = timestamp_ms;
    empty.latency_ms = 0;
    return empty;
}

void WenetSTTWrapper::reset() {
    // Flush any pending data
    pImpl->flush();
}

WenetSTTWrapper::Stats WenetSTTWrapper::getStats() const {
    // WenetSTTImpl doesn't provide stats, return empty
    Stats stats;
    stats.total_samples = 0;
    stats.total_chunks = 0;
    stats.real_time_factor = 0.0;
    return stats;
}

} // namespace wenet_streams