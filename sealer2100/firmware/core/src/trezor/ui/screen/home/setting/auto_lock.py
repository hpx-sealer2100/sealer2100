from trezor.ui import i18n, log
from storage import device
from .detail import OptionsDetail, TimeOption, TimeOptionColored


class AutoLock(OptionsDetail):
    def __init__(self):
        options = [TimeOption(o) for o in [60, 120, 300, 600, 1800, -1]]
        super().__init__(i18n.Setting.auto_lock, options)

        option = self.current()
        if option.v < 0:
            self.desc.set_text(i18n.Text.auto_lock_never_desc)
        else:
            self.desc.set_text(i18n.Text.auto_lock_desc.format(option))

    @classmethod
    def current(cls) -> TimeOption:
        t = device.get_autolock_delay_ms()
        log.debug(__name__, f"auto lock in {t}ms")
        return TimeOptionColored(t // 1000)

    def save_option(self, option: TimeOption):
        device.set_autolock_delay_ms(option.v * 1000)
        from apps.base import reload_settings_from_storage

        reload_settings_from_storage()
        if option.v < 0:
            self.desc.set_text(i18n.Text.auto_lock_never_desc)
        else:
            self.desc.set_text(i18n.Text.auto_lock_desc.format(option))
