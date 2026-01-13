import lvgl as lv

from . import *
from .entry import ObserveSettingEntry

from .auto_lock import AutoLock
from .auto_shutdown import AutoShutdown

class Auto(Navigation):
    def __init__(self):
        super().__init__()
        self.set_title(i18n.Setting.auto_lock_and_shutdown)
        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Styles.container, lv.PART.MAIN)
        self.content.spacing(8)

        self.content.add(lambda parent: ObserveSettingEntry(parent, i18n.Setting.auto_lock, AutoLock))
        self.content.add(lambda parent: ObserveSettingEntry(parent, i18n.Setting.auto_shutdown, AutoShutdown))

    @classmethod
    def current(cls):
        return ""
