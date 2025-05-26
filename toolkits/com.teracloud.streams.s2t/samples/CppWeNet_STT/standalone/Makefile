CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I../../../impl/include
LDFLAGS = -L../../../impl/lib -lwenetcpp -pthread

TARGET = standalone_test
MODEL_DIR = models

all: $(TARGET)

$(TARGET): Standalone.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

create_model_dir:
	mkdir -p $(MODEL_DIR)
	# Create a dummy model file to allow the test to initialize
	echo "This is a placeholder for the real model files" > $(MODEL_DIR)/README.txt

run_with_mock: create_model_dir $(TARGET)
	LD_LIBRARY_PATH=../../../impl/lib:$$LD_LIBRARY_PATH ./$(TARGET)

run_with_real: $(TARGET)
	@echo "Switching to real model implementation..."
	$(CXX) -std=c++14 -Wall -Wextra -O2 -I../../../impl/include -o $(TARGET) Standalone.cpp $(LDFLAGS)
	@echo "Running with real model (requires model files in ./models)..."
	LD_LIBRARY_PATH=../../../impl/lib:$$LD_LIBRARY_PATH ./$(TARGET)

run: run_with_mock

# Setup the real implementation
setup_wenet:
	@echo "Setting up WeNet. This may take a while..."
	@chmod +x ./install_wenet_deps.sh
	./install_wenet_deps.sh --install-deps --build-wenet --download-model
	@echo "WeNet setup complete. You can now run 'make run_with_real'"

.PHONY: all clean create_model_dir run run_with_mock run_with_real setup_wenet