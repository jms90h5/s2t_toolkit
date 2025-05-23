#ifdef MOCK_WENET_IMPLEMENTATION

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

// Mock implementation of the WeNet API for testing purposes
extern "C" {

void* wenet_init(const char* model_dir) {
    std::cout << "MOCK: wenet_init called with model_dir=" << model_dir << std::endl;
    // Return a non-null pointer to indicate success
    static int dummy = 0;
    return &dummy;
}

void wenet_free(void* decoder) {
    std::cout << "MOCK: wenet_free called" << std::endl;
}

void wenet_reset(void* decoder) {
    std::cout << "MOCK: wenet_reset called" << std::endl;
}

const char* wenet_decode(void* decoder, const char* data, int len, int last) {
    std::cout << "MOCK: wenet_decode called with len=" << len << ", last=" << last << std::endl;
    
    // Return a mock JSON result
    if (last) {
        static const char* final_result = "{"
            "\"type\": \"final_result\","
            "\"nbest\": ["
                "{"
                    "\"sentence\": \"This is a mock final transcription result\","
                    "\"word_pieces\": ["
                        "{\"word\": \"This\", \"start\": 0, \"end\": 400},"
                        "{\"word\": \"is\", \"start\": 450, \"end\": 600},"
                        "{\"word\": \"a\", \"start\": 650, \"end\": 700},"
                        "{\"word\": \"mock\", \"start\": 750, \"end\": 900},"
                        "{\"word\": \"final\", \"start\": 950, \"end\": 1100},"
                        "{\"word\": \"transcription\", \"start\": 1150, \"end\": 1800},"
                        "{\"word\": \"result\", \"start\": 1850, \"end\": 2000}"
                    "]"
                "},"
                "{"
                    "\"sentence\": \"This is a mock final result\""
                "}"
            "]"
        "}";
        return final_result;
    } else {
        static const char* partial_result = "{"
            "\"type\": \"partial_result\","
            "\"nbest\": ["
                "{"
                    "\"sentence\": \"This is a mock partial result\""
                "}"
            "]"
        "}";
        return partial_result;
    }
}

const char* wenet_get_result(void* decoder) {
    std::cout << "MOCK: wenet_get_result called" << std::endl;
    static const char* result = "{"
        "\"type\": \"final_result\","
        "\"nbest\": ["
            "{"
                "\"sentence\": \"This is a mock result from get_result\""
            "}"
        "]"
    "}";
    return result;
}

void wenet_set_nbest(void* decoder, int n) {
    std::cout << "MOCK: wenet_set_nbest called with n=" << n << std::endl;
}

void wenet_set_timestamp(void* decoder, int flag) {
    std::cout << "MOCK: wenet_set_timestamp called with flag=" << flag << std::endl;
}

void wenet_set_continuous_decoding(void* decoder, int flag) {
    std::cout << "MOCK: wenet_set_continuous_decoding called with flag=" << flag << std::endl;
}

void wenet_set_chunk_size(void* decoder, int chunk_size) {
    std::cout << "MOCK: wenet_set_chunk_size called with chunk_size=" << chunk_size << std::endl;
}

} // extern "C"

#endif // MOCK_WENET_IMPLEMENTATION