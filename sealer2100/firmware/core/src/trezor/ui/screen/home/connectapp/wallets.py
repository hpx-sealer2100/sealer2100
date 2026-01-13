from trezor import utils, log
from typing import TYPE_CHECKING

from trezor.ui import i18n
from trezor.airgap.ur.ur import UR
from trezor.airgap.ur.ur_encoder import UREncoder

if TYPE_CHECKING:
    from typing import List, Iterator, Tuple
    from trezor.messages import EthereumPublicKey
    pass

def parser_path(path: str) -> List[int]:
    # m/44'/60'/0'/0/0
    items = path.split("/")
    if items[0] == "m":
        items.remove("m")

    # check path is valid
    # path item is x' or x, x is int
    if not all(x in "'0123456789" for item in items for x in item):
        # no need do tip user here, because the path is passed by developer
        # so the developer should make sure the path is valid
        # this is just helper developer to debug
        raise ValueError("invalid path")

    def convert(item: str) -> int:
        hardened = item.endswith("'")
        if hardened:
            index = 0x80000000 | int(item[:-1])
        else:
            index = int(item)
        return index

    return list(convert(item) for item in items)


class URIterator:
    def __init__(self, ur: UR):
        self.encoder = UREncoder(ur, 256)
        self.have_yield = False

    def __iter__(self) -> Iterator[str]:
        return self

    def __next__(self) -> str:
        if self.encoder.is_single_part() and self.have_yield:
            raise StopIteration
        if self.encoder.is_single_part():
            self.have_yield = True
        return self.encoder.next_part().upper()

class MetamaskAirgapWallet:
    def wallet(self) -> str:
        return i18n.Title.connect_metamask_wallet

    def description(self) -> str:
        return i18n.Subtitle.qr_connect_wallet_desc.format("MetaMask")

    def get_path(self) -> str:
        return "m/44'/60'/0'/0/0"

    async def get_keys(self) -> Tuple[EthereumPublicKey, EthereumPublicKey]:
        """
        get `key` and `master key`
        """
        from trezor.wire import DUMMY_CONTEXT as ctx, ActionCancelled
        from apps.base import handle_Initialize
        from trezor.messages import Initialize, EthereumGetPublicKey
        from apps.ethereum.get_public_key import get_public_key

        count = 5
        # step 1: initialize
        init = Initialize(session_id=b"\x00")
        await handle_Initialize(ctx, init)
        while count:
            try:
                # step 2: get public key
                # path is m/44'/60'/0'/0/0
                # we hope app can watch m/44'/60'/0'/0/*'
                address_n = parser_path(self.get_path())
                address_n = address_n[:3]
                req = EthereumGetPublicKey(address_n=address_n)
                key = await get_public_key(ctx, req)

                # step 2: get master key fingerprint
                address_n = address_n[:1]
                req = EthereumGetPublicKey(address_n=address_n)
                master_key = await get_public_key(ctx, req)
                return (key, master_key)
            except ActionCancelled as e:
                raise e
            except Exception as e:
                log.exception(__name__, e)
                log.debug(__name__, "get public keys failed, retry")
                count -= 1
                continue

        raise Exception('generate keys failed')

    async def airgap_address(self) -> Iterator[str]:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            key, master_key = await self.get_keys()
            # step 3: generate airgap address, aka HDKey
            from trezor.airgap.bc_types.coininfo import (
                CoinInfo,
                COIN_TYPE_ETH,
                NETWORK_MAINNET,
            )
            from trezor.airgap.bc_types.hdkey import HDKey
            from trezor.airgap.bc_types.keypath import KeyPath, PathComponent
            from storage import device

            coin_info = CoinInfo(COIN_TYPE_ETH, NETWORK_MAINNET)
            # pk
            key_data = key.node.public_key
            # chain code
            chain_code = key.node.chain_code
            # source fingerprint
            source_fingerprint = key.node.fingerprint
            # path is m/44'/60'/0'/0/0
            # we hope app can watch m/44'/60'/0'/0/*'
            # origin path, all hardened
            paths = PathComponent.parser_path(self.get_path())
            origin = KeyPath(paths[:3], master_key.node.fingerprint, 3)
            children = KeyPath([paths[3], PathComponent()], depth=0)
            device_label = device.get_label()
            sn = device.get_serial()
            passphrase = "1" if (hasattr(ctx, "_have_passphrase") and ctx._have_passphrase) else "0"
            device_id = device.get_device_id()

            # add `default label` as device type
            note = '|'.join([device_label, sn, passphrase, device_id, utils.DEFAULT_LABEL])
            log.debug(__name__, f"HDKey note: {note}")
            hdkey = HDKey.derived(
                key_data,
                chain_code=chain_code,
                origin=origin,
                parent_fingerprint=source_fingerprint,
                children=children,
                use_info=coin_info,
                name=device_label,
                note=note,
            )

            # UR encode
            ur = UR(hdkey.type(), hdkey.cbor())
            return URIterator(ur)

class HpxAirgapWallet(MetamaskAirgapWallet):
    def wallet(self):
        return i18n.Title.connect_hpx_wallet

    def description(self) -> str:
        return i18n.Subtitle.qr_connect_wallet_desc.format(i18n.Text.hpx_wallet)
class OkxAirgapWallet(MetamaskAirgapWallet):
    def wallet(self):
        return i18n.Title.connect_okx_wallet

    def description(self) -> str:
        return i18n.Subtitle.qr_connect_wallet_desc.format(i18n.Text.okx_wallet)
