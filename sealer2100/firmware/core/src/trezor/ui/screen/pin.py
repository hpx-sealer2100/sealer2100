import lvgl as lv

from . import Modal, Navigation

from trezor import log
from trezor.ui import Style, font, colors, i18n, Cancel, Continue
from trezor.ui.theme import Styles
from trezor.ui.constants import *
from trezor.ui.component import VStack, PinKeyboard, CheckboxItem

class InputPinScreen(Navigation):
    def __init__(self, title: str | None = None):
        super().__init__()
        self.title.set_text(title)

        self.ta = lv.textarea(self.content)
        self.ta.set_password_mode(True)
        # only allow numbers
        self.ta.set_accepted_chars("0123456789")
        self.ta.set_one_line(True)
        self.ta.set_max_length(MAX_PIN_LENGTH)
        self.ta.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)

        self.ta.add_style(
            Style()
            .border_width(0)
            .bg_opa(lv.OPA.TRANSP)
            .width(lv.pct(100))
            .text_font(font.Medium.PF44)
            .text_letter_space(8)
            .text_align_center(),
            lv.PART.MAIN,
        )

        kbd = PinKeyboard(self)
        kbd.align(lv.ALIGN.BOTTOM_MID, 0, 0)
        kbd.textarea = self.ta
        kbd.tip_count_min = MIN_PIN_LENGTH
        kbd.max_pin_length = MAX_PIN_LENGTH
        kbd.min_pin_length = MIN_PIN_LENGTH
        kbd.add_event_cb(lambda e: self.input_done(), lv.EVENT.READY, None)
        kbd.add_event_cb(lambda e: self.cancel(), lv.EVENT.CANCEL, None)

        self.ta.align(lv.ALIGN.OUT_TOP_MID, 0, 0)

    def warning(self, msg: str):
        """
        Show a warning msg when show PIN screen
        """
        if not msg:
            return
        warn = self.add(lv.label)
        warn.set_width(lv.pct(100))
        warn.move_to_index(0)
        warn.set_text(msg)

        warn.set_long_mode(lv.label.LONG.WRAP)
        warn.add_style(
            Style()
            .text_color(colors.USER.DANGER)
            .text_align(lv.TEXT_ALIGN.CENTER),
            lv.PART.MAIN,
        )
    def dismiss(self):
        super().dismiss()
        from trezor.ui.screen.manager import mark_dismissing
        mark_dismissing(self)

    def input_done(self):
        pin = self.ta.get_text()
        self.close(pin)

    def cancel(self):
        self.close(Cancel())


class EnablePinScreen(Modal):
    def __init__(self):        
        super().__init__()
        self.set_icon("A:/res/hp/ic_pin_protect.png")
        self.title.set_text(i18n.Guide.enable_pin_protection)
        self.subtitle.set_text(i18n.Text.set_pin_length)
        
        # default disable continue
        self.btn.set_text(i18n.Button.continue_)
        self.btn.add_state(lv.STATE.DISABLED)
        self.btn.add_event_cb(lambda e: self.close(Continue()), lv.EVENT.CLICKED, None)
            
        # create self.content
        self.create_content(VStack)
        
        self.cbs: list[CheckboxItem] = []
        for txt,txt_title in zip(i18n.Text.enable_pin_tips,i18n.Text.enable_pin_titles):
            cb = self.add(CheckboxItem)
            cb.set_icon("A:/res/hp/ic_enable_pin_iris.png")
            cb.set_title(txt_title)
            cb.set_desc(txt)
            cb.add_style(Styles.group, lv.PART.MAIN)
            cb.add_event_cb(self.on_click_checkbox, lv.EVENT.CLICKED, None)
            self.cbs.append(cb)
        
    def on_click_checkbox(self, e: lv.event_t):
        obj = e.current_target
        if obj not in self.cbs:
            return
        enabled = all(cb.has_state(lv.STATE.CHECKED) for cb in self.cbs)
        if enabled:
            self.btn.clear_state(lv.STATE.DISABLED)
        else:
            self.btn.add_state(lv.STATE.DISABLED)
