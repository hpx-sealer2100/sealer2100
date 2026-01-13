import lvgl as lv

from . import *

from trezor import log
from trezor.ui import i18n
from trezor.ui.screen.message import Message
from trezor.ui.theme import Styles

class Help(Message):
    def __init__(self):
        super().__init__(i18n.Text.need_help, i18n.Text.need_help_desc, "A:/res/hp/ic_help.png")

        url = self.add(lv.label)
        url.set_recolor(True)
        url.set_text('help.sealer2100.com')
        url.set_style_text_color(colors.USER.GREEN, lv.PART.MAIN)

        self.button_text(i18n.Button.close)
        self.btn.mode("close")
    def on_click(self, e: lv.event_t):
        self.dismiss()
