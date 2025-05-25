#!/bin/bash
# Test building the ONNX implementation

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Source environment
source "${SCRIPT_DIR}/setup_env.sh"

# Create build directory
mkdir -p "${SCRIPT_DIR}/build_test"
cd "${SCRIPT_DIR}/build_test"

# Test compile just the implementation file
echo "Testing compilation of WenetONNXImpl..."

g++ -std=c++17 -c \
    -I"${ONNXRUNTIME_ROOT_PATH}/include" \
    -I"${ONNXRUNTIME_ROOT_PATH}/include/onnxruntime/core/session" \
    -I"${SCRIPT_DIR}/impl/include" \
    "${SCRIPT_DIR}/impl/include/WenetONNXImpl.cpp" \
    -o WenetONNXImpl.o

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful!"
    
    # Check symbols
    echo ""
    echo "Checking exported symbols:"
    nm -C WenetONNXImpl.o | grep "WenetONNXImpl::" | head -5
    
    # Clean up
    rm -f WenetONNXImpl.o
else
    echo "✗ Compilation failed"
    exit 1
fi

echo ""
echo "To build the complete toolkit with CMake:"
echo "  cd ${SCRIPT_DIR}"
echo "  mkdir -p build && cd build"
echo "  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=\"${ONNXRUNTIME_ROOT_PATH}\" .."
echo "  make"