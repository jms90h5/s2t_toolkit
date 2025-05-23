# WeNet Speech-to-Text Samples

This toolkit provides three implementation approaches for WeNet STT, each with different trade-offs between ease of development and performance.

## Implementation Approaches

### 1. 🐍 Python Prototype (`01_BasicPythonPrototype/`)
- **Architecture**: Python → ONNX → Python
- **Latency**: ~200-300ms
- **Use Case**: Rapid prototyping, research
- **Pros**: Easy to modify, debug, experiment
- **Cons**: Higher latency, more overhead

### 2. 🚀 Optimized Python (`02_OptimizedPythonONNX/`)
- **Architecture**: Single Python operator with ONNX Runtime
- **Latency**: ~150-200ms
- **Use Case**: Production where Python is acceptable
- **Pros**: Good balance of performance and maintainability
- **Cons**: Still has Python overhead

### 3. ⚡ Production C++ (`03_ProductionCppRealtime/`)
- **Architecture**: Pure C++ with ONNX Runtime
- **Latency**: ~100-150ms
- **Use Case**: Production real-time systems
- **Pros**: Lowest latency, highest throughput
- **Cons**: Harder to develop and debug

### 4. 📊 Performance Comparison (`04_PerformanceComparison/`)
- Runs all three implementations side-by-side
- Measures latency, throughput, and accuracy
- Helps choose the right approach for your use case

## Quick Start

### Prerequisites
```bash
# Install Python dependencies
pip install -r requirements.txt

# Download models
./download_models.sh

# Build C++ components
./build.sh
```

### Running the Samples

#### Python Prototype
```bash
cd 01_BasicPythonPrototype
streamtool submitjob -P modelPath=../models/wenet BasicPrototype.spl
```

#### Optimized Python
```bash
cd 02_OptimizedPythonONNX
streamtool submitjob -P modelPath=../models/wenet OptimizedPython.spl
```

#### Production C++
```bash
cd 03_ProductionCppRealtime
streamtool submitjob -P modelPath=../models/wenet ProductionRealtime.spl
```

## Choosing the Right Approach

| Criteria | Python Prototype | Optimized Python | Production C++ |
|----------|-----------------|------------------|----------------|
| Development Time | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ |
| Debugging | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ |
| Latency | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Throughput | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Flexibility | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ |
| Maintenance | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ |

### Decision Tree
```
Need < 150ms latency?
├─ Yes → Use Production C++
└─ No → Need to modify/experiment frequently?
    ├─ Yes → Use Python Prototype
    └─ No → Use Optimized Python
```

## Model Requirements

All implementations use the same WeNet ONNX model:
- Input: Fbank features (80-dim)
- Output: Encoder states
- Vocabulary: Character-based or BPE

See `models/README.md` for model export instructions.

## Performance Benchmarks

Typical results on standard hardware:

| Implementation | Latency (p50) | Latency (p99) | Throughput |
|----------------|---------------|---------------|------------|
| Python Prototype | 250ms | 350ms | 0.8x RT |
| Optimized Python | 175ms | 225ms | 1.2x RT |
| Production C++ | 125ms | 150ms | 2.0x RT |

*RT = Real Time (1x = processes 1 second of audio in 1 second)*

## Contributing

When adding new implementations:
1. Create a new numbered directory
2. Include a README with approach details
3. Add performance metrics to comparison
4. Update this guide

## License

All samples share the same license as the toolkit.