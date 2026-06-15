from trezor import log
from typing import TYPE_CHECKING

from trezor.ui import i18n
from trezor.airgap.ur.ur import UR
from trezor.airgap.rust_ur.rust_ur import RustEncodedUR
import ur_parser

if TYPE_CHECKING:
    from typing import List, Iterator, Tuple
    from trezor.messages import EthereumPublicKey
    pass

btc_legacy_path = ("m/44'/0'/0'", "secp256k1")
btc_segwit_p2sh_path = ("m/49'/0'/0'", "secp256k1")
btc_segwit_native_path = ("m/84'/0'/0'", "secp256k1")
btc_taproot_path = ("m/86'/0'/0'", "secp256k1")
eth_path = ("m/44'/60'/0'", "secp256k1")
trx_path = ("m/44'/195'/0'", "secp256k1")
sol_path = ("m/44'/501'/0'/0'", "ed25519")

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


class MetamaskAirgapWallet:
    def __init__(self):
        self.rust_encoder = RustEncodedUR()
    def wallet(self) -> str:
        return i18n.Title.connect_metamask_wallet

    def description(self) -> str:
        return i18n.Subtitle.qr_connect_wallet_desc_metamask

    def get_paths(self) -> list[Tuple[str, str]]:
        return [eth_path]

    async def get_keys(self, have_initialize: bool = False) -> list[Tuple[str, str]]:
        """
        get `key` and `master key`
        """
        from trezor.wire import DUMMY_CONTEXT as ctx, ActionCancelled
        from apps.base import handle_Initialize
        from trezor.messages import Initialize, EthereumGetPublicKey,SolanaGetAddress
        from apps.ethereum.get_public_key import get_public_key
        from apps.solana.get_address import get_address
        from trezor.crypto import base58
        from trezor.airgap.rust_ur.utils import bytes_to_hex

        try:

            # step 1: initialize
            if not have_initialize:
                init = Initialize(session_id=b"\x00")
                await handle_Initialize(ctx, init)
            # step 2: get public key
            paths = self.get_paths()
            keys = []
            for path in paths:
                if(path[1] == "secp256k1"):
                    address_n = parser_path(path[0])
                    req = EthereumGetPublicKey(address_n=address_n)
                    key = await get_public_key(ctx, req)
                    keys.append((path[0], key.xpub))
                else:
                    # for ed25519, we only return the public key, because ed25519 has no master key
                    address_n = parser_path(path[0])
                    req = SolanaGetAddress(address_n=address_n)
                    key = await get_address(ctx, req)
                    keys.append((path[0], bytes_to_hex(base58.decode(key.address))))

            return keys
        except Exception as e:
            log.exception(__name__, e)
            raise Exception('generate keys failed')


    async def airgap_connect_ur_str(self) -> str:
        from trezor.wire import DUMMY_CONTEXT as ctx
        from apps.airgap import get_mfp

        if self.rust_encoder.urEncodeResult is not None:
            log.debug(__name__, "generate ur for the next part")
            log.debug(__name__, f"is_multi_part: {self.rust_encoder.is_multi_part()}")
            if self.rust_encoder.is_multi_part():
                next_data = self.rust_encoder.get_next_data()
                if next_data is not None:
                    return next_data
                else:
                    raise Exception("get next part failed")
            else:
                return self.rust_encoder.data()

        else:
            log.debug(__name__, "generate ur for the first time")
            keys = await self.get_keys()
            mfp = await get_mfp(ctx)
            ur_result = self.airgap_connect_ur(mfp, keys, 'Sealer2100')
            if ur_result.error_code() == 0:
                self.rust_encoder.encoding(ur_result)
                return self.rust_encoder.data()
            else:
                log.debug(__name__, f"generate ur failed, error code: {ur_result.error_code()}")
                log.debug(__name__, f"generate ur failed, error message: {ur_result.error_message()}")
                raise Exception("generate ur failed")

    def airgap_connect_ur(self,mfp,keys,origin) -> ur_parser.UREncodeResult:
        return ur_parser.get_connect_metamask_ur(mfp, keys, origin)



class HpxAirgapWallet(MetamaskAirgapWallet):
    def __init__(self, paths: list[Tuple[str, str]]| None = None):
        super().__init__()
        self.paths = paths

    def wallet(self):
        return i18n.Title.connect_hpx_wallet

    def description(self) -> str:
        return i18n.Subtitle.qr_connect_wallet_desc_hpx

    def get_paths(self) -> list[Tuple[str, str]]:
        return self.paths or [btc_legacy_path, btc_segwit_p2sh_path, btc_segwit_native_path, btc_taproot_path, eth_path, trx_path, sol_path]

    def airgap_connect_ur(self,mfp, keys, origin)-> ur_parser.UREncodeResult:
        from storage import device
        from trezor.wire import DUMMY_CONTEXT as ctx
        label = device.get_label()
        device_id = device.get_device_id()
        sn = device.get_serial()
        passphrase_enabled = hasattr(ctx, "_have_passphrase") and ctx._have_passphrase
        return ur_parser.get_connect_hpx_ur(mfp, keys, origin, device_id = device_id, label = label, sn = sn, passphrase = passphrase_enabled)

class OkxAirgapWallet(MetamaskAirgapWallet):
    def wallet(self):
        return i18n.Title.connect_okx_wallet

    def description(self) -> str:
        return i18n.Subtitle.qr_connect_wallet_desc.format(i18n.Text.okx_wallet)

    def get_paths(self) -> list[Tuple[str, str]]:
        return [btc_legacy_path, btc_segwit_p2sh_path, btc_segwit_native_path, btc_taproot_path, eth_path, trx_path]

class BtcAirgapWallet(MetamaskAirgapWallet):

    def get_paths(self) -> list[Tuple[str, str]]:
        return [btc_legacy_path, btc_segwit_p2sh_path, btc_segwit_native_path, btc_taproot_path]
