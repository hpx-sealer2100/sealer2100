import lvgl as lv

from . import Modal
from trezor import workflow
from trezor.ui import i18n, colors, Done,log, Cancel
from trezor.ui.component import VStack
from trezor.ui.component.button import Button
from trezor.ui.constants import *
from trezor.ui.theme import Styles

class EthSignature(Modal):
    def __init__(self, sig: str):
        super().__init__()
        self.set_title(i18n.Text.export_signed_transactions)
        self.subtitle.set_text(i18n.Text.export_signed_transactions_desc)
        self.btn.set_text(i18n.Button.close)
        self.btn.mode('close')

        # signature qrcode
        view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        view.add_style(Styles.qrcode, lv.PART.MAIN)
        view.update(sig, len(sig))
        view.center()

        self.btn.add_event_cb(lambda _: self.on_click_ok(), lv.EVENT.CLICKED, None)

    def on_click_ok(self):
        self.close(Done())

class AirgapPublicKey(Modal):
    def __init__(self, pubkey: str, path: str, network: str):
        super().__init__()
        self.set_title(i18n.Title.public_key.format(network))
        self.subtitle.set_text(i18n.Subtitle.account_qr_address)
        self.btn.set_text(i18n.Button.close)
        self.btn.mode('close')

        self.btn_confirm = self.btn
        self.btn_confirm.add_event_cb(self.on_click_confirm, lv.EVENT.CLICKED, None)

        self.pubkey = pubkey

        self._qrcode_view: lv.qrcode = None

    def on_loaded(self):
        super().on_loaded()
        self.qrcode_view.update(self.pubkey, len(self.pubkey))

    def on_click_confirm(self, e):
        log.debug(__name__, "user click confirm")
        self.close(Done())

    @property
    def qrcode_view(self) -> lv.qrcode:
        if self._qrcode_view:
            return self._qrcode_view
        view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        view.add_style(Styles.qrcode, lv.PART.MAIN)
        self._qrcode_view.center()
        return self._qrcode_view
