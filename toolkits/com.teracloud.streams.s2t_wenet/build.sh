#!/bin/bash
#
# Build script for com.teracloud.streams.s2t_wenet toolkit
#

set -e

print_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Build the TeraCloud Streams S2T WeNet toolkit."
    echo ""
    echo "Options:"
    echo "  -h, --help             Show this help message and exit"
    echo "  -c, --clean            Clean build artifacts before building"
    echo "  -t, --test             Run tests after building"
    echo "  -d, --doc              Generate documentation"
    echo "  --wenet-path PATH      Path to WeNet installation"
    echo "  --torch-path PATH      Path to LibTorch installation"
    echo "  --wenet-model PATH     Path to WeNet model for testing"
    echo "  --install              Install the toolkit to STREAMS_INSTALL/toolkits"
    echo "  --install-deps         Install WeNet dependencies"
    echo "  --mock-implementation  Build with mock implementation (no real WeNet dependencies)"
    echo "  --verbose              Verbose output during build and tests"
    echo ""
}

# Default values
CLEAN=0
TEST=0
DOC=0
INSTALL=0
INSTALL_DEPS=0
VERBOSE=0
MOCK_IMPLEMENTATION=0
WENET_PATH=""
TORCH_PATH=""
WENET_MODEL=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -h|--help)
            print_help
            exit 0
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        -t|--test)
            TEST=1
            shift
            ;;
        -d|--doc)
            DOC=1
            shift
            ;;
        --wenet-path)
            WENET_PATH="$2"
            shift
            shift
            ;;
        --torch-path)
            TORCH_PATH="$2"
            shift
            shift
            ;;
        --wenet-model)
            WENET_MODEL="$2"
            shift
            shift
            ;;
        --install)
            INSTALL=1
            shift
            ;;
        --install-deps)
            INSTALL_DEPS=1
            shift
            ;;
        --mock-implementation)
            MOCK_IMPLEMENTATION=1
            shift
            ;;
        --verbose)
            VERBOSE=1
            shift
            ;;
        *)
            echo "Unknown option: $key"
            print_help
            exit 1
            ;;
    esac
done

# Check if STREAMS_INSTALL is set
if [ -z "$STREAMS_INSTALL" ]; then
    echo "Error: STREAMS_INSTALL environment variable is not set."
    echo "Please set it to your TeraCloud Streams installation directory."
    exit 1
fi

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TOOLKIT_DIR="$SCRIPT_DIR"
BUILD_DIR="$TOOLKIT_DIR/build"
TEST_DIR="$TOOLKIT_DIR/test"

echo "Building TeraCloud Streams S2T WeNet toolkit..."
echo "Toolkit directory: $TOOLKIT_DIR"

# Install dependencies if requested
if [ $INSTALL_DEPS -eq 1 ]; then
    echo "Installing WeNet dependencies..."
    "$TOOLKIT_DIR/install_wenet_deps.sh"
fi

# Clean build artifacts if requested
if [ $CLEAN -eq 1 ]; then
    echo "Cleaning build artifacts..."
    rm -rf "$BUILD_DIR" "$TOOLKIT_DIR/impl/lib/libwenetcpp.so"
    mkdir -p "$TOOLKIT_DIR/impl/lib"
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Prepare CMake arguments
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"

# Add WeNet path if provided
if [ -n "$WENET_PATH" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DWENET_ROOT=$WENET_PATH"
fi

# Add LibTorch path if provided
if [ -n "$TORCH_PATH" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_PREFIX_PATH=$TORCH_PATH"
fi

# Set mock implementation if requested
if [ $MOCK_IMPLEMENTATION -eq 1 ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DMOCK_IMPLEMENTATION=ON"
    echo "Using mock implementation (no actual WeNet dependencies required)"
fi

# Set verbose output if requested
if [ $VERBOSE -eq 1 ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
fi

# Build the C++ primitive operator library using CMake
echo "Building C++ primitive operator library..."
cd "$BUILD_DIR"

echo "Running CMake with args: $CMAKE_ARGS"
cmake $CMAKE_ARGS ..

echo "Building library..."
cmake --build . --config Release

# Copy the built library to the impl/lib directory
echo "Installing library to toolkit directory..."
cp "$BUILD_DIR/libwenetcpp.so" "$TOOLKIT_DIR/impl/lib/"

# Generate documentation if requested
if [ $DOC -eq 1 ]; then
    echo "Generating toolkit documentation..."
    # Use SPL documentation generator if available
    if command -v spl-doc &> /dev/null; then
        spl-doc --directory "$TOOLKIT_DIR" --output-directory "$TOOLKIT_DIR/doc/spldoc"
    else
        echo "Warning: spl-doc command not found. Using basic documentation."
        mkdir -p "$TOOLKIT_DIR/doc/spldoc"
    fi
fi

# Run tests if requested
if [ $TEST -eq 1 ]; then
    echo "Running toolkit tests..."
    
    if [ -z "$WENET_MODEL" ]; then
        echo "Warning: No WeNet model specified for testing. Using a default path."
        WENET_MODEL="/tmp/wenet_model"
        
        if [ $MOCK_IMPLEMENTATION -eq 1 ]; then
            echo "Using mock implementation - model path is not critical for tests."
        else
            echo "Checking for models in common locations..."
            
            # Check common model locations
            POTENTIAL_MODELS=(
                "$HOME/wenet_models/gigaspeech_s3_conformer"
                "$HOME/wenet_models/wenetspeech_u2pp_conformer_libtorch"
                "../wenet_models/gigaspeech_s3_conformer"
                "../wenet_build/models/gigaspeech_s3_conformer"
            )
            
            for model_path in "${POTENTIAL_MODELS[@]}"; do
                if [ -d "$model_path" ]; then
                    WENET_MODEL="$model_path"
                    echo "Found model at: $WENET_MODEL"
                    break
                fi
            done
        fi
    fi
    
    # Ensure directories exist
    mkdir -p "$TEST_DIR"
    
    # Build test executable
    echo "Building test executable..."
    cd "$BUILD_DIR"
    
    # Build with testing enabled
    if [ $MOCK_IMPLEMENTATION -eq 1 ]; then
        cmake -DBUILD_TESTING=ON -DMOCK_IMPLEMENTATION=ON ..
    else
        cmake -DBUILD_TESTING=ON ..
    fi
    
    cmake --build . --config Release --target wenet_tests
    
    if [ ! -f "$BUILD_DIR/wenet_tests" ]; then
        echo "Error: Test executable not built successfully."
        exit 1
    fi
    
    # Run the tests
    echo "Running tests with model path: $WENET_MODEL"
    "$BUILD_DIR/wenet_tests" --model_path="$WENET_MODEL" ${TEST_WAV:+--test_wav="$TEST_WAV"} ${VERBOSE:+--verbose}
    
    if [ $? -eq 0 ]; then
        echo "All tests passed successfully!"
    else
        echo "Error: Some tests failed. See output for details."
        exit 1
    fi
fi

# Install the toolkit if requested
if [ $INSTALL -eq 1 ]; then
    echo "Installing toolkit to $STREAMS_INSTALL/toolkits..."
    TOOLKIT_NAME=$(basename "$TOOLKIT_DIR")
    DEST_DIR="$STREAMS_INSTALL/toolkits/$TOOLKIT_NAME"
    
    mkdir -p "$DEST_DIR"
    cp -a "$TOOLKIT_DIR"/* "$DEST_DIR/"
    
    echo "Toolkit installed successfully."
fi

echo "Build completed successfully!"
echo ""
if [ $MOCK_IMPLEMENTATION -eq 1 ]; then
    echo "NOTE: This build uses a mock implementation of WeNet."
    echo "      For production use, build with real WeNet dependencies."
    echo ""
fi
echo "To use this toolkit in your SPL applications, include:"
echo "  use com.teracloud.streams.s2t_wenet::*;"
echo ""
echo "For a sample application, see samples/RealtimeTranscriber"