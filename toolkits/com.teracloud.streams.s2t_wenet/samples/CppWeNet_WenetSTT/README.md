# Real-time Speech-to-Text Transcriber Sample

This sample demonstrates real-time speech recognition using the WeNet toolkit with TeraCloud Streams. It receives streaming audio data, performs real-time transcription with incremental results, and sends the transcriptions to a WebSocket for visualization.

## Overview

The `RealtimeTranscriber` application showcases:

- Real-time audio streaming ingestion
- Low-latency speech transcription with WeNet
- Incremental transcription results for real-time feedback
- WebSocket integration for live visualization
- Performance metrics for monitoring

## Prerequisites

- TeraCloud Streams 7.x
- WeNet model (see setup instructions below)
- WebSocket audio source (or use the included simulator)
- WebSocket visualization client (or use the included HTML viewer)

## Setup

### 1. Download WeNet Model

Download a pre-trained WeNet model:

```bash
mkdir -p $HOME/wenet_models
wget -P $HOME/wenet_models https://wenet-1256283475.cos.ap-shanghai.myqcloud.com/models/wenetspeech/wenetspeech_u2pp_conformer_libtorch.tar.gz
tar -xzf $HOME/wenet_models/wenetspeech_u2pp_conformer_libtorch.tar.gz -C $HOME/wenet_models/
```

### 2. Compile the Sample

Navigate to the sample directory and compile:

```bash
cd samples/RealtimeTranscriber
sc -M com.teracloud.streams.s2t.sample::RealtimeTranscriber -t $STREAMS_INSTALL/toolkits:../../
```

## Running the Sample

The sample includes a convenience script to run the application:

```bash
./etc/run-transcriber.sh [MODEL_PATH] [AUDIO_ENDPOINT] [OUTPUT_ENDPOINT]
```

Example:

```bash
./etc/run-transcriber.sh $HOME/wenet_models/wenetspeech_u2pp_conformer_libtorch ws://audio-source:9000/stream ws://localhost:8080/transcription
```

### Parameters:

- `MODEL_PATH`: Path to the WeNet model directory (default: $HOME/wenet_models/u2pp_conformer_libtorch)
- `AUDIO_ENDPOINT`: WebSocket URL for audio input (default: ws://audio-source:9000/stream)
- `OUTPUT_ENDPOINT`: WebSocket URL for transcription output (default: ws://localhost:8080/transcription)

## Visualization

The real-time transcription results can be visualized using the included HTML viewer:

```bash
cd etc
python -m http.server 8080
```

Then open your web browser and navigate to http://localhost:8080/viewer.html

## Audio Simulator

If you don't have a real audio source, you can use the included audio simulator to generate test audio:

```bash
cd etc
./audio-simulator.sh
```

This will start a WebSocket server that simulates audio data from sample files.

## Output

The application produces:

1. Real-time transcription results sent to the WebSocket endpoint
2. Final transcription results saved to `transcription_results.csv`
3. Periodic performance metrics logged to the console

## Customization

You can customize the application by modifying:

- `RealtimeTranscriber.spl`: Main application logic
- `etc/viewer.html`: Visualization client
- Parameters passed to the application when starting it