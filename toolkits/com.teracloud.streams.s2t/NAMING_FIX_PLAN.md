# Naming Convention Fix Plan

## Current Issues
1. **WenetONNX** - Misleading name since it doesn't use WeNet C++ API, only ONNX Runtime
2. **WenetRealtimeSTT** - Redundant "Realtime" since all Streams operators are real-time
3. Inconsistent naming between operators that use WeNet vs those that don't

## Proposed Changes

### 1. Rename WenetONNX → OnnxSTT
- **Rationale**: This operator uses ONNX Runtime directly, not WeNet's C++ API
- **Files to rename**:
  - `/com.teracloud.streams.s2t/WenetONNX/` → `/com.teracloud.streams.s2t/OnnxSTT/`
  - `WenetONNX_cpp.pm` → `OnnxSTT_cpp.pm`
  - `WenetONNX_h.pm` → `OnnxSTT_h.pm`
  - `WenetONNX_cpp.cgt` → `OnnxSTT_cpp.cgt`
  - `WenetONNX_h.cgt` → `OnnxSTT_h.cgt`
  - `WenetONNX.xml` → `OnnxSTT.xml`
  - `WenetONNXImpl.hpp` → `OnnxSTTImpl.hpp`
  - `WenetONNXImpl.cpp` → `OnnxSTTImpl.cpp`

### 2. Remove WenetRealtimeSTT
- **Rationale**: This composite is redundant with WenetSpeechToText
- **Action**: Delete this operator entirely as it adds no value

### 3. Keep These Names
- **WenetSTT** - Correct name, uses WeNet C++ API
- **WenetSpeechToText** - Composite wrapper for WenetSTT
- **AudioStreamSource** - Clear purpose
- **WebSocketSink** - Clear purpose

## Implementation Steps

1. **Update toolkit.xml**
   - Change WenetONNX references to OnnxSTT
   - Remove WenetRealtimeSTT entry

2. **Rename directories and files**
   - Move and rename all WenetONNX files to OnnxSTT

3. **Update code references**
   - Fix all imports and class names
   - Update namespace references

4. **Update documentation**
   - README.md
   - Sample documentation
   - Architecture docs

5. **Update samples**
   - Rename sample directories
   - Fix SPL imports

## Benefits
- Clear distinction between WeNet-based (WenetSTT) and ONNX-based (OnnxSTT) operators
- No redundant "Realtime" naming since everything is real-time
- Easier for users to understand which implementation they're using