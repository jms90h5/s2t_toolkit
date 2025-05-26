#include "WenetDynamicLoader.hpp"
#include <dlfcn.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

namespace wenet_streams {

WenetDynamicLoader::WenetDynamicLoader() : libHandle_(nullptr) {
    // Load the WeNet library dynamically
    // Try multiple paths to find the library
    const char* paths[] = {
        "/homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl/lib/libwenetcpp.so",
        "impl/lib/libwenetcpp.so",
        "lib/libwenetcpp.so",
        "./libwenetcpp.so",
        nullptr
    };
    
    const char* libPath = nullptr;
    for (int i = 0; paths[i] != nullptr; i++) {
        if (access(paths[i], F_OK) == 0) {
            libPath = paths[i];
            break;
        }
    }
    
    if (!libPath) {
        throw std::runtime_error("Cannot find WeNet library in any of the expected paths");
    }
    libHandle_ = dlopen(libPath, RTLD_LAZY);
    
    if (!libHandle_) {
        throw std::runtime_error("Cannot load WeNet library: " + std::string(dlerror()));
    }
    
    // Load function pointers
    wenet_init_ = (wenet_init_t)dlsym(libHandle_, "wenet_init");
    wenet_free_ = (wenet_free_t)dlsym(libHandle_, "wenet_free");
    wenet_decode_ = (wenet_decode_t)dlsym(libHandle_, "wenet_decode");
    wenet_reset_ = (wenet_reset_t)dlsym(libHandle_, "wenet_reset");
    wenet_set_chunk_size_ = (wenet_set_chunk_size_t)dlsym(libHandle_, "wenet_set_chunk_size");
    wenet_set_continuous_decoding_ = (wenet_set_continuous_decoding_t)dlsym(libHandle_, "wenet_set_continuous_decoding");
    wenet_set_timestamp_ = (wenet_set_timestamp_t)dlsym(libHandle_, "wenet_set_timestamp");
    
    if (!wenet_init_ || !wenet_free_ || !wenet_decode_ || !wenet_reset_ ||
        !wenet_set_chunk_size_ || !wenet_set_continuous_decoding_ || !wenet_set_timestamp_) {
        dlclose(libHandle_);
        throw std::runtime_error("Cannot load WeNet function symbols");
    }
    
    std::cout << "WeNet library loaded successfully" << std::endl;
}

WenetDynamicLoader::~WenetDynamicLoader() {
    if (libHandle_) {
        dlclose(libHandle_);
    }
}

void* WenetDynamicLoader::wenet_init(const char* model_dir) {
    return wenet_init_(model_dir);
}

void WenetDynamicLoader::wenet_free(void* decoder) {
    wenet_free_(decoder);
}

const char* WenetDynamicLoader::wenet_decode(void* decoder, const char* data, int len, int last) {
    return wenet_decode_(decoder, data, len, last);
}

void WenetDynamicLoader::wenet_reset(void* decoder) {
    wenet_reset_(decoder);
}

void WenetDynamicLoader::wenet_set_chunk_size(void* decoder, int chunk_size) {
    wenet_set_chunk_size_(decoder, chunk_size);
}

void WenetDynamicLoader::wenet_set_continuous_decoding(void* decoder, int flag) {
    wenet_set_continuous_decoding_(decoder, flag);
}

void WenetDynamicLoader::wenet_set_timestamp(void* decoder, int flag) {
    wenet_set_timestamp_(decoder, flag);
}

} // namespace wenet_streams