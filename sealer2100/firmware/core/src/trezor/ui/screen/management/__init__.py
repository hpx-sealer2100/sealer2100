import lvgl as lv

from trezor.ui import i18n, Style, font, colors
from trezor.ui.theme import Styles
from trezor.ui.constants import *
from trezor.ui.component import VStack, CheckboxItem
from trezor.ui.screen import Modal
from trezor.ui.screen.confirm import SimpleConfirm, Confirm

class PinEnableConfirm(SimpleConfirm):
    def __init__(self):
        super().__init__(i18n.Text.enable_pin_desc)
        self.set_icon("A:/res/hp/ic_pin.png")
        self.set_title(i18n.Text.enable_pin)
        self.text.set_recolor(True)
        self.mode('result')
        self.btn_confirm.set_text(i18n.Button.to_open)

class PinChangeConfirm(SimpleConfirm):
    def __init__(self):
        super().__init__(i18n.Text.change_pin)
        self.set_icon("A:/res/hp/ic_pin_change.png")
        self.title.set_text(i18n.Setting.change_pin,)
        self.text.set_recolor(True)
        self.mode('result')

class PinRemoveConfirm(SimpleConfirm):
    def __init__(self):
        super().__init__(i18n.Text.remove_pin_desc)
        self.set_icon( "A:/res/hp/ic_pin34.png")
        self.title.set_text(i18n.Setting.remove_pin)
        self.text.set_recolor(True)
        self.mode('result')
        self.btn_confirm.set_text(i18n.Button.delete)
        self.btn_confirm.mode('danger')

class PinLimitError(Modal):
    def __init__(self):
        super().__init__()
        self.create_content(VStack)
        self.content: VStack
        self.content.items_center()
        self.content.spacing(CONTENT_SPACE)

        img = self.add(lv.img)
        img.set_src("A:/res/hp/ic_cuowu23.png")

        title = self.add(lv.label)
        title.set_text(i18n.Text.limit_error)
        title.add_style(Styles.title, 0)

        subtitle = self.add(lv.label)
        subtitle.set_recolor(True)
        subtitle.set_text(i18n.Text.limit_error_desc)
        subtitle.add_style(Styles.subtitle, 0)
        subtitle.set_long_mode(lv.label.LONG.WRAP)
        subtitle.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        self.auto_close_timeout = 5000

class DeviceProtectionVerifyRetry(Confirm):
    def __init__(self, rem: int):
        super().__init__()
        self.create_content(VStack)
        self.content: VStack
        self.content.items_center()
        self.content.spacing(CONTENT_SPACE)

        img = self.add(lv.img)
        img.set_src("A:/res/hp/ic_cuowu555.png")

        title = self.add(lv.label)
        title.set_text(i18n.Title.device_protection_verification_failed)
        title.add_style(Styles.title, 0)

        subtitle = self.add(lv.label)
        subtitle.set_recolor(True)
        subtitle.set_text(i18n.Text.device_protection_verification_failed_msg)
        subtitle.add_style(Styles.subtitle, 0)
        subtitle.set_long_mode(lv.label.LONG.WRAP)
        subtitle.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)

        msg = self.add(lv.label)
        msg.set_text(i18n.Text.device_protection_verification_left.format(rem))
        msg.add_style(
            Style()
            .text_font(font.small)
            .text_color(colors.USER.RED),
            lv.PART.MAIN
        )

        self.btn_confirm.set_text(i18n.Button.try_again)

class IrisRegistConfirm(SimpleConfirm):
    def __init__(self):
        super().__init__(i18n.Text.register_iris_desc)
        self.set_icon("A:/res/hp/ic_hongmo_1.png")
        self.set_title(i18n.Text.register_iris)
        self.mode('result')
        self.btn_confirm.set_text(i18n.Button.to_open)

class IrisChangeConfirm(SimpleConfirm):
    def __init__(self):
        super().__init__(i18n.Text.change_iris_desc)
        self.set_icon("A:/res/hp/ic_hongmo_genggai.png")
        self.set_title(i18n.Setting.change_iris)
        self.mode('result')
        self.btn_confirm.set_text(i18n.Button.confirm)

class IrisRemoveConfirm(SimpleConfirm):
    def __init__(self):
        super().__init__(i18n.Text.remove_iris_desc)
        self.set_icon("A:/res/hp/ic_hongmo_yichu.png")
        self.set_title(i18n.Setting.remove_iris)
        self.mode('result')
        self.btn_confirm.set_text(i18n.Button.delete)
        self.btn_confirm.mode('delete')