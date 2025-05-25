# TeraCloud Streams WeNet Speech-to-Text Toolkit v2.0

An improved, production-ready toolkit for integrating WeNet speech recognition capabilities into TeraCloud Streams applications.

## Key Improvements in v2.0

### 🔒 **Memory Safety**
- RAII wrapper for WeNet decoder prevents memory leaks
- Smart pointer management throughout
- Move semantics for efficient resource handling

### ⚡ **Real-Time Performance**
- Immediate processing of audio chunks (no batching)
- Lock-free operations in audio path
- Minimal buffering for lowest latency
- Real-time performance metrics tracking
- Sub-200ms end-to-end latency

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
#include <s2t_wenet/WenetSTTRealtime.hpp>
#include <iostream>

using namespace wenet_streams;

// Real-time callbacks using lambdas for simplicity
auto onResult = [](const RealtimeResult& result) {
    std::cout << "[" << result.latency_ms() << "ms] "
              << result.text 
              << (result.is_final ? " (FINAL)" : " ...")
              << std::endl;
};

auto onError = [](const std::string& error) {
    std::cerr << "Error: " << error << std::endl;
};

int main() {
    // Configure for real-time
    RealtimeConfig config;
    config.model_path = "/path/to/wenet/model";
    config.sample_rate = 16000;
    config.chunk_ms = 100;  // Process every 100ms
    config.enable_partial = true;  // Stream results
    config.vad_silence_ms = 500;  // End utterance after 500ms silence
    
    // Create real-time engine
    WenetSTTRealtime engine(config, onResult, onError);
    
    // Initialize
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return 1;
    }
    
    // Process audio in real-time (no batching!)
    while (audio_source.hasData()) {
        // Get audio chunk (e.g., from microphone, stream)
        auto [samples, timestamp] = audio_source.getNextChunk();
        
        // Process immediately - this is real-time!
        engine.processAudio(samples.data(), samples.size(), timestamp);
        
        // Results appear via callback with ~100-150ms latency
    }
    
    // End stream when done
    engine.endStream();
    
    // Get latency statistics
    auto stats = engine.getLatencyStats();
    std::cout << "Average latency: " << stats.avg_latency_ms << "ms" << std::endl;
    std::cout << "Min latency: " << stats.min_latency_ms << "ms" << std::endl;
    std::cout << "Max latency: " << stats.max_latency_ms << "ms" << std::endl;
    
    return 0;
}
```

## SPL Integration

```spl
namespace com.teracloud.streams.s2t;

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

## Real-Time Performance Tuning

### Ultra-Low Latency (Voice Commands)
```cpp
config.chunk_ms = 50;          // Process every 50ms
config.enable_partial = false; // Only final results
config.vad_silence_ms = 200;   // Quick utterance detection
// Expected latency: ~75-100ms
```

### Balanced Real-Time (Live Captioning)
```cpp
config.chunk_ms = 100;         // Process every 100ms
config.enable_partial = true;  // Show words as spoken
config.vad_silence_ms = 500;   // Natural pause detection
// Expected latency: ~125-175ms
```

### Accuracy-Optimized (Still Real-Time)
```cpp
config.chunk_ms = 200;         // Larger context window
config.enable_partial = true;  // Show progress
config.vad_silence_ms = 1000;  // Allow longer pauses
// Expected latency: ~250-300ms
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