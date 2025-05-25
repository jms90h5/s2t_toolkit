CXX = g++
PYTHON_INCLUDES = $(shell python3-config --includes)
PYTHON_LDFLAGS = $(shell python3-config --ldflags)
CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I. -I/homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t_wenet/impl/include $(PYTHON_INCLUDES)
LDFLAGS = $(PYTHON_LDFLAGS) -pthread -lpython3.9

TARGET = standalone_real
SOURCE_DIR = ..
MODEL_DIR = ../models

all: $(TARGET)

$(TARGET): Standalone.cpp WenetRealImpl.cpp
	$(CXX) $(CXXFLAGS) -o $@ Standalone.cpp WenetRealImpl.cpp $(LDFLAGS)

run: $(TARGET)
	export LD_LIBRARY_PATH=$(shell python3-config --prefix)/lib:$$LD_LIBRARY_PATH && \
	./$(TARGET) --model_path $(MODEL_DIR)

clean:
	rm -f $(TARGET) wenet_loader.py

.PHONY: all run clean
