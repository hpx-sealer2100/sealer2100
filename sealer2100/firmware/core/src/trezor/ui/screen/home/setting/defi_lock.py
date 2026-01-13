from trezor.ui.screen.result import Result
from trezor.ui.component.labeled_item import LabeledItem
from trezor.ui.component.item import ItemToggle
import lvgl as lv

from . import *
from trezor.ui import i18n
from trezor.ui.screen import Navigation,VStack
from storage import device
from trezor import log, motor,workflow
from .detail import ToggleDetail

class DefiLock(ToggleDetail):
    @classmethod
    def get_current_value(cls) -> bool:
        return device.get_defi_lock()

    def __init__(self):        
        super().__init__(i18n.Setting.defi_Lock, i18n.Setting.defi_Lock)
        self.desc.set_text(i18n.Text.defi_lock_desc)

    def on_value_changed(self, e):
        workflow.spawn(self.toggling())

    async def toggling(self):
        enable = self.switch.has_state(lv.STATE.CHECKED)
        device.set_defi_lock(enable)
        if not enable:
            return

        # only tip when enabled
        icon = "A:/res/hp/ic_cuowu555.png"
        title = i18n.Text.defi_disable
        msg = i18n.Text.defi_disable_desc
        btn = i18n.Button.close
        from trezor.ui.screen.message import Message
        screen = Message(title, msg, icon)
        screen.btn.mode('close')
        screen.btn.set_text(i18n.Button.close)
        await screen.show()
        await screen
