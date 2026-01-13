import lvgl as lv

from trezor import io, log, loop, workflow
from trezor.ui import i18n, colors, Style, font
from trezor.ui.theme import Styles
from trezor.ui.screen import Navigation
from trezor.ui.constants import *
from trezor.ui.screen import manager

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Literal
    CameraMode = Literal['front', 'back']

CAMERA_WIDTH = 400
CAMERA_HEIGHT = 400
CORNER_SPACE = 4

CAMERA = io.Camera(CAMERA_WIDTH, CAMERA_HEIGHT)

class ScanApp(Navigation):

    def __init__(self):
        super().__init__()

        self.preview = self.add(lv.canvas)
        self.preview.set_buffer(CAMERA.buffer(), CAMERA_WIDTH, CAMERA_HEIGHT, lv.img.CF.TRUE_COLOR)
        if utils.EMULATOR:
            self.preview.fill_bg(colors.USER.DARK_GRAY, lv.OPA.COVER)
        else:
            self.preview.fill_bg(colors.USER.BLACK, lv.OPA.COVER)
        self.preview.set_style_radius(8, lv.PART.MAIN)
        self.preview.set_style_clip_corner(True, lv.PART.MAIN)
        self.preview.center()
        # self.preview.add_style(Styles.debug, 0)

        # left top
        img = self.add(lv.img)
        # img.add_style(Styles.debug, 0)
        img.set_src("A:/res/hp/scan_corner.png")
        img.align_to(self.preview, lv.ALIGN.TOP_LEFT, -CONTENT_SPACE, -CONTENT_SPACE)

        # right top
        img = self.add(lv.img)
        # img.add_style(Styles.debug, 0)
        img.set_src("A:/res/hp/scan_corner.png")
        img.set_angle(900)
        img.align_to(self.preview, lv.ALIGN.TOP_RIGHT, CONTENT_SPACE, -CONTENT_SPACE)

        # right bottom
        img = self.add(lv.img)
        # img.add_style(Styles.debug, 0)
        img.set_src("A:/res/hp/scan_corner.png")
        img.set_angle(1800)
        img.align_to(self.preview, lv.ALIGN.BOTTOM_RIGHT, CONTENT_SPACE, CONTENT_SPACE)

        # right bottom
        img = self.add(lv.img)
        # img.add_style(Styles.debug, 0)
        img.set_src("A:/res/hp/scan_corner.png")
        img.set_angle(2700)
        img.align_to(self.preview, lv.ALIGN.BOTTOM_LEFT, -CONTENT_SPACE, CONTENT_SPACE)
        self.preview.move_foreground()

        label = self.add(lv.label)
        label.add_style(
            Style()
            .width(lv.pct(100))
            .pad_left(CONTENT_PAD)
            .pad_right(CONTENT_PAD)
            .text_align(lv.TEXT_ALIGN.CENTER)
            .text_color(colors.USER.WHITE)
            .text_font(font.medium),
            lv.PART.MAIN
        )
        label.align_to(self.preview, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
        label.set_text(i18n.Text.scan_app)
        # label.add_style(Styles.debug, 0)

        self.tasks = []

        # airgap task
        self.unimporter = utils.unimport()
        self.unimporter.__enter__()
        task = workflow.spawn(self.scanning())
        self.tasks.append(task)
        # preview task
        task = workflow.spawn(self.preview_refresh())
        self.tasks.append(task)

        # # fps for debug
        self.fps = 0
        # self.fps_label = self.add(lv.label)
        # self.fps_label.align(lv.ALIGN.BOTTOM_RIGHT, 0, 0)
        # self.fps_label.set_text('')
        # task = workflow.spawn(self.show_fps())
        # self.tasks.append(task)


    def on_loaded(self):
        import lvgldrv as lcd
        lcd.set_directly_copy(True)
        super().on_loaded()
        state = CAMERA.state()
        log.debug(__name__, f"camera state: {state}")
        if state == io.Camera.NONE:
            workflow.spawn(self.start_camera())
        elif CAMERA.state() == io.Camera.SUSPENDED:
            log.debug(__name__, "show camera")
            CAMERA.resume()

    def on_unload_start(self):
        import lvgldrv as lcd
        lcd.set_directly_copy(False)
        super().on_unload_start()
        CAMERA.suspend()

    def on_deleting(self):
        super().on_deleting()
        CAMERA.stop()
        for task in self.tasks:
            task.close()
        CAMERA.deinit()
        self.unimporter.__exit__(None, None, None)

    def dismiss(self):
        # 页面类型从Modal换成了Navigation
        # 在用户完成交易时，需要关闭当前页面
        super().dismiss()
        loop.spawn(manager.pop(self))

    @staticmethod
    async def start_camera():
        await loop.sleep(100)
        CAMERA.init()
        # CAMERA.switch("front")
        CAMERA.switch("back")
        CAMERA.start()

    async def preview_refresh(self):
        while True:
            await loop.wait(io.CAMERA_STATE)
            self.fps += 1
            self.preview.invalidate()
            CAMERA.resume()

    async def show_fps(self):
        while True:
            await loop.sleep(1000)
            self.fps_label.set_text(f"FPS: {self.fps}")
            self.fps = 0

    def toggle_camera(self):
        CAMERA.stop()
        CAMERA.deinit()
        CAMERA.init()
        if self.camera == 'back':
            CAMERA.switch('front')
            self.camera = 'front'
        elif self.camera == 'front':
            CAMERA.switch('back')
            self.camera = 'back'
        CAMERA.start()

    async def scanning(self):
        from trezor.airgap import scan, scaning_event
        from trezor.airgap.event import InvalidUR
        from trezor.ui.layouts import show_error
        from trezor.wire import DUMMY_CONTEXT as ctx

        CAMERA.scan_reset()
        task = scan()
        self.tasks.append(task)

        while True:
            event = await scaning_event()
            log.debug(__name__, f"event: {event}")
            if isinstance(event, InvalidUR):
                await show_error(
                    ctx,
                    i18n.Text.invalid_ur,
                    i18n.Title.invalid_data,
                    i18n.Button.try_again,
                )
                self.tasks.remove(task)
                CAMERA.scan_reset()
                task = scan()
                self.tasks.append(task)
            elif event == 3:
                self.tasks.remove(task)
                CAMERA.scan_reset()
                task = scan()
                self.tasks.append(task)

            elif event == 4:
                self.close(None)
