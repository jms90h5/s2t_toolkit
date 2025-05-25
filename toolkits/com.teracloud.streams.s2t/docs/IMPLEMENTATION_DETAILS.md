# Implementation Details

This document contains technical details about the different implementations in the Speech-to-Text Toolkit.

## WenetONNX Operator (ONNX Runtime Implementation)

### Why ONNX?

1. **Portability**: ONNX models run on any platform with ONNX Runtime
2. **No WeNet Dependency**: Don't need to build/maintain WeNet C libraries at runtime
3. **Easy Updates**: Just replace the ONNX file to update models
4. **Multiple Backends**: CPU, CUDA, TensorRT, DirectML, etc.
5. **Production Ready**: Microsoft's ONNX Runtime is battle-tested

### Architecture

```
┌─────────────────────────┐
│   SPL Application       │
├─────────────────────────┤
│    WenetONNX Operator   │ (SPL Primitive Operator)
├─────────────────────────┤
│   WenetONNXImpl (C++)   │ (Implementation class)
├─────────────────────────┤
│   ONNX Runtime C++ API  │
├─────────────────────────┤
│   Encoder ONNX Model    │
└─────────────────────────┘
```

### Implementation Details

The ONNX implementation handles:
1. **Feature Extraction**: Using kaldi-native-fbank for Mel-filterbank features
2. **CMVN Normalization**: Applied to features before inference
3. **ONNX Inference**: Encoder-only model for CTC decoding
4. **CTC Beam Search**: Custom C++ implementation
5. **Streaming State**: Maintains feature buffer for continuous processing

## WenetSTT Operator (Full WeNet Implementation)

### Key Features

#### 🔒 **Memory Safety**
- RAII wrapper for WeNet decoder prevents memory leaks
- Smart pointer management throughout
- Move semantics for efficient resource handling

#### ⚡ **Real-Time Performance**
- Immediate processing of audio chunks (no batching)
- Lock-free operations in audio path
- Minimal buffering for lowest latency
- Real-time performance metrics tracking
- Sub-200ms end-to-end latency

#### 🛡️ **Robust Error Handling**
- Comprehensive exception handling
- Graceful degradation on errors
- Detailed error logging

### Threading Model

```
Main Thread                    Worker Thread
─────────────                 ──────────────
ProcessAudioChunk() ─────┐
     │                   └───> Process Queue
     │                         │
     │                         ├─> Extract Features
     │                         ├─> Run Decoder
     │                         └─> Update Results
     │                               │
GetTranscription() <─────────────────┘
```

## Performance Comparison

| Implementation | Latency | Dependencies | Deployment Complexity |
|----------------|---------|--------------|----------------------|
| WenetONNX | ~100-150ms | ONNX Runtime, kaldi-native-fbank | Low |
| WenetSTT | ~100-150ms | Full WeNet, PyTorch C++ | High |
| Python ONNX | ~150-300ms | Python, ONNX Runtime | Medium |

## Choosing an Implementation

### Use WenetONNX when:
- You need simple deployment
- Cross-platform compatibility is important
- You want to use different hardware accelerators
- Model updates should be easy

### Use WenetSTT when:
- You need all WeNet features
- You have existing WeNet infrastructure
- You need advanced decoding options
- You're already managing WeNet dependencies