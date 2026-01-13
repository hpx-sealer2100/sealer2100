from trezor.ui.component.labeled_item import LabeledItem
import lvgl as lv
from trezor import log, utils
from trezor.ui import i18n, font, colors
from trezor.ui.component import VStack, Item
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from storage import device

from .detail import OptionsDetail

class PinKeyboardOption:
    def __init__(self, v: bool):
        self.v = v
    def __eq__(self, value: 'PinKeyboardOption'):
        return self.v == value.v
    def __str__(self):
        if self.v:
            return i18n.Text.random
        else:
            return i18n.Text.default

class PinKeyboard(OptionsDetail): 
    def __init__(self):
        options = [PinKeyboardOption(False), PinKeyboardOption(True)]
        super().__init__(i18n.Setting.pin_keyboard, options)

        option = self.current()
        if option.v:
            self.desc.set_text(i18n.Text.pin_random)
        else:
            self.desc.set_text(i18n.Text.pin_default)
        self.desc.set_style_text_color(colors.USER.TAUPE,lv.PART.MAIN)
    
    @classmethod
    def current(cls) -> PinKeyboardOption:
        v = device.is_random_pin_map_enabled()
        return PinKeyboardOption(v)
    
    def save_option(self, option: PinKeyboardOption):
        device.set_random_pin_map_enable(option.v)
        if option.v:
            self.desc.set_text(i18n.Text.pin_random)
        else:
            self.desc.set_text(i18n.Text.pin_default)
        self.desc.set_style_text_color(colors.USER.TAUPE,lv.PART.MAIN)