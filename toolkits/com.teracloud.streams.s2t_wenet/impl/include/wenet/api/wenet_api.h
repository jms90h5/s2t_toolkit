#ifndef WENET_API_H_
#define WENET_API_H_

#ifdef __cplusplus
extern "C" {
#endif

void* wenet_init(const char* model_dir);
void wenet_free(void* decoder);
void wenet_reset(void* decoder);
const char* wenet_decode(void* decoder, const char* data, int len, int last);
const char* wenet_get_result(void* decoder);
void wenet_set_log_level(int level);
void wenet_set_nbest(void* decoder, int n);
void wenet_set_timestamp(void* decoder, int flag);
void wenet_set_max_active(void* decoder, int max_active);
void wenet_set_output_chunk_size(void* decoder, int size);
void wenet_set_continuous_decoding(void* decoder, int flag);
void wenet_set_chunk_size(void* decoder, int chunk_size);

#ifdef __cplusplus
}
#endif

#endif  // WENET_API_H_
