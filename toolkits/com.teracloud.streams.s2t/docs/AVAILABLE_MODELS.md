# Available Pre-Trained ONNX Models

This toolkit supports various pre-trained streaming ASR models. Since the toolkit is designed for **inference only**, we recommend using pre-exported ONNX models to avoid Python/PyTorch dependencies.

## Recommended Models

### 1. Sherpa-ONNX Paraformer (Recommended for Easy Setup)
- **Type**: Paraformer architecture
- **Language**: English
- **Sample Rate**: 16kHz (broadband)
- **Parameters**: 42M
- **WER**: 2.4% (clean) / 5.0% (other) on LibriSpeech
- **Training Data**: 4,500 hours
- **Chunk Size**: 32 frames
- **Cache Tensor**: `cache`
- **Download**: Pre-exported ONNX model available
- **Script**: `./download_sherpa_onnx_model.sh`

**Advantages**:
- No Python/PyTorch needed
- Pre-exported and ready to use
- Excellent accuracy
- Single model file (no separate CTC/decoder)

### 2. Icefall Conformer RNN-T Large
- **Repository**: k2-fsa/icefall
- **Parameters**: 120M
- **WER**: 1.8% (clean) / 4.2% (other)
- **Chunk Size**: 16 frames
- **Cache Tensors**: `encoder_out_cache`, `cnn_cache`, `att_cache`
- **Export**: Requires `tools/export_onnx.py`
- **Output**: encoder.onnx, decoder.onnx, joiner.onnx

### 3. WeNet Conformer RNN-T Base
- **Repository**: wenet-e2e/wenet
- **Parameters**: 30M
- **WER**: 2.1% (clean) / 4.6% (other)
- **Chunk Size**: 16 frames
- **Cache Tensor**: `cache`
- **Export**: Requires `runtime/onnx/export_onnx.py --mode streaming`

### 4. SpeechBrain CRDNN RNN-T
- **Repository**: speechbrain
- **Parameters**: 69M
- **WER**: 1.9% (clean) / 4.4% (other)
- **Chunk Size**: 5 frames
- **Cache Tensor**: `h0`
- **Export**: Requires `utils/export_onnx.py`

## Model Architecture Support

The toolkit's ONNX implementation supports:

1. **Paraformer Models** (like Sherpa-ONNX)
   - Single model file
   - Built-in attention mechanism
   - No separate CTC/decoder needed

2. **Conformer Models** (like WeNet/Icefall)
   - Encoder + CTC + Decoder architecture
   - Supports both CTC-only and joint CTC-attention decoding

3. **RNN-T Models** (like SpeechBrain)
   - Encoder + Joiner architecture
   - Transducer-based decoding

## Quick Start

For the fastest setup without Python dependencies:

```bash
# Download Sherpa-ONNX Paraformer (recommended)
./download_sherpa_onnx_model.sh

# The model will be downloaded to:
# models/sherpa_onnx_paraformer/
```

## Cache Tensor Configuration

Different models use different cache tensor names and chunk sizes:

| Model | Chunk Size | Cache Tensor Names |
|-------|------------|-------------------|
| Sherpa Paraformer | 32 frames | `cache` |
| WeNet Conformer | 16 frames | `cache` |
| Icefall Conformer | 16 frames | `encoder_out_cache`, `cnn_cache`, `att_cache` |
| SpeechBrain CRDNN | 5 frames | `h0` |

The implementation automatically handles these differences based on the model configuration.

## Narrowband Support

Currently, all listed models are trained on 16kHz (broadband) audio. For 8kHz (narrowband) telephony applications, models would need to be fine-tuned on telephony data.

## Performance Considerations

- **Latency**: All models support streaming with <100ms latency
- **Memory**: Model sizes range from 30M to 120M parameters
- **Accuracy**: WER ranges from 1.8% to 5.0% on LibriSpeech
- **Speed**: Real-time factor typically 0.1-0.3x on CPU

## References

- [Sherpa-ONNX Models](https://github.com/k2-fsa/sherpa-onnx/releases/tag/asr-models)
- [WeNet Model Zoo](https://github.com/wenet-e2e/wenet/blob/main/docs/pretrained_models.md)
- [Icefall Models](https://github.com/k2-fsa/icefall)
- [SpeechBrain Recipes](https://github.com/speechbrain/speechbrain)