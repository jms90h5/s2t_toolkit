# WeNet Speech-to-Text
# Standalone WeNet STT Test

This is a standalone test application for the WeNet speech-to-text engine.

## Implementation Options

This project provides three different implementation options:

1. **Mock Implementation (Default)**: Simple mock API that doesn't use real models
2. **Python Implementation**: Uses Python with PyTorch to interface with real model files
3. **C++ Implementation**: Direct C++ API for optimal performance with real models

## Mock Implementation (Default)

### Building

```bash
make
```

This will compile the standalone test application with the mock implementation enabled.

### Running

The simplest way to run the test:

```bash
make run
```

Alternatively, you can run it manually:

```bash
# Make sure the library is in the library path
LD_LIBRARY_PATH=../../../impl/lib:$LD_LIBRARY_PATH ./standalone_test
```

## Python Implementation

The Python implementation uses Python and PyTorch to interface with the WeNet models, providing a more realistic experience while still being easy to set up.

### Building and Running

```bash
cd real_implementation
make
make run
```

This implementation:
- Uses Python bindings to interface with PyTorch
- Loads and verifies the real model files
- Produces realistic transcription results

## C++ Implementation

The C++ implementation directly uses the WeNet C++ API for optimal performance.

### Building and Running

```bash
cd cpp_implementation
make
make run
```

This implementation:
- Uses the WeNet C++ API directly
- Performs native audio processing
- Provides optimal performance for real-time applications

## Working with Models

### Mock Implementation

The standalone test is configured to use the mock implementation of the WeNet API by default, which does not require the actual WeNet model files or dependencies. The mock implementation is enabled by the `-DMOCK_WENET_IMPLEMENTATION` compiler flag in the Makefile.

To run with the mock implementation:

```bash
make run
# or
make run_with_mock
```

### Using Real Models

You can set up the real WeNet implementation using the included installation script or by following the manual steps.

#### Option 1: Using the Makefile Target (Recommended)

The simplest way to set up WeNet is to use the provided Makefile target:

```bash
# This will install dependencies, build WeNet, and download models in one step
make setup_wenet
```

After the setup is complete:

1. Remove the mock implementation flag from the Makefile:
   ```diff
   - CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I../../../impl/include -DMOCK_WENET_IMPLEMENTATION
   + CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I../../../impl/include
   ```

2. Rebuild and run the test with the real implementation:
   ```bash
   make clean
   make
   make run_with_real
   ```

#### Option 2: Using the Installation Script Directly

You can also run the installation script directly if you need more control:

```bash
# Make the script executable if needed
chmod +x install_wenet_deps.sh

# Install all dependencies, build WeNet, and download a model
./install_wenet_deps.sh --install-deps --build-wenet --download-model

# Or run specific tasks individually
./install_wenet_deps.sh --install-deps      # Only install dependencies
./install_wenet_deps.sh --build-wenet       # Only build WeNet
./install_wenet_deps.sh --download-model    # Only download the model
```

After running the script, follow the same steps as Option 1 to rebuild and run.

#### Option 2: Manual Setup

If you prefer to set up manually, follow these steps:

1. **Install Dependencies**:
   ```bash
   # Basic dependencies
   sudo apt-get update
   sudo apt-get install -y build-essential cmake git python3-dev python3-pip
   
   # WeNet dependencies
   sudo apt-get install -y libgflags-dev libgoogle-glog-dev
   
   # Install PyTorch and other Python dependencies
   pip3 install torch torchaudio
   ```

2. **Clone and Build WeNet**:
   ```bash
   # Clone WeNet repository
   git clone https://github.com/wenet-e2e/wenet.git
   cd wenet

   # Build the runtime library
   cd runtime/server/x86
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make -j $(nproc)
   
   # Note the path to the built libraries
   WENET_RUNTIME_PATH=$(pwd)
   ```

3. **Download a Pre-trained Model**:
   - Visit the [WeNet pre-trained models page](https://github.com/wenet-e2e/wenet/blob/main/docs/pretrained_models.md)
   - For English models, look for "Pretrained Models(En)" section
   - Download the model that has "Runtime" suffix (.zip format)
   - Extract the model:
     ```bash
     mkdir -p /path/to/models
     unzip /path/to/downloaded/model.zip -d /path/to/models
     ```
     
4. **Link WeNet with Our Project**:
   ```bash
   # Copy WeNet headers
   mkdir -p /homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl/include/wenet
   cp -r wenet/runtime/server/x86/third_party/wenet/include/* /homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl/include/wenet/
   
   # Copy WeNet libraries
   mkdir -p /homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl/lib
   cp wenet/runtime/server/x86/build/lib/libwenet.so /homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl/lib/
   ```

5. **Update the Makefile**:
   - Remove the `-DMOCK_WENET_IMPLEMENTATION` flag from CXXFLAGS
   - Add the linking to WeNet libraries

   ```diff
   - CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I../../../impl/include -DMOCK_WENET_IMPLEMENTATION
   + CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I../../../impl/include
   ```

6. **Rebuild and Run**:
   ```bash
   make clean
   make
   
   # Set environment variables and run
   LD_LIBRARY_PATH=../../../impl/lib:$LD_LIBRARY_PATH ./standalone_test --model_path /path/to/models
   ```

**Troubleshooting**:

- If you encounter missing header files: Ensure you've copied all necessary headers from the WeNet repository
- If you encounter missing libraries: Check your `LD_LIBRARY_PATH` and ensure all required libraries are present
- If you encounter segmentation faults: Check that your model path is correct and contains the necessary model files

**Common Issues**:

1. **Missing WeNet Dependencies**: WeNet depends on several libraries, including gflags, glog, and others. Make sure all dependencies are installed.

2. **Incompatible Model Format**: Make sure you're using the correct model format. WeNet models come in two formats: checkpoint format (.pt files) for training and runtime format (.zip files) for deployment.

3. **CUDA Issues**: If you're using CUDA for GPU acceleration, make sure your CUDA version is compatible with the PyTorch version used.

**Note:** Using the real implementation requires additional dependencies such as PyTorch, CUDA (for GPU acceleration), and other libraries that the WeNet runtime depends on.

### Using Custom Models

If you have your own WeNet model:

1. Place the model files in the `models` directory or specify a custom path with `--model_path`
2. Make sure the `-DMOCK_WENET_IMPLEMENTATION` flag is removed if you want to use real models

## Test Description

The standalone test:

1. Initializes the WeNet STT engine
2. Generates 10 synthetic audio chunks with varying frequencies
3. Processes each audio chunk as if it were streaming audio
4. Flushes the engine to get the final transcription result
5. Prints all transcription results (both partial and final) to console