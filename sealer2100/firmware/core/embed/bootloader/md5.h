#ifndef __MD5_H__
#define __MD5_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t total[2];
    uint32_t state[4];
    uint8_t buffer[64];
} md5_context;

void md5_init( md5_context *ctx );
void md5_free( md5_context *ctx );
void md5_clone( md5_context *dst, const md5_context *src );
int md5_starts( md5_context *ctx );
int md5_update( md5_context *ctx, const uint8_t *input, size_t ilen );
int md5_finish( md5_context *ctx, uint8_t output[16] );

int md5_process( md5_context *ctx, const uint8_t data[64] );
int md5( const uint8_t *input, size_t ilen, uint8_t output[16] );

#ifdef __cplusplus
}
#endif

#endif // end of __MD5_H__
