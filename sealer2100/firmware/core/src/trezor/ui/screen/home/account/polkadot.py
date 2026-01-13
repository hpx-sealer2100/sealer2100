from trezor import utils
from .helper import parser_path

# a `CoinProtocol` class
class Polkadot:
    def get_name(self) -> str:
        return "Polkadot"
    def get_icon(self) -> str:
        return "A:/res/account/chain-dot.png"
    def get_path(self) -> str:
        return "m/44'/354'/0'/0'/0'"

    async def get_address(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, PolkadotGetAddress
            from apps.polkadot.get_address import get_address

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get address
            req = PolkadotGetAddress(address_n=parser_path(self.get_path()), prefix=0, network='polkadot')
            resp = await get_address(ctx, req)

            return resp.address
