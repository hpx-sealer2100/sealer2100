import lvgl as lv

from . import Navigation,Modal

from trezor.ui import Style, font, colors, i18n, Cancel
from trezor.ui.theme import Styles
from trezor.ui.component.container import VStack
from trezor.ui.component import PassphraseKeyboard
from trezor import  workflow ,log
from trezor.ui.screen.confirm import Confirm

class PassphraseInput(Navigation):
    def __init__(self, max_length: int):
        super().__init__()
        #self.set_style_bg_color(colors.USER.WHITE, lv.PART.MAIN)
        self.title.set_text(i18n.Title.enter_passphrase)
        self.max_length = max_length
        self.create_content(VStack)
        self.content: VStack

        self.ta = self.add(lv.textarea)
        self.ta.set_password_mode(False)
        self.ta.set_one_line(True)
        self.ta.set_max_length(max_length)
        self.ta.set_size(432, 220)
        self.ta.add_style(Styles.board, lv.PART.MAIN)
        self.ta.add_style(
            Style()
            .text_font(font.Medium.PF32)
            .text_color(colors.USER.WHITE)
            .bg_color(colors.USER.DARK_GRAY)
            .text_align(lv.TEXT_ALIGN.LEFT),
            lv.PART.MAIN)

        spans = lv.spangroup(self.ta)
        spans.align(lv.ALIGN.BOTTOM_RIGHT, 0, 0)
        spans.set_style_text_font(font.small, lv.PART.MAIN)

        self.inputed = spans.new_span()
        self.inputed.set_text("0")
        self.inputed.style.set_text_color(colors.USER.GREEN)
        
        span = spans.new_span()
        span.set_text(f"/{max_length}")

        self.kbd = PassphraseKeyboard(self)
        self.kbd.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.kbd.set_height(216)
        self.kbd.set_textarea(self.ta)
        self.kbd.align(lv.ALIGN.BOTTOM_MID, 0, -8)
        self.kbd.add_event_cb(self.on_input_ready, lv.EVENT.READY, None)
        self.kbd.add_event_cb(self.on_input_change,lv.EVENT.VALUE_CHANGED, None)

    def reset(self):
        self.kbd.textarea.set_text("")
        self.kbd.lower_style()
    def on_input_ready(self, event):
        workflow.spawn(self.do_confirm_passphrase())
    def on_input_change(self, event):
        self.inputed.set_text(str(len(self.ta.get_text())))

    def on_nav_back(self, event):
        from trezor.ui.result import NavigationBack
        self.close(NavigationBack())
    def dismiss(self):
        super().dismiss()
        from trezor.ui.screen import manager
        manager.mark_dismissing(self)

    async def do_confirm_passphrase(self):
        passphrase = self.ta.get_text()
        screen = PassphraseConfirm(passphrase)
        await screen.show()

        r = await screen
        if r:
            self.channel.publish(passphrase)
            # navigation not provide dismiss
            from trezor.ui.screen import manager
            manager.mark_dismissing(self)
class PassphraseConfirm(Confirm):
    def __init__(self,passphrase: str):
        super().__init__()
        #self.set_style_bg_color(colors.USER.WHITE, lv.PART.MAIN)
        self.title.set_text(i18n.Text.use_passphrase)
        self.subtitle.set_text(i18n.Text.use_passphrase_desc)

        self.create_content(VStack)
        self.content: VStack

        label = self.add(lv.label)
        label.add_style(Styles.board, lv.PART.MAIN)
        label.add_style(
            Style()
            .width(432)
            .height(220)
            .text_font(font.Medium.PF32)
            .text_align(lv.TEXT_ALIGN.LEFT),
            lv.PART.MAIN
        )
        label.set_text(passphrase)

        # mpy can't import
        # from apps.common.passphrase import _MAX_PASSPHRASE_LEN
        spans = lv.spangroup(label)
        spans.align(lv.ALIGN.BOTTOM_RIGHT, 0, 0)
        spans.set_style_text_font(font.small, lv.PART.MAIN)

        span = spans.new_span()
        span.set_text(str(len(passphrase)))
        span.style.set_text_color(colors.USER.GREEN)
        
        span = spans.new_span()
        span.set_text("/50")

        self.btn_cancel.mode('cancel')
        self.btn_confirm.set_text(i18n.Button.continue_)
