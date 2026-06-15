#ifndef __BLE_H__
#define __BLE_H__
#include <stdint.h>

/// ble ctrl command
#define BLE_CMD_CTRL 0x81
// ctrl ble disconnect the connection
#define BLE_CMD_CTRL_PARAM_DISCONNECT 0x03

/// get ble infomation command
#define BLE_CMD_INFO 0x83
#define BLE_CMD_INFO_PARAM_ADV_NAME 0x01 // get ble name
#define BLE_CMD_INFO_PARAM_VERSION  0x02 // get ble version

// ble adv name, response of (BLE_CMD_INFO, BLE_CMD_INFO_PARAM_ADV_NAME)
#define BLE_RESP_ADV_NAME 0x01
// ble version, response of (BLE_CMD_INFO, BLE_CMD_INFO_PARAM_VERSION)
#define BLE_RESP_VERSION  0x05

/// BLE_CMD_REBOOT
#define BLE_CMD_REBOOT 0x84
#define BLE_CMD_REBOOT_PARAM 0x01


// ble connect state, notified from ble
#define BLE_NOTIFY_STATE 0x02
#define BLE_NOTIFY_STATE_CONNECTED    0x01 // ble have connected
#define BLE_NOTIFY_STATE_DISCONNECTED 0x02 // ble have disconnected

// ble pair code, notifyed from ble
#define BLE_NOTIFY_PAIR_CODE 0x03

// ble pair result, notified from ble
#define BLE_NOTIFY_PAIR_RESULT 0x04
#define BLE_NOTIFY_PAIR_RESULT_SUCCESS 0x01
#define BLE_NOTIFY_PAIR_RESULT_FAILURE 0x02

void ble_cmd_req(uint8_t cmd, uint8_t value);

void ble_function_on(void);
void ble_function_off(void);

void ble_reboot(void);
void ble_enter_dfu(void);

// function only send the ble command, poll the `usart` interface get result
void ble_async_get_name(void);
void ble_async_get_version(void);
void ble_async_disconnect(void);
void ble_async_refresh_dev_info(void);
#endif
