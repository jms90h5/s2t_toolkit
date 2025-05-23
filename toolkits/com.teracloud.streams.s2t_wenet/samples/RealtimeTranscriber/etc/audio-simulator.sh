#!/bin/bash

# Simple script to simulate an audio stream for the RealtimeTranscriber sample

echo "Starting audio simulator WebSocket server on port 9000..."
echo "This will generate sample audio data for testing purposes."

# In a real implementation, this would use actual WebSocket server
# For demonstration purposes, we'll just show a placeholder message

echo "Audio simulator is now running"
echo "Sending simulated audio chunks to ws://localhost:9000/stream"
echo "Press Ctrl+C to stop"

# Infinite loop to simulate running server
while true; do
    sleep 1
    echo -n "."
done