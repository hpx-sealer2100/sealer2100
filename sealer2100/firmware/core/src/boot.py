import storage
from trezor import config, io, log, loop, ui, utils
from trezor.ui import lvgl_tick

lvgl_task = lvgl_tick()

log.info("boot", "lvgl_task successfully")

async def boot() -> None:
    from apps.common.request_pin import can_lock_device, verify_protection
    from trezor.ui.screen.bootscreen import BootScreen
    screen = BootScreen()
    await screen.show()
    await screen

    if not can_lock_device():
        await verify_protection()
        storage.init_unlocked()

    loop.close(lvgl_task)


loop.schedule(lvgl_task)
loop.schedule(boot())
loop.run()
