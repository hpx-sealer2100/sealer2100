import lvgl as lv

from . import *
from trezor.ui import i18n, log
from trezor.ui.screen import Navigation,VStack
from trezor.ui.theme import Styles
from trezor import workflow
class MetacardScreen(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.start_setting)
        self.subtitle.set_text(i18n.Subtitle.start_setting)
        # Create continue_btn
        self.btn.set_text(i18n.Button.continue_)
        self.btn.mode("done")
        self.create_content(VStack)
        self.content: VStack
        self.content.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)        
        # self.content.set_style_pad_left(24, lv.PART.MAIN) 
        self.content.set_height(lv.SIZE.CONTENT)
 
        img = lv.img(self)
        img.set_src("A:/res/hp/nfc_card.png")
        img.align(lv.ALIGN.BOTTOM_MID, 0, -120)
        img.set_size(300, 360)

        
        self.btn.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, e: lv.event_t):
        log.debug(__name__, f"TODO TO pin")
        pass
    
        
       
    
