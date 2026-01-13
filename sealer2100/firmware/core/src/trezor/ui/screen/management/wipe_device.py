import lvgl as lv


from trezor.ui import i18n, Style, font, colors
from trezor.ui.constants import *
from trezor.ui.component import VStack, HStack,CheckboxItem
from trezor.ui.screen.confirm import Confirm
from trezor.ui.component.item import ItemImg
from trezor.ui.theme import Styles

class WipeDeviceTip(Confirm):
    def __init__(self):
        super().__init__()
        self.set_icon("A:/res/hp/ic_cuowu23.png")
        self.set_title(i18n.Setting.reset)
        self.subtitle.set_text(i18n.Subtitle.reset_wallet)
        self.subtitle.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)

        self.create_content(VStack)
        self.content: VStack        
        self.content.set_flex_align( lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.START )
        self.content.set_style_pad_top(20, lv.PART.MAIN)
        item = ItemImg(self.content, i18n.Text.reset_warning, "A:/res/hp/ic_infotip1.png","left")
        item.set_width(lv.pct(100))

        item.add_style(Styles.group, lv.PART.MAIN)
        item.set_style_flex_cross_place(lv.FLEX_ALIGN.START, lv.PART.MAIN)
        item.set_style_border_width(1, lv.PART.MAIN)
        item.set_style_border_color(lv.color_hex(0xFED500), lv.PART.MAIN)
        
        self.btn_confirm.set_text(i18n.Button.continue_)

class WipeDeviceConfirm(Confirm):
    def __init__(self):
        super().__init__()
        self.set_icon("A:/res/hp/ic_reset_warning.png")
        self.set_title(i18n.Title.reset_device)
        self.subtitle.set_text(i18n.Text.reset_device_desc)
        #self.subtitle.set_style_text_line_space(8, lv.PART.MAIN)
        self.btn_confirm.set_text(i18n.Button.reset)
        self.btn_confirm.add_state(lv.STATE.DISABLED)
        self.btn_confirm.mode('delete')

        # create self.content
        self.create_content(VStack)
        self.content : VStack
        self.content.spacing(8)
        self.cbs: list[CheckboxItem] = []
        for check_text in i18n.Text.reset_device_check:
            cb = self.add(CheckboxItem)
            self.cbs.append(cb)
            cb.set_text(check_text)
            cb.add_style(Styles.group, lv.PART.MAIN)
            cb.add_event_cb(self.on_click_checkbox, lv.EVENT.CLICKED, None)

    def on_click_checkbox(self, e: lv.event_t):
        obj = e.current_target
        if obj not in self.cbs:
            return
        enabled = all(cb.has_state(lv.STATE.CHECKED) for cb in self.cbs)
        if enabled:
            self.btn_confirm.clear_state(lv.STATE.DISABLED)
        else:
            self.btn_confirm.add_state(lv.STATE.DISABLED)
