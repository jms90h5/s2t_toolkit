# Basic Working Example

This directory contains basic examples demonstrating the speech-to-text toolkit.

## Important Note about Audio Files

The `FileAudioSource` operator reads raw PCM audio data using FileSource's block format.
It does NOT parse WAV file headers. If you're using WAV files, you need to:

1. Convert them to raw PCM format first, OR
2. Skip the WAV header (typically 44 bytes) when reading

For testing, you can convert WAV to raw PCM using:
```bash
sox input.wav -t raw -r 16000 -e signed -b 16 -c 1 output.raw
```

## Running the Samples

1. Build the sample:
   ```bash
   source $STREAMS_INSTALL/bin/streamsprofile.sh
   make
   ```

2. Run the sample:
   ```bash
   output/bin/standalone
   ```

## Sample Applications

- `SimpleSTT.spl` - Minimal example showing basic STT usage
- `BasicDemo.spl` - More complete demo with file output and monitoring