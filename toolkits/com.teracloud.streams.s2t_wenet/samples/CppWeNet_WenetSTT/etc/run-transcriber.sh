#!/bin/bash

# Sample script to run the RealtimeTranscriber application

# Set the path to the WeNet model directory
MODEL_PATH=${1:-"$HOME/wenet_models/u2pp_conformer_libtorch"}

# Set the endpoint for audio input
AUDIO_ENDPOINT=${2:-"ws://audio-source:9000/stream"}

# Set the endpoint for transcription output
OUTPUT_ENDPOINT=${3:-"ws://localhost:8080/transcription"}

# Check if the model directory exists
if [ ! -d "$MODEL_PATH" ]; then
    echo "Error: Model directory does not exist at $MODEL_PATH"
    echo "Please download the WeNet model using the instructions in the README.md file."
    exit 1
fi

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
SAMPLE_DIR="$(dirname "$SCRIPT_DIR")"

# Compile the application if needed
if [ ! -f "$SAMPLE_DIR/output/RealtimeTranscriber.sab" ]; then
    echo "Compiling RealtimeTranscriber application..."
    cd "$SAMPLE_DIR"
    sc -M com.teracloud.streams.s2t.sample::RealtimeTranscriber -t $STREAMS_INSTALL/toolkits:../../
    if [ $? -ne 0 ]; then
        echo "Compilation failed. Please check the output for errors."
        exit 1
    fi
fi

# Submit the job to Streams
echo "Submitting RealtimeTranscriber job to Streams..."
streamtool submitjob "$SAMPLE_DIR/output/com.teracloud.streams.s2t.sample.RealtimeTranscriber.sab" \
    -P modelPath="$MODEL_PATH" \
    -P audioEndpoint="$AUDIO_ENDPOINT" \
    -P outputEndpoint="$OUTPUT_ENDPOINT"

if [ $? -eq 0 ]; then
    echo "Job submitted successfully."
    echo "Real-time transcription is now running."
    echo "Transcription results will be sent to $OUTPUT_ENDPOINT"
    echo "Final results will also be saved to transcription_results.csv"
else
    echo "Job submission failed. Please check the output for errors."
    exit 1
fi