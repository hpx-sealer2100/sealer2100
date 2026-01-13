#ifndef __THD89_H__
#define __THD89_H__

#include <stddef.h>
#include <stdint.h>

typedef enum {
  THD89_SUCCESS = 0,
  THD89_ERR_TIMEOUT,
  THD89_ERR_TRANS_FAILD,
  THD89_ERR_BUFFER_SMALL,
  THD89_ERR_DATA_TOO_LONG,
  THD89_ERR_CRC,
  THD89_ERR_UNEXPECTED,
} thd89_result_t;

thd89_result_t thd89_init(void);
void thd89_deinit(void);
thd89_result_t thd89_ping(void);
thd89_result_t thd89_reset(void);
thd89_result_t thd89_handshake(const uint8_t* secret, size_t secret_size);
thd89_result_t thd89_execute_command(const uint8_t* command,
                                     size_t command_size, uint8_t* response,
                                     size_t response_buf_size,
                                     size_t* response_len);

#endif
