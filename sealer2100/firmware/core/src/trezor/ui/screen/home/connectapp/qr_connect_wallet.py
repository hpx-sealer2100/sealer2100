import lvgl as lv

from typing import TYPE_CHECKING

from trezor import loop, workflow
from trezor.ui import colors
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor.ui.constants import *
if TYPE_CHECKING:
    from .qr_code import AirgapWallet
    pass

class ConnectWallet(Navigation):
    def __init__(self, wallet: AirgapWallet):
        super().__init__()
        self.wallet = wallet
        self.title.set_text(wallet.wallet())
        self.subtitle.set_text(wallet.description())

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
            lv.event_send(self, events.NAVIGATION_BACK, None)
