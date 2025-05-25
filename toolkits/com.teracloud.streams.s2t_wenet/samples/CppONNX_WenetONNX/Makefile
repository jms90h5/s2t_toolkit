# Makefile for ONNX C++ sample

.PHONY: all clean run

# Application name
APP = ONNXRealtime

# SPL compiler
SC = $(STREAMS_INSTALL)/bin/sc

# Toolkit location
TOOLKIT = ../..

all: standalone

standalone:
	$(SC) -M -t $(TOOLKIT) --output-directory=output -m com.teracloud.streams.s2t_wenet.samples.onnx::$(APP)

run: all
	./output/bin/standalone

clean:
	rm -rf output