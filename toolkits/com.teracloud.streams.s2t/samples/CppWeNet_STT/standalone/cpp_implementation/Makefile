CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -I. -I/homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t_wenet/impl/include
LDFLAGS = -L./lib -lwenet -pthread

TARGET = standalone_cpp
MODEL_DIR = ../models

all: $(TARGET)

$(TARGET): ../Standalone.cpp WenetCppImpl.cpp WenetSTTImpl.hpp
	$(CXX) $(CXXFLAGS) -o $@ ../Standalone.cpp WenetCppImpl.cpp $(LDFLAGS)

run: $(TARGET)
	LD_LIBRARY_PATH=./lib:$$LD_LIBRARY_PATH ./$(TARGET) --model_path $(MODEL_DIR)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
