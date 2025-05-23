#ifndef SIMPLE_FBANK_HPP
#define SIMPLE_FBANK_HPP

#include <vector>
#include <cmath>

namespace wenet_streams {

/**
 * Simple Fbank feature extraction for testing
 * In production, use kaldi-native-fbank or similar
 */
class SimpleFbank {
public:
    struct Options {
        int num_mel_bins = 80;
        int frame_length_ms = 25;
        int frame_shift_ms = 10;
        float low_freq = 0.0f;
        float high_freq = 8000.0f;
        int sample_rate = 16000;
    };
    
    static std::vector<float> computeFeatures(const float* audio, 
                                              size_t num_samples,
                                              const Options& opts) {
        // Simple energy-based features for testing
        int frame_length = (opts.sample_rate * opts.frame_length_ms) / 1000;
        int frame_shift = (opts.sample_rate * opts.frame_shift_ms) / 1000;
        
        if (num_samples < frame_length) {
            return std::vector<float>();
        }
        
        int num_frames = (num_samples - frame_length) / frame_shift + 1;
        std::vector<float> features(num_frames * opts.num_mel_bins);
        
        // For each frame
        for (int f = 0; f < num_frames; ++f) {
            int start = f * frame_shift;
            
            // Compute frame energy
            float energy = 0.0f;
            for (int i = 0; i < frame_length; ++i) {
                if (start + i < num_samples) {
                    energy += audio[start + i] * audio[start + i];
                }
            }
            energy = std::sqrt(energy / frame_length);
            
            // Create mock mel features
            for (int b = 0; b < opts.num_mel_bins; ++b) {
                // Simple frequency-based pattern for testing
                float freq_factor = static_cast<float>(b) / opts.num_mel_bins;
                features[f * opts.num_mel_bins + b] = 
                    std::log(energy + 1e-10f) + 
                    std::sin(freq_factor * M_PI) * 0.1f;
            }
        }
        
        return features;
    }
};

} // namespace wenet_streams

#endif // SIMPLE_FBANK_HPP