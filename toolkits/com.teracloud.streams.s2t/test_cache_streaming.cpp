#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <chrono>
#include "ZipformerRNNT.hpp"
#include "simple_fbank.hpp"

// Simple WAV reader
bool readWav(const std::string& filename, std::vector<float>& samples, int& sample_rate) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    
    // Skip WAV header (44 bytes)
    file.seekg(44);
    
    // Read raw samples
    std::vector<int16_t> raw_samples;
    int16_t sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        raw_samples.push_back(sample);
    }
    
    // Convert to float
    samples.clear();
    samples.reserve(raw_samples.size());
    for (int16_t s : raw_samples) {
        samples.push_back(s / 32768.0f);
    }
    
    sample_rate = 16000; // Assume 16kHz
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <audio.wav>" << std::endl;
        return 1;
    }
    
    try {
        // Load audio
        std::vector<float> samples;
        int sample_rate;
        if (!readWav(argv[1], samples, sample_rate)) {
            std::cerr << "Failed to read WAV file" << std::endl;
            return 1;
        }
        std::cout << "Loaded " << samples.size() << " samples at " << sample_rate << "Hz" << std::endl;
        
        // Configure model
        onnx_stt::ZipformerRNNT::Config config;
        config.encoder_path = "models/sherpa_onnx_paraformer/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/encoder-epoch-99-avg-1.onnx";
        config.decoder_path = "models/sherpa_onnx_paraformer/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/decoder-epoch-99-avg-1.onnx";
        config.joiner_path = "models/sherpa_onnx_paraformer/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/joiner-epoch-99-avg-1.onnx";
        config.tokens_path = "models/sherpa_onnx_paraformer/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/tokens.txt";
        config.chunk_size = 39;  // Model expects 39 frames per chunk
        config.beam_size = 4;
        
        // Initialize model
        onnx_stt::ZipformerRNNT model(config);
        if (!model.initialize()) {
            std::cerr << "Failed to initialize model" << std::endl;
            return 1;
        }
        
        // Initialize feature extractor
        simple_fbank::FbankComputer::Options fbank_opts;
        fbank_opts.sample_rate = sample_rate;
        fbank_opts.num_mel_bins = 80;
        simple_fbank::FbankComputer fbank(fbank_opts);
        
        // Extract all features
        auto all_features = fbank.computeFeatures(samples);
        std::cout << "\nExtracted " << all_features.size() << " feature frames" << std::endl;
        
        // Test 1: Process full audio in one chunk vs streaming
        std::cout << "\n=== Test 1: Full vs Streaming Comparison ===" << std::endl;
        
        // Reset model for full processing
        model.reset();
        std::vector<float> full_features;
        for (const auto& frame : all_features) {
            full_features.insert(full_features.end(), frame.begin(), frame.end());
        }
        
        // Pad to multiple of chunk_size
        while (full_features.size() % (config.chunk_size * 80) != 0) {
            full_features.push_back(0.0f);
        }
        
        auto full_result = model.processChunk(full_features);
        std::cout << "Full processing result: " << full_result.text << std::endl;
        
        // Reset model for streaming
        model.reset();
        
        // Process in small chunks
        std::cout << "\nStreaming with " << config.chunk_size << " frame chunks:" << std::endl;
        int chunk_frames = config.chunk_size;
        int total_chunks = (all_features.size() + chunk_frames - 1) / chunk_frames;
        
        std::string streaming_text;
        for (int chunk = 0; chunk < total_chunks; ++chunk) {
            // Prepare chunk features
            int start_frame = chunk * chunk_frames;
            int end_frame = std::min(start_frame + chunk_frames, (int)all_features.size());
            
            std::vector<float> chunk_features;
            for (int i = start_frame; i < end_frame; ++i) {
                chunk_features.insert(chunk_features.end(),
                                    all_features[i].begin(),
                                    all_features[i].end());
            }
            
            // Pad if necessary
            while (chunk_features.size() < chunk_frames * 80) {
                chunk_features.push_back(0.0f);
            }
            
            // Process chunk
            auto result = model.processChunk(chunk_features);
            streaming_text = result.text;
            std::cout << "  Chunk " << chunk+1 << "/" << total_chunks << ": " << result.text << std::endl;
        }
        
        std::cout << "\nFinal streaming result: " << streaming_text << std::endl;
        
        // Test 2: Process same chunk multiple times
        std::cout << "\n=== Test 2: Cache Consistency Check ===" << std::endl;
        model.reset();
        
        // Create a test chunk
        std::vector<float> test_chunk;
        for (int i = 0; i < chunk_frames && i < all_features.size(); ++i) {
            test_chunk.insert(test_chunk.end(),
                            all_features[i].begin(),
                            all_features[i].end());
        }
        while (test_chunk.size() < chunk_frames * 80) {
            test_chunk.push_back(0.0f);
        }
        
        // Process same chunk 3 times and check if results differ (showing cache is updated)
        std::cout << "Processing same audio chunk 3 times:" << std::endl;
        for (int i = 0; i < 3; ++i) {
            auto result = model.processChunk(test_chunk);
            std::cout << "  Iteration " << i+1 << ": " << result.text << std::endl;
        }
        
        std::cout << "\nCache streaming test completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}