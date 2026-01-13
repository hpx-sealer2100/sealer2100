from trezor import utils

from .helper import parser_path

# a `CoinProtocol` class
class Eos:
    def get_name(self) -> str:
        return "EOS"
    def get_icon(self) -> str:
        return "A:/res/chain-eos.png"
    def get_path(self) -> str:
        return "m/44'/194'/0'/0/0"

    async def get_address(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, EosGetPublicKey
            from apps.eos.get_public_key import get_public_key

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get address
            req = EosGetPublicKey(address_n=parser_path(self.get_path()))
            resp = await get_public_key(ctx, req)

            return resp.wif_public_key
