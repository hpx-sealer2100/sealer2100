#include "iris.h"

void iris_usart_init(void) {}
void iris_usart_deinit(void) {}
void iris_usart_send(const uint8_t* msg, size_t len) {
    (void)msg;
    (void)len;
}
size_t iris_usart_read(uint8_t* msg, size_t msg_buf_len) {
    (void)msg;
    (void)msg_buf_len;
    return 0;
}

// power up iris and initialize uart
void iris_open(void) {}
// power up iris and deinitialize uart
void iris_close(void){}