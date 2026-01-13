import lvgl as lv

from . import *

from trezor import log
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles

class Result(Modal):
    def __init__(self,title_str,subtitle_str,icon,btn_txt,result:bool| None):
        super().__init__()
        # Create img
        img = lv.img(self.content)
        self._result = result
        img.set_src(icon)
        img.set_pos(166, 140)
        
        self.btn.set_text(btn_txt)
        self.btn.mode("done" if result else "info")
        self.btn.width(432)

        title = lv.label(self.content)
        title.set_text(title_str)
        title.set_pos(0, 256)
        title.add_style(Styles.title, 0)

        subtitle = lv.label(self.content)
        subtitle.set_text(subtitle_str)
        subtitle.add_style(Styles.subtitle, 0)
        subtitle.set_long_mode(lv.label.LONG.WRAP)
        subtitle.align_to(title, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        
        self.btn.add_event_cb(self.action, lv.EVENT.CLICKED, None)
    def action(self, e: lv.event_t):
        log.debug(__name__, f"event: click  {self._result}")
        pass

