# ONNX Pipeline Update - Three-Stage Architecture

## Overview

Based on guidance from the speech recognition team, the ONNX implementation has been updated to follow a three-stage pipeline architecture that is compatible with most modern ASR frameworks using Conformer/RNN-T models.

## Pipeline Stages

### 1. Voice Activity Detection (VAD)
- **Model**: Silero VAD exported to ONNX
- **Purpose**: Removes silence and music, processes only speech
- **Implementation**: Pure ONNX operator, no external dependencies
- **Benefits**: Reduces compute by skipping non-speech segments

### 2. Feature Extraction (FBank)
- **Library**: kaldifeat (C++17 implementation)
- **Important**: Uses static library without PyTorch or CUDA dependencies
- **Features**: 80-dimensional log Mel filterbank features
- **Frame**: 25ms window, 10ms shift (standard for speech recognition)

### 3. Speech Recognition
- **Models**: WeNet models exported to ONNX format
  - `encoder.onnx`: Acoustic encoder (Conformer/Transformer)
  - `ctc.onnx`: CTC decoder for alignment
  - `decoder.onnx`: Attention decoder (optional)
- **Mode**: Streaming-compatible export required
- **Inference**: Pure ONNX Runtime, supports CPU/GPU

## Key Implementation Details

### Model Export

Models must be exported from WeNet using the streaming-compatible export script:

```bash
python -m wenet.bin.export_onnx_cpu \
    --config $exp/train.yaml \
    --checkpoint $exp/final.pt \
    --chunk_size 16 \              # Streaming chunk size
    --output_dir $onnx_dir \
    --num_decoding_left_chunks -1  # Context configuration
```

### Streaming Support

The implementation supports real-time streaming with:
- Configurable chunk size (typically 16 frames = 160ms)
- Left context for better accuracy
- State caching between chunks

### Input/Output Schema

The WeNet ONNX models expect specific tensor names:
- **Encoder inputs**: `chunk`, `att_cache`, `cnn_cache` (for streaming)
- **CTC input**: Encoder output
- **VAD input**: Raw audio samples

## Important Notes

### Broadband Only

Currently, only broadband (16kHz) models are available. Narrowband (8kHz) support for telephony will require:
- Fine-tuning broadband models on telephony datasets
- Modifying feature extraction parameters
- This will be addressed once the operator is ready

### Compatibility

This pipeline is compatible with frameworks that use:
- Conformer models
- RNN-T models  
- Similar feature configurations

Even with slightly different feature parameters, the pipeline remains compatible.

## Code Structure

The updated implementation (`OnnxSTTImpl_v2.cpp`) includes:

1. **VAD Processing**
   - `runVAD()`: Runs Silero VAD on audio chunks
   - `updateVADState()`: Manages speech/silence transitions

2. **Feature Extraction**
   - `extractFbankFeatures()`: Uses kaldifeat for FBank
   - `applyCMVN()`: Applies cepstral mean/variance normalization

3. **Model Inference**
   - `runEncoder()`: Processes features through encoder
   - `runCTC()`: Computes CTC scores
   - `runDecoder()`: Optional attention decoding

4. **Decoding**
   - `ctcGreedyDecode()`: Fast greedy CTC decoding
   - `ctcPrefixBeamSearch()`: Beam search for better accuracy

## Performance Expectations

With this pipeline:
- **VAD**: ~5ms per chunk
- **Feature Extraction**: ~10ms per 100ms audio
- **Encoder**: ~30-50ms (CPU), ~10-20ms (GPU)
- **CTC**: ~5-10ms
- **Total Latency**: ~50-100ms end-to-end

## Next Steps

1. Obtain Silero VAD ONNX model
2. Build kaldifeat as static library
3. Export WeNet models using the provided script
4. Test the complete pipeline with real audio

## References

- [Silero VAD](https://github.com/snakers4/silero-vad)
- [kaldifeat](https://github.com/csukuangfj/kaldifeat)
- [WeNet ONNX Export](https://github.com/wenet-e2e/wenet/tree/main/runtime/onnxruntime)