#ifndef __IRIS_H__
#define __IRIS_H__
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

void iris_usart_init(void);
void iris_usart_deinit(void);
void iris_usart_send(const uint8_t* msg, size_t len);
size_t iris_usart_read(uint8_t* msg, size_t msg_buf_len);

// power up iris and initialize uart
void iris_open(void);
// power up iris and deinitialize uart
void iris_close(void);

#endif
