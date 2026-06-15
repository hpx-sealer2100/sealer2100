#include "boot_updater.h"

#include <assert.h>

#include "bootui.h"
#include "common.h"
#include "fsm.h"
#include "messages.h"
#include "messages.pb.h"
#include "updater.h"
#include "boot_context.h"
#include "power_manager.h"

void boot_updater_poll(void) {
  updater_context_t *ctx = &boot_ctx.updater;
  if (ctx->state != UPDATER_STATE_NONE) {
    boot_alive_time_touch();
  }
  switch (ctx->state) {
    case UPDATER_STATE_NONE:
      // do nothing
      return;
    case UPDATER_STATE_NOTIFY_APP:
      ui_home();
      ctx->wait_confirm_uintil = hal_ticks_ms() + UPDATER_WAIT_CONFIRM_TIMEOUT;
      fsm_sendButtonRequest(ButtonRequestType_ButtonRequest_Other);
      updater_success(UPDATER_STATE_WAIT_BUTTON_ACK);
      return;
    case UPDATER_STATE_WAIT_BUTTON_ACK:
      if (hal_ticks_ms() > ctx->wait_confirm_uintil) {
        // timeout, switch to cancelled
        updater_success(UPDATER_STATE_CANCELLED);
        return;
      }
      if (msg_tiny_id == MessageType_MessageType_ButtonAck) {
        updater_success(UPDATER_STATE_ASK_CONFIRM);
        return;
      }
      if (msg_tiny_id == MessageType_MessageType_Cancel) {
        updater_success(UPDATER_STATE_CANCELLED);
        return;
      }
      return;
    case UPDATER_STATE_ASK_CONFIRM:
      // update ui, and wait confirm
      ui_jump_to(ui_updater_confirm(ctx->updater->get_name_fn()));
      // switch None for avoid redraw confirm view
      // in confirm view btn callback will change sate
      updater_success(UPDATER_STATE_WAITING_CONFIRM);
      return;
    case UPDATER_STATE_WAITING_CONFIRM:
      if (hal_ticks_ms() > ctx->wait_confirm_uintil) {
        // timeout, switch to cancelled
        updater_success(UPDATER_STATE_CANCELLED);
      }
      return;
    case UPDATER_STATE_CONFIRMED:
      // update ui
      // set device busy true
      boot_ctx.device.busy = true;
      ui_jump_to(ui_updater(ctx->updater->get_name_fn()));
      updater_success(UPDATER_STATE_VERIFY);
      return;
    case UPDATER_STATE_CANCELLED:
      ctx->updater->cancelled_fn();
      fs_remove(&boot_ctx.fs, ctx->updating->path);
      ui_updater_reset();
      ui_home();
      updater_finished();
      return;
    case UPDATER_STATE_VERIFY:
      ctx->updater->verify_image_fn();
      // update ui
      ui_updater_update("Verifying", ctx->updater->get_processed_fn());
      return;
    case UPDATER_STATE_INSTALL:
      // begin install, disable long press power key restart device
      pm_battery_long_press_reset(false);
      ctx->updater->install_image_fn();
      // update ui
      ui_updater_update("Installing", ctx->updater->get_processed_fn());
      return;
    case UPDATER_STATE_ERROR:
      // update ui
      fs_remove(&boot_ctx.fs, ctx->updating->path);
      ui_updater_reset();
      ui_home();
      updater_finished();
      return;
    case UPDATER_STATE_FINISHED: {
      // need reboot after update, broadcast reboot event first, the `ui_updater_reset` will free the `ctx->updating`resource
      if (ctx->updating->reboot_on_success) {
        ui_event_broadcast(UI_EVENT_REBOOT, UI_REBOOT_TYPE_MAKE_PARAM(UI_REBOOT_TYPE_NORMAL));
      }
      fs_remove(&boot_ctx.fs, ctx->updating->path);
      ui_updater_reset();
      ui_home();
      updater_finished();
      return;
    }
    default:
      // not reachable
      assert(false);
  }
}
