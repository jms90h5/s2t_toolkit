# Speech-to-Text Toolkit Documentation

This directory contains detailed technical documentation for the TeraCloud Streams Speech-to-Text Toolkit.

## Documentation Index

### Architecture and Implementation
- [**IMPLEMENTATION_DETAILS.md**](IMPLEMENTATION_DETAILS.md) - Comprehensive comparison of WenetONNX vs WenetSTT implementations
- [**ONNX_ARCHITECTURE.md**](ONNX_ARCHITECTURE.md) - Deep dive into the ONNX Runtime-based implementation

### Development History
- [**IMPROVEMENTS_V2.md**](IMPROVEMENTS_V2.md) - Version 2.0 improvements including memory safety and performance enhancements
- [**IMPROVEMENTS_SUMMARY.md**](IMPROVEMENTS_SUMMARY.md) - Summary of all toolkit improvements
- [**REAL_TIME_IMPROVEMENTS.md**](REAL_TIME_IMPROVEMENTS.md) - Specific improvements for real-time processing
- [**CORRECTED_SUMMARY.md**](CORRECTED_SUMMARY.md) - Corrections and updates made to the toolkit
- [**TESTING_SUMMARY.md**](TESTING_SUMMARY.md) - Testing approach and results

## Quick Reference

### Which Document Should I Read?

- **New users**: Start with the main [README.md](../README.md), then read [IMPLEMENTATION_DETAILS.md](IMPLEMENTATION_DETAILS.md)
- **Choosing an implementation**: Read [IMPLEMENTATION_DETAILS.md](IMPLEMENTATION_DETAILS.md) 
- **Understanding ONNX approach**: Read [ONNX_ARCHITECTURE.md](ONNX_ARCHITECTURE.md)
- **Performance tuning**: Check [REAL_TIME_IMPROVEMENTS.md](REAL_TIME_IMPROVEMENTS.md)
- **Development history**: Browse the improvement and summary documents

## Key Topics Covered

1. **Implementation Approaches**
   - WenetONNX (ONNX Runtime-based)
   - WenetSTT (Full WeNet integration)
   - Python prototypes

2. **Performance Optimization**
   - Memory management
   - Threading models
   - Latency reduction techniques

3. **Deployment Options**
   - Self-contained packaging
   - Dependency management
   - Cross-platform support

4. **Best Practices**
   - Real-time processing patterns
   - Error handling
   - Resource management