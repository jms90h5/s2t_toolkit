#!/bin/bash
#
# Script to install WeNet dependencies and prepare development environment
#

set -e

print_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Install WeNet dependencies and prepare development environment."
    echo ""
    echo "Options:"
    echo "  -h, --help             Show this help message and exit"
    echo "  -d, --download-model   Download a pre-trained WeNet model"
    echo "  --model-lang LANG      Model language (default: en)"
    echo "  --model-dir DIR        Directory to save models (default: ~/wenet_models)"
    echo ""
}

# Default values
DOWNLOAD_MODEL=0
MODEL_LANG="en"
MODEL_DIR="$HOME/wenet_models"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -h|--help)
            print_help
            exit 0
            ;;
        -d|--download-model)
            DOWNLOAD_MODEL=1
            shift
            ;;
        --model-lang)
            MODEL_LANG="$2"
            shift
            shift
            ;;
        --model-dir)
            MODEL_DIR="$2"
            shift
            shift
            ;;
        *)
            echo "Unknown option: $key"
            print_help
            exit 1
            ;;
    esac
done

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
WENET_BUILD_DIR="$SCRIPT_DIR/../wenet_build"
WENET_SRC_DIR="$WENET_BUILD_DIR/wenet"

echo "Installing WeNet dependencies and preparing development environment..."
echo "Toolkit directory: $SCRIPT_DIR"
echo "WeNet build directory: $WENET_BUILD_DIR"

# Check if WeNet source is already cloned
if [ ! -d "$WENET_SRC_DIR" ]; then
    echo "Cloning WeNet repository..."
    mkdir -p "$WENET_BUILD_DIR"
    cd "$WENET_BUILD_DIR"
    git clone https://github.com/wenet-e2e/wenet.git
else
    echo "WeNet repository already exists at $WENET_SRC_DIR"
fi

# Create directories for WeNet includes
echo "Creating directories for WeNet includes..."
mkdir -p "$SCRIPT_DIR/impl/include/wenet"
mkdir -p "$SCRIPT_DIR/impl/lib"

# Copy WeNet header files
echo "Copying WeNet header files..."
cp -r "$WENET_SRC_DIR/runtime/core" "$SCRIPT_DIR/impl/include/wenet/"

# Download WeNet model if requested
if [ $DOWNLOAD_MODEL -eq 1 ]; then
    echo "Downloading WeNet model for language: $MODEL_LANG"
    mkdir -p "$MODEL_DIR"
    
    # Different model URLs based on language
    if [ "$MODEL_LANG" = "en" ]; then
        MODEL_URL="https://wenet-1256283475.cos.ap-shanghai.myqcloud.com/models/gigaspeech/s3_conformer_libtorch.tar.gz"
        MODEL_NAME="gigaspeech_s3_conformer"
    elif [ "$MODEL_LANG" = "cn" ] || [ "$MODEL_LANG" = "zh" ]; then
        MODEL_URL="https://wenet-1256283475.cos.ap-shanghai.myqcloud.com/models/wenetspeech/wenetspeech_u2pp_conformer_libtorch.tar.gz"
        MODEL_NAME="wenetspeech_u2pp_conformer"
    else
        echo "Error: Unsupported language $MODEL_LANG. Use 'en' or 'cn'."
        exit 1
    fi
    
    echo "Downloading model from $MODEL_URL..."
    wget -P "$MODEL_DIR" "$MODEL_URL"
    
    echo "Extracting model..."
    tar -xzf "$MODEL_DIR/$(basename $MODEL_URL)" -C "$MODEL_DIR"
    
    echo "Model downloaded and extracted to $MODEL_DIR/$MODEL_NAME"
    echo "You can use this model with the WeNet STT toolkit by setting the modelPath parameter to:"
    echo "  $MODEL_DIR/$MODEL_NAME"
fi

echo "Installation completed."
echo ""
echo "Next steps:"
echo "1. Build the WeNet STT toolkit using the build.sh script"
echo "2. Try the sample application in samples/RealtimeTranscriber"
echo ""
echo "For more details, see the README.md file."