#include "OnnxSTTWrapper.hpp"
#include "OnnxSTTImpl.hpp"

namespace onnx_stt {

OnnxSTTWrapper::OnnxSTTWrapper(const Config& config) {
    OnnxSTTImpl::Config implConfig;
    implConfig.encoder_onnx_path = config.encoder_onnx_path;
    implConfig.vocab_path = config.vocab_path;
    implConfig.cmvn_stats_path = config.cmvn_stats_path;
    implConfig.sample_rate = config.sample_rate;
    implConfig.chunk_size_ms = config.chunk_size_ms;
    implConfig.num_threads = config.num_threads;
    implConfig.use_gpu = config.use_gpu;
    
    pImpl = std::make_unique<OnnxSTTImpl>(implConfig);
}

OnnxSTTWrapper::~OnnxSTTWrapper() = default;

bool OnnxSTTWrapper::initialize() {
    return pImpl->initialize();
}

OnnxSTTWrapper::TranscriptionResult OnnxSTTWrapper::processAudioChunk(
    const int16_t* samples, size_t num_samples, uint64_t timestamp_ms) {
    
    auto implResult = pImpl->processAudioChunk(samples, num_samples, timestamp_ms);
    
    TranscriptionResult result;
    result.text = implResult.text;
    result.is_final = implResult.is_final;
    result.confidence = implResult.confidence;
    result.timestamp_ms = implResult.timestamp_ms;
    result.latency_ms = implResult.latency_ms;
    
    return result;
}

void OnnxSTTWrapper::reset() {
    pImpl->reset();
}

OnnxSTTWrapper::Stats OnnxSTTWrapper::getStats() const {
    auto implStats = pImpl->getStats();
    
    Stats stats;
    stats.total_samples = 0;  // Not tracked in impl
    stats.total_chunks = 0;   // Not tracked in impl
    stats.real_time_factor = implStats.real_time_factor;
    stats.avg_latency_ms = 0; // Not tracked in impl
    
    return stats;
}

} // namespace onnx_stt