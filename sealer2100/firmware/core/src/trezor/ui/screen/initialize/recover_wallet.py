
import lvgl as lv
from micropython import const

from . import *
from trezor.ui.component.item import Item
from trezor.ui.screen.result import Result
from trezor.ui import i18n, log, font, colors
from trezor.ui.screen import Navigation,VStack,Modal
from trezor.ui import Continue, Cancel
from trezor.ui.theme import Styles

RECOVER_FROM_MNEMOIC = const(1)
RECOVER_FROM_METACARD = const(2)

class RecoverWallet(Navigation):
    def __init__(self):
        super().__init__()
        self.set_icon("A:/res/hp/ic_import_wallet.png")
        self.title.set_text(i18n.Title.import_wallet)
        self.subtitle.set_text(i18n.Subtitle.import_wallet)

        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)



        app = Item(self.content, i18n.Button.mnemonic, "A:/res/arrow-right_3.png")
        app.label.set_style_text_font(font.medium, lv.PART.MAIN)
        app.set_width(lv.pct(100))
        app.add_flag(lv.obj.FLAG.CLICKABLE)
        app.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        app.add_style(Styles.board, lv.PART.MAIN)
        app.action = self.on_mnemonic

        app = Item(self.content, i18n.Button.metacard, "A:/res/arrow-right_3.png")
        # app.label.set_style_text_font(font.medium, lv.PART.MAIN)
        # app.add_style(Style().bg_color(colors.USER.LIGHT_GRAY).text_color(colors.USER.DARK_GRAY), lv.STATE.DISABLED)
        # app.add_style(Styles.board, lv.PART.MAIN)
        # app.add_state(lv.STATE.DISABLED)
        app.label.set_style_text_font(font.medium, lv.PART.MAIN)
        app.set_width(lv.pct(100))
        app.add_flag(lv.obj.FLAG.CLICKABLE)
        app.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        app.add_style(Styles.board, lv.PART.MAIN)
        app.add_state(lv.STATE.DISABLED)
        app.action = self.on_metacard

    def on_mnemonic(self):
        self.close(RECOVER_FROM_MNEMOIC)
    def on_metacard(self):
        self.close(RECOVER_FROM_METACARD)

    def on_nav_back(self, event):
        self.close(Cancel())

    def dismiss(self):
        super().dismiss()
        from trezor.ui.screen import manager
        manager.mark_dismissing(self)

class ImportError(Result):
    def __init__(self):
        super().__init__(i18n.Text.mismatch,
                         i18n.Text.mismatch_desc,
                         "A:/res/hp/ic_cuowu555.png",
                         i18n.Button.try_again
                         ,False)
    def action(self):
        log.debug(__name__, f"event:  {self._result}")


class EnterMnemonic(Modal):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.enter_mnemonic)
        self.subtitle.set_text(i18n.Text.enter_mnemonic_desc)

        self.btn_left.set_text(i18n.Button.cancel)
        self.btn_left.mode("cancel")
        self.btn_right.set_text(i18n.Button.continue_)
        self.btn_right.mode("done")

        self.btn_right.add_event_cb(self.on_continue, lv.EVENT.CLICKED, None)
        self.btn_left.add_event_cb(self.on_cancel, lv.EVENT.CLICKED, None)
    def on_continue(self, event: lv.event_t):
        self.channel.publish(Continue())

    def on_cancel(self, event: lv.event_t):
        self.channel.publish(Cancel())
