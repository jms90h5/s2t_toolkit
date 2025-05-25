# Makefile for SPL operators

# Find spl-make-toolkit
SPLMAKE := $(STREAMS_INSTALL)/bin/spl-make-toolkit
ifeq ($(wildcard $(SPLMAKE)),)
    $(error Cannot find spl-make-toolkit. Is STREAMS_INSTALL set correctly?)
endif

# Include implementation paths
TOOLKIT_DIR := ..
include $(TOOLKIT_DIR)/impl/Makefile.common

# Toolkit name
TOOLKIT = com.teracloud.streams.s2t_wenet

.PHONY: all clean

all:
	# Set up environment for ONNX Runtime
	@echo "Building with ONNX Runtime from: $(ONNXRUNTIME_ROOT)"
	# Copy implementation library to toolkit
	@mkdir -p $(TOOLKIT_DIR)/impl/lib
	@if [ -f $(TOOLKIT_DIR)/impl/include/libwenetonnx.so ]; then \
		cp $(TOOLKIT_DIR)/impl/include/libwenetonnx.so $(TOOLKIT_DIR)/impl/lib/; \
	fi
	# Copy ONNX Runtime library if using local version
	@if [ -f $(ONNXRUNTIME_ROOT)/lib/libonnxruntime.so ]; then \
		cp $(ONNXRUNTIME_ROOT)/lib/libonnxruntime.so* $(TOOLKIT_DIR)/impl/lib/; \
	fi
	# Build toolkit with library paths
	LD_LIBRARY_PATH=$(TOOLKIT_DIR)/impl/lib:$(ONNXRUNTIME_ROOT)/lib:$$LD_LIBRARY_PATH \
	$(SPLMAKE) -i .. -m -t $(TOOLKIT) \
		--cpp-additional-flags "-I$(ONNXRUNTIME_ROOT)/include" \
		--ld-additional-flags "-L$(TOOLKIT_DIR)/impl/lib -L$(ONNXRUNTIME_ROOT)/lib -lonnxruntime -Wl,-rpath,'$$ORIGIN/../impl/lib'"

clean:
	$(SPLMAKE) -i .. -c -t $(TOOLKIT)
	rm -rf $(TOOLKIT_DIR)/output