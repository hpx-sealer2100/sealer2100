import lvgl as lv
from storage import device
from trezor import motor, log
from trezor.ui import i18n

from .detail import ToggleDetail

class Vibration(ToggleDetail):
    @classmethod
    def get_current_value(cls):
        return device.keyboard_haptic_enabled()

    def __init__(self):
        super().__init__(i18n.Setting.vibration, i18n.Text.keyboard_feedback)
    
        self.desc.set_text(i18n.Text.keyboard_feedback_desc)
    def on_value_changed(self, e):
        motor.vibrate()
        enabled = self.switch.has_state(lv.STATE.CHECKED)
        log.debug(__name__, f"vibration {'enabled' if enabled else 'disabled'}")
        device.toggle_keyboard_haptic(enabled)
        if enabled:
            motor.vibrate()

        super().on_value_changed(e)
