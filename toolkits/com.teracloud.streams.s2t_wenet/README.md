# TeraCloud Streams Speech-to-Text Toolkit

This toolkit provides high-performance speech-to-text capabilities for TeraCloud Streams applications. It offers multiple implementation approaches including [WeNet](https://github.com/wenet-e2e/wenet) integration and ONNX Runtime-based solutions, enabling real-time speech recognition with various deployment options.

## Overview

The `com.teracloud.streams.s2t_toolkit` toolkit enables:

- Real-time speech-to-text processing with low latency
- Support for streaming audio input formats
- Configurable recognition parameters for optimal real-time performance
- State-of-the-art speech recognition accuracy
- Fully streaming operation with incremental results

This toolkit is designed for scenarios requiring low-latency speech-to-text capabilities within Streams applications, including:

- Call center real-time analytics
- Live media transcription
- Voice-controlled systems
- Real-time captioning
- Live meeting transcription

## Key Components

- **WenetSTT**: Primary operator for real-time speech recognition using WeNet C++ API
- **WenetONNX**: New operator for real-time speech recognition using ONNX Runtime (recommended)
- **AudioStreamSource**: Operator for ingesting streaming audio
- **WebSocketSink**: Operator for streaming transcription results
- **WenetRealtimeSTT**: Composite operator demonstrating real-time STT pipeline

## Prerequisites

- TeraCloud Streams 7.x
- C++17 compatible compiler
- For WenetSTT operator:
  - WeNet runtime library dependencies
  - CMake 3.14 or higher for building the toolkit
  - LibTorch (PyTorch C++ libraries) for WeNet
- For WenetONNX operator (recommended):
  - ONNX Runtime 1.16.3 or higher
  - WeNet model exported to ONNX format

## Installing WeNet Dependencies

### Option 1: Automatic Installation

The toolkit includes a script to automatically download and install WeNet dependencies:

```bash
# Install WeNet dependencies
./install_wenet_deps.sh --install-deps

# Optionally download a pre-trained model (recommended for testing)
./install_wenet_deps.sh --download-model --model-lang en
```

This script will:
1. Clone the WeNet repository
2. Copy necessary header files to the toolkit's include directory
3. Set up the build environment
4. Optionally download a pre-trained model for English or Chinese

### Option 2: Manual Installation

If you already have WeNet installed or prefer to manage dependencies yourself:

```bash
# 1. Clone WeNet repository
git clone https://github.com/wenet-e2e/wenet.git /path/to/wenet

# 2. Install LibTorch
# For CPU-only version:
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.2.2%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-2.2.2+cpu.zip -d /path/to/libtorch

# 3. Set environment variables when building
export WENET_ROOT=/path/to/wenet
export Torch_DIR=/path/to/libtorch/share/cmake/Torch
```

### Option 3: Using Existing System Installation

If WeNet is already installed on your system, you can point the toolkit to it when building:

```bash
# Build using existing WeNet installation
./build.sh --clean --wenet-path /path/to/wenet --torch-path /path/to/libtorch
```

### Required Dependencies

WeNet requires the following dependencies:

- **CMake**: Version 3.14 or higher
- **LibTorch**: PyTorch C++ libraries (version 2.0 or higher recommended)
- **CUDA** (optional): For GPU acceleration
- **glog**: Google logging library
- **gflags**: Command line flags library
- **RapidJSON**: For parsing transcription results

## ONNX Runtime Support (Recommended)

The toolkit now includes a WenetONNX operator that uses ONNX Runtime for inference, eliminating the dependency on WeNet C++ API and LibTorch. This approach offers several advantages:

- **Simplified deployment**: No need for WeNet or PyTorch dependencies
- **Better performance**: ONNX Runtime optimizations for CPU and GPU
- **Cross-platform compatibility**: Works on more platforms
- **Smaller footprint**: Reduced memory and disk usage
- **Production-quality feature extraction**: Uses kaldi-native-fbank for acoustic features

### Installing Dependencies

The toolkit requires two main dependencies for the ONNX operator:

#### 1. ONNX Runtime

```bash
# Install ONNX Runtime (downloads and configures automatically)
./setup_onnx_runtime.sh
```

This will download ONNX Runtime 1.16.3 and configure it in the `deps/onnxruntime` directory.

#### 2. Kaldi Native Fbank (for feature extraction)

```bash
# Install kaldi-native-fbank for production-quality feature extraction
./setup_kaldi_fbank.sh

# Or build from source if you need custom options
./setup_kaldi_fbank.sh --build-from-source
```

This installs kaldi-native-fbank in the `deps/kaldi-native-fbank` directory. The library provides:
- Industry-standard Mel-filterbank feature extraction
- Compatible with WeNet, Kaldi, and other ASR systems
- Optimized C++ implementation
- Streaming support for real-time processing

### Exporting WeNet Models to ONNX

To use the WenetONNX operator, you need to export your WeNet model to ONNX format:

```python
# Example script to export WeNet model to ONNX
import torch
import wenet

# Load your WeNet model
model = wenet.load_model('path/to/wenet/model')

# Export encoder to ONNX
torch.onnx.export(
    model.encoder,
    (example_input, example_input_lengths),
    "encoder.onnx",
    input_names=['speech', 'speech_lengths'],
    output_names=['output'],
    dynamic_axes={
        'speech': {0: 'batch', 1: 'time'},
        'speech_lengths': {0: 'batch'},
        'output': {0: 'batch', 1: 'time'}
    }
)
```

### Using the WenetONNX Operator

```spl
stream<rstring text, boolean isFinal, float64 confidence> Transcription = 
    WenetONNX(AudioStream) {
        param
            encoderModel: "models/encoder.onnx";
            vocabFile: "models/units.txt";
            cmvnFile: "models/global_cmvn";
            sampleRate: 16000;
            provider: "CPU";  // or "CUDA" for GPU
    }
```

## Building the Toolkit

### For WenetONNX Operator (Recommended)

```bash
# 1. Install dependencies
./setup_onnx_runtime.sh
./setup_kaldi_fbank.sh

# 2. Build the toolkit
./build.sh --clean --onnx

# 3. Build and install to STREAMS_INSTALL directory
./build.sh --clean --onnx --install
```

### For WenetSTT Operator (Legacy)

Once WeNet dependencies are installed, you can build the toolkit:

```bash
# Build with default options (using auto-detected dependencies)
./build.sh --clean

# Build with explicitly specified paths
./build.sh --clean --wenet-path /path/to/wenet --torch-path /path/to/libtorch

# Build and install to STREAMS_INSTALL directory
./build.sh --clean --install
```

### Build Options

The build script supports several options:

- `--clean`: Clean build artifacts before building
- `--onnx`: Build only the ONNX operator (recommended)
- `--wenet-path PATH`: Path to WeNet installation (for WenetSTT operator)
- `--torch-path PATH`: Path to LibTorch installation (for WenetSTT operator)
- `--test`: Run tests after building (requires a model)
- `--wenet-model PATH`: Path to WeNet model for testing
- `--install`: Install the toolkit to STREAMS_INSTALL/toolkits
- `--install-deps`: Install WeNet dependencies (for WenetSTT operator)

## QuickStart Guide

This toolkit follows a **build once, deploy anywhere** approach. After the initial setup, all dependencies are bundled with your Streams applications automatically.

### 1. Install Dependencies and Build the Toolkit

#### For WenetONNX (Recommended)

```bash
# Step 1: Download and install dependencies (one-time setup)
./setup_onnx_runtime.sh      # Downloads ONNX Runtime
./setup_kaldi_fbank.sh        # Builds feature extraction library

# Step 2: Build the toolkit (packages all dependencies)
./build.sh --clean --onnx

# That's it! The toolkit now contains all necessary libraries in impl/lib/
```

#### For WenetSTT (Legacy)

```bash
# Install dependencies and download a model
./install_wenet_deps.sh --install-deps --download-model

# Build the toolkit
./build.sh --clean
```

### 2. Create a Real-time Transcription Application

#### Using WenetONNX (Recommended)

```spl
composite RealtimeTranscriber {
    param
        expression<rstring> $encoderModel: getSubmissionTimeValue("encoderModel");
        expression<rstring> $vocabFile: getSubmissionTimeValue("vocabFile");
        expression<rstring> $audioEndpoint: getSubmissionTimeValue("audioEndpoint");
    
    graph
        stream<blob audio, uint64 audioTimestamp> AudioStream = AudioStreamSource() {
            param
                endpoint: $audioEndpoint;
                format: "pcm";
                sampleRate: 16000;
                channelCount: 1;
        }
        
        stream<rstring text, boolean isFinal, float64 confidence> Transcription = 
            WenetONNX(AudioStream) {
                param
                    encoderModel: $encoderModel;
                    vocabFile: $vocabFile;
                    cmvnFile: getThisToolkitDir() + "/etc/global_cmvn";
                    sampleRate: 16000;
                    chunkSizeMs: 100;  // Process every 100ms for low latency
        }
        
        () as WebSocketOutput = WebSocketSink(Transcription) {
            param
                websocketUrl: "ws://localhost:8080/transcription";
        }
}
```

#### Using WenetSTT (Legacy)

```spl
stream<rstring partialText, boolean isFinal, float64 confidence> Transcription = 
    WenetSTT(AudioStream) {
        param
            modelPath: $modelPath;
            partialResultsEnabled: true;
            maxLatency: 0.3; // 300ms max latency
    }
```

### 3. Compile and Run the Application

```bash
# Compile (the -t flag includes the toolkit and all its dependencies)
sc -M RealtimeTranscriber -t /path/to/com.teracloud.streams.s2t_toolkit

# The resulting .sab file contains all necessary libraries - no additional 
# installation needed on runtime nodes!

# Run with WenetONNX
streamtool submitjob output/RealtimeTranscriber.sab \
    -P encoderModel=~/wenet_models/encoder.onnx \
    -P vocabFile=~/wenet_models/units.txt \
    -P audioEndpoint=ws://audio-source:9000/stream

# Run with WenetSTT (legacy)
streamtool submitjob output/RealtimeTranscriber.sab \
    -P modelPath=~/wenet_models/gigaspeech_s3_conformer \
    -P audioEndpoint=ws://audio-source:9000/stream
```

**Note**: The toolkit is self-contained. After building the toolkit, all dependencies (ONNX Runtime, kaldi-native-fbank, etc.) are packaged with your application. You don't need to install these libraries on your Streams runtime nodes.

## Sample Applications

The toolkit includes multiple sample applications demonstrating different implementation approaches:

### Sample Naming Convention
Samples follow the pattern: `<Language><Technology>_<OperatorName>/`

- **PythonONNX_MultiOperator**: Python with ONNX Runtime (multi-operator approach)
- **PythonONNX_SingleOperator**: Python with ONNX Runtime (single operator, optimized)
- **CppONNX_WenetONNX**: C++ using the WenetONNX operator (ONNX Runtime)
- **CppWeNet_WenetSTT**: C++ using the WenetSTT operator (full WeNet runtime)

Each sample demonstrates:
- Streaming audio ingestion
- Real-time transcription with incremental results
- Different latency-accuracy tradeoffs
- Integration patterns for production use

See `samples/README.md` for detailed descriptions of each approach.

## Performance and Latency

This toolkit is designed for real-time speech recognition with target latencies of 100-200ms:

| Implementation | Latency | Best For |
|----------------|---------|----------|
| **WenetONNX** (C++ ONNX) | ~100-150ms | Production deployments requiring portability |
| **WenetSTT** (C++ WeNet) | ~100-150ms | Full WeNet features and existing infrastructure |
| **Python ONNX** | ~150-300ms | Prototyping and research |

### Key Design Principles

1. **No Batching**: Audio processed immediately for lowest latency
2. **Streaming Architecture**: Incremental results as soon as available
3. **Configurable Performance**: Tune chunk size, threads, and hardware acceleration

For detailed performance tuning, see [docs/IMPLEMENTATION_DETAILS.md](docs/IMPLEMENTATION_DETAILS.md).

## Troubleshooting

### Common Issues

1. **Missing WeNet headers**: 
   - Ensure you've run `./install_wenet_deps.sh` or manually installed WeNet
   - Check that header files exist in `impl/include/wenet`

2. **LibTorch not found**:
   - Set `Torch_DIR` environment variable to your LibTorch installation directory
   - Or use `--torch-path` when building

3. **Library loading errors**:
   - Make sure `LD_LIBRARY_PATH` includes both LibTorch and the toolkit's lib directory
   - Run `ldd /path/to/libwenetcpp.so` to check for missing dependencies

4. **ONNX Runtime not found**:
   - Run `./setup_onnx_runtime.sh` to install ONNX Runtime
   - Check that `deps/onnxruntime/lib` exists
   - Verify `libonnxruntime.so` is in the library path

5. **ONNX model loading errors**:
   - Ensure the model was exported with correct input/output names
   - Check that vocabulary and CMVN files match the model
   - Verify the model supports dynamic batch sizes

6. **Feature extraction errors with WenetONNX**:
   - Run `./setup_kaldi_fbank.sh` to install kaldi-native-fbank
   - Check that `deps/kaldi-native-fbank/lib/libkaldi-native-fbank-core.so` exists
   - Verify the library is properly linked: `ldd impl/lib/libwenetonnx.so | grep kaldi`
   - If missing, rebuild with: `./build.sh --clean --onnx`

### Verifying Installation

#### For WenetONNX:

```bash
# Check that all dependencies are properly linked
ldd impl/lib/libwenetonnx.so

# Verify ONNX Runtime is found
ldd impl/lib/libwenetonnx.so | grep onnxruntime

# Verify kaldi-native-fbank is found  
ldd impl/lib/libwenetonnx.so | grep kaldi-native-fbank
```

#### For WenetSTT:

```bash
# Build and run the test suite
./build.sh --test --wenet-model /path/to/model

# Check library dependencies
ldd impl/lib/libwenetcpp.so
```

## Additional Documentation

- [Implementation Details](docs/IMPLEMENTATION_DETAILS.md) - Technical details of each operator implementation
- [ONNX Architecture](docs/ONNX_ARCHITECTURE.md) - Deep dive into the ONNX-based implementation
- [Version 2.0 Improvements](docs/IMPROVEMENTS_V2.md) - Details on memory safety and performance improvements
- [Sample Applications](samples/README.md) - Detailed guide to all sample applications
- [Real-time Design Guide](samples/docs/RealtimeDesignGuide/README.md) - Best practices for real-time STT

## License

Apache License 2.0