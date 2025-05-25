#!/bin/bash
# Build script for WeNet ONNX toolkit

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Building WeNet ONNX Toolkit..."

# 1. Ensure ONNX Runtime is installed
if [ ! -d "${SCRIPT_DIR}/deps/onnxruntime" ]; then
    echo "Installing ONNX Runtime..."
    "${SCRIPT_DIR}/setup_onnx_runtime.sh"
fi

# 2. Set up environment
source "${SCRIPT_DIR}/setup_env.sh"

# 3. Build implementation library
echo "Building implementation library..."
cd "${SCRIPT_DIR}/impl"
make clean
make

# 4. Copy libraries to toolkit
echo "Copying libraries..."
mkdir -p "${SCRIPT_DIR}/impl/lib"
cp libwenetonnx.so "${SCRIPT_DIR}/impl/lib/"
cp "${ONNXRUNTIME_ROOT_PATH}/lib/libonnxruntime.so"* "${SCRIPT_DIR}/impl/lib/" || true

# 5. Build SPL toolkit
echo "Building SPL toolkit..."
cd "${SCRIPT_DIR}/com.teracloud.streams.s2t"

if [ -z "$STREAMS_INSTALL" ]; then
    echo "Error: STREAMS_INSTALL not set"
    exit 1
fi

# Use spl-make-toolkit directly
"${STREAMS_INSTALL}/bin/spl-make-toolkit" -i .. -m

echo ""
echo "Build complete!"
echo ""
echo "To use the toolkit:"
echo "  1. Add to your application's toolkit path: -t ${SCRIPT_DIR}"
echo "  2. Use the WenetONNX operator in your SPL code"
echo ""
echo "To run the sample:"
echo "  cd samples/05_ONNXCppOperator"
echo "  make run"