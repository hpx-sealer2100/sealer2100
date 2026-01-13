import lvgl as lv

from . import *

from trezor.ui import i18n, Style
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor.ui.component.container import HStack, VStack


class Quickstart(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.wallet)
        self.title.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN)  # 设置文本颜色为白色

        self.create_content(VStack)
        self.content: VStack
        self.content.set_style_align(lv.ALIGN.CENTER, lv.PART.MAIN)
        self.content.set_flex_flow(lv.FLEX_FLOW.COLUMN)  # 设置为列布局，居中对齐
        self.content.set_style_pad_left(16, lv.PART.MAIN)  # 设置左边距为0
        self.content.set_style_pad_right(16, lv.PART.MAIN)  # 设置右边距为0
        

        # create new wallet
        create = Item(
            self.content, i18n.Title.create_wallet, "A:/res/create-new-wallet.png"
        )
        create.add_event_cb(self.on_create_new_wallet, lv.EVENT.CLICKED, None)

        # restore new wallet
        restore = Item(
            self.content, i18n.Title.import_wallet, "A:/res/restore-wallet.png"
        )
        restore.add_event_cb(self.on_restore_wallet, lv.EVENT.CLICKED, None)

    def on_create_new_wallet(self, event: lv.event_t):
        from trezor import workflow
        from trezor.wire import DUMMY_CONTEXT
        from apps.management.reset_device import reset_device
        from trezor.messages import ResetDevice

        workflow.spawn(
            reset_device(
                DUMMY_CONTEXT,
                ResetDevice(
                    strength=128,
                    language=i18n.using.code,
                    pin_protection=True,
                ),
            )
        )

    def on_restore_wallet(self, event: lv.event_t):
        from apps.management.recovery_device import recovery_device
        from trezor.messages import RecoveryDevice
        from trezor import workflow
        from trezor.wire import DUMMY_CONTEXT

        workflow.spawn(
            recovery_device(
                DUMMY_CONTEXT,
                RecoveryDevice(
                    enforce_wordlist=True,
                    language=i18n.using.code,
                    pin_protection=True,
                ),
            )
        )


class Item(HStack):
    def __init__(self, parent, title, icon):
        super().__init__(parent)
        self.items_center()
        self.add_style(
            Style()
            .radius(16)
            .bg_opa(lv.OPA.COVER)
            .width(lv.pct(100))  # 432
            .height(80)
            .pad_right(32)
            .bg_color(lv.color_hex(0x111126)),
            0,
        )
        self.add_flag(lv.obj.FLAG.CLICKABLE)

        # icon
        self.icon = lv.img(self)
        self.icon.set_src(icon)

        # text
        self.title = lv.label(self)
        self.title.add_style(Styles.title, lv.PART.MAIN)
        self.title.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN)
        self.title.set_text(title)
        self.title.set_flex_grow(1)

        # right-arrow
        self.arrow = lv.label(self)
        self.arrow.set_text(lv.SYMBOL.RIGHT)
        self.arrow.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN)
        self.arrow.add_style(
            Styles.subtitle
            .size(32)
            .text_align_center(),
            0,
        )
