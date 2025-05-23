#include "impl/include/WenetONNXImpl.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace wenet_streams;

// Generate test audio (sine wave)
std::vector<int16_t> generateTestAudio(int duration_ms, int sample_rate = 16000) {
    int num_samples = (sample_rate * duration_ms) / 1000;
    std::vector<int16_t> audio(num_samples);
    
    // 440 Hz sine wave
    for (int i = 0; i < num_samples; ++i) {
        float t = static_cast<float>(i) / sample_rate;
        float value = 0.3f * std::sin(2.0f * M_PI * 440.0f * t);
        audio[i] = static_cast<int16_t>(value * 32767.0f);
    }
    
    return audio;
}

// Create mock ONNX model file for testing
void createMockModel(const std::string& path) {
    // In real usage, this would be an actual ONNX model
    std::ofstream file(path, std::ios::binary);
    // Write ONNX magic number
    const char magic[] = {0x08, 0x01, 0x12, 0x00};
    file.write(magic, 4);
    file.close();
}

// Create mock vocab file
void createMockVocab(const std::string& path) {
    std::ofstream file(path);
    file << "<blank>\n<unk>\n";
    for (char c = 'a'; c <= 'z'; ++c) {
        file << c << "\n";
    }
    file << "<space>\n";
    file.close();
}

// Create mock CMVN stats
void createMockCMVN(const std::string& path) {
    std::ofstream file(path);
    // Write means
    for (int i = 0; i < 80; ++i) {
        file << "0.0 ";
    }
    file << "\n";
    // Write variances
    for (int i = 0; i < 80; ++i) {
        file << "1.0 ";
    }
    file << "\n";
    file.close();
}

int main() {
    std::cout << "=== WeNet ONNX Implementation Test ===" << std::endl;
    
    // Create mock files
    std::cout << "Creating mock model files..." << std::endl;
    createMockModel("test_model.onnx");
    createMockVocab("test_vocab.txt");
    createMockCMVN("test_cmvn.stats");
    
    // Configure
    WenetONNXImpl::Config config;
    config.encoder_onnx_path = "test_model.onnx";
    config.vocab_path = "test_vocab.txt";
    config.cmvn_stats_path = "test_cmvn.stats";
    config.sample_rate = 16000;
    config.chunk_size_ms = 100;
    config.num_threads = 2;
    config.use_gpu = false;
    
    // Create implementation
    std::cout << "\nCreating WenetONNXImpl..." << std::endl;
    WenetONNXImpl stt(config);
    
    // Initialize (will fail with mock model, but tests the flow)
    std::cout << "Initializing (expecting to fail with mock model)..." << std::endl;
    bool init_success = stt.initialize();
    
    if (!init_success) {
        std::cout << "Initialization failed as expected with mock model." << std::endl;
        std::cout << "In production, use a real ONNX model exported from WeNet." << std::endl;
    }
    
    // Test audio processing flow (even without model)
    std::cout << "\nTesting audio processing flow..." << std::endl;
    auto test_audio = generateTestAudio(100); // 100ms of audio
    
    std::cout << "Generated " << test_audio.size() << " samples of test audio" << std::endl;
    
    // Try processing (will return empty result without real model)
    uint64_t timestamp = 0;
    auto result = stt.processAudioChunk(test_audio.data(), test_audio.size(), timestamp);
    
    std::cout << "\nProcessing result:" << std::endl;
    std::cout << "  Text: " << (result.text.empty() ? "(empty)" : result.text) << std::endl;
    std::cout << "  Is Final: " << (result.is_final ? "true" : "false") << std::endl;
    std::cout << "  Confidence: " << result.confidence << std::endl;
    std::cout << "  Latency: " << result.latency_ms << "ms" << std::endl;
    
    // Get performance stats
    auto stats = stt.getStats();
    std::cout << "\nPerformance stats:" << std::endl;
    std::cout << "  Total audio: " << stats.total_audio_ms << "ms" << std::endl;
    std::cout << "  Total processing: " << stats.total_processing_ms << "ms" << std::endl;
    std::cout << "  Real-time factor: " << stats.real_time_factor << std::endl;
    
    // Clean up test files
    std::remove("test_model.onnx");
    std::remove("test_vocab.txt");
    std::remove("test_cmvn.stats");
    
    std::cout << "\n✓ Test completed successfully!" << std::endl;
    std::cout << "\nNote: To use real speech recognition, you need:" << std::endl;
    std::cout << "1. Export a WeNet model to ONNX format" << std::endl;
    std::cout << "2. Provide real vocabulary and CMVN stats files" << std::endl;
    
    return 0;
}