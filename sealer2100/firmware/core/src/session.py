from trezor import log, loop, utils, workflow
from trezor.ui import lvgl_tick
from trezor.ui.screen.manager import monitor_task, monitor_task_exit
from trezor.uart import handle_ble_info, handle_uart
from storage import device

# register background tasks
from trezor import tasks as _

import apps.base

if not device.ble_enabled():
    from trezor import io
    io.BLE().power_off()

apps.base.boot()

if __debug__:
    import apps.debug

    apps.debug.boot()

# for testing, commit `loop.schedule(apps.base.set_homescreen() `
# async def home():
#     from trezor.ui.screen.iris import IrisRegistTip as TestView
#     screen = TestView()
#     await screen.show()

# loop.schedule(home())

# run main event loop and specify which screen is the default
loop.schedule(apps.base.set_homescreen())

loop.schedule(handle_uart())

loop.schedule(handle_ble_info())

loop.schedule(lvgl_tick())

loop.schedule(monitor_task())

# async def heart():
#     time = 0
#     while True:
#         await loop.sleep(1000)
#         log.debug(__name__, f"[{time: 08d}] I'm alive ...")
#         time += 1
# loop.schedule(heart())

utils.set_up()
if utils.show_app_guide():
    from trezor.ui.layouts import show_app_guide

    loop.schedule(show_app_guide())

if not device.is_initialized() and not device.get_iris_version():
    from trezor import iris
    loop.schedule(iris.refresh_iris_version())


loop.run()
workflow.close_others()
monitor_task_exit()

if __debug__:
    log.debug(__name__, "Restarting main loop")
