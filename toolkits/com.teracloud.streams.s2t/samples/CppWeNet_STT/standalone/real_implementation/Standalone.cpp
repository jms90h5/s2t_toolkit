#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <string>

#include "WenetSTTImpl.hpp"

using namespace wenet_streams;

// A simple callback implementation for displaying transcription results
class SimpleCallback : public TranscriptionCallback {
public:
    void onTranscriptionResult(const TranscriptionResult& result) override {
        std::cout << "==========================================================\n";
        std::cout << (result.isFinal ? "FINAL: " : "PARTIAL: ");
        std::cout << result.text << "\n";
        std::cout << "Confidence: " << result.confidence << ", Time: " << result.timestamp << "\n";
        std::cout << "==========================================================\n";
    }

    void onError(const std::string& errorMessage) override {
        std::cerr << "ERROR: " << errorMessage << std::endl;
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

int main(int argc, char* argv[]) {
    std::string modelPath = "./models";
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--model_path" && i + 1 < argc) {
            modelPath = argv[i + 1];
            i++;
        }
    }
    
    std::cout << "Standalone WeNet STT Test\n";
    std::cout << "Using model path: " << modelPath << "\n\n";
    
    // Create configuration
    WeNetConfig config;
    config.modelPath = modelPath;
    config.sampleRate = 16000;
    config.partialResultsEnabled = true;
    config.maxLatency = 0.3;
    config.vadEnabled = true;
    
    // Create callback
    SimpleCallback callback;
    
    // Create WeNet STT implementation
    auto wenetSTT = std::make_unique<WenetSTTImpl>(config, &callback);
    
    // Initialize
    std::cout << "Initializing WeNet STT...\n";
    if (!wenetSTT->initialize()) {
        std::cerr << "Failed to initialize WeNet STT\n";
        return 1;
    }
    
    std::cout << "Processing audio...\n";
    
    // Generate test audio (multiple chunks to simulate streaming)
    const int chunkDurationMs = 500; // 500ms chunks
    const int numChunks = 10;        // 5 seconds total
    
    for (int i = 0; i < numChunks; i++) {
        // Generate sine wave with varying frequency
        float frequency = 440.0f + (i * 50.0f);
        auto audioChunk = generateSineWave(chunkDurationMs, config.sampleRate, frequency);
        
        // Get current time as timestamp
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Process the chunk
        std::cout << "Processing chunk " << (i+1) << "/" << numChunks 
                  << " (frequency: " << frequency << " Hz)\n";
                  
        wenetSTT->processAudioChunk(AudioChunk(audioChunk, timestamp));
        
        // Sleep to simulate real-time processing
        std::this_thread::sleep_for(std::chrono::milliseconds(chunkDurationMs / 2));
    }
    
    // Flush to get final results
    std::cout << "Flushing...\n";
    wenetSTT->flush();
    
    // Wait for processing to complete
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "Done!\n";
    return 0;
}