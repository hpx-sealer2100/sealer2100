from storage import device
from trezor import io, log, loop, utils, motor, config
from trezor.ui.screen.statusbar import StatusBar
from apps import base

async def handle_usb_state():
    usb_state = loop.wait(io.USB_STATE)
    cached_connected = await usb_state
    # setup
    StatusBar.instance().show_usb(cached_connected)

    while True:
        await loop.sleep(500)
        connected = await usb_state

        # connected changed?
        changed = not cached_connected == connected
        if changed:
            cached_connected = connected
            # usb connector not connect USB controler
            # StatusBar.instance().show_charging(connected)
            utils.lcd_resume()
            StatusBar.instance().show_usb(connected)
            base.reload_settings_from_storage()

            if connected:
                motor.vibrate()

        if changed:
            try:
                usb_auto_lock = device.is_usb_lock_enabled()
                if usb_auto_lock and device.is_initialized() and config.has_pin():
                    if config.is_unlocked():
                        config.lock()
                        await safe_reloop()
                        raise loop.TASK_CLOSED
                elif not usb_auto_lock and not connected:
                    await safe_reloop()
            except Exception as exec:
                if __debug__:
                    log.exception(__name__, exec)
                loop.clear()
                return  # pylint: disable=lost-exception

async def safe_reloop():
    from trezor import wire
    await wire.signal_ack()
