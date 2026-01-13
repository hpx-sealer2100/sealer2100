from trezor import utils
from .helper import parser_path

# a `CoinProtocol` class
class Ton:
    def get_name(self) -> str:
        return "Ton"
    def get_icon(self) -> str:
        return "A:/res/chain-ton.png"
    def get_path(self) -> str:
        return "m/44'/607'/0'/0'/0'"

    async def get_address(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, TonGetAddress
            from apps.ton.get_address import get_address

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            print("address_n--"+str(parser_path(self.get_path())))
            # step 2: get address
            req = TonGetAddress(address_n=parser_path(self.get_path()),wallet_version=3, workchain=0, is_bounceable=False,
                                is_testnet_only=False, wallet_id=698983191, show_display=False)

            resp = await get_address(ctx, req)

            return resp.address
