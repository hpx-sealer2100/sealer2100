import lvgl as lv

from . import *

from trezor import log
from trezor.ui import i18n
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles
from trezor.ui.result import Continue
from trezor.ui.screen.message import Message

class Ready(Message):
    def __init__(self, backup: bool):

        ready = i18n.Subtitle.wallet_is_ready
        if backup:
            ready += i18n.Text.please_backup
        super().__init__(i18n.Title.wallet_is_ready, ready, "A:/res/hp/ic_shwnfen_1.png")
        self.btn.set_text(i18n.Button.continue_)
        
