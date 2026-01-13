import lvgl as lv

from . import WalletItem
from trezor import  workflow
from trezor.ui import i18n, Style, colors,font
from trezor.ui.component import VStack, HStack
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles

class BlueConnect(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.blue_connect)
        self.subtitle.set_text(i18n.Subtitle.choose_connection_wallet)
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)

        # hpx = WalletItem(self.content, "A:/res/hp/HPX.png", i18n.Text.hpx_wallet, "BTC.ETH.TRON.SOL.OP.BASE.ARB.POLYGON.OKTC...")
        hpx = WalletItem(self.content, "A:/res/hp/HPX.png", i18n.Text.hpx_wallet, "BTC.ETH.TRON.SOL.OP.BA...")
        hpx.add_event_cb(self.click_hpx, lv.EVENT.CLICKED, None)

        # oxk = WalletItem(self.content, "A:/res/hp/OKEX.png", i18n.Text.okx_wallet, "BTC·ETH·TRON·SOL·NEAR...")
        # oxk.add_event_cb(self.click_okx, lv.EVENT.CLICKED, None)

    def click_hpx(self,e: lv.event_t):
        from .blue_connect_wallet import ConnectWallet
        workflow.spawn(ConnectWallet("hpx").show())

    def click_okx(self,e: lv.event_t):
        from .blue_connect_wallet import ConnectWallet
        workflow.spawn(ConnectWallet("okx").show())
