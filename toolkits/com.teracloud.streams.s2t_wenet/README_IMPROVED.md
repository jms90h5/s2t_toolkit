# TeraCloud Streams WeNet Speech-to-Text Toolkit v2.0

An improved, production-ready toolkit for integrating WeNet speech recognition capabilities into TeraCloud Streams applications.

## Key Improvements in v2.0

### 🔒 **Memory Safety**
- RAII wrapper for WeNet decoder prevents memory leaks
- Smart pointer management throughout
- Move semantics for efficient resource handling

### ⚡ **Performance Optimizations**
- Lock-free circular buffer for audio queue
- Batch processing of audio chunks
- Configurable buffer sizes and processing parameters
- Real-time performance metrics tracking

### 🛡️ **Robust Error Handling**
- Comprehensive exception handling
- Graceful degradation on errors
- Detailed error reporting via callbacks

### 🧪 **Testing & Quality**
- Comprehensive unit test suite
- Thread safety tests
- Performance benchmarks
- Memory leak detection

### 🔧 **Modern Build System**
- CMake 3.14+ with modern practices
- Automatic dependency management
- Support for sanitizers and static analysis
- Package configuration files for easy integration

### 📊 **Enhanced Features**
- N-best results support
- Word-level timestamps
- Performance metrics API
- JSON parsing with RapidJSON
- Configurable VAD parameters

## Building the Toolkit

### Prerequisites
- CMake 3.14 or higher
- C++17 compatible compiler
- (Optional) PyTorch/LibTorch
- (Optional) WeNet library

### Quick Build
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
make test
sudo make install
```

### Build Options
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DENABLE_SANITIZERS=OFF \
  -DMOCK_IMPLEMENTATION=OFF \
  -DBUILD_DOCUMENTATION=ON
```

## Usage Example

```cpp
#include <s2t_wenet/WenetSTTImplImproved.hpp>
#include <iostream>

using namespace wenet_streams;

class MyCallback : public TranscriptionCallback {
public:
    void onTranscriptionResult(const TranscriptionResult& result) override {
        std::cout << "Transcription: " << result.text 
                  << " (confidence: " << result.confidence << ")" << std::endl;
        
        // Print alternatives if available
        for (const auto& [text, conf] : result.alternatives) {
            std::cout << "  Alternative: " << text << " (" << conf << ")" << std::endl;
        }
    }
    
    void onError(const std::string& error) override {
        std::cerr << "Error: " << error << std::endl;
    }
};

int main() {
    // Configure the engine
    WeNetConfig config;
    config.modelPath = "/path/to/wenet/model";
    config.sampleRate = 16000;
    config.nBest = 3;  // Get top 3 alternatives
    config.timestampEnabled = true;
    config.bufferCapacity = 200;  // Larger buffer for heavy load
    
    // Create callback and engine
    MyCallback callback;
    WenetSTTImplImproved engine(config, &callback);
    
    // Initialize
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return 1;
    }
    
    // Process audio (example with simulated audio)
    for (int i = 0; i < 100; ++i) {
        std::vector<int16_t> audioData(1600);  // 0.1 seconds at 16kHz
        // ... fill audioData with actual audio samples ...
        
        AudioChunk chunk(std::move(audioData), i * 100);
        engine.processAudioChunk(std::move(chunk));
    }
    
    // Flush remaining audio
    engine.flush();
    
    // Get performance metrics
    auto metrics = engine.getMetrics();
    std::cout << "Processed " << metrics.totalSamplesProcessed << " samples" << std::endl;
    std::cout << "Average latency: " << metrics.averageLatencyMs << " ms" << std::endl;
    
    return 0;
}
```

## SPL Integration

```spl
namespace com.teracloud.streams.s2t_wenet;

composite ImprovedTranscriber {
    param
        expression<rstring> $modelPath;
        expression<float64> $maxLatency : 0.3;
        expression<int32> $nBest : 1;
        
    graph
        stream<rstring text, float64 confidence, list<rstring> alternatives> 
            Transcription = WenetSpeechToText(AudioStream) {
            param
                modelPath: $modelPath;
                partialResultsEnabled: true;
                maxLatency: $maxLatency;
                nBest: $nBest;
                timestampEnabled: true;
                bufferCapacity: 200;
        }
}
```

## Performance Tuning

### Low Latency Configuration
```cpp
config.maxLatency = 0.1;  // 100ms max latency
config.maxChunkDuration = 0.5;  // Process every 0.5 seconds
config.bufferCapacity = 50;  // Smaller buffer
config.chunkSize = 8;  // Smaller batches
```

### High Throughput Configuration
```cpp
config.maxLatency = 1.0;  // Allow more latency
config.maxChunkDuration = 3.0;  // Larger chunks
config.bufferCapacity = 500;  // Large buffer
config.chunkSize = 32;  // Larger batches
```

### Memory Constrained Environments
```cpp
config.bufferCapacity = 20;  // Minimal buffer
config.nBest = 1;  // Only best result
config.timestampEnabled = false;  // Disable timestamps
```

## Monitoring & Observability

The toolkit provides real-time metrics:

```cpp
auto metrics = engine.getMetrics();
// Available metrics:
// - totalSamplesProcessed: Total audio samples processed
// - totalProcessingTimeMs: Total processing time in milliseconds
// - queueDepth: Current audio queue depth
// - errorCount: Number of errors encountered
// - averageLatencyMs: Average processing latency
```

## Troubleshooting

### Common Issues

1. **High Memory Usage**
   - Reduce `bufferCapacity`
   - Disable features like timestamps or N-best
   - Check for memory leaks with sanitizers

2. **Poor Recognition Accuracy**
   - Verify model compatibility
   - Check audio format (must be 16kHz, 16-bit PCM)
   - Adjust VAD thresholds

3. **High Latency**
   - Reduce `maxChunkDuration`
   - Decrease batch size
   - Use release build with optimizations

### Debug Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
make
./wenet_tests
```

## Architecture

```
┌─────────────────────────┐
│   SPL Application       │
├─────────────────────────┤
│  WenetSpeechToText      │ (High-level SPL wrapper)
├─────────────────────────┤
│     WenetSTT            │ (C++ Primitive Operator)
├─────────────────────────┤
│ WenetSTTImplImproved    │ (Improved C++ Implementation)
├─────────────────────────┤
│  CircularBuffer         │ (Lock-free queue)
│  WenetDecoderWrapper    │ (RAII wrapper)
│  PerformanceMetrics     │ (Monitoring)
├─────────────────────────┤
│    WeNet C++ API        │ (External dependency)
└─────────────────────────┘
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Run tests (`make test`)
4. Commit changes (`git commit -m 'Add amazing feature'`)
5. Push to branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

## License

This toolkit is part of the TeraCloud Streams project and follows its licensing terms.

## Acknowledgments

- WeNet team for the excellent speech recognition engine
- TeraCloud Streams community for feedback and contributions