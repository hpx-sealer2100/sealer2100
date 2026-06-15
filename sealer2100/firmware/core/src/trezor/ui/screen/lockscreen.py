import lvgl as lv

from . import Modal
from storage import device, config
from trezor import ui, utils, loop
from trezor.ui import i18n, font, colors, Style
from trezor.ui.component.container import VStack
from trezor.ui.theme import Styles


class LockScreen(Modal):
    def __init__(self):
        super().__init__()
        self.timeline = None
        wallpaper = device.get_homescreen()
        self.set_style_bg_img_src(wallpaper, lv.PART.MAIN)
        self.add_flag(lv.obj.FLAG.CLICKABLE)

        self.create_content(VStack)
        self.content: VStack
        self.content.add_flag(lv.obj.FLAG.CLICKABLE)
        self.content.set_style_border_color(colors.USER.DARK_GREEN, lv.PART.MAIN)
        self.content.set_height(lv.pct(100))
        self.content.set_style_flex_main_place(
            lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN
        )
        self.content.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.content.set_style_flex_track_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        container.set_size(lv.pct(100), lv.SIZE.CONTENT)

        device_name = device.get_label()
        # Create product_lab
        self.device_name = container.add(lv.label)
        self.device_name.set_width(lv.pct(100))
        self.device_name.set_height(lv.SIZE.CONTENT)
        self.device_name.set_long_mode(lv.label.LONG.WRAP)
        self.device_name.set_text(device_name or "Iris cnnc")
        self.device_name.add_style(
            Style().text_color(colors.USER.WHITE).text_font(font.Medium.PF44).text_align_center(),
            lv.PART.MAIN,
        )

        self.ble_name_lab = container.add(lv.label)
        self.ble_name_lab.set_text("")
        self.ble_name_lab.set_style_text_color(colors.USER.WHITE, lv.PART.MAIN)

        container = self.add(VStack)
        container.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        container.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        # Create lock_img
        self.img = container.add(lv.img)

        # Create unlock_lab
        self.label = container.add(lv.label)
        self.label.add_style(
            Style().text_color(colors.USER.WHITE).text_font(font.small),
            lv.PART.MAIN,
        )
        self.img.set_src("A:/res/hp/lock.png")
        self.label.set_text(i18n.Text.tap_to_unlock)


        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        loop.spawn(self.delay_get_ble_name())

        # add a animation when not ready
        if not self.is_ready():
            self.busying()

    def busying(self):
        self.img = lv.img(self)
        self.img.set_src("A:/res/hp/loading-1.png")
        self.img.align(lv.ALIGN.BOTTOM_MID, 0, -196)

        # a animation
        a = lv.anim_t()
        a.init()
        a.set_var(self.img)
        a.set_time(1000)
        a.set_values(0, 3600)
        a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
        a.set_custom_exec_cb(self.busy_cb)

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, a)
        lv.anim_timeline_start(self.timeline)

    def busy_cb(self, a: lv.anim_t, v: int):
        self.img.set_angle(v)
        if not self.is_ready():
            return

        # when ready, stop the animation and remove
        lv.anim_timeline_stop(self.timeline)
        lv.anim_timeline_del(self.timeline)
        self.timeline = None
        self.img.delete()

    def is_ready(self) -> bool:
        ready = bool(utils.IRIS_VERSION)
        if not device.ble_enabled():
            # if ble is not enabled, just test iris version
            return ready

        # if ble is enabled, test ble name and iris version
        return ready and bool(utils.BLE_NAME)

    def on_click(self, e):
        if not ui.display.backlight() and not device.is_tap_awake_enabled():
            return
        if utils.turn_on_lcd_if_possible():
            return
        if not self.is_ready():
            return
        from apps.base import unlock_device

        loop.spawn(unlock_device())

    def on_loaded(self):
        super().on_loaded()
        device_name = device.get_label()
        self.device_name.set_text(device_name or "Iris cnnc")

    def on_deleting(self):
        super().on_deleting()
        if self.timeline:
            lv.anim_timeline_stop(self.timeline)
            lv.anim_timeline_del(self.timeline)
            self.timeline = None

    async def delay_get_ble_name(self):
        if not device.ble_enabled():
            return
        while not utils.BLE_NAME:
            await loop.sleep(300)

        self.ble_name_lab.set_text(utils.BLE_NAME)

class ProtectScreen(LockScreen):
    def __init__(self):
        super().__init__()
        self.img.set_src("A:/res/hp/ic_jiesuo_1.png")
        self.label.set_text(i18n.Text.swipe_to_view_app)
        self.add_event_cb(self.on_swipe_up, lv.EVENT.GESTURE, None)
        self._busy = False

    def on_click(self, e):
        # overwrite parent method
        # disable click event
        pass
    def on_swipe_up(self, e: lv.event_t):
        gesture = lv.indev_get_act().get_gesture_dir()
        indev = lv.indev_get_act()
        if self._busy:
            return
        if gesture == lv.DIR.TOP:
            indev.wait_release()
            self.dismiss()

    def set_busy(self, busy: bool):
        if self._busy == busy:
            return
        self._busy = busy
        if busy:
            self.label.set_text(i18n.Text.processing)
            # self.img.add_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.label.set_text(i18n.Text.swipe_to_view_app)
            # self.img.clear_flag(lv.obj.FLAG.HIDDEN)


def set_busy(busy: bool):
    if not utils.is_show_busy():
        return
    if not device.is_initialized() or not config.is_unlocked():
        return
    from . import manager
    screen = manager.try_switch_to(ProtectScreen)
    if screen:
        screen.set_busy(busy)
        return

    screen = ProtectScreen()
    screen.set_busy(busy)
    loop.spawn(screen.show())
