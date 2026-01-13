# pylint: disable=wrong-import-position
import lvgl as lv
from micropython import const
from trezorui import Display

from typing import TYPE_CHECKING

def init_file_system():
    from trezor import utils
    import trezorio as io
    io.fatfs.mount()
    io.fatfs.mkdir("1:res", True)
    io.fatfs.mkdir("1:res/wallpapers", True)
    io.fatfs.mkdir("1:res/nfts", True)
    io.fatfs.mkdir("1:res/nfts/imgs", True)
    io.fatfs.mkdir("1:res/nfts/zooms", True)
    io.fatfs.mkdir("1:res/nfts/desc", True)

# initialize lvgl
def init_lvgl():
    import lvgldrv as lcd
    from trezor import utils
    if lv.is_initialized():
        log.error(__name__, "lvgl already initialized")
        log.error(__name__, "please check your code")

    lv.init()
    if not utils.EMULATOR:
        import stjpeg
        stjpeg.init()
    lcd.set_directly_copy(False)
    disp_buf1 = lv.disp_draw_buf_t()
    buf_1 = lcd.framebuffer(0)
    buf_2 = lcd.framebuffer(1)
    disp_buf1.init(buf_1, buf_2, len(buf_1) // lv.color_t.__SIZE__)
    disp_drv = lv.disp_drv_t()
    disp_drv.init()
    disp_drv.draw_buf = disp_buf1
    disp_drv.flush_cb = lcd.flush
    disp_drv.hor_res = 480
    disp_drv.ver_res = 800
    disp_drv.register()

    indev_drv = lv.indev_drv_t()
    indev_drv.init()
    indev_drv.type = lv.INDEV_TYPE.POINTER
    indev_drv.read_cb = lcd.ts_read
    indev_drv.long_press_time = 2000
    indev_drv.register()

def init_theme() -> None:
    from . import colors
    from . import font
    dispp = lv.disp_get_default()
    theme = lv.theme_default_init(
        dispp,
        colors.USER.PRIMARY,
        colors.USER.SECONDARY,
        True,
        font.default
    )
    dispp.set_theme(theme)

## init lvgl first, then do setting for it
try:
    from trezor import log
    init_file_system()
    init_lvgl()
    init_theme()
    log.debug(__name__, "lvgl initialized successfully")
except BaseException as e:
    log.error(__name__, "lvgl initialized failed")
    log.exception(__name__, e)
    raise e

# under code all need lvgl

# import style
from trezor.ui.style import Style  # isort:skip

# import theme use it side effects to set the theme
from . import theme as _
from .result import *

if TYPE_CHECKING:
    from typing import Any, Awaitable, Generator

    Pos = tuple[int, int]
    Area = tuple[int, int, int, int]

# all rendering is done through a singleton of `Display`
display = Display()

# backlight brightness 0 ~ 100
BACKLIGHT_NORMAL = const(60)
BACKLIGHT_LOW = const(30)
BACKLIGHT_NONE = const(0)
BACKLIGHT_MAX = const(100)

NORMAL = Display.FONT_NORMAL
BOLD = Display.FONT_BOLD
MONO = Display.FONT_MONO
WIDTH = Display.WIDTH
HEIGHT = Display.HEIGHT

async def lvgl_tick():
    from trezor import workflow, utils, loop

    disp = lv.disp_get_default()
    last_active_time = disp.last_activity_time
    while True:
        if utils.EMULATOR:
           lv.tick_inc(10)
        await loop.sleep(10)
        lv.timer_handler()
        if last_active_time != disp.last_activity_time:
            # log.debug(__name__, f"last activity time: {last_active_time}")
            # if user operating, touch idle timer
            workflow.idle_timer.touch()
            last_active_time = disp.last_activity_time
