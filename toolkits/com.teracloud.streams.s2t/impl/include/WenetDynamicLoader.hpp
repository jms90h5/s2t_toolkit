#ifndef WENET_DYNAMIC_LOADER_HPP
#define WENET_DYNAMIC_LOADER_HPP

namespace wenet_streams {

class WenetDynamicLoader {
public:
    WenetDynamicLoader();
    ~WenetDynamicLoader();
    
    // WeNet API function wrappers
    void* wenet_init(const char* model_dir);
    void wenet_free(void* decoder);
    const char* wenet_decode(void* decoder, const char* data, int len, int last);
    void wenet_reset(void* decoder);
    void wenet_set_chunk_size(void* decoder, int chunk_size);
    void wenet_set_continuous_decoding(void* decoder, int flag);
    void wenet_set_timestamp(void* decoder, int flag);
    
private:
    void* libHandle_;
    
    // Function pointer types
    typedef void* (*wenet_init_t)(const char*);
    typedef void (*wenet_free_t)(void*);
    typedef const char* (*wenet_decode_t)(void*, const char*, int, int);
    typedef void (*wenet_reset_t)(void*);
    typedef void (*wenet_set_chunk_size_t)(void*, int);
    typedef void (*wenet_set_continuous_decoding_t)(void*, int);
    typedef void (*wenet_set_timestamp_t)(void*, int);
    
    // Function pointers
    wenet_init_t wenet_init_;
    wenet_free_t wenet_free_;
    wenet_decode_t wenet_decode_;
    wenet_reset_t wenet_reset_;
    wenet_set_chunk_size_t wenet_set_chunk_size_;
    wenet_set_continuous_decoding_t wenet_set_continuous_decoding_;
    wenet_set_timestamp_t wenet_set_timestamp_;
};

} // namespace wenet_streams

#endif // WENET_DYNAMIC_LOADER_HPP