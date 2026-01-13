#include "se.h"

#include <stdint.h>
#include <stddef.h>

#include "alignment.h"
#include "thd89.h"

enum {
  // 生产用指令
  CMD_ID_ERASE_STORAGE = 0x80,
  CMD_ID_SWITCH_LIFE_CYCLE = 0x81,
  CMD_ID_SET_SN = 0x82,
  CMD_ID_SET_PRE_SHARED_KEY = 0x83,
  CMD_ID_GEN_DEV_KEY = 0x84,
  CMD_ID_SET_DEV_CERT = 0x85,
};

// struct for command and response
typedef struct {
  uint8_t cmd;
  uint8_t len[2];
  uint8_t payload[0];
} __attribute__((packed)) request_t;

typedef struct {
  uint8_t code;
  uint8_t len[2];
  uint8_t payload[0];
} __attribute__((packed)) response_t;

static inline size_t request_get_length(const request_t* req) {
  return GET_UINT16_BE(req->len, 0);
}
static inline void request_set_length(request_t* req, size_t len) {
  PUT_UINT16_BE(len, req->len, 0);
}

static inline size_t response_get_length(const response_t* resp) {
  return GET_UINT16_BE(resp->len, 0);
}
static inline void response_set_length(response_t* resp, size_t len) {
  PUT_UINT16_BE(len, resp->len, 0);
}

#define REQ_INIT_CMD(BUF, CMD) request_t *req = (void *)BUF; req->cmd = CMD
#define REQ_EMPTY_PAYLOAD(req) request_set_length(req, 0)
#define REQ_PAYLOAD(req, __payload__, __len__) \
  do {                                         \
    memcpy(req->payload, __payload__, __len__); \
    request_set_length(req, __len__);          \
  } while (0)

#define RESP_INIT(addr) response_t* resp = (void*)(addr)

static inline size_t command_size(const request_t* req) {
  return 3 + request_get_length(req);
}

typedef enum {
  RESP_CODE_SUCCESS = 0,
  RESP_CODE_INTERNAL_FAILED,
  RESP_CODE_INVALID_STATE,
  RESP_CODE_INVALID_PARAM_DATA,
  RESP_CODE_UNSUPPORTED_COMMAND,
  RESP_CODE_OBJECT_ALREADY_EXIST,
  RESP_CODE_OBJECT_NOT_EXIST,
} response_code_t;

int se_erase_storage(void) {
    uint8_t command[3] = {0};
    uint8_t response[16] = {0};
    size_t response_size = 0;

    REQ_INIT_CMD(command, CMD_ID_ERASE_STORAGE);
    REQ_EMPTY_PAYLOAD(req);

    thd89_result_t ret = thd89_execute_command(command, sizeof(command), response, sizeof(response), &response_size);
    // transmit result
    if (ret != THD89_SUCCESS) {
        return 1;
    }
    RESP_INIT(response);
    if (resp->code != RESP_CODE_SUCCESS) {
        return 1;
    }
    return 0;
}

int se_set_sn(const uint8_t *sn, size_t sn_len) {
    if (sn_len > 32) {
      return 1;
    }
    uint8_t command[128] = {0};
    uint8_t response[16] = {0};
    size_t response_size = 0;

    REQ_INIT_CMD(command, CMD_ID_SET_SN);
    REQ_PAYLOAD(req, sn, sn_len);
    thd89_result_t ret = thd89_execute_command(command, command_size(req), response, sizeof(response), &response_size);
    // transmit result
    if (ret != THD89_SUCCESS) {
        return 1;
    }
    RESP_INIT(response);
    if (resp->code != RESP_CODE_SUCCESS) {
        return 1;
    }
    return 0;
}

int se_set_sheared_key(const uint8_t *key, size_t key_len) {
    if (key_len > 32) {
      return 1;
    }
    uint8_t command[128] = {0};
    uint8_t response[16] = {0};
    size_t response_size = 0;

    REQ_INIT_CMD(command, CMD_ID_SET_PRE_SHARED_KEY);
    REQ_PAYLOAD(req, key, key_len);
    thd89_result_t ret = thd89_execute_command(command, command_size(req), response, sizeof(response), &response_size);
    // transmit result
    if (ret != THD89_SUCCESS) {
        return 1;
    }
    RESP_INIT(response);
    if (resp->code != RESP_CODE_SUCCESS) {
        return 1;
    }
    return 0;
}

int se_gen_dev_keypair(void) {
    uint8_t command[3] = {0};
    uint8_t response[128] = {0};
    size_t response_size = 0;

    REQ_INIT_CMD(command, CMD_ID_GEN_DEV_KEY);
    REQ_EMPTY_PAYLOAD(req);

    thd89_result_t ret = thd89_execute_command(command, sizeof(command), response, sizeof(response), &response_size);
    // transmit result
    if (ret != THD89_SUCCESS) {
        return 1;
    }
    RESP_INIT(response);
    if (resp->code != RESP_CODE_SUCCESS) {
        return 1;
    }

    return 0;
}

int se_write_certificate(const uint8_t *cert, size_t cert_len) {
    if (cert_len > 1024) {
      return 1;
    }
    uint8_t command[1024 + 3] = {0};
    uint8_t response[16] = {0};
    size_t response_size = 0;

    REQ_INIT_CMD(command, CMD_ID_SET_DEV_CERT);
    REQ_PAYLOAD(req, cert, cert_len);

    thd89_result_t ret = thd89_execute_command(command, command_size(req), response, sizeof(response), &response_size);
    // transmit result
    if (ret != THD89_SUCCESS) {
        return 1;
    }
    RESP_INIT(response);
    if (resp->code != RESP_CODE_SUCCESS) {
        return 1;
    }
    return 0;
}


int se_switch_life_cycle(void) {
    uint8_t command[3] = {0};
    uint8_t response[16] = {0};
    size_t response_size = 0;

    REQ_INIT_CMD(command, CMD_ID_SWITCH_LIFE_CYCLE);
    REQ_EMPTY_PAYLOAD(req);

    thd89_result_t ret = thd89_execute_command(command, sizeof(command), response, sizeof(response), &response_size);
    // transmit result
    if (ret != THD89_SUCCESS) {
        return 1;
    }
    RESP_INIT(response);
    if (resp->code != RESP_CODE_SUCCESS) {
        return 1;
    }
    return 0;
}
