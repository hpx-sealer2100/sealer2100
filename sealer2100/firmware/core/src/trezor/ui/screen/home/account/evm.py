from typing import TYPE_CHECKING
import lvgl as lv

from .helper import parser_path

from trezor import utils
from trezor.ui import i18n
from trezor.ui.theme import Styles
from trezor.ui.component import VStack, ItemImg
from trezor.ui.constants import *
from .detail import Detail

if TYPE_CHECKING:
    from typing import Literal, List
    from . import CoinProtocol, CoinConsturctor
    AddressState = Literal["receive", "airgap"]

def networks() -> List[int]:
    import_manager = utils.unimport()
    with import_manager:
        from apps.ethereum.networks import _networks_iterator
        return list(n[0] for n in _networks_iterator())


# a `CoinProtocol` class with chain_id
class EvmCoin:
    def __init__(self, chain_id: int):
        self.chain_id = chain_id
    def get_name(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from apps.ethereum.networks import by_chain_id
            return by_chain_id(self.chain_id).name

    def get_icon(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from apps.ethereum.networks import by_chain_id
            return f"A:/res/account/{by_chain_id(self.chain_id).icon}"

    def get_family_icon(self) -> str:
        return "A:/res/evm-family.png"

    def get_path(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from apps.ethereum.networks import by_chain_id
            slip44_id = by_chain_id(self.chain_id).slip44
            return f"m/44'/{slip44_id}'/0'/0/0"

    async def get_address(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, EthereumGetAddress
            from apps.ethereum.get_address import get_address

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get address
            req = EthereumGetAddress(address_n=parser_path(self.get_path()))
            resp = await get_address(ctx, req)
            return resp.address

    async def get_airgap_address(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, EthereumGetPublicKey
            from apps.ethereum.get_public_key import get_public_key

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get public key
            # path is m/44'/60'/0'/0/0
            # we hope app can watch m/44'/60'/0'/0/*'
            address_n = parser_path(self.get_path())
            address_n = address_n[:3]
            req = EthereumGetPublicKey(address_n=address_n)
            resp = await get_public_key(ctx, req)

            # step 2: get master key fingerprint
            address_n = address_n[:1]
            req = EthereumGetPublicKey(address_n=address_n)
            master_key = await get_public_key(ctx, req)

            # step 3: generate airgap address, aka HDKey
            from trezor.airgap.bc_types.coininfo import (
                CoinInfo,
                COIN_TYPE_ETH,
                NETWORK_MAINNET,
            )
            from trezor.airgap.bc_types.hdkey import HDKey
            from trezor.airgap.bc_types.keypath import KeyPath, PathComponent

            coin_info = CoinInfo(COIN_TYPE_ETH, NETWORK_MAINNET)
            # pk
            key_data = resp.node.public_key
            # chain code
            chain_code = resp.node.chain_code
            # source fingerprint
            source_fingerprint = resp.node.fingerprint
            # path is m/44'/60'/0'/0/0
            # we hope app can watch m/44'/60'/0'/0/*'
            # origin path, all hardened
            paths = PathComponent.parser_path(self.get_path())
            origin = KeyPath(paths[:3], master_key.node.fingerprint, 3)
            children = KeyPath([paths[3], PathComponent()], depth=0)
            hdkey = HDKey.derived(
                key_data,
                chain_code=chain_code,
                origin=origin,
                parent_fingerprint=source_fingerprint,
                children=children,
                use_info=coin_info,
                name="HyperMate",
                note="eth.account.standard",
            )

            # UR encode
            from trezor.airgap.ur.ur import UR
            from trezor.airgap.ur.ur_encoder import UREncoder
            ur = UR(hdkey.type(), hdkey.cbor())
            data = UREncoder.encode(ur)

            return data.upper()


# not used
class EvmDetail(Detail):
    def __init__(self, coin: CoinProtocol):
        super().__init__(coin)

    def address_view(self):
        super().address_view()

        container: VStack = self._address_view.get_parent()
        obj = ItemImg(container, i18n.Text.evm_address_desc, "A:/res/hp/ic_guanyu1.png", 'left')
        obj.add_style(Styles.container, lv.PART.MAIN)
        obj.add_style(Styles.group, lv.PART.MAIN)


def evm_coin(chain_id: int) -> CoinConsturctor:
    return lambda: EvmCoin(chain_id)
