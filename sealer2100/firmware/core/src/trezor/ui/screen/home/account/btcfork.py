from trezor import utils

from .helper import parser_path
from trezor.enums import InputScriptType
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from .protocol import CoinConsturctor
    pass

BTC_COIN_NAME = "Bitcoin"
BCH_COIN_NAME = "BCH"
LTC_COIN_NAME = "Litecoin"
DASH_COIN_NAME = "DASH"
# DOGE_COIN_NAME = "Dogecoin"

name_map = {
    BTC_COIN_NAME: "Bitcoin",
    BCH_COIN_NAME: "Bcash",
    LTC_COIN_NAME: "Litecoin",
    DASH_COIN_NAME: "Dash",
}

icon_map = {
    BTC_COIN_NAME: "A:/res/account/btc-btc.png",
    BCH_COIN_NAME: "A:/res/account/btc-bch.png",
    LTC_COIN_NAME: "A:/res/account/btc-ltc.png",
    DASH_COIN_NAME: "A:/res/account/btc-dash.png",
    # DOGE_COIN_NAME: "A:/res/account/btc-doge.png",
}

path_map = {
    BTC_COIN_NAME: "m/44'/0'/0'/0/0",
    BCH_COIN_NAME: "m/44'/145'/0'/0/0",
    LTC_COIN_NAME: "m/44'/2'/0'/0/0",
    DASH_COIN_NAME: "m/44'/5'/0'/0/0",
    # DOGE_COIN_NAME: "m/44'/3'/0'/0/0",
}

script_map = {
    BTC_COIN_NAME: InputScriptType.SPENDADDRESS,
    LTC_COIN_NAME: InputScriptType.SPENDADDRESS,
    BCH_COIN_NAME: InputScriptType.SPENDADDRESS,
    DASH_COIN_NAME: InputScriptType.SPENDADDRESS,
    # DOGE_COIN_NAME: InputScriptType.SPENDADDRESS,
}

# a `CoinConsturctor` class with name
class BTCFork:
    def __init__(self, coin_name: str):
        self.coin_name = coin_name

    def get_name(self) -> str:
        return self.coin_name
    def get_icon(self) -> str:
        return icon_map[self.coin_name]
    def get_path(self) -> str:
        return path_map[self.coin_name]

    async def get_address(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, GetAddress
            from apps.bitcoin.get_address import get_address

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get address
            name = name_map[self.coin_name]
            req = GetAddress(address_n=parser_path(self.get_path()),
                             coin_name = name,
                             script_type = script_map[self.coin_name])
            resp = await get_address(ctx, req)

            return resp.address

def btcfork(coin_name: str) -> CoinConsturctor:
    return lambda: BTCFork(coin_name)
