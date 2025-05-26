#include <iostream>
#include <dlfcn.h>

int main() {
    // Try to load the library
    void* handle = dlopen("/homes/jsharpe/teracloud/toolkits/com.teracloud.streams.s2t/impl/lib/libwenetcpp.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot load library: " << dlerror() << std::endl;
        return 1;
    }
    
    // Get the wenet_set_log_level function
    typedef void (*wenet_set_log_level_t)(int);
    wenet_set_log_level_t wenet_set_log_level = (wenet_set_log_level_t)dlsym(handle, "wenet_set_log_level");
    
    if (!wenet_set_log_level) {
        std::cerr << "Cannot find wenet_set_log_level: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    
    // Set log level to 2 (INFO)
    std::cout << "Setting WeNet log level to 2 (INFO)" << std::endl;
    wenet_set_log_level(2);
    
    std::cout << "WeNet library loaded and basic function call successful!" << std::endl;
    
    dlclose(handle);
    return 0;
}