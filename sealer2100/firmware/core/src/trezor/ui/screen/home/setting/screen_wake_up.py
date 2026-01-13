from trezor.ui import i18n,log
from storage import device
from .detail import OptionsDetail

class WakeUpOptionValue:
    def __init__(self, v: device.WeakupType):
        self.v = v

    def __eq__(self, o):
        self.v == o.v

    def __str__(self):
        # weakup by click screen
        if self.v == 0:
            return i18n.Text.device_wakeup_by_touch_screen
        else:
            return i18n.Text.device_wakeup_by_click_power_button

class WakeUp(OptionsDetail):
    def __init__(self):
        options = [WakeUpOptionValue(o) for o in [0, 1]]
        super().__init__(i18n.Setting.auto_lock, options)

        option = self.current()
        if option.v == 0:
            self.desc.set_text(i18n.Text.screen_wake_up_tap_desc)
        else:
            self.desc.set_text(i18n.Text.screen_wake_up_button_desc)

    @classmethod
    def current(cls) -> WakeUpOptionValue:
        t = device.get_device_weakup()
        return WakeUpOptionValue(t)
            
    def save_option(self, option: WakeUpOptionValue):
        if option.v == 0:
            self.desc.set_text(i18n.Text.screen_wake_up_tap_desc)
        else:
            self.desc.set_text(i18n.Text.screen_wake_up_button_desc)
        device.set_device_weakup(option.v)
