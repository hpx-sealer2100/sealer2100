from trezor.ui.component.button import Button
import lvgl as lv

from . import *

from trezor.ui import i18n, colors
from trezor.ui.screen import Navigation,VStack

class ActivateDevice(Navigation):
    def __init__(self):        
        super().__init__()
        #self.set_style_bg_color(colors.USER.WHITE, lv.PART.MAIN)
        self.set_icon("A:/res/hp/ic_active.png")
        self.title.set_text(i18n.Title.activate_device)
        self.subtitle.set_text(i18n.Subtitle.activate_device)
        
        self.create_content(VStack)
        self.content.set_height(lv.SIZE.CONTENT)

        self.btn_upper.set_text(i18n.Button.create_wallet)
                
        self.btn_down.set_text(i18n.Button.import_wallet)
        self.btn_down.mode("second")
        
        self.btn_upper.add_event_cb(self.on_create_new_wallet, lv.EVENT.CLICKED, None)
        self.btn_down.add_event_cb(self.on_restore_wallet, lv.EVENT.CLICKED, None)

    def on_create_new_wallet(self, event: lv.event_t):
        from trezor import workflow
        from .create_wallet import CreateWallet

        workflow.spawn(CreateWallet().show())

    def on_restore_wallet(self, _: lv.event_t):
        from apps.management.recovery_device import recovery_device
        from trezor.messages import RecoveryDevice
        from trezor import workflow
        from trezor.wire import DUMMY_CONTEXT

        workflow.spawn(
            recovery_device(
                DUMMY_CONTEXT,
                RecoveryDevice(
                    enforce_wordlist=True,
                    language=i18n.using.code,
                    pin_protection=True,
                ),
            )
        )