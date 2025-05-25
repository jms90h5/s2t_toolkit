#!/bin/bash
# Source this file to set up ONNX Runtime environment

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ONNX_ROOT="${SCRIPT_DIR}/deps/onnxruntime"

export LD_LIBRARY_PATH="${ONNX_ROOT}/lib:${LD_LIBRARY_PATH}"
export CMAKE_PREFIX_PATH="${ONNX_ROOT}:${CMAKE_PREFIX_PATH}"
export ONNXRUNTIME_ROOT_PATH="${ONNX_ROOT}"

echo "ONNX Runtime environment configured:"
echo "  ONNXRUNTIME_ROOT_PATH=${ONNXRUNTIME_ROOT_PATH}"
echo "  LD_LIBRARY_PATH includes ${ONNX_ROOT}/lib"
