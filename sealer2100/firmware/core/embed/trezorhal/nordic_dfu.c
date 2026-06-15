#include STM32_HAL_H

#include <assert.h>

#include "alignment.h"
#include "log.h"
#include "nordic_dfu.h"
#include "common.h"
#include "display.h"
#include "spi.h"
#include "usart.h"
#include "ble.h"

#define MODULE "NRF dfu"

#define NRF_DFU_SHOW_TRANSFER 0
#if !NRF_DFU_SHOW_TRANSFER
#undef LOG_HEXDUMP_DEBUG
#define LOG_HEXDUMP_DEBUG(...) ((void)0)
#endif

#define PRN 0
#define PACKET_SIZE 64

#define CMD_INIT(BUF, CMD) do {   \
  BUF[0] = CMD;                       \
}while(0)

#define RESP_CHECK(RESP, OP)  (RESP[0] == NRF_DFU_OP_RESPONSE && \
                               RESP[1] == OP && \
                               RESP[2] == NRF_DFU_RES_CODE_SUCCESS)

#define default_prn 0
#define resp_header 0x60
#define init_type 0x01
#define fw_type 0x02

#define SLIP_END 0xC0
#define SLIP_ESC 0xDB
#define SLIP_ESC_END 0xDC
#define SLIP_ESC_ESC 0xDD

#define default_delay 20
#define long_delay 100

void nrf_dfu_write(const uint8_t *cmd, size_t cmd_len) {
  LOG_HEXDUMP_DEBUG(MODULE, "write", cmd, cmd_len);
  while(cmd_len --) {
    if (*cmd == SLIP_END) {
      ble_usart_send_byte(SLIP_ESC);
      ble_usart_send_byte(SLIP_ESC_END);
    } else if (*cmd == SLIP_ESC) {
      ble_usart_send_byte(SLIP_ESC);
      ble_usart_send_byte(SLIP_ESC_ESC);
    } else {
      ble_usart_send_byte(*cmd);
    }
    cmd++;
  }
  ble_usart_send_byte(SLIP_END);
}

bool nrf_dfu_read(uint8_t *resp, size_t resp_buf_size, uint32_t *resp_len) {
  bool done = false;
  bool slip = false;
  uint8_t *p = resp;
  while (resp_buf_size --) {
    if (!ble_usart_read_byte(p)) {
      continue;
    }
    if (!slip) {
      if (*p == SLIP_END) {
        (*resp_len)++;
        p++;
        done = true;
        break;
      } else if (*p == SLIP_ESC) {
        slip = true;
      } else {
        p++;
        (*resp_len)++;
      }
    } else {
      slip = false;
      if (*p == SLIP_ESC_ESC) {
        *p= SLIP_ESC;
      } else if (*p == SLIP_ESC_END) {
        *p = SLIP_END;
      } else {
        // invalid packet
        break;
      }
      (*resp_len)++;
      p++;
    }
  }

  if (!done) {
    // read something but failed
    LOG_HEXDUMP_ERROR(MODULE, "read", resp, *resp_len);
  } else {
    LOG_HEXDUMP_DEBUG(MODULE, "read", resp, *resp_len);
  }
  return done;
}

bool nrf_dfu_transfer(const uint8_t *cmd, size_t cmd_len, uint8_t *resp, uint32_t resp_buf_size, uint32_t *resp_len) {
  nrf_dfu_write(cmd, cmd_len);
  bool r = nrf_dfu_read(resp, resp_buf_size, resp_len);
  return r;
}

bool nrf_dfu_ping(uint8_t id) {
  uint8_t cmd[64] = {0};
  uint8_t resp[10] = {0};
  uint32_t resp_len = 0;
  CMD_INIT(cmd, NRF_DFU_OP_PING);
  cmd[1] = id;
  bool r = nrf_dfu_transfer(cmd, 2, resp, sizeof(resp), &resp_len);
  r = r && RESP_CHECK(resp, NRF_DFU_OP_PING) && resp[3] == id;
  return r;
}

bool nrf_dfu_set_prn(void) {
  uint8_t cmd[64] = {0};
  uint8_t resp[64] = {0};
  uint32_t resp_len = 0;
  CMD_INIT(cmd, NRF_DFU_OP_RECEIPT_NOTIF_SET);
  PUT_UINT16_LE(PRN, cmd, 1);
  bool r = nrf_dfu_transfer(cmd, 3, resp, sizeof(resp), &resp_len);
  return r && RESP_CHECK(resp, NRF_DFU_OP_RECEIPT_NOTIF_SET);
}

bool nrf_dfu_get_mtu(size_t *mtu) {
  uint8_t cmd[64] = {0};
  uint8_t resp[64] = {0};
  uint32_t resp_len = 0;
  CMD_INIT(cmd, NRF_DFU_OP_MTU_GET);
  bool r = nrf_dfu_transfer(cmd, 1, resp, sizeof(resp), &resp_len);
  r = r && RESP_CHECK(resp, NRF_DFU_OP_MTU_GET);
  if (r) {
    *mtu = resp[3] + resp[4] * 256;
  }
  return r;
}

bool nrf_dfu_create_object(nrf_dfu_obj_type_t type, uint32_t size) {
  uint8_t cmd[64] = {0};
  uint8_t resp[64] = {0};
  uint32_t resp_len = 0;
  CMD_INIT(cmd, NRF_DFU_OP_OBJECT_CREATE);
  cmd[1] = type;
  PUT_UINT32_LE(size, cmd, 2);
  bool r = nrf_dfu_transfer(cmd, 6, resp, sizeof(resp), &resp_len);
  r = r && RESP_CHECK(resp, NRF_DFU_OP_OBJECT_CREATE);
  return r;
}

bool nrf_dfu_select_object(nrf_dfu_obj_type_t type, uint32_t *max_size) {
  uint8_t cmd[64] = {0};
  uint8_t resp[64] = {0};
  uint32_t resp_len = 0;
  CMD_INIT(cmd, NRF_DFU_OP_OBJECT_SELECT);
  cmd[1] = type;
  bool r = nrf_dfu_transfer(cmd, 2, resp, sizeof(resp), &resp_len);
  r = r && RESP_CHECK(resp, NRF_DFU_OP_OBJECT_SELECT);
  // (max_size, offset, crc)
  if (r) {
    *max_size = GET_UINT32_LE(resp, 3);
  }
  return r;
}

void nrf_dfu_write_object(const uint8_t* data, size_t data_len) {
  while(data_len) {
    uint8_t cmd[128] = {0};
    uint8_t block_size = MIN(data_len, PACKET_SIZE);
    CMD_INIT(cmd, NRF_DFU_OP_OBJECT_WRITE);
    memcpy(cmd+1, data, block_size);
    // `cmd` + packet, no response
    nrf_dfu_write(cmd, block_size+1);
    data += block_size;
    data_len -= block_size;
  }
}

bool nrf_dfu_execute_object(void) {
  uint8_t cmd[64] = {0};
  uint8_t resp[64] = {0};
  uint32_t resp_len = 0;
  CMD_INIT(cmd, NRF_DFU_OP_OBJECT_EXECUTE);
  bool r = nrf_dfu_transfer(cmd, 1, resp, sizeof(resp), &resp_len);
  r = r && RESP_CHECK(resp, NRF_DFU_OP_OBJECT_EXECUTE);
  return r;
}

bool nrf_dfu_get_crc(uint32_t* crc) {
  uint8_t cmd[64] = {0};
  uint8_t resp[64] = {0};
  uint32_t resp_len = 0;
  CMD_INIT(cmd, NRF_DFU_OP_CRC_GET);
  bool r = nrf_dfu_transfer(cmd, 1, resp, sizeof(resp), &resp_len);
  r = r && RESP_CHECK(resp, NRF_DFU_OP_CRC_GET);
  // (offset, crc)
  if (r) {
    *crc = GET_UINT32_LE(resp, 7);
  }

  return r;
}

void nrf_dfu_restart(void) {
  uint8_t cmd[64] = {0};
  CMD_INIT(cmd, NRF_DFU_OP_ABORT);
  nrf_dfu_write(cmd, 1);
}
