#!/bin/bash
# Download and prepare WeNet models for the samples

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODEL_DIR="${SCRIPT_DIR}/models"

echo "Creating model directory..."
mkdir -p "${MODEL_DIR}"

# Check if models already exist
if [ -f "${MODEL_DIR}/wenet_encoder.onnx" ]; then
    echo "Models already downloaded. Delete ${MODEL_DIR} to re-download."
    exit 0
fi

echo "Downloading WeNet pre-trained model..."
# Example URL - replace with actual model location
MODEL_URL="https://example.com/wenet_model.tar.gz"

# For now, create mock files for testing
echo "Creating mock model files for testing..."

# Create mock ONNX model info
cat > "${MODEL_DIR}/model_info.json" << EOF
{
    "model": "wenet_encoder",
    "version": "1.0",
    "input_shapes": {
        "speech": ["batch", "time", 80],
        "speech_lengths": ["batch"]
    },
    "output_shapes": {
        "encoder_out": ["batch", "time", 256],
        "encoder_out_lens": ["batch"]
    }
}
EOF

# Create mock vocabulary
cat > "${MODEL_DIR}/vocab.txt" << EOF
<blank>
<unk>
a
b
c
d
e
f
g
h
i
j
k
l
m
n
o
p
q
r
s
t
u
v
w
x
y
z
'
-
<space>
EOF

# Create mock CMVN stats
cat > "${MODEL_DIR}/global_cmvn.stats" << EOF
{
    "mean": [0.0] * 80,
    "std": [1.0] * 80
}
EOF

# Create Python utilities module
mkdir -p "${MODEL_DIR}/python"
cat > "${MODEL_DIR}/python/wenet_utils.py" << 'EOF'
"""Utilities for WeNet STT samples"""

import numpy as np
import json
from typing import List, Dict, Tuple

def compute_fbank_features(audio: np.ndarray, 
                          sample_rate: int = 16000,
                          num_mel_bins: int = 80,
                          frame_length: int = 25,
                          frame_shift: int = 10) -> np.ndarray:
    """Compute Fbank features from audio (simplified mock)"""
    # Mock implementation - returns random features
    num_frames = len(audio) // (sample_rate * frame_shift // 1000)
    return np.random.randn(num_frames, num_mel_bins).astype(np.float32)

def load_cmvn_stats(stats_file: str) -> Dict[str, np.ndarray]:
    """Load CMVN statistics"""
    # Mock implementation
    return {
        'mean': np.zeros(80, dtype=np.float32),
        'std': np.ones(80, dtype=np.float32)
    }

def load_vocabulary(vocab_file: str) -> List[str]:
    """Load vocabulary from file"""
    with open(vocab_file, 'r') as f:
        return [line.strip() for line in f]

class CTCPrefixBeamSearch:
    """Mock CTC beam search decoder"""
    
    def __init__(self, vocab_size: int, beam_size: int = 10, blank_id: int = 0):
        self.vocab_size = vocab_size
        self.beam_size = beam_size
        self.blank_id = blank_id
    
    def search(self, encoder_out: np.ndarray, encoder_len: int) -> List[Dict]:
        """Mock beam search - returns dummy hypothesis"""
        # Generate some mock tokens
        tokens = np.random.randint(2, self.vocab_size, size=encoder_len // 4)
        return [{
            'tokens': tokens.tolist(),
            'score': -np.random.random() * 10
        }]
EOF

echo "Model files created in ${MODEL_DIR}"
echo ""
echo "NOTE: These are mock files for testing."
echo "To use real models, you need to:"
echo "1. Export WeNet model to ONNX format"
echo "2. Replace the mock files with real ones"
echo ""
echo "See documentation for model export instructions."

chmod +x "${SCRIPT_DIR}/download_models.sh"