#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include "impl/include/ZipformerRNNT.hpp"
#include "simple_fbank.hpp"

// Simple WAV reader
bool readWav(const std::string& filename, std::vector<float>& samples, int& sample_rate) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;
    
    // Skip header
    file.seekg(44);
    
    // Read samples
    std::vector<int16_t> raw_samples;
    int16_t sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        raw_samples.push_back(sample);
    }
    
    // Convert to float
    samples.resize(raw_samples.size());
    for (size_t i = 0; i < raw_samples.size(); ++i) {
        samples[i] = raw_samples[i] / 32768.0f;
    }
    
    sample_rate = 16000;  // Assume 16kHz
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
        config.beam_size = 4;
        config.chunk_size = 39;  // 390ms chunks
        
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
        
        // Process in streaming chunks
        std::cout << "\nStreaming transcription:" << std::endl;
        std::cout << "------------------------" << std::endl;
        
        int chunk_frames = config.chunk_size;
        int total_chunks = (all_features.size() + chunk_frames - 1) / chunk_frames;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
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
            
            // Show progress
            float progress = (chunk + 1) * 100.0f / total_chunks;
            float audio_time = (end_frame * 10.0f) / 1000.0f;  // 10ms per frame
            
            std::cout << "\r[" << std::fixed << std::setprecision(1) << progress << "%] "
                      << std::setprecision(2) << audio_time << "s: "
                      << result.text << std::flush;
            
            // Simulate real-time processing
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Finalize
        auto final_result = model.finalize();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\n\n------------------------" << std::endl;
        std::cout << "Final transcription: " << final_result.text << std::endl;
        std::cout << "Confidence: " << final_result.confidence << std::endl;
        std::cout << "Processing time: " << duration.count() << "ms" << std::endl;
        std::cout << "Audio duration: " << (samples.size() * 1000 / sample_rate) << "ms" << std::endl;
        std::cout << "Real-time factor: " << (duration.count() / (samples.size() * 1000.0 / sample_rate)) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}