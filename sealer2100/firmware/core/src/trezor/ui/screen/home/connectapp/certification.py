import lvgl as lv

from . import *

from trezor.ui import i18n,log
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles
class Certification(Modal):
    def __init__(self):
        super().__init__()
        img = lv.img(self.content)
        img.set_src("A:/res/hp/ic_device_anquan.png")
        img.set_pos(166, 120)
        
        title = lv.label(self.content)
        title.set_text(i18n.Setting.authentication)
        title.set_pos(0, 256)
        title.add_style(Styles.title, 0)

        subtitle = lv.label(self.content)
        subtitle.set_text(i18n.Subtitle.equipment_certification)
        subtitle.add_style(Styles.subtitle, 0)
        subtitle.set_long_mode(lv.label.LONG.WRAP)
        subtitle.align_to(title, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        
        self.btn_right.set_text(i18n.Button.continue_)
        self.btn_right.mode("done")
        self.btn_left.set_text(i18n.Button.cancel)
        self.btn_left.mode("cancel")

        self.btn_right.add_event_cb(self.on_continue, lv.EVENT.CLICKED, None)
        self.btn_left.add_event_cb(self.on_cancel, lv.EVENT.CLICKED, None)
        
    def on_continue(self, event: lv.event_t):
        log.debug(__name__, "click continue")
       
    def on_cancel(self, event: lv.event_t):
        log.debug(__name__, "click cancel")    
