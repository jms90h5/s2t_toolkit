#!/bin/bash

# Run the standalone test application

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TOOLKIT_DIR="$(dirname "$(dirname "$(dirname "$SCRIPT_DIR")")")"

# Set LD_LIBRARY_PATH to include the toolkit's lib directory
export LD_LIBRARY_PATH="$TOOLKIT_DIR/impl/lib:$LD_LIBRARY_PATH"

# Build the application
cd "$SCRIPT_DIR" && make

# Run the application
./standalone_test --model_path="/tmp/wenet_model" "$@"