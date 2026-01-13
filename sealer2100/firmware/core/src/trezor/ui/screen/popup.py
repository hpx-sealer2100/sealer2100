import lvgl as lv

from . import Modal
from trezor.ui import i18n, colors
from trezor.ui.component import VStack
from trezor.ui.theme import Styles
from trezor.ui.constants import *

class Popup(Modal):
    """
    A Popup screen with a log and a message, will auto close after a timeout


    User not need do anything, it just show some message. e.g. "Unlocking..." "Wiping ..."
    """
    def __init__(self, operating: str, icon: str|None=None):
        super().__init__()
        self.create_content(VStack)
        self.content: VStack
        self.content.spacing(CONTENT_SPACE)
        self.content.align(lv.ALIGN.TOP_MID, 0, 128)
        self.content.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)

        logo = self.add(lv.img)
        logo.set_src("A:/res/hp/loading-1.png")

        self.text = self.add(lv.label)
        self.text.add_style(Styles.popup, lv.PART.MAIN)
        self.text.set_long_mode(lv.label.LONG.WRAP)
        self.text.set_text(operating)
        self.auto_close_timeout = 2500

        # a animation
        a = lv.anim_t()
        a.init()
        a.set_var(logo)
        a.set_time(1000)
        a.set_values(0, 3600)
        a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
        a.set_custom_exec_cb(lambda a, v: logo.set_angle(v))

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, a)
        lv.anim_timeline_start(self.timeline)

    def on_deleting(self):
        super().on_deleting()
        lv.anim_timeline_stop(self.timeline)
        lv.anim_timeline_del(self.timeline)

    def text_color(self, color):
        self.text.set_style_text_color(color, 0)

class Loading(Modal):
    def __init__(self):
        super().__init__()

        self.create_content(VStack)
        self.content: VStack
        self.content.spacing(CONTENT_SPACE)
        self.content.align(lv.ALIGN.TOP_MID, 0, 128)
        self.content.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)

        logo = self.add(lv.img)
        logo.set_src("A:/res/hp/loading-1.png")

        self.text = self.add(lv.label)
        self.text.add_style(Styles.popup, lv.PART.MAIN)
        self.text.set_long_mode(lv.label.LONG.WRAP)
        self.text.set_text(i18n.Text.please_wait)
        self.auto_close_timeout = 2500

        # a animation
        a = lv.anim_t()
        a.init()
        a.set_var(logo)
        a.set_time(1000)
        a.set_values(0, 3600)
        a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
        a.set_custom_exec_cb(lambda a, v: logo.set_angle(v))

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, a)
        lv.anim_timeline_start(self.timeline)

    def on_deleting(self):
        super().on_deleting()
        lv.anim_timeline_stop(self.timeline)
        lv.anim_timeline_del(self.timeline)

class Doing(Modal):
    def __init__(self, msg: str):
        super().__init__()
        self.create_content(VStack)
        self.content: VStack
        self.content.spacing(CONTENT_SPACE)
        self.content.align(lv.ALIGN.TOP_MID, 0, 128)
        self.content.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)

        logo = self.add(lv.img)
        logo.set_src("A:/res/hp/loading-1.png")

        self.text = self.add(lv.label)
        self.text.set_width(lv.pct(100))
        self.text.add_style(Styles.popup, lv.PART.MAIN)
        self.text.set_long_mode(lv.label.LONG.WRAP)
        self.text.set_text(msg)

        # a animation
        a = lv.anim_t()
        a.init()
        a.set_var(logo)
        a.set_time(1000)
        a.set_values(0, 3600)
        a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
        a.set_custom_exec_cb(lambda a, v: logo.set_angle(v))

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, a)
        lv.anim_timeline_start(self.timeline)

    def on_deleting(self):
        super().on_deleting()
        lv.anim_timeline_stop(self.timeline)
        lv.anim_timeline_del(self.timeline)