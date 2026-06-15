#include "iris.h"

void iris_usart_init(void) {}
void iris_usart_deinit(void) {}
int iris_usart_send(const uint8_t* msg, size_t len) {
    (void)msg;
    (void)len;
    return IRIS_ERROR_OK;
}
int iris_usart_read(uint8_t* msg, size_t msg_buf_len) {
    (void)msg;
    (void)msg_buf_len;
    return IRIS_ERROR_OK;
}

// power up iris and initialize uart
void iris_open(void) {}
// power up iris and deinitialize uart
void iris_close(void){}

int iris_sec_channel_open(const uint8_t *secret, size_t secret_size) {
    (void)secret;
    (void)secret_size;
    return 0;
}
