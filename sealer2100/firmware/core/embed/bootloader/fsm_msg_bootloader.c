#include "fsm_msg_bootloader.h"

#include "bootui.h"
#include "common.h"
#include "fsm.h"
#include "messages-bootloader.pb.h"
#include "updater.h"

void fsm_msgReboot(const Reboot* msg){
  (void) msg;
  // send message first
  fsm_sendSuccess("Success");

  // use ui event to reboot, not block the main
  uint32_t *param;
  switch (msg->reboot_type) {
    case RebootType_Normal:
      param = UI_REBOOT_TYPE_MAKE_PARAM(UI_REBOOT_TYPE_NORMAL);
      break;
    case RebootType_Boardloader:
      param = UI_REBOOT_TYPE_MAKE_PARAM(UI_REBOOT_TYPE_BOARDLOADER);
      break;
    case RebootType_BootLoader:
      param = UI_REBOOT_TYPE_MAKE_PARAM(UI_REBOOT_TYPE_BOOTLOADER);
      break;
    default:
      param = UI_REBOOT_TYPE_MAKE_PARAM(UI_REBOOT_TYPE_NORMAL);
      break;
  }
  ui_event_broadcast(UI_EVENT_REBOOT, param);
}
void fsm_msgFirmwareUpdate(const FirmwareUpdate* msg) {
    (void) msg;
    bool reboot_on_success = msg->has_reboot_on_success && msg->reboot_on_success;
    updater_update_image(msg->path, reboot_on_success);
}
