
import lvgl as lv

from trezor import log,workflow
from trezor.ui import i18n, colors, Style, font
from trezor.ui.theme import Styles
from trezor.ui.constants import CONTENT_PAD, SCREEN_WIDTH
from trezor.ui.component import VStack, HStack
from trezor.ui.screen import Navigation
from trezor.ui.screen.confirm import Confirm
from storage import device
from .entry import ActionSettingEntry, SampleSettingEntry
from .detail import ToggleDetail, ToggleOptionValue

class Wallet(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.wallet)

        # create self.content
        self.create_content(VStack)
        self.content: VStack

        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.spacing(8)

        # check mnemonic, action setting entry
        ActionSettingEntry(group, i18n.Setting.check_mnemonic, self.check_mnemonic)

        # passphrase, sample setting entry
        SampleSettingEntry(group, "Passphrase", Passphrase)

        reset_view = group.add(lv.label)
        reset_view.set_width(lv.pct(100))
        reset_view.add_style(Styles.board, lv.PART.MAIN)
        reset_view.set_style_text_font(font.medium, lv.PART.MAIN)
        reset_view.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        reset_view.set_style_text_color(colors.USER.RED, lv.PART.MAIN)
        reset_view.set_text(i18n.Setting.reset)
        reset_view.add_flag(lv.obj.FLAG.CLICKABLE)
        reset_view.add_event_cb(self.click_reset, lv.EVENT.CLICKED, None)


    def check_mnemonic(self):
        from trezor import workflow
        from trezor.ui.screen import manager
        from apps.management.recovery_device import recovery_device
        from trezor.messages import RecoveryDevice
        from trezor.wire import DUMMY_CONTEXT, ProcessError
        async def check_mnemonic():
            try:
                await recovery_device(DUMMY_CONTEXT, RecoveryDevice(dry_run=True))
                manager.try_switch_to(Wallet)
            except Exception as e:
                log.debug(__name__, "got a exception ....")
                log.exception(__name__, e)

        workflow.spawn(check_mnemonic())

    def click_reset(self, e: lv.event_t):
        log.debug(__name__, "click reset")
        from apps.management.wipe_device import wipe_device
        from trezor.wire import DUMMY_CONTEXT
        from trezor.messages import WipeDevice
        workflow.spawn(wipe_device(DUMMY_CONTEXT, WipeDevice()))

class Passphrase(ToggleDetail):

    # the master toggle value
    @classmethod
    def current(cls) ->ToggleOptionValue:
        enabled = device.is_passphrase_enabled()
        return ToggleOptionValue(enabled)

    def __init__(self):
        super().__init__("Passphrase", "Passphrase")

        option = self.current()
        if option.v:
            self.desc.set_text(i18n.Text.passphrase_enable)
        else:
            self.desc.set_text(i18n.Text.passphrase_disable)

    def on_value_changed(self, e):
        # not have subscriber, no need call super
        # super().on_value_changed(e)
        workflow.spawn(self.togging())

    async def togging(self):
        enable = self.switch.has_state(lv.STATE.CHECKED)
        # user enabing passphrase
        if enable:
            screen = EnablePassphrase()
        else:
            screen = DisablePassphrase()

        await screen.show()

        # wait user confirm
        confirmed = await screen

        # if user confirmed, we store this value, else revert it
        if confirmed:
            device.set_passphrase_enabled(enable)
            if enable:
                self.desc.set_text(i18n.Text.passphrase_enable)
            else:
                self.desc.set_text(i18n.Text.passphrase_disable)
        else:
            if enable:
                self.switch.clear_state(lv.STATE.CHECKED)
            else:
                self.switch.add_state(lv.STATE.CHECKED)

class EnablePassphrase(Confirm):
    def __init__(self):
        super().__init__()
        self.btn_cancel.mode('cancel')
        self.btn_confirm.set_text(i18n.Button.enable)

        self.create_content(VStack)
        self.content: VStack
        self.content.items_center()

        style = (
            Style()
            .height(lv.SIZE.CONTENT)
            .pad_top(0)
            .pad_bottom(0)
            .pad_row(16)
            .pad_left(CONTENT_PAD)
            .pad_right(CONTENT_PAD)
        )
        self.content.add_style(style, lv.PART.MAIN)

        self.add(lv.img).set_src("A:/res/hp/ic_Passphrase_qiyong.png")

        obj = self.add(lv.label)
        obj.set_text(i18n.Title.enable_passphrase)
        obj.add_style(Styles.title, lv.PART.MAIN)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(i18n.Text.passphrase_enable_desc)
        obj.add_style(Styles.subtitle, lv.PART.MAIN)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)

class DisablePassphrase(Confirm):
    def __init__(self):
        super().__init__()
        self.btn_cancel.mode('cancel')
        self.btn_confirm.set_text(i18n.Button.disable)

        self.create_content(VStack)
        self.content: VStack
        self.content.items_center()

        style = (
            Style()
            .height(lv.SIZE.CONTENT)
            .pad_top(0)
            .pad_bottom(0)
            .pad_row(16)
            .pad_left(CONTENT_PAD)
            .pad_right(CONTENT_PAD)
        )
        self.content.add_style(style, lv.PART.MAIN)

        self.add(lv.img).set_src("A:/res/hp/ic_jinyong.png")

        obj = self.add(lv.label)
        obj.set_text(i18n.Title.disable_passphrase)
        obj.add_style(Styles.title, lv.PART.MAIN)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(i18n.Text.passphrase_disable_desc)
        obj.add_style(Styles.subtitle, lv.PART.MAIN)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
