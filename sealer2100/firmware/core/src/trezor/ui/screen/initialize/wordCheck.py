import lvgl as lv

from . import *
from trezor.ui.screen.result import Result
from trezor.ui import i18n, Style, font,colors,log
from trezor.ui.screen import Modal,VStack,HStack
from trezor.ui.theme import Styles

class CheckError(Result):
    def __init__(self):
        super().__init__(i18n.Text.word_error,i18n.Text.word_error_desc,"A:/res/hp/ic_cuowu555.png"
                            , i18n.Button.try_again ,False)
    def action(self, e: lv.event_t):
        log.debug(__name__, f"event: click check try again ")  
        self.close(True)
        
class CheckPassed(Result):
    def __init__(self):
        super().__init__(i18n.Text.verification_passed,i18n.Text.verification_passed_desc,"A:/res/hp/ic_done566.png"
                            ,i18n.Button.continue_,True)
    def action(self, e: lv.event_t):
        log.debug(__name__, f"event: click continue ")  
        self.close(True)

