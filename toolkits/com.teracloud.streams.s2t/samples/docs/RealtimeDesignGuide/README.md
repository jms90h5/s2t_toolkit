# Real-time vs Batch Processing Comparison

This document explains why batching is **inappropriate** for real-time speech-to-text applications.

## The Problem with Batching in Real-time STT

### What is Real-time STT?
Real-time speech-to-text means:
- Audio is processed **as it arrives**
- Results are delivered with **minimal latency**
- Users see transcription **while speaking**

### Why Batching Breaks Real-time
```
Timeline with batching (BAD):
Time:    0ms   100ms  200ms  300ms  400ms  500ms
Audio:   [...] [...] [...] [...] [...] [...]
Process:                            [------]  <- Wait 500ms, then process
Result:                                   [Text]
Latency: 500ms+ 😱
```

```
Timeline without batching (GOOD):
Time:    0ms   100ms  200ms  300ms  400ms  500ms
Audio:   [...] [...] [...] [...] [...] [...]
Process: [--]  [--]  [--]  [--]  [--]  [--]
Result:  [T]   [Te]  [Tes] [Test] [Test ] [Test text]
Latency: ~100ms 🚀
```

## Correct Real-time Implementation

### 1. Process Each Chunk Immediately
```cpp
void processAudio(const int16_t* samples, size_t num_samples, uint64_t timestamp) {
    // Add to minimal buffer (just for chunk alignment)
    audio_buffer_.insert(audio_buffer_.end(), samples, samples + num_samples);
    
    // Process AS SOON AS we have minimum chunk size
    if (audio_buffer_.size() >= chunk_samples) {
        // Process NOW - don't wait for more chunks!
        wenet_decode(decoder, audio_buffer_.data(), audio_buffer_.size(), 0);
        audio_buffer_.clear();
    }
}
```

### 2. Stream Partial Results
```cpp
// Enable continuous decoding for streaming results
wenet_set_continuous_decoding(decoder, 1);

// Get partial results frequently
if (enable_partial_results) {
    // Decode with is_last=0 for partial results
    const char* result = wenet_decode(decoder, audio_data, size, 0);
    sendPartialResult(result);
}
```

### 3. Minimize Buffering
- Only buffer to align with model's chunk size (typically 10-200ms)
- Never accumulate multiple chunks before processing
- Use smallest viable chunk size for lowest latency

## Performance Metrics

### Latency Targets
- **Excellent**: < 150ms (feels instantaneous)
- **Good**: 150-300ms (barely noticeable)
- **Acceptable**: 300-500ms (slight delay)
- **Poor**: > 500ms (noticeable lag)

### Real-time Requirements
1. **Consistent latency** - No spikes
2. **Incremental results** - See words as spoken
3. **Low memory usage** - Minimal buffering
4. **CPU efficiency** - Process faster than real-time

## Example: Live Transcription

```spl
composite LiveTranscriber {
    graph
        // Audio source - 100ms chunks
        stream<blob audio, uint64 timestamp> Audio = AudioSource() {
            param
                chunkDuration: 0.1; // 100ms chunks
        }
        
        // Real-time transcription - NO BATCHING
        stream<rstring text, uint64 latencyMs> Transcripts = 
            WenetRealtimeSTT(Audio) {
            param
                modelPath: "/models/wenet";
                chunkMs: 100;        // Process every 100ms
                enablePartial: true; // Stream results
        }
        
        // Display with latency info
        () as Display = Custom(Transcripts) {
            logic onTuple Transcripts: {
                println("Text: " + text + " (latency: " + 
                       (rstring)latencyMs + "ms)");
            }
        }
}
```

## Conclusion

For real-time STT:
- **Never batch** audio chunks
- **Process immediately** when enough audio arrives
- **Optimize for latency**, not throughput
- **Stream partial results** for responsive UX

The original implementation's batching approach would add 100s of milliseconds of unnecessary latency, making it unsuitable for real-time applications like:
- Live captioning
- Voice assistants
- Real-time translation
- Interactive voice response