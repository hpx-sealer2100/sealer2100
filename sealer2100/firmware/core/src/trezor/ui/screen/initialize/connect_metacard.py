import lvgl as lv

from . import *

from trezor import log
from trezor.ui import i18n
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles

class ConnectMetacardScreen(Modal):
    def __init__(self,):
        super().__init__()
        # Create img
        img1 = lv.img(self.content)
        img1.set_src("A:/res/hp/ic_card_2.png")
        img1.set_pos(193, 167)

        img = lv.img(self.content)
        img.set_src("A:/res/hp/ic_card_1.png")
        img.set_pos(166, 140)
        img.set_pivot(50, 50) 
        
        self.anim = lv.anim_t()
        self.anim.init()
        self.anim.set_var(img)
        self.anim.set_values(0, 3600)  
        self.anim.set_time(2000)  
        try:
            self.anim.set_repeat_count(lv.ANIM_REPEAT_INFINITE)  
        except AttributeError:
            try:
                self.anim.set_repeat_count(lv.anim.REPEAT_INFINITE)  
            except AttributeError:
                self.anim.set_repeat_count(0xFFFF)  
        
        self.anim.set_custom_exec_cb(lambda anim, val: img.set_angle(val))
        self.anim.start()
        
        self.btn.set_text(i18n.Button.cancel)
        self.btn.mode("info")

        title = lv.label(self.content)
        title.set_text(i18n.Text.in_connection)
        title.set_pos(0, 256)
        title.add_style(Styles.title, 0)

        subtitle = lv.label(self.content)
        subtitle.set_text(i18n.Text.in_connection_desc)
        subtitle.add_style(Styles.subtitle, 0)
        subtitle.set_long_mode(lv.label.LONG.WRAP)
        subtitle.align_to(title, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        
        self.btn.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        
    def on_click(self, e: lv.event_t):
        log.debug(__name__, f"event: click cancel")