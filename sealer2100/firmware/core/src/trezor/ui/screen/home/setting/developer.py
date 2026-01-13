from trezor.ui.component.labeled_item import LabeledItem
from trezor.ui.component.item import ItemToggle
import lvgl as lv

from . import *
from trezor.ui import i18n
from trezor.ui.screen import Navigation,VStack
from storage import device
from trezor import log, motor
from .detail import ToggleDetail

class Developer(ToggleDetail):
    @classmethod
    def get_current_value(cls):
        return device.safety_check_level() == device.SAFETY_CHECK_LEVEL_STRICT

    def __init__(self):        
        super().__init__(i18n.Setting.check_security, i18n.Setting.check_security)
        self.desc.set_text(i18n.Text.check_security_enable_desc)
        
    def on_value_changed(self, e):
        enabled = self.switch.has_state(lv.STATE.CHECKED)
        log.debug(__name__, f"developer {'enabled' if enabled else 'disabled'}")
        level = device.SAFETY_CHECK_LEVEL_STRICT if enabled else device.SAFETY_CHECK_LEVEL_PROMPT
        device.set_safety_check_level(level)
        return super().on_value_changed(e)