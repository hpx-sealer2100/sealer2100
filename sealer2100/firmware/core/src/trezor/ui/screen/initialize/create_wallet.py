import lvgl as lv

from . import *
from trezor import log, workflow, motor
from trezor.ui.component import Button, CheckboxItem
from trezor.ui import i18n, log, colors, font
from trezor.ui.theme import Styles
from trezor.ui.screen import Modal, Navigation, VStack
from .ready import Ready
from trezor.ui.result import Cancel, Continue


#class CreateWallet(Modal):
class CreateWallet(Navigation):
    def __init__(self):
        super().__init__()
        #self.set_style_bg_color(colors.USER.WHITE, lv.PART.MAIN)
        self.set_icon("A:/res/hp/ic_create_wallet.png")
        self.title.set_text(i18n.Title.create_wallet)
        self.subtitle.set_text(i18n.Subtitle.create_wallet)

        #self.btn_left.set_text(i18n.Button.cancel)
        #self.btn_left.mode("cancel")
        #self.btn_right.set_text(i18n.Button.continue_)
        #self.btn_right.mode("continue")
        self.btn.set_text(i18n.Button.continue_)
        self.btn.add_event_cb(self.on_create_new_wallet, lv.EVENT.CLICKED, None)

        #self.btn_right.add_event_cb(self.on_create_new_wallet, lv.EVENT.CLICKED, None)
       # self.btn_left.add_event_cb(self.on_cancel_new_wallet, lv.EVENT.CLICKED, None)

    def on_create_new_wallet(self, event: lv.event_t):
        log.debug(__name__, "Create new wallet")
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

    def on_cancel_new_wallet(self, event: lv.event_t):
        log.debug(__name__, "Cancel new wallet")
        self.close(Cancel())


class ReadyCreateWallet(Ready):
    def __init__(self):
        super().__init__()

    def on_click_continue(self, e: lv.event_t):
        workflow.spawn(StartBackup().show())


class StartBackup(Modal):
    def __init__(self):
        super().__init__()
        self.set_icon("A:/res/hp/ic_start_backup.png")
        self.title.set_text(i18n.Title.start_backup)
        self.subtitle.set_text(i18n.Subtitle.start_backup)

        self.content.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)
        self.cbs: list[CheckboxItem] = []
        for check_text,check_title in zip(i18n.Text.backup_check,i18n.Text.backup_title):
            cb = CheckboxItem(self.content)
            #cb.text.set_style_text_font(font.small, lv.PART.MAIN)
            cb.set_icon("A:/res/hp/ic_enable_pin_iris.png")
            cb.set_title(check_title)
            #cb.set_text(check_text)
            cb.set_desc(check_text)
            cb.add_event_cb(self.on_click_checkbox, lv.EVENT.CLICKED, None)
            cb.add_style(Styles.group, lv.PART.MAIN)
            self.cbs.append(cb)
        # Create _btn
        self.btn.set_text(i18n.Button.continue_)
        self.btn.mode("done")
        self.btn.add_event_cb(self.on_continue, lv.EVENT.CLICKED, None)
        self.btn.add_state(lv.STATE.DISABLED)

    def on_continue(self, _):
        self.close(Continue())

    def on_click_checkbox(self, e: lv.event_t):
        obj = e.current_target
        if obj not in self.cbs:
            return
        enabled = all(cb.has_state(lv.STATE.CHECKED) for cb in self.cbs)
        if enabled:
            self.btn.clear_state(lv.STATE.DISABLED)
        else:
            self.btn.add_state(lv.STATE.DISABLED)


class Completion(Modal):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.nearing_completion)
        self.subtitle.set_text(i18n.Subtitle.nearing_completion)

        self.btn.set_text(i18n.Button.continue_)
        self.btn.mode("done")
        self.btn.width(432)
        # create self.content
        self.create_content(VStack)
        self.content: VStack

        self.btn.add_event_cb(self.on_continue, lv.EVENT.CLICKED, None)

    def on_continue(self, e):
        self.close(Continue())
