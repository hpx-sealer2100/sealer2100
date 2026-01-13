import lvgl as lv

from typing import TYPE_CHECKING

from . import WalletItem

from trezor import loop, workflow, log
from trezor.ui import i18n, Style, theme, colors, font
from trezor.ui.component import VStack, HStack, LabeledText
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles

if TYPE_CHECKING:
    from typing import Iterator, Protocol

    class AirgapWallet(Protocol):
        # which wallet
        def wallet(self) -> str: ...

        def description(self) -> str: ...

        # the airgap address
        async def airgap_address(self) -> Iterator[str]: ...


class QRConnect(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.qr_connect)
        self.subtitle.set_text(i18n.Subtitle.choose_connection_wallet)
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)
        self.content.spacing(8)

        hpx = WalletItem(
            self.content,
            "A:/res/hp/HPX.png",
            i18n.Text.hpx_wallet,
            # "ETH.OPTIMISM.BASE.ARBITRUM.POLYGON.OKTC...",
            "ETH.OPTIMISM.BASE.ARB...",
        )
        hpx.add_event_cb(self.click_hpx, lv.EVENT.CLICKED, None)

        metamask = WalletItem(
            self.content,
            "A:/res/hp/ic_huli.png",
            "MetaMask",
            i18n.Text.metamask_connect_desc,
        )
        metamask.add_event_cb(self.click_metamask, lv.EVENT.CLICKED, None)

        # oxk = WalletItem(
        #     self.content,
        #     "A:/res/hp/OKEX.png",
        #     i18n.Text.okx_wallet,
        #     "BTC·ETH·TRON·SOL·NEAR...",
        # )
        # oxk.add_event_cb(self.click_okx, lv.EVENT.CLICKED, None)

    def click_hpx(self, e: lv.event_t):
        from .qr_connect_wallet import ConnectWallet
        from .wallets import HpxAirgapWallet

        workflow.spawn(ConnectWallet(HpxAirgapWallet()).show())

    def click_okx(self, e: lv.event_t):
        from .qr_connect_wallet import ConnectWallet
        from .wallets import OkxAirgapWallet
        workflow.spawn(ConnectWallet(OkxAirgapWallet()).show())

    def click_metamask(self, e: lv.event_t):
        from .qr_connect_wallet import ConnectWallet
        from .wallets import MetamaskAirgapWallet
        workflow.spawn(ConnectWallet(MetamaskAirgapWallet()).show())
