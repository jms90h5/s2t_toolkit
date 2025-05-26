# Makefile for ONNX Streams sample application

.PHONY: all clean submit cancel

# Application name matches the info.xml
APP_NAME = ONNXDemo

# Streams variables
STREAMS_INSTALL ?= $(shell echo $$STREAMS_INSTALL)
STREAMTOOL = $(STREAMS_INSTALL)/bin/streamtool

# Build directory
BUILD_DIR = output

all: $(BUILD_DIR)/$(APP_NAME).sab

# Build the Streams application bundle
$(BUILD_DIR)/$(APP_NAME).sab:
	@echo "Building Streams application: $(APP_NAME)"
	@mkdir -p $(BUILD_DIR)
	$(STREAMTOOL) mkappbundle --sab-file $(BUILD_DIR)/$(APP_NAME).sab .

# Submit job to Streams instance  
submit: $(BUILD_DIR)/$(APP_NAME).sab
	@echo "Submitting $(APP_NAME) to Streams..."
	$(STREAMTOOL) submitjob $(BUILD_DIR)/$(APP_NAME).sab

# Cancel running jobs
cancel:
	@echo "Cancelling $(APP_NAME) jobs..."
	$(STREAMTOOL) canceljob -f --jobname $(APP_NAME) 2>/dev/null || true

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build the application bundle (.sab file)"
	@echo "  submit  - Submit the application to Streams"
	@echo "  cancel  - Cancel running application jobs"
	@echo "  clean   - Remove build artifacts"
	@echo ""
	@echo "Usage:"
	@echo "  make all          # Build the application"
	@echo "  make submit       # Build and submit to Streams"
	@echo "  make cancel clean # Stop and clean"