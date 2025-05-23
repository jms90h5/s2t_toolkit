#include "WenetONNXImpl.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>

// For feature extraction
#ifdef USE_KALDI_NATIVE_FBANK
#include "kaldi-native-fbank/csrc/feature-fbank.h"
#else
// Simple Fbank implementation
#include "SimpleFbank.hpp"
#endif

namespace wenet_streams {

WenetONNXImpl::WenetONNXImpl(const Config& config)
    : config_(config)
    , env_(ORT_LOGGING_LEVEL_WARNING, "WenetONNX")
    , memory_info_(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)) {
    
    // Calculate frame parameters
    feature_frame_shift_samples_ = (config_.sample_rate * config_.frame_shift_ms) / 1000;
    feature_frame_length_samples_ = (config_.sample_rate * config_.frame_length_ms) / 1000;
}

WenetONNXImpl::~WenetONNXImpl() = default;

bool WenetONNXImpl::initialize() {
    try {
        // Configure session options
        session_options_.SetIntraOpNumThreads(config_.num_threads);
        session_options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
        
        if (config_.use_gpu) {
            // Add CUDA provider if available
            #ifdef USE_CUDA
            OrtCUDAProviderOptions cuda_options;
            session_options_.AppendExecutionProvider_CUDA(cuda_options);
            #endif
        }
        
        // Load encoder model
        std::cout << "Loading ONNX model from: " << config_.encoder_onnx_path << std::endl;
        encoder_session_ = std::make_unique<Ort::Session>(env_, 
            config_.encoder_onnx_path.c_str(), session_options_);
        
        // Verify model inputs/outputs
        size_t num_inputs = encoder_session_->GetInputCount();
        size_t num_outputs = encoder_session_->GetOutputCount();
        std::cout << "Model has " << num_inputs << " inputs and " 
                  << num_outputs << " outputs" << std::endl;
        
        // Load vocabulary
        if (!loadVocabulary(config_.vocab_path)) {
            std::cerr << "Failed to load vocabulary" << std::endl;
            return false;
        }
        
        // Load CMVN stats
        if (!loadCMVNStats(config_.cmvn_stats_path)) {
            std::cerr << "Failed to load CMVN stats" << std::endl;
            return false;
        }
        
        std::cout << "WenetONNX initialized successfully" << std::endl;
        return true;
        
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

WenetONNXImpl::TranscriptionResult WenetONNXImpl::processAudioChunk(
    const int16_t* samples, size_t num_samples, uint64_t timestamp_ms) {
    
    auto start_time = std::chrono::steady_clock::now();
    TranscriptionResult result;
    result.timestamp_ms = timestamp_ms;
    
    // Check if initialized
    if (!encoder_session_) {
        result.text = "";
        result.is_final = false;
        result.confidence = 0.0;
        result.latency_ms = 0;
        return result;
    }
    
    try {
        // 1. Extract Fbank features
        auto features = extractFbankFeatures(samples, num_samples);
        
        // 2. Apply CMVN normalization
        applyCMVN(features);
        
        // 3. Add to feature buffer
        feature_buffer_.insert(feature_buffer_.end(), features.begin(), features.end());
        
        // 4. Check if we have enough features to process
        int num_frames = feature_buffer_.size() / config_.num_mel_bins;
        if (num_frames < 5) {  // Need minimum frames
            result.text = previous_text_;
            result.is_final = false;
            result.confidence = 0.0;
            return result;
        }
        
        // 5. Prepare input tensor
        std::vector<int64_t> input_shape = {1, num_frames, config_.num_mel_bins};
        std::vector<float> input_data = feature_buffer_;  // Copy for ONNX
        
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info_, 
            input_data.data(), 
            input_data.size(),
            input_shape.data(), 
            input_shape.size()
        );
        
        // Also need sequence length tensor
        std::vector<int64_t> length_shape = {1};
        std::vector<int64_t> length_data = {num_frames};
        
        Ort::Value length_tensor = Ort::Value::CreateTensor<int64_t>(
            memory_info_,
            length_data.data(),
            length_data.size(),
            length_shape.data(),
            length_shape.size()
        );
        
        // 6. Run encoder
        const char* input_names[] = {"speech", "speech_lengths"};
        const char* output_names[] = {"encoder_out", "encoder_out_lens"};
        
        std::vector<Ort::Value> input_tensors;
        input_tensors.push_back(std::move(input_tensor));
        input_tensors.push_back(std::move(length_tensor));
        
        auto output_tensors = encoder_session_->Run(
            Ort::RunOptions{nullptr},
            input_names, 
            input_tensors.data(), 
            input_tensors.size(),
            output_names, 
            2
        );
        
        // 7. Get encoder output
        auto& encoder_out_tensor = output_tensors[0];
        auto encoder_shape = encoder_out_tensor.GetTensorTypeAndShapeInfo().GetShape();
        float* encoder_data = encoder_out_tensor.GetTensorMutableData<float>();
        
        int output_frames = encoder_shape[1];
        int output_dim = encoder_shape[2];
        
        // 8. Apply CTC decoding
        std::vector<float> logits(encoder_data, encoder_data + output_frames * output_dim);
        double confidence = 0.0;
        result.text = ctcDecode(logits, output_frames, confidence);
        result.confidence = confidence;
        
        // 9. Determine if final
        result.is_final = (result.text != previous_text_ && 
                          result.text.length() > previous_text_.length());
        
        if (result.is_final) {
            previous_text_ = result.text;
        }
        
        // 10. Keep only recent features for next chunk (sliding window)
        if (feature_buffer_.size() > static_cast<size_t>(config_.num_mel_bins * 50)) {  // Keep ~500ms
            feature_buffer_.erase(
                feature_buffer_.begin(), 
                feature_buffer_.begin() + feature_buffer_.size() - config_.num_mel_bins * 30
            );
        }
        
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime error: " << e.what() << std::endl;
        result.text = "";
        result.is_final = false;
        result.confidence = 0.0;
    } catch (const std::exception& e) {
        std::cerr << "Processing error: " << e.what() << std::endl;
        result.text = "";
        result.is_final = false;
        result.confidence = 0.0;
    }
    
    // Calculate latency
    auto end_time = std::chrono::steady_clock::now();
    result.latency_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time).count();
    
    // Update stats
    stats_.total_audio_ms += (num_samples * 1000) / config_.sample_rate;
    stats_.total_processing_ms += result.latency_ms;
    if (stats_.total_audio_ms > 0) {
        stats_.real_time_factor = static_cast<double>(stats_.total_processing_ms) / 
                                  static_cast<double>(stats_.total_audio_ms);
    }
    
    return result;
}

void WenetONNXImpl::reset() {
    feature_buffer_.clear();
    previous_text_.clear();
    hypothesis_scores_.clear();
}

bool WenetONNXImpl::loadVocabulary(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open vocabulary file: " << path << std::endl;
        return false;
    }
    
    std::string token;
    while (std::getline(file, token)) {
        vocab_.push_back(token);
    }
    
    std::cout << "Loaded vocabulary with " << vocab_.size() << " tokens" << std::endl;
    return !vocab_.empty();
}

bool WenetONNXImpl::loadCMVNStats(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open CMVN stats file: " << path << std::endl;
        return false;
    }
    
    std::string line;
    
    // Read means
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        float value;
        while (iss >> value) {
            cmvn_mean_.push_back(value);
        }
    }
    
    // Read variances
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        float value;
        while (iss >> value) {
            cmvn_var_.push_back(value);
        }
    }
    
    if (cmvn_mean_.size() != static_cast<size_t>(config_.num_mel_bins) || 
        cmvn_var_.size() != static_cast<size_t>(config_.num_mel_bins)) {
        std::cerr << "CMVN stats dimension mismatch" << std::endl;
        // Use defaults if file is invalid
        cmvn_mean_.assign(config_.num_mel_bins, 0.0f);
        cmvn_var_.assign(config_.num_mel_bins, 1.0f);
    }
    
    return true;
}

std::vector<float> WenetONNXImpl::extractFbankFeatures(
    const int16_t* samples, size_t num_samples) {
    
    // Convert int16 to float
    std::vector<float> audio(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        audio[i] = samples[i] / 32768.0f;
    }
    
    #ifdef USE_KALDI_NATIVE_FBANK
    // Use Kaldi native fbank
    knf::FbankOptions opts;
    opts.frame_opts.dither = 0.0;
    opts.frame_opts.window_type = "hamming";
    opts.frame_opts.frame_length_ms = config_.frame_length_ms;
    opts.frame_opts.frame_shift_ms = config_.frame_shift_ms;
    opts.mel_opts.num_bins = config_.num_mel_bins;
    opts.mel_opts.low_freq = 0;
    opts.mel_opts.high_freq = config_.sample_rate / 2;
    
    knf::Fbank fbank(opts);
    auto features = fbank.ComputeFeatures(audio.data(), num_samples, config_.sample_rate);
    
    // Flatten to vector
    std::vector<float> result;
    for (const auto& frame : features) {
        result.insert(result.end(), frame.begin(), frame.end());
    }
    return result;
    #else
    // Simple Fbank implementation (placeholder)
    // In production, use proper Fbank extraction
    int num_frames = (num_samples - feature_frame_length_samples_) / 
                     feature_frame_shift_samples_ + 1;
    
    std::vector<float> features(num_frames * config_.num_mel_bins, 0.0f);
    
    // Simplified energy-based features for testing
    for (int f = 0; f < num_frames; ++f) {
        int start = f * feature_frame_shift_samples_;
        
        // Compute frame energy
        float energy = 0.0f;
        for (size_t i = 0; i < feature_frame_length_samples_; ++i) {
            if (start + i < num_samples) {
                energy += audio[start + i] * audio[start + i];
            }
        }
        
        // Create mock mel-scale features
        for (int b = 0; b < config_.num_mel_bins; ++b) {
            features[f * config_.num_mel_bins + b] = 
                std::log(energy + 1e-10f) + (b - 40) * 0.1f;
        }
    }
    
    return features;
    #endif
}

void WenetONNXImpl::applyCMVN(std::vector<float>& features) {
    int num_frames = features.size() / config_.num_mel_bins;
    
    for (int f = 0; f < num_frames; ++f) {
        for (int b = 0; b < config_.num_mel_bins; ++b) {
            int idx = f * config_.num_mel_bins + b;
            features[idx] = (features[idx] - cmvn_mean_[b]) / std::sqrt(cmvn_var_[b] + 1e-10f);
        }
    }
}

std::string WenetONNXImpl::ctcDecode(const std::vector<float>& logits, 
                                     int num_frames, double& confidence) {
    // Simple greedy decoding for now
    // In production, use proper beam search
    
    std::vector<int> tokens;
    float total_score = 0.0f;
    int vocab_size = vocab_.size();
    
    for (int t = 0; t < num_frames; ++t) {
        const float* frame_logits = &logits[t * vocab_size];
        
        // Find max
        int best_idx = 0;
        float best_score = frame_logits[0];
        
        for (int v = 1; v < vocab_size; ++v) {
            if (frame_logits[v] > best_score) {
                best_score = frame_logits[v];
                best_idx = v;
            }
        }
        
        // Apply CTC rules
        if (best_idx != config_.blank_id && 
            (tokens.empty() || best_idx != tokens.back())) {
            tokens.push_back(best_idx);
            total_score += best_score;
        }
    }
    
    // Convert tokens to text
    std::string text;
    for (int token : tokens) {
        if (token >= 0 && token < vocab_size) {
            text += vocab_[token];
        }
    }
    
    // Calculate confidence
    confidence = tokens.empty() ? 0.0 : 
                 1.0 / (1.0 + std::exp(-total_score / tokens.size()));
    
    return text;
}


} // namespace wenet_streams
