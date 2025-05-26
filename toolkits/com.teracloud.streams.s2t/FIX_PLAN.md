# Comprehensive Plan to Fix the Speech-to-Text Toolkit

## Overview
The com.teracloud.streams.s2t toolkit has fundamental implementation issues that need to be fixed systematically. This plan documents all required fixes and tracks progress.

## Key Issues Identified

1. **Composite Operators have invalid SPL syntax**
   - AudioStreamSource, WebSocketSink, WenetSpeechToText define the same identifier as both a type and stream name
   - Missing proper output port definitions
   - Using non-existent functions like `createBlob()`

2. **Code Generation Templates are broken**
   - WenetSTT_cpp.pm has Perl errors (missing getType method)
   - WenetONNX implementation missing ONNX Runtime setup

3. **Sample Applications don't build**
   - Using reserved words ('timestamp') as identifiers
   - Type mismatches in operator invocations
   - Incorrect directory structures

4. **Missing Dependencies**
   - ONNX Runtime not properly integrated
   - kaldi-native-fbank setup incomplete

## Detailed Fix Plan

### Phase 1: Study and Understand SPL Properly
- [ ] Study SPL Language Reference for:
  - [ ] Reserved words list
  - [ ] Composite operator syntax
  - [ ] Type system and casting
  - [ ] Output port definitions
  - [ ] Built-in functions
- [ ] Analyze working samples in ~/teracloud/streams/7.2.0.0/samples
  - [ ] Study WordCount for basic patterns
  - [ ] Study toolkit samples for operator implementation patterns

### Phase 2: Fix Composite Operators

#### AudioStreamSource.spl
- [ ] Remove duplicate identifier (type vs stream name)
- [ ] Define output port properly
- [ ] Replace createBlob() with proper blob creation
- [ ] Test compilation independently

#### WebSocketSink.spl  
- [ ] Fix input port definition
- [ ] Implement proper sink pattern
- [ ] Test compilation independently

#### WenetSpeechToText.spl
- [ ] Fix composite operator structure
- [ ] Define proper input/output ports
- [ ] Test compilation independently

#### WenetRealtimeSTT.spl
- [ ] Fix composite operator structure
- [ ] Define proper types
- [ ] Test compilation independently

### Phase 3: Fix Primitive Operators

#### WenetSTT
- [ ] Debug WenetSTT_cpp.pm Perl code generation
- [ ] Fix getType method error
- [ ] Ensure C++ implementation matches interface
- [ ] Test code generation

#### WenetONNX
- [ ] Complete ONNX Runtime integration
- [ ] Fix header includes
- [ ] Implement proper initialization
- [ ] Test code generation

### Phase 4: Fix Dependencies
- [ ] Complete setup_onnx_runtime.sh
  - [ ] Fix download/extraction issues
  - [ ] Ensure headers are in correct location
- [ ] Complete setup_kaldi_fbank.sh
  - [ ] Build and install properly
  - [ ] Link with toolkit build
- [ ] Update CMakeLists_ONNX.txt
  - [ ] Proper dependency paths
  - [ ] Correct linking

### Phase 5: Fix Sample Applications

#### BasicWorkingExample
- [ ] Create minimal working sample with correct SPL
- [ ] No reserved words as identifiers
- [ ] Proper type usage
- [ ] Test build and run

#### CppWeNet_WenetSTT
- [ ] Fix all 'timestamp' reserved word usage
- [ ] Fix type mismatches
- [ ] Remove non-existent operator parameters
- [ ] Test build

#### CppONNX_WenetONNX
- [ ] Fix namespace issues
- [ ] Correct SPL syntax
- [ ] Test build

### Phase 6: Testing and Validation
- [ ] Build toolkit with spl-make-toolkit
- [ ] Test each operator independently
- [ ] Test sample applications
- [ ] Document working examples

## Implementation Order

1. **First**: Study SPL documentation and working examples
2. **Second**: Fix composite operators (simplest first)
3. **Third**: Fix primitive operator code generation
4. **Fourth**: Complete dependency setup
5. **Fifth**: Fix and test sample applications
6. **Sixth**: Full integration testing

## Success Criteria

- [ ] All composite operators compile without errors
- [ ] Primitive operators generate valid C++ code
- [ ] At least one sample application builds and runs
- [ ] Dependencies are properly installed and linked
- [ ] No SPL syntax errors or type mismatches

## Notes

- Always check SPL reserved words before using identifiers
- Test each component independently before integration
- Use working samples as reference for correct patterns
- Document all fixes for future reference

## Progress Tracking

### Completed Items
- None yet

### In Progress
- Studying SPL documentation

### Blocked Items
- None yet

---
Last Updated: [Current Date]