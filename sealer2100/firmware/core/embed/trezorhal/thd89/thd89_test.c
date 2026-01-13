#include "device.h"
#include "stm32h7xx_hal.h"
#include "thd89.h"
#include "se_spi.h"

#include <stdio.h>
#include <memory.h>

void test_frame_ping(void) {
  // send control frame
  extern thd89_result_t thd89_ping(void);
  thd89_result_t ret = thd89_ping();
  if (ret != THD89_SUCCESS) {
    printf("ping failed: %d\n", ret);
  } else {
    printf("ping success\n");
  }
}
void test_handshake(void) {
  uint8_t SECRET[32] = {0};
  if (!device_get_pre_shared_key(SECRET)) {
    printf("get pre-shared key failed\n");
    return;
  }
  thd89_result_t ret = thd89_handshake(SECRET, sizeof(SECRET));
  if (ret != THD89_SUCCESS) {
    printf("handshake failed: %d\n", ret);
    return;
  } else {
    printf("handshake success\n");
  }
}

void test_echo(void) {
  uint8_t byte = 0;
  uint8_t cmd[257] = {0};
  uint32_t count = 0;
  while (1) {
    // 延迟 1000 毫秒
    HAL_Delay(1000);
    memset(cmd, byte++, sizeof(cmd));
    // echo command
    cmd[0] = 'E';
    uint8_t resp[257] = {0};
    size_t resp_size = 0;
    thd89_result_t ret =
        thd89_execute_command(cmd, sizeof(cmd), resp, sizeof(resp), &resp_size);

    count++;
    if (ret != THD89_SUCCESS) {
      printf("[%08ld] execute command failed: %d\n", count, ret);
      continue;
    }
    if (resp_size != sizeof(resp) || memcmp(cmd, resp, sizeof(cmd)) != 0) {
      printf("[%08ld] unexpected response\n", count);
      continue;
    }

    printf("[%08ld] execute command success\n", count);
  }
}

void test_block(void) {
  uint8_t byte = 0;
  while (1)
  {
    uint8_t data[16] = {0};
    memset(data, byte, sizeof(data));
    for (int i = 0; i < sizeof(data); i++) {
      data[i] += i;
    }
    byte += 0x10;
    se_send(data, sizeof(data), 1000);
    HAL_Delay(1000);
    se_recv(data, sizeof(data), 1000);
    HAL_Delay(1000);
  }
}

void thd89_test(void) {
  se_spi_init();
  HAL_Delay(1000);
  // test_block();
  thd89_init();
  // test_frame_ping();
  test_handshake();
  // test_echo();
}
