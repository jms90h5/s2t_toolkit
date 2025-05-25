#ifndef WENET_STT_ONNX_HPP
#define WENET_STT_ONNX_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <onnxruntime_cxx_api.h>

namespace wenet_streams {

/**
 * C++ WeNet implementation using ONNX Runtime
 * This replaces the WeNet C API with ONNX Runtime for the neural network parts
 */
class WenetSTTONNX {
public:
    struct Config {
        std::string encoder_model_path;  // Path to encoder.onnx
        std::string decoder_model_path;  // Path to decoder.onnx (if using attention)
        std::string vocab_path;
        std::string cmvn_path;
        int sample_rate = 16000;
        int chunk_ms = 100;
        int feature_dim = 80;
        int beam_size = 10;
        bool enable_timestamp = false;
    };
    
    explicit WenetSTTONNX(const Config& config);
    ~WenetSTTONNX();
    
    // Process audio and return transcription
    struct Result {
        std::string text;
        bool is_final;
        double confidence;
        uint64_t latency_ms;
    };
    
    Result processAudio(const int16_t* samples, size_t num_samples, uint64_t timestamp_ms);
    void reset();
    
private:
    // ONNX Runtime components
    Ort::Env env_;
    Ort::SessionOptions session_options_;
    std::unique_ptr<Ort::Session> encoder_session_;
    std::unique_ptr<Ort::Session> decoder_session_;  // Optional for attention models
    Ort::MemoryInfo memory_info_;
    
    // Feature extraction (keeping in C++ for speed)
    class FeatureExtractor {
    public:
        FeatureExtractor(int sample_rate, int feature_dim);
        std::vector<float> extractFeatures(const int16_t* samples, size_t num_samples);
        void applyCMVN(std::vector<float>& features, const std::vector<float>& mean, 
                      const std::vector<float>& istd);
    private:
        int sample_rate_;
        int feature_dim_;
        // Fbank extraction components...
    };
    
    // CTC decoder (keeping in C++ for speed)
    class CTCBeamSearchDecoder {
    public:
        CTCBeamSearchDecoder(const std::vector<std::string>& vocab, int beam_size);
        std::vector<int> decode(const std::vector<float>& logits, int time_steps);
        double getConfidence() const { return confidence_; }
    private:
        std::vector<std::string> vocab_;
        int beam_size_;
        double confidence_;
        // Beam search state...
    };
    
    Config config_;
    FeatureExtractor feature_extractor_;
    CTCBeamSearchDecoder decoder_;
    
    // CMVN stats
    std::vector<float> cmvn_mean_;
    std::vector<float> cmvn_istd_;
    
    // Streaming state
    std::vector<float> feature_buffer_;
    std::string prev_text_;
    
    // Helper methods
    void loadCMVN(const std::string& cmvn_path);
    std::vector<std::string> loadVocabulary(const std::string& vocab_path);
};

// Implementation snippet showing ONNX usage
inline WenetSTTONNX::Result WenetSTTONNX::processAudio(
    const int16_t* samples, size_t num_samples, uint64_t timestamp_ms) {
    
    auto start = std::chrono::steady_clock::now();
    
    // 1. Extract features (C++ for speed)
    auto features = feature_extractor_.extractFeatures(samples, num_samples);
    
    // 2. Apply CMVN
    feature_extractor_.applyCMVN(features, cmvn_mean_, cmvn_istd_);
    
    // 3. Add to buffer for streaming
    feature_buffer_.insert(feature_buffer_.end(), features.begin(), features.end());
    
    // 4. Prepare input tensor
    int num_frames = feature_buffer_.size() / config_.feature_dim;
    std::vector<int64_t> input_shape = {1, num_frames, config_.feature_dim};
    
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info_, feature_buffer_.data(), feature_buffer_.size(),
        input_shape.data(), input_shape.size()
    );
    
    // 5. Run encoder
    const char* input_names[] = {"speech"};
    const char* output_names[] = {"encoder_out"};
    
    auto output_tensors = encoder_session_->Run(
        Ort::RunOptions{nullptr}, 
        input_names, &input_tensor, 1,
        output_names, 1
    );
    
    // 6. Get encoder output
    float* encoder_out = output_tensors[0].GetTensorMutableData<float>();
    auto out_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
    int time_steps = out_shape[1];
    
    // 7. CTC decode (C++ for speed)
    std::vector<float> logits(encoder_out, encoder_out + time_steps * vocab_.size());
    auto token_ids = decoder_.decode(logits, time_steps);
    
    // 8. Convert to text
    std::string text;
    for (int id : token_ids) {
        if (id > 0 && id < vocab_.size()) {
            text += vocab_[id];
        }
    }
    
    // 9. Determine if final
    bool is_final = (text != prev_text_ && text.length() > prev_text_.length());
    prev_text_ = text;
    
    // 10. Calculate latency
    auto end = std::chrono::steady_clock::now();
    auto latency_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();
    
    return {text, is_final, decoder_.getConfidence(), latency_ms};
}

} // namespace wenet_streams

#endif // WENET_STT_ONNX_HPP