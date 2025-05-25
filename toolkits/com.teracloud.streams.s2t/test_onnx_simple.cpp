#include <iostream>
#include "onnxruntime_cxx_api.h"

int main() {
    std::cout << "Testing ONNX Runtime..." << std::endl;
    
    try {
        // Create ONNX Runtime environment
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
        std::cout << "✓ ONNX Runtime environment created successfully!" << std::endl;
        
        // Create session options
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        std::cout << "✓ Session options configured" << std::endl;
        
        // Show version
        std::cout << "ONNX Runtime API Version: " << ORT_API_VERSION << std::endl;
        
        std::cout << "\nONNX Runtime is properly installed and working!" << std::endl;
        std::cout << "\nThe issue is that the WenetONNXImpl needs the include path configured differently." << std::endl;
        std::cout << "In a real Streams build, this would be handled by CMake." << std::endl;
        
        return 0;
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime error: " << e.what() << std::endl;
        return 1;
    }
}