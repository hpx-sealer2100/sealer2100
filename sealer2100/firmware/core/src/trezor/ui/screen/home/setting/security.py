import lvgl as lv
from trezor.ui.component.labeled_item import LabeledItem

from typing import TYPE_CHECKING

from trezor import  workflow ,log
from trezor.ui import i18n, Style, colors,font
from trezor.ui.component import VStack
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor.ui.constants import *
from .entry import SampleSettingEntry
class Security(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.security)
        # create self.content
        self.create_content(VStack)
        self.content: VStack

        from .authentication import Authentication
        from .pin_keyboard import PinKeyboard
        from .iris_manage import IrisManage
        from .pin_manage import PinManage
        from .defi_lock import DefiLock
        from .developer import Developer
        securities = [
            {
                "label": i18n.Setting.authentication,
                "cls": Authentication,
            },
            {
                "label": i18n.Setting.pin_keyboard,
                "cls": PinKeyboard
            },
            {
                "label": i18n.Setting.iris_management,
                "cls": IrisManage
            },
            {
                "label": i18n.Setting.pin_management,
               "cls": PinManage
            },
            {
                "label": i18n.Setting.defi_Lock,
                "cls": DefiLock
 
            },
            {
                "label": i18n.Setting.check_security,
                "cls": Developer
            }
        ]
        
        # Create list
        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.spacing(8)

        for a in securities:
            text, cls = a["label"], a["cls"]
            item: SampleSettingEntry = group.add(lambda p: SampleSettingEntry(p, text, cls))
            item.add_style(Styles.board, lv.PART.MAIN)
            
        item.set_style_border_side(lv.BORDER_SIDE.NONE, lv.PART.MAIN)