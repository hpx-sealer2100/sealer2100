import lvgl as lv
from storage import device
from trezor import motor, log
from trezor.ui import i18n

from .detail import ToggleDetail

class DynamicEffects(ToggleDetail):
    @classmethod
    def get_current_value(cls):
        return device.is_animation_enabled()

    def __init__(self):
        super().__init__(i18n.Setting.dynamic_effects, i18n.Setting.dynamic_effects)

        if self.get_current_value():
            self.desc.set_text(i18n.Text.dynamic_effects_allow_desc)
        else:
            self.desc.set_text(i18n.Text.dynamic_effects_disabled_desc)
    
    def on_value_changed(self, e):
        motor.vibrate()
        enabled = self.switch.has_state(lv.STATE.CHECKED)
        log.debug(__name__, f"animation {'enabled' if enabled else 'disabled'}")
        if enabled:
            self.desc.set_text(i18n.Text.dynamic_effects_allow_desc)
        else:
            self.desc.set_text(i18n.Text.dynamic_effects_disabled_desc)
        device.set_animation_enable(enabled)
        super().on_value_changed(e)
