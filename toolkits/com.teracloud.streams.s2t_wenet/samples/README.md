# WeNet Speech-to-Text Toolkit Samples

This directory contains sample applications demonstrating different approaches to implementing real-time speech-to-text using the WeNet toolkit with TeraCloud Streams.

## Sample Applications

### Python-based Implementations

#### PythonONNX_MultiOperator
- **Technology**: Python with ONNX Runtime (multi-operator approach)
- **Operator**: Custom Python operators
- **Use Case**: Research and prototyping where component flexibility is needed
- **Latency**: ~200-300ms
- **Key Features**: Separate operators for feature extraction, encoding, and decoding

#### PythonONNX_SingleOperator
- **Technology**: Python with embedded ONNX Runtime (single operator)
- **Operator**: Custom Python operator with all processing embedded
- **Use Case**: Production scenarios where Python is acceptable
- **Latency**: ~150-200ms
- **Key Features**: Better performance through reduced inter-operator communication

### C++-based Implementations

#### CppONNX_WenetONNX
- **Technology**: Pure C++ with ONNX Runtime
- **Operator**: `WenetONNX` (toolkit primitive operator)
- **Use Case**: Production real-time systems requiring low latency
- **Latency**: ~100-150ms
- **Key Features**:
  - Uses kaldi-native-fbank for feature extraction
  - Supports CPU/CUDA/TensorRT providers
  - Self-contained deployment (no WeNet runtime needed)

#### CppWeNet_WenetSTT
- **Technology**: C++ with full WeNet runtime
- **Operator**: `WenetSTT` (toolkit primitive operator)
- **Use Case**: Applications requiring full WeNet features
- **Latency**: ~100-150ms
- **Key Features**:
  - Complete WeNet implementation
  - Advanced features like VAD
  - WebSocket I/O for streaming

## Choosing the Right Implementation

### Use Python implementations when:
- Prototyping new features
- Integration with Python ML ecosystem is needed
- Latency requirements are moderate (150-300ms)
- Development speed is prioritized

### Use CppONNX_WenetONNX when:
- Low latency is critical (<150ms)
- Deployment simplicity is important
- Cross-platform compatibility is needed
- GPU acceleration is required

### Use CppWeNet_WenetSTT when:
- Full WeNet features are needed
- Already have WeNet infrastructure
- Need advanced features like built-in VAD
- Willing to manage WeNet dependencies

## Documentation

See the `docs/` directory for design guides and best practices:
- **RealtimeDesignGuide**: Explains why batching is inappropriate for real-time STT

## Building and Running Samples

Each sample directory contains:
- `README.md`: Specific instructions for that sample
- `Makefile`: Build configuration
- `*.spl`: Streams application source

General build pattern:
```bash
cd <sample_directory>
make
```

Run pattern:
```bash
streamtool submitjob output/<app_name>.sab
```

Refer to individual sample READMEs for specific parameters and configuration.
