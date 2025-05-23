# TeraCloud Streams Speech-to-Text Toolkit with WeNet

This toolkit integrates [WeNet](https://github.com/wenet-e2e/wenet) (an open-source End-to-End Speech Recognition Toolkit) with TeraCloud Streams. It provides a high-performance C++ primitive operator that leverages WeNet's capabilities for real-time speech recognition in streaming applications.

## Overview

The `com.teracloud.streams.s2t_wenet` toolkit enables:

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

- **WenetSTT**: Primary operator for real-time speech recognition
- **AudioStreamSource**: Operator for ingesting streaming audio
- **Utility functions**: Format converters and transcription enhancement functions

## Prerequisites

- TeraCloud Streams 7.x
- C++17 compatible compiler
- WeNet runtime library dependencies
- CMake 3.14 or higher for building the toolkit
- LibTorch (PyTorch C++ libraries) for WeNet

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

## Building the Toolkit

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
- `--wenet-path PATH`: Path to WeNet installation
- `--torch-path PATH`: Path to LibTorch installation
- `--test`: Run tests after building (requires a model)
- `--wenet-model PATH`: Path to WeNet model for testing
- `--install`: Install the toolkit to STREAMS_INSTALL/toolkits
- `--install-deps`: Install WeNet dependencies

## QuickStart Guide

### 1. Install Dependencies and Build the Toolkit

```bash
# Install dependencies and download a model
./install_wenet_deps.sh --install-deps --download-model

# Build the toolkit
./build.sh --clean
```

### 2. Create a Real-time Transcription Application

```spl
composite RealtimeTranscriber {
    param
        expression<rstring> $modelPath: getSubmissionTimeValue("modelPath");
        expression<rstring> $audioEndpoint: getSubmissionTimeValue("audioEndpoint");
    
    graph
        stream<blob audioChunk, uint64 timestamp> AudioStream = AudioStreamSource() {
            param
                endpoint: $audioEndpoint;
                format: "pcm";
                sampleRate: 16000;
                channelCount: 1;
        }
        
        stream<rstring partialText, boolean isFinal, float64 confidence> Transcription = WenetSTT(AudioStream) {
            param
                modelPath: $modelPath;
                partialResultsEnabled: true;
                maxLatency: 0.3; // 300ms max latency
        }
        
        () as WebSocketSink = WebSocketSink(Transcription) {
            param
                websocketUrl: "ws://localhost:8080/transcription";
        }
}
```

### 3. Compile and Run the Application

```bash
# Compile
sc -M RealtimeTranscriber -t $STREAMS_INSTALL/toolkits/com.teracloud.streams.s2t_wenet

# Run
streamtool submitjob output/RealtimeTranscriber.sab -P modelPath=~/wenet_models/gigaspeech_s3_conformer -P audioEndpoint=ws://audio-source:9000/stream
```

## Sample Application

The toolkit includes a sample real-time transcription application that demonstrates:

- Streaming audio ingestion from a WebSocket source
- Real-time transcription with incremental results
- Configuring WeNet for optimal latency-accuracy tradeoff
- Integration with a simple web interface for visualization

The sample is located in the `samples/RealtimeTranscriber` directory.

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

### Verifying Installation

To verify that WeNet is properly integrated:

```bash
# Build and run the test suite
./build.sh --test --wenet-model /path/to/model

# Check library dependencies
ldd impl/lib/libwenetcpp.so
```

## License

Apache License 2.0