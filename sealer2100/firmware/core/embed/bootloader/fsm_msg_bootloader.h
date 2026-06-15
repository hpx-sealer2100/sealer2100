#ifndef __FSM_MSG_BOOTLOADER_H__
#define __FSM_MSG_BOOTLOADER_H__

#include "messages-bootloader.pb.h"
void fsm_msgReboot(const Reboot* msg);
void fsm_msgFirmwareUpdate(const FirmwareUpdate* msg);
#endif
