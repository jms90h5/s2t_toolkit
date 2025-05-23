# Main Makefile for WeNet ONNX Toolkit

.PHONY: all clean

# Build all components
all:
	# First ensure ONNX Runtime is available
	@if [ ! -d "deps/onnxruntime" ]; then \
		echo "ONNX Runtime not found. Running setup..."; \
		./setup_onnx_runtime.sh || exit 1; \
	fi
	# Build implementation library
	$(MAKE) -C impl/include -f ../Makefile
	# Build toolkit (SPL operators)
	$(MAKE) -C com.teracloud.streams.s2t_wenet

clean:
	$(MAKE) -C impl/include -f ../Makefile clean
	$(MAKE) -C com.teracloud.streams.s2t_wenet clean
	rm -rf output

# Install ONNX Runtime
setup:
	./setup_onnx_runtime.sh

# Build samples
samples: all
	$(MAKE) -C samples/05_ONNXCppOperator