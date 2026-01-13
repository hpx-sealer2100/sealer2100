import lvgl as lv

from trezor import log, workflow
from trezor.ui import i18n, font, colors, Style
from trezor.ui import Confirm, Reject, Cancel, Detail
from trezor.ui.screen import Modal
from trezor.ui.screen.confirm import HolderConfirm, Confirm as ConfirmView
from trezor.ui.component import VStack, LabeledText, HStack, DividerLine
from trezor.ui.theme import Styles
from trezor.ui.constants import *

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Literal
    AddressState = Literal["address", "qrcode"]
    AddressMode = Literal["normal", 'airgap-verify']

class TransactionBase(Modal):
    """
    交易页面的基类

    交易内容以 `VStack` 方式组织

    交易页面包含两个`Button`, 两个按钮上下排列

    Buttons:

    `Continue`, 继续当前交易
    `Reject`, 拒绝当前交易
    """

    def __init__(self):
        super().__init__()
        self.title.set_style_text_color(colors.USER.TAUPE, lv.PART.MAIN)
        # 内容区域
        self.create_content(VStack)
        self.content: VStack

        # 按钮
        self.btn_continue = self.btn_upper
        self.btn_reject = self.btn_down

        self.btn_continue.set_text(i18n.Button.continue_)
        self.btn_reject.set_text(i18n.Button.reject)
        self.btn_reject.mode('reject')

        self.btn_continue.add_event_cb(lambda e: self.close(Confirm()), lv.EVENT.CLICKED, None)
        self.btn_reject.add_event_cb(lambda e: self.close(Reject()), lv.EVENT.CLICKED, None)

    @staticmethod
    def labeled(parent, label: str, text: str) -> LabeledText:
        obj = LabeledText(parent)
        obj.set_label(label)
        obj.set_text(text)
        return obj

class Address(Modal):
    def __init__(
        self, address: str, path: str, network: str, chain_id: int | None = None
    ):
        super().__init__()
        self.set_title(i18n.Title.address.format(network))
        self.subtitle.add_flag(lv.obj.FLAG.HIDDEN)
        self.btn_toggle = self.btn_left
        self.btn_confirm = self.btn_right
        self.btn_toggle.set_text(i18n.Button.qr_code)
        self.btn_toggle.mode('primary')
        self.btn_confirm.set_text(i18n.Button.done)
        self.btn_confirm.mode('second')

        self.address = address
        self.path = path
        self.network = network
        self.chain_id = chain_id
        self._mode :AddressMode = 'normal'


        self.btn_toggle.add_event_cb(self.on_click_toggle, lv.EVENT.CLICKED, None)
        self.btn_confirm.add_event_cb(self.on_click_confirm, lv.EVENT.CLICKED, None)

        self.content: lv.obj

        self._address_view: VStack | None = None
        self._qrcode_view: lv.qrcode | None = None

        # default show address
        self.state: AddressState = "address"

        self.address_view.clear_flag(lv.obj.FLAG.HIDDEN)

    def mode(self, m: AddressMode):
        self._mode = m
        if m == 'airgap-verify':
            # when airgap-verify mode, have a subtitle
            # add a subtile
            self.subtitle.set_text(i18n.Text.export_signed_transactions_desc)
        elif m == 'normal':
            # normal mode is display address when get_address
            pass

    @property
    def address_view(self):
        if self._address_view is not None:
            return self._address_view

        view = self.add(VStack)
        view.set_height(lv.SIZE.CONTENT)

        # `address`
        label = view.add(lv.label)
        label.set_width(lv.pct(100))
        label.add_style(Styles.board, lv.PART.MAIN)
        label.set_text(self.address)
        label.set_long_mode(lv.label.LONG.WRAP)
        label.set_style_text_font(font.mono, lv.PART.MAIN)

        def labeled(label: str, text: str):
            item = view.add(LabeledText)
            item.set_label(label)
            item.set_text(text)
            item.add_style(Styles.board, lv.PART.MAIN)
        # path
        labeled(i18n.Text.path, self.path)

        if self.chain_id is not None:
            labeled(i18n.Text.chain_id, str(self.chain_id))

        self._address_view = view
        return self._address_view

    @property
    def qrcode_view(self):
        if self._qrcode_view is not None:
            return self._qrcode_view

        view = lv.qrcode(self.content, 300, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        view.add_style(Styles.qrcode, lv.PART.MAIN)
        view.center()
        view.update(self.address, len(self.address))
        self._qrcode_view = view
        return self._qrcode_view

    def on_click_toggle(self, e):
        if self.state == "qrcode":
            self.state = "address"
            if self._mode == 'airgap-verify':
                self.subtitle.add_flag(lv.obj.FLAG.HIDDEN)
            self.btn_toggle.set_text(i18n.Button.qr_code)
            self.address_view.clear_flag(lv.obj.FLAG.HIDDEN)
            self.qrcode_view.add_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.state = "qrcode"
            if self._mode == 'airgap-verify':
                self.subtitle.clear_flag(lv.obj.FLAG.HIDDEN)
            self.btn_toggle.set_text(i18n.Button.address)
            self.qrcode_view.clear_flag(lv.obj.FLAG.HIDDEN)
            self.address_view.add_flag(lv.obj.FLAG.HIDDEN)

    def on_click_confirm(self, e):
        log.debug(__name__, "user click confirm")
        self.close(Confirm())

    def on_click(self, e):
        log.debug(__name__, "user click address screen")
        target = e.target
        if target == self.btn_confirm:
            self.on_click_confirm(e)
        if target == self.btn_toggle:
            self.on_click_toggle(e)


class Blob(TransactionBase):
    """
    A `Modal` contain: `message`, `label`: `blob`
    """
    def __init__(self, title: str, message: str, *, label: str|None = None, blob: str|bytes|None = None):
        super().__init__()

        self.set_title(title)

        self.content.add_style(Styles.group, lv.PART.MAIN)

        if message:
            item = self.add(lv.label)
            item.set_width(lv.pct(100))
            item.set_long_mode(lv.label.LONG.WRAP)
            item.set_text(message)
            if item:
                self.add(DividerLine)
                
        if isinstance(blob,(bytes, bytearray)):
            from ubinascii import hexlify
            blob = '0x'+hexlify(blob).decode()

        self.labeled(self.content, label or "", blob)

class SignMessage(TransactionBase):
    def __init__(
        self,
        title: str,
        message: str,
        *,
        address: str,
        chain_id: int | None = None,
        icon: str | None = None
    ):
        super().__init__()

        self.set_icon(icon)
        self.set_title(title)

        self.content.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(self.content, label, text)

        # chin_id
        if chain_id is not None:
            labeled(i18n.Text.chain_id, str(chain_id))
            self.add(DividerLine)

        # address
        labeled(i18n.Text.address, address)
        self.add(DividerLine)

        # message
        labeled(i18n.Text.message, message)

        self.btn_continue.set_text(i18n.Button.sign)

    def set_mode(self, mode: Literal["sign", "verify"]):
        if mode == "sign":
            self.btn_upper.set_text(i18n.Button.sign)
        elif mode == "verify":
            self.btn_down.set_text(i18n.Button.verify)

class TransactionOverview(TransactionBase):
    def __init__(self, network: str, amount: str, to: str, icon: str):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Text.send)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(amount)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        # to
        self.labeled(container, i18n.Text.receiver, to)
        container.add(DividerLine)

        # view detail button
        self.btn_detail = container.add(HStack)
        self.btn_detail.set_height(lv.SIZE.CONTENT)
        self.btn_detail.add_style(Styles.container, lv.PART.MAIN)
        self.btn_detail.add_flag(lv.obj.FLAG.CLICKABLE)

        img = self.btn_detail.add(lv.img)
        img.set_src("A:/res/hp/down.png")

        label = self.btn_detail.add(lv.label)
        label.set_text(i18n.Text.detail)
        label.add_style(
            Style()
            .text_font(font.small)
            .text_color(colors.USER.WHITE),
            lv.PART.MAIN
        )

        self.btn_detail.add_event_cb(self.on_click_detail, lv.EVENT.CLICKED, None)

    def on_click_detail(self, e):
        self.close(Detail())

class HoldConfirmAction(HolderConfirm):
    def __init__(self, msg: str):
        super().__init__()
        label = lv.label(self.content)
        label.set_width(lv.pct(100))
        label.set_long_mode(lv.label.LONG.WRAP)
        label.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
        label.set_text("")
        label.align(lv.ALIGN.TOP_MID, 0, 16)

        self.holder.set_text(i18n.Button.hold_to_sign)
        self.btn_cancel.set_text(i18n.Button.reject)
        self.btn_cancel.mode('reject')

class TransactionConfirm(ConfirmView):
    def __init__(self, title: str, subtitle: str):
        super().__init__()
        self.set_icon("A:/res/hp/ic_sign_confirm.png")
        self.set_title(title)
        self.set_subtitle(subtitle)
        self.buttons_mode('vertical')
        self.btn_confirm.set_text(i18n.Button.signature_transaction)
        self.btn_cancel.set_text(i18n.Button.cancel)
        self.btn_cancel.mode('cancel')

class UnImplemented(Modal):
    def __init__(self):
        super().__init__()
        self.set_title(i18n.Title.unimplemented)
        label = lv.label(self.content)
        label.set_long_mode(lv.label.LONG.WRAP)
        label.set_text(i18n.Title.unimplemented)
        label.set_width(lv.pct(100))
        label.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
        label.set_style_text_color(colors.USER.DANGER, lv.PART.MAIN)


        # confirm button
        self.btn.set_text(i18n.Button.continue_)
        self.btn.add_event_cb(self.on_btn_continue, lv.EVENT.CLICKED, None)

    def on_btn_continue(self, e):
        self.close(Confirm())
