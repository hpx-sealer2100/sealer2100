import lvgl as lv

from typing import TYPE_CHECKING

from . import Modal
from trezor import workflow, loop
from trezor.ui import i18n, colors, Done,log, Cancel
from trezor.ui.component import VStack
from trezor.ui.component.button import Button
from trezor.ui.constants import *
from trezor.ui.theme import Styles
from trezor.airgap.rust_ur.rust_ur import *
from trezor.ui.screen.home.connectapp.wallets import HpxAirgapWallet

if TYPE_CHECKING:
    from trezor.ui.screen.home.connectapp.qr_code import AirgapWallet


class AirgappedQrcode(Modal):
    def __init__(self, rust_encoded_ur: RustEncodedUR):
        super().__init__()
        self.set_title(i18n.Text.export_signed_transactions)
        self.subtitle.set_text(i18n.Text.export_signed_transactions_desc)
        self.btn.set_text(i18n.Button.close)
        self.btn.mode('close')
        self.rust_encoded_ur = rust_encoded_ur

        # signature qrcode
        self._qrcode_view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        self._qrcode_view.add_style(Styles.qrcode, lv.PART.MAIN)
        self._qrcode_view.update(self.rust_encoded_ur.data(), len(self.rust_encoded_ur.data()))
        self._qrcode_view.center()

        self._timer = lv.timer_create(self._on_timer, 150, None)
        self.btn.add_event_cb(lambda _: self.on_click_ok(), lv.EVENT.CLICKED, None)

    def _on_timer(self, timer):
        if self.rust_encoded_ur.is_multi_part():
            data = self.rust_encoded_ur.get_next_data()
            self._qrcode_view.update(data, len(data))

    def on_click_ok(self):
        self._timer._del()
        from trezor.ui.screen.home import HomeScreen
        from trezor.ui.screen.manager import try_switch_to
        try_switch_to(HomeScreen)

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
        from trezor.ui.screen.home import HomeScreen
        from trezor.ui.screen.manager import try_switch_to
        try_switch_to(HomeScreen)

    @property
    def qrcode_view(self) -> lv.qrcode:
        if self._qrcode_view:
            return self._qrcode_view
        view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        view.add_style(Styles.qrcode, lv.PART.MAIN)
        self._qrcode_view.center()
        return self._qrcode_view

class AirgapAddWallet(Modal):
    def __init__(self, wallet: AirgapWallet):
        super().__init__()
        self.wallet = wallet
        self.title.set_text(wallet.wallet())
        self.subtitle.set_text(wallet.description())
        self.btn.set_text(i18n.Button.close)
        self.btn.mode('close')
        self.btn.add_event_cb(lambda _: self.close(None), lv.EVENT.CLICKED, None)

        self._qrcode_view: lv.qrcode = None

        async def load_address():
            # wait a while, not block the UI
            await loop.sleep(150)
            await self.do_update_qrcode()
        self.task = workflow.spawn(load_address())

    def on_deleting(self):
        super().on_deleting()
        self.task.close()

    @property
    def qrcode_view(self) -> lv.qrcode:
        if self._qrcode_view:
            return self._qrcode_view
        self._qrcode_view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        self._qrcode_view.add_style(Styles.qrcode, lv.PART.MAIN)
        self._qrcode_view.center()
        return self._qrcode_view

    async def do_update_qrcode(self):
        from trezor import wire
        from trezor.ui import events
        try:
            while True:
                ur = await self.wallet.airgap_connect_ur_str()
                self.qrcode_view.update(ur, len(ur))
                await loop.sleep(150)
        except wire.ActionCancelled:
            self.close()
