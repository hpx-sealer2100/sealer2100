import lvgl as lv

from trezor.ui import i18n
from trezor.ui.component import VStack, HStack
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor.ui.constants import *
from trezor import log

from .detail import Detail
from .protocol import *

class AccountApp(Navigation):
    def __init__(self):
        # import trezor.ui.screen
        #trezor.ui.screen.TITLE_LOCALTION = 'center'
        super().__init__()
        self.title.set_text(i18n.App.account)
        self.create_content(VStack)
        self.content :VStack
        self.content.spacing(8)

        self.init_coins()

    def init_coins(self):
        from .bitcion import Bitcoin, BTCDetail
        from .btcfork import btcfork, LTC_COIN_NAME, BCH_COIN_NAME, DASH_COIN_NAME
        from .evm import evm_coin, EvmDetail
        from .solana import Solana
        from .tron import Tron
        from .xrp import Xrp
        from .fil import Fil

        # Bitcoin
        obj = self.add_coin(Bitcoin)
        obj.detail_constructor = BTCDetail
        # Ethereum
        obj = self.add_coin(evm_coin(1))
        obj.detail_constructor = EvmDetail
        # Solana
        self.add_coin(Solana)
        # Tron
        self.add_coin(Tron)
        # Litecion
        self.add_coin(btcfork(LTC_COIN_NAME))
        # BCH
        self.add_coin(btcfork(BCH_COIN_NAME))
        # DASH
        self.add_coin(btcfork(DASH_COIN_NAME))
        # Ripple
        self.add_coin(Xrp)
        # Filecion
        self.add_coin(Fil)

    def add_coin(self, constructor: CoinConsturctor) -> 'Coin':
        coin = constructor()
        obj = Coin(self.content, coin, Detail)
        obj.set_width(lv.pct(100))
        obj.add_flag(lv.obj.FLAG.CLICKABLE)
        obj.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        obj.add_style(Styles.board, lv.PART.MAIN)
        
        return obj

class Coin(HStack):
    def __init__(self, parent, coin: CoinProtocol, detail_constructor: CoinDetailViewConstructor):
        super().__init__(parent)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.spacing(12)

        self.coin = coin
        self.detail_constructor = detail_constructor

        # 左侧图标
        obj = self.add(lv.img)
        obj.set_src(self.coin.get_icon())

        # coin name
        obj = self.add(lv.label)
        obj.set_flex_grow(1)
        obj.set_text(self.coin.get_name())
        if hasattr(coin, "get_family_icon"):
            log.debug(__name__, f"{coin.__class__.__name__} has get_family_icon")
        if hasattr(coin, "get_family_icon") and coin.get_family_icon():
            obj = self.add(lv.img)
            obj.set_src(coin.get_family_icon())

        # a arrow
        obj = self.add(lv.img)
        obj.set_src("A:/res/arrow-right_3.png")

        self.add_event_cb(lambda _: self.action(), lv.EVENT.CLICKED, None)

    def action(self):
        from trezor import workflow
        screen = self.detail_constructor(self.coin)
        workflow.spawn(screen.show())
