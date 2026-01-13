import lvgl as lv

from trezor.ui import i18n, Done, colors, font
from trezor.ui.screen import Modal
from trezor.ui.component import VStack
from trezor.ui.theme import Styles
from trezor.ui.constants import *

class Message(Modal):
    """
    Message screen use for display message for user

    It have a button. User only need click the button, means "Yes, I have know"
    """
    def __init__(self, title, message, icon=None):
        super().__init__()

        self.btn.set_text(i18n.Button.ok)

        self.create_content(VStack)
        self.content: VStack
        self.content.items_center()
        self.content.set_style_flex_main_place(lv.FLEX_ALIGN.START, lv.PART.MAIN)
        self.content.spacing(CONTENT_SPACE)

        # icon
        if icon:
            self.add(lv.img).set_src(icon)

        # title
        if title:
            obj = self.add(lv.label)
            obj.set_text(title)
            obj.set_width(lv.pct(100))
            obj.set_long_mode(lv.label.LONG.WRAP)
            obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)
            self.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
        # msg
        if message:
            self.text = self.add(lv.label)
            self.text.set_width(lv.pct(100))
            self.text.set_text(message)
            self.text.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
            self.text.set_style_text_font(font.small, lv.PART.MAIN)
            self.text.set_style_text_line_space(8, 0)

        self.btn.add_event_cb(lambda _: self.on_click_ok(), lv.EVENT.CLICKED, None)

    def button_text(self, text):
        self.btn.set_text(text)

    def text_color(self, color):
        self.text.set_style_text_color(color, 0)

    def on_click_ok(self):
        self.close(Done())

class Info(Message):
    def __init__(self, title, message, icon = None):
        super().__init__(title, message, icon or "A:/res/info-two.png")

class Success(Message):
    def __init__(self, title, message, icon = None):
        super().__init__(title, message, icon or "A:/res/hp/icon_done_3.png")

class Warning(Message):
    def __init__(self, title, message, icon = None ):
        super().__init__(title, message, icon or "A:/res/warning.png")

class Error(Message):
    def __init__(self, title, message, icon = None):
        super().__init__(title, message, icon or "A:/res/hp/icon_error_3.png")
