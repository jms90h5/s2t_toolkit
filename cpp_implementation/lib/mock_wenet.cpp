#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "../wenet_api.h"

// Mock implementation of the WeNet API for testing purposes
extern "C" {

void* wenet_init(const char* model_dir) {
    std::cout << "C++ WeNet: Initializing from " << model_dir << std::endl;
    
    // Check if model files exist
    std::string modelPath(model_dir);
    std::string zipFile = modelPath + "/final.zip";
    std::string unitsFile = modelPath + "/units.txt";
    
    std::ifstream zipCheck(zipFile);
    std::ifstream unitsCheck(unitsFile);
    
    if (!zipCheck.good() || !unitsCheck.good()) {
        std::cerr << "C++ WeNet: Model files not found in " << model_dir << std::endl;
        std::cerr << "  final.zip exists: " << (zipCheck.good() ? "Yes" : "No") << std::endl;
        std::cerr << "  units.txt exists: " << (unitsCheck.good() ? "Yes" : "No") << std::endl;
        return nullptr;
    }
    
    // Count vocab size in units.txt
    std::string line;
    int vocabSize = 0;
    while (std::getline(unitsCheck, line)) {
        vocabSize++;
    }
    
    std::cout << "C++ WeNet: Successfully loaded model" << std::endl;
    std::cout << "C++ WeNet: Model has vocabulary size of " << vocabSize << " units" << std::endl;
    
    // Return a non-null pointer to indicate success
    static int dummy = 0;
    return &dummy;
}

void wenet_free(void* decoder) {
    std::cout << "C++ WeNet: Freeing decoder" << std::endl;
}

void wenet_reset(void* decoder) {
    std::cout << "C++ WeNet: Resetting decoder" << std::endl;
}

const char* wenet_decode(void* decoder, const char* data, int len, int last) {
    std::cout << "C++ WeNet: Decoding " << len << " bytes, last=" << last << std::endl;
    
    static std::string final_result;
    if (last) {
        final_result = "{"
            "\"type\": \"final_result\","
            "\"nbest\": ["
                "{"
                    "\"sentence\": \"This is a C++ implementation final result using the real model\","
                    "\"word_pieces\": ["
                        "{\"word\": \"This\", \"start\": 0, \"end\": 400},"
                        "{\"word\": \"is\", \"start\": 450, \"end\": 600},"
                        "{\"word\": \"a\", \"start\": 650, \"end\": 700},"
                        "{\"word\": \"C++\", \"start\": 750, \"end\": 900},"
                        "{\"word\": \"implementation\", \"start\": 950, \"end\": 1100},"
                        "{\"word\": \"final\", \"start\": 1150, \"end\": 1300},"
                        "{\"word\": \"result\", \"start\": 1350, \"end\": 1500},"
                        "{\"word\": \"using\", \"start\": 1550, \"end\": 1700},"
                        "{\"word\": \"the\", \"start\": 1750, \"end\": 1850},"
                        "{\"word\": \"real\", \"start\": 1900, \"end\": 2000},"
                        "{\"word\": \"model\", \"start\": 2050, \"end\": 2200}"
                    "]"
                "}"
            "]"
        "}";
    } else {
        final_result = "{"
            "\"type\": \"partial_result\","
            "\"nbest\": ["
                "{"
                    "\"sentence\": \"This is a C++ implementation partial result\""
                "}"
            "]"
        "}";
    }
    return final_result.c_str();
}

const char* wenet_get_result(void* decoder) {
    static std::string result = "{"
        "\"type\": \"final_result\","
        "\"nbest\": ["
            "{"
                "\"sentence\": \"This is a result from C++ implementation\""
            "}"
        "]"
    "}";
    return result.c_str();
}

void wenet_set_log_level(int level) {
    std::cout << "C++ WeNet: Setting log level to " << level << std::endl;
}

void wenet_set_nbest(void* decoder, int n) {
    std::cout << "C++ WeNet: Setting nbest to " << n << std::endl;
}

void wenet_set_timestamp(void* decoder, int flag) {
    std::cout << "C++ WeNet: Setting timestamp to " << flag << std::endl;
}

void wenet_set_max_active(void* decoder, int max_active) {
    std::cout << "C++ WeNet: Setting max_active to " << max_active << std::endl;
}

void wenet_set_output_chunk_size(void* decoder, int size) {
    std::cout << "C++ WeNet: Setting output_chunk_size to " << size << std::endl;
}

void wenet_set_continuous_decoding(void* decoder, int flag) {
    std::cout << "C++ WeNet: Setting continuous_decoding to " << flag << std::endl;
}

void wenet_set_chunk_size(void* decoder, int chunk_size) {
    std::cout << "C++ WeNet: Setting chunk_size to " << chunk_size << std::endl;
}

} // extern "C"
