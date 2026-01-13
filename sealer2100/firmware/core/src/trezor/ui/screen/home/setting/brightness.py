import lvgl as lv

from trezor.ui import i18n, Style, colors, font
from trezor.ui.screen import Navigation
from storage import device
from trezor.ui import display


class BrightnessOptionValue:
    def __init__(self, v: int):
        self.v = v

    def __eq__(self, value):
        self.v == value.v

    def __str__(self):
        return f"{self.v}%"


class Brightness(Navigation):
    @classmethod
    def current(cls) -> BrightnessOptionValue:
        v = device.get_brightness()
        return BrightnessOptionValue(v)

    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.brightness)

        # the property of SettingDetailProtocol
        self.subscriber: lv.obj = None

        # a slider for adjusting brightness
        slider = lv.slider(self.content)

        slider.set_size(100, 432)
        style_main = (
            Style()
            .radius(16)
            .bg_color(colors.USER.DARK_GRAY)
            .bg_opa(lv.OPA.COVER)
            .border_width(0)
        )
        style_indicator = (
            Style()
            .radius(0)
            .bg_color(lv.color_hex(0x00FE33))
            .bg_opa(lv.OPA.COVER)
            .border_width(0)
        )
        style_knob = (
            Style()
            .radius(16)
            .pad_left(0)
            .pad_right(0)
            .pad_top(0)
            .bg_opa(lv.OPA.TRANSP)
            .border_width(0)
        )
        slider.add_style(style_main, lv.PART.MAIN)
        slider.add_style(style_indicator, lv.PART.INDICATOR)
        slider.add_style(style_knob, lv.PART.KNOB)

        slider.set_range(10, 100)
        slider.align(lv.ALIGN.CENTER, 0, 0)
        slider.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)
        slider.add_event_cb(self.on_released, lv.EVENT.RELEASED, None)

        brightness = device.get_brightness()
        slider.set_value(brightness, lv.ANIM.OFF)

        self.label = lv.label(slider)
        self.label.set_text(f"{BrightnessOptionValue(brightness)}")
        self.label.set_style_bg_opa(lv.OPA.TRANSP, lv.PART.MAIN)
        self.label.set_style_text_color(colors.USER.BLACK, lv.PART.MAIN)
        self.label.set_style_text_font(font.Medium.PF32, lv.PART.MAIN)
        self.label.align(lv.ALIGN.BOTTOM_MID, 0, -32)

    def on_value_changed(self, event):
        slider = event.target
        brightness = slider.get_value()
        device.set_brightness(brightness)
        display.backlight(brightness)
        self.label.set_text(f"{BrightnessOptionValue(brightness)}")

    def on_released(self, event):
        from . import _SETTING_VALUE_CHANGED

        lv.event_send(self.subscriber, _SETTING_VALUE_CHANGED, None)
