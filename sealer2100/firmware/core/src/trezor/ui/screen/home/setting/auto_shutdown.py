from trezor.ui import i18n
from storage import device
from .detail import OptionsDetail, TimeOption, TimeOptionColored

class AutoShutdown(OptionsDetail):
    def __init__(self):
        options = [TimeOption(o) for o in [60, 120, 300, 600, -1]]
        super().__init__(i18n.Setting.auto_shutdown, options)

        option = self.current()
        if (option.v < 0):
            self.desc.set_text(i18n.Text.auto_shutdown_never_desc)
        else :
            self.desc.set_text(i18n.Text.auto_shutdown_desc.format(option))
    @classmethod
    def current(cls) -> TimeOption:
        t = device.get_autoshutdown_delay_ms()
        return TimeOptionColored(t // 1000) 
    def save_option(self, option: TimeOption):
        device.set_autoshutdown_delay_ms(option.v * 1000)
        from apps.base import reload_settings_from_storage
        reload_settings_from_storage()
        if (option.v < 0):
            self.desc.set_text(i18n.Text.auto_shutdown_never_desc)
        else :
            self.desc.set_text(i18n.Text.auto_shutdown_desc.format(option))
