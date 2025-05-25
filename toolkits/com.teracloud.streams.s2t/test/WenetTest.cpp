#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>

// Check if we're using the mock implementation
#ifdef MOCK_WENET_IMPLEMENTATION
#include <cstring>
#define USING_MOCK "Yes (Mock implementation)"
#else
#include <gflags/gflags.h>
#define USING_MOCK "No (Real implementation)"
DEFINE_string(model_path, "", "Path to the WeNet model directory");
DEFINE_string(test_wav, "", "Path to test WAV file (optional)");
DEFINE_bool(verbose, false, "Verbose output");
#endif

#include "WenetSTTImpl.hpp"

// Test callback for receiving transcription results
class TestCallback : public wenet_streams::TranscriptionCallback {
public:
    std::vector<wenet_streams::TranscriptionResult> results;
    bool hasError = false;
    std::string errorMessage;

    void onTranscriptionResult(const wenet_streams::TranscriptionResult& result) override {
        std::cout << (result.isFinal ? "FINAL: " : "PARTIAL: ");
        std::cout << result.text << " (confidence: " << result.confidence << ")" << std::endl;
        results.push_back(result);
    }

    void onError(const std::string& message) override {
        std::cerr << "ERROR: " << message << std::endl;
        hasError = true;
        errorMessage = message;
    }
};

// Generate a sine wave for testing
std::vector<int16_t> generateSineWave(int durationMs, int sampleRate, float frequency) {
    int numSamples = durationMs * sampleRate / 1000;
    std::vector<int16_t> samples(numSamples);
    
    for (int i = 0; i < numSamples; i++) {
        float time = static_cast<float>(i) / sampleRate;
        float value = sin(2.0f * M_PI * frequency * time);
        samples[i] = static_cast<int16_t>(value * 16383.0f); // Scale to int16_t range
    }
    
    return samples;
}

// Read a WAV file (very simple implementation - assumes PCM format)
std::vector<int16_t> readWavFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }
    
    // Read WAV header (44 bytes)
    char header[44];
    file.read(header, 44);
    
    // Skip header and read data
    std::vector<int16_t> samples;
    int16_t sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(int16_t))) {
        samples.push_back(sample);
    }
    
    return samples;
}

// Test basic initialization
bool testInitialization(const std::string& modelPath) {
    std::cout << "Testing initialization..." << std::endl;
    
    wenet_streams::WeNetConfig config;
    config.modelPath = modelPath;
    
    TestCallback callback;
    auto wenetSTT = std::make_unique<wenet_streams::WenetSTTImpl>(config, &callback);
    
    bool result = wenetSTT->initialize();
    if (!result) {
        std::cerr << "Initialization failed!" << std::endl;
        return false;
    }
    
    std::cout << "Initialization successful" << std::endl;
    return true;
}

// Test processing with generated audio
bool testProcessingWithGeneratedAudio(const std::string& modelPath) {
    std::cout << "Testing processing with generated audio..." << std::endl;
    
    wenet_streams::WeNetConfig config;
    config.modelPath = modelPath;
    
    TestCallback callback;
    auto wenetSTT = std::make_unique<wenet_streams::WenetSTTImpl>(config, &callback);
    
    if (!wenetSTT->initialize()) {
        std::cerr << "Initialization failed!" << std::endl;
        return false;
    }
    
    // Generate a 3-second sine wave at 440 Hz
    auto samples = generateSineWave(3000, 16000, 440.0f);
    
    // Process the audio
    wenet_streams::AudioChunk chunk(samples, 0);
    wenetSTT->processAudioChunk(chunk);
    
    // Flush to get final result
    wenetSTT->flush();
    
    // Wait for processing
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Check if we have any results or errors
    if (callback.hasError) {
        std::cerr << "Error during processing: " << callback.errorMessage << std::endl;
        return false;
    }
    
    std::cout << "Processing completed with " << callback.results.size() << " results" << std::endl;
    return true;
}

// Test processing with real audio if provided
bool testProcessingWithRealAudio(const std::string& modelPath, const std::string& wavFile) {
    if (wavFile.empty()) {
        std::cout << "No test WAV file provided, skipping real audio test" << std::endl;
        return true;
    }
    
    std::cout << "Testing processing with real audio from " << wavFile << "..." << std::endl;
    
    wenet_streams::WeNetConfig config;
    config.modelPath = modelPath;
    
    TestCallback callback;
    auto wenetSTT = std::make_unique<wenet_streams::WenetSTTImpl>(config, &callback);
    
    if (!wenetSTT->initialize()) {
        std::cerr << "Initialization failed!" << std::endl;
        return false;
    }
    
    // Read WAV file
    auto samples = readWavFile(wavFile);
    if (samples.empty()) {
        std::cerr << "Failed to read WAV file or file is empty" << std::endl;
        return false;
    }
    
    std::cout << "Read " << samples.size() << " samples from WAV file" << std::endl;
    
    // Process the audio in chunks to simulate streaming
    const int chunkSize = 16000; // 1 second at 16kHz
    for (size_t i = 0; i < samples.size(); i += chunkSize) {
        size_t remaining = std::min(chunkSize, static_cast<int>(samples.size() - i));
        std::vector<int16_t> chunkSamples(samples.begin() + i, samples.begin() + i + remaining);
        
        wenet_streams::AudioChunk chunk(chunkSamples, i / 16);
        wenetSTT->processAudioChunk(chunk);
        
        // Small delay to simulate real-time processing
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Flush to get final result
    wenetSTT->flush();
    
    // Wait for processing
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Check if we have any results or errors
    if (callback.hasError) {
        std::cerr << "Error during processing: " << callback.errorMessage << std::endl;
        return false;
    }
    
    std::cout << "Processing completed with " << callback.results.size() << " results" << std::endl;
    
    // Print final transcription if available
    if (!callback.results.empty()) {
        for (const auto& result : callback.results) {
            if (result.isFinal) {
                std::cout << "Final transcription: " << result.text << std::endl;
                return true;
            }
        }
    }
    
    return true;
}

// Test direct API calls to WeNet
bool testDirectApiCalls(const std::string& modelPath) {
    std::cout << "Testing direct WeNet API calls..." << std::endl;
    
    // Initialize WeNet decoder
    void* decoder = wenet_init(modelPath.c_str());
    if (decoder == nullptr) {
        std::cerr << "Failed to initialize WeNet decoder" << std::endl;
        return false;
    }
    
    // Test API functions
#ifndef MOCK_WENET_IMPLEMENTATION
    wenet_set_log_level(0);
#endif
    wenet_set_nbest(decoder, 1);
    wenet_set_timestamp(decoder, 1);
    
    // Generate test audio
    auto samples = generateSineWave(1000, 16000, 440.0f);
    const char* audio_data = reinterpret_cast<const char*>(samples.data());
    int audio_len = samples.size() * sizeof(int16_t);
    
    // Test decoding
    const char* result = wenet_decode(decoder, audio_data, audio_len, 1);
    
    if (result == nullptr) {
        std::cerr << "Decode returned nullptr" << std::endl;
        wenet_free(decoder);
        return false;
    }
    
    std::cout << "WeNet result: " << result << std::endl;
    
    // Free decoder
    wenet_free(decoder);
    
    std::cout << "Direct API test completed successfully" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    std::string modelPath = "/tmp/wenet_model";
    std::string wavFile = "";
    bool verbose = false;
    
#ifndef MOCK_WENET_IMPLEMENTATION
    // Parse command line arguments with gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    modelPath = FLAGS_model_path;
    wavFile = FLAGS_test_wav;
    verbose = FLAGS_verbose;
#else
    // Simple argument parsing for mock implementation
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--model_path" && i + 1 < argc) {
            modelPath = argv[i+1];
            i++;
        } else if (std::string(argv[i]) == "--test_wav" && i + 1 < argc) {
            wavFile = argv[i+1];
            i++;
        } else if (std::string(argv[i]) == "--verbose") {
            verbose = true;
        }
    }
#endif
    
    std::cout << "Running WeNet STT toolkit tests..." << std::endl;
    std::cout << "Using model: " << modelPath << std::endl;
    std::cout << "Using mock implementation: " << USING_MOCK << std::endl;
    
    bool success = true;
    
    // Run tests
    success &= testInitialization(modelPath);
    success &= testDirectApiCalls(modelPath);
    success &= testProcessingWithGeneratedAudio(modelPath);
    success &= testProcessingWithRealAudio(modelPath, wavFile);
    
    if (success) {
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
    } else {
        std::cerr << "Some tests failed." << std::endl;
        return 1;
    }
}