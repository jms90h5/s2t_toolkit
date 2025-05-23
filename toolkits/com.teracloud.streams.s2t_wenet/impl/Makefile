# Makefile for building the WeNet ONNX implementation library

# Get toolkit directory
TOOLKIT_DIR := ..

# Include common definitions
include Makefile.common

# Source files
SOURCES = include/WenetONNXImpl.cpp

# Target library
LIB = libwenetonnx.so

# Compiler flags
CXXFLAGS += -fPIC -std=c++14 -Wall -Wextra
CXXFLAGS += -Iinclude
CXXFLAGS += -I$(ONNXRUNTIME_ROOT)/include

# Linker flags
LDFLAGS += -shared
LDFLAGS += -L$(ONNXRUNTIME_ROOT)/lib
LDFLAGS += -lonnxruntime
LDFLAGS += -Wl,-rpath,'$$ORIGIN'
LDFLAGS += -Wl,-rpath,$(ONNXRUNTIME_ROOT)/lib

# Build rules
all: $(LIB)

$(LIB): $(SOURCES:.cpp=.o)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(LIB)

.PHONY: all clean