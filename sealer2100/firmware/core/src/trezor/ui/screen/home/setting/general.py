import lvgl as lv

from typing import TYPE_CHECKING

from trezor import loop, workflow, log
from trezor.ui import i18n, Style, colors, font
from trezor.ui.component import VStack, HStack
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor.ui.constants import CONTENT_PAD

from .entry import ObserveSettingEntry

from . import _SETTING_VALUE_CHANGED

# general settings
from .language import Language
from .brightness import Brightness
from .vibration import Vibration
from .dynamic_effects import DynamicEffects
from .screen_wake_up import WakeUp
from .auto import Auto


class General(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.general)

        generals = [
            {
                "label": i18n.Setting.language,
                "cls": Language,
            },
            {
                "label": i18n.Setting.brightness,
                "cls": Brightness,
            },
            {
                "label": i18n.Setting.vibration,
                "cls": Vibration,
            },
            # {
            #     "label": i18n.Setting.dynamic_effects,
            #     "cls": DynamicEffects,
            # },
            # {
            #     "label": i18n.Setting.screen_wake_up,
            #     "cls": WakeUp,
            # },
            {
                "label": i18n.Setting.auto_lock_and_shutdown,
                "cls": Auto,
            },
        ]

        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.spacing(8)
        for g in generals:
            group.add(lambda parent: ObserveSettingEntry(parent=parent, **g))

