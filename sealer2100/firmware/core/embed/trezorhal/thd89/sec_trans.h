#ifndef __SEC_TRANS_H__
#define __SEC_TRANS_H__

#include <stdint.h>
#include <stddef.h>

// packet size: 256
// PCTR[1]|<data>
// FCTR[1]|<LEN[2]>|<packet>|FCS[2]
#define SEC_MAX_FRAME_SIZE (256+5+1)

#define SEC_TRANS_SUCCESS 0
#define SEC_TRANS_ERR_TIMEOUT 1
#define SEC_TRANS_ERR_FAILED  2
// 这里只定义两个接口，由具体slave去实现，比如mock、uart、spi

// 1. 数据 主 -> 从 写函数
int sec_trans_write(const uint8_t* frame, size_t frame_size, uint32_t timeout);

// 2. 数据 主 <- 从 读函数
int sec_trans_read(uint8_t* frame, size_t frame_buf_size, uint32_t timeout);

#endif