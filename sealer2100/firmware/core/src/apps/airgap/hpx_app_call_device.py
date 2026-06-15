import ujson as json
from . import parser_path, MFPNotMatch
import ur_parser
from trezor import log, wire
from trezor.airgap.ur.ur import UR
from trezor.airgap.ur.ur_encoder import UREncoder
from trezor.wire import errors
from trezor.airgap.bc_types.eth_sign_request import (
    EthSignRequest,
    ETH_TRANSACTION_DATA,
    ETH_TYPED_DATA,
    ETH_RAW_BYTES,
    ETH_TYPED_TRANSACTION,
)

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from trezor.airgap.bc_types.keypath import KeyPath, PathComponent
    from typing import Any, Callable, Coroutine, List, Optional, Protocol, Literal
    from trezor.wire import GenericContext
    from trezor.airgap.rust_ur.rust_ur import RustDecodedUR
    Methods = Literal['verifyAddress', 'getPublickey']
    class MethodHandler(Protocol):
        """A handler for method"""
        def __init__(self, request_id: str, xfp: str, parms: List['Param']):
            ...
        async def __call__(self, ctx: GenericContext):
            ...

async def app_call_device(ctx: "GenericContext", ur: RustDecodedUR):
    payload = ur_parser.hpx_app_call_get_payload(ur.data())
    request = Request.from_json(payload)
    h = request.method_handler()
    # not find a handler
    if not h:
        raise wire.unexpected_message()
    await h(ctx)

class Request:
    def __init__(self, request_id: str, method: Methods, xfp: str, params: List["Param"]):
        self.request_id = request_id
        self.method: Methods = method
        self.xfp = xfp
        self.params = params

    @classmethod
    def from_json(cls, data: bytes) -> "Request":
        try:
            obj: dict = json.loads(data)
        except ValueError as e:
            raise wire.DataError(f"{e}")
        # check keys
        if not all(k in obj for k in ("requestId", "method", "params")):
            raise wire.DataError("Invalid hpx-call-app-device request")

        request_id = obj.get("requestId")
        method = obj.get("method")
        xfp = obj.get("xfp")
        params_obj = obj.get("params")
        if params_obj:
            if not isinstance(params_obj, list):
                raise wire.DataError("params expect a json array")
            params = [Param.from_dict(o) for o in params_obj]
        else:
            params = None
        return cls(request_id, method, xfp, params)

    def __repr__(self):
        return f"Request(method={self.method}, request_id={self.request_id}, xfp={self.xfp}, params={self.params})"


    def method_handler(self) -> MethodHandler:
        if self.method == 'verifyAddress':
            return VerifyAddress(self.request_id, self.xfp, self.params)
        elif self.method == 'getPublickey':
            return GetPublickey(self.request_id, self.xfp, self.params)
        elif self.method == 'addWallet':
            return AddWallet(self.request_id, self.params)
        return None

class Param:
    def __init__(
        self,
        chain: str,
        path: str,
        address: Optional[str],
        chain_id: Optional[str | int],
        script_type: Optional[str],
    ):
        self.chain = chain
        self.path = path
        self.address = address
        self.chain_id = chain_id
        self.script_type = script_type

    @classmethod
    def from_json(cls, data: bytes) -> "Param":
        obj = json.loads(data)
        return cls.from_dict(obj)

    @classmethod
    def from_dict(cls, obj: dict) -> "Param":
        if not all(k in obj for k in ("chain", "path")):
            raise ValueError("Invalid param")

        chain = obj.get("chain")
        path = obj.get("path")
        address = obj.get("address")
        chain_id = obj.get("chainId")
        script_type = obj.get("scriptType")
        return cls(chain, path, address, chain_id, script_type)

    def __repr__(self):
        return f"Param(chain={self.chain}, path={self.path}, address={self.address}, chain_id={self.chain_id}, script_type={self.script_type})"

## `verifyAddress` handler
class VerifyAddress:
    def __init__(self, request_id: str, xfp: str, params: List[Param]):
        self.request_id = request_id
        self.xfp = xfp
        self.params = params
    async def __call__(self, ctx: GenericContext):
        if len(self.params) < 1:
            raise wire.DataError('Invalid param')
        param = self.params[0]
        log.debug(__name__, f"param: {param}")
        try:
            # initialize session
            from apps.base import handle_Initialize
            from trezor.messages import Initialize
            from trezor import utils

            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # get public key
            # verify keypath, keypath.source_fingerprint is matching
            from trezor.messages import EthereumGetPublicKey
            from apps.ethereum.get_public_key import get_public_key
            address_n = parser_path(param.path)
            # get master key fingerprint
            msg = EthereumGetPublicKey(address_n=address_n[:1])
            pubkey = await get_public_key(ctx, msg)
            log.debug(
                __name__, f"master key fingerprint in device: {hex(pubkey.node.fingerprint)}"
            )
            log.debug(
                __name__,
                f"master key fingerprint in request: {self.xfp}",
            )
            if pubkey.node.fingerprint != int(self.xfp, 16):
                raise MFPNotMatch()

            # m / purpose' / coin_type' / account' / change / address_index
            coin_type = address_n[1]
            if coin_type & 0x80000000 != 0x80000000:
                raise wire.DataError(f"Invalid path")

            coin_type = coin_type & 0x7fffffff
            network = None

            if coin_type == 0:
                # Bitcoin
                from trezor.messages import GetAddress
                from trezor.enums import InputScriptType
                from apps.bitcoin.get_address import get_address
                network = "Bitcoin"

                purpose = address_n[0]
                if purpose == 0x80000000 | 44:
                    # legacy
                    script_type = InputScriptType.SPENDADDRESS
                elif purpose == 0x80000000 | 49:
                    # segwit
                    script_type = InputScriptType.SPENDP2SHWITNESS
                elif purpose == 0x80000000 | 84:
                    # native segwit
                    script_type = InputScriptType.SPENDWITNESS
                elif purpose == 0x80000000 | 86:
                    # taproot
                    script_type = InputScriptType.SPENDTAPROOT
                else:
                    raise wire.DataError(f"Invalid path")

                msg = GetAddress(address_n=address_n, coin_name="Bitcoin", script_type=script_type)

            elif coin_type in (2, 5, 145):
                # Litecoin, Dash, Bcash
                from trezor.messages import GetAddress
                from trezor.enums import InputScriptType
                from apps.bitcoin.get_address import get_address
                network = "Litecoin" if coin_type == 2 else "Dash" if coin_type == 5 else "Bcash"
                msg = GetAddress(address_n=address_n, coin_name=network, script_type=InputScriptType.SPENDADDRESS,)
            elif coin_type == 60:
                # Ethereum
                from trezor.messages import EthereumGetAddress
                from apps.ethereum.get_address import get_address
                network = "Ethereum"
                msg = EthereumGetAddress(address_n=address_n)
            elif coin_type == 195:
                # Tron
                from trezor.messages import TronGetAddress
                from apps.tron.get_address import get_address
                network = "Tron"
                msg = TronGetAddress(address_n=address_n)
            elif coin_type == 501:
                # Solana
                from trezor.messages import SolanaGetAddress
                from apps.solana.get_address import get_address
                network = "Solana"
                msg = SolanaGetAddress(address_n=address_n)
            elif coin_type == 144:
                # Ripple
                from trezor.messages import RippleGetAddress
                from apps.ripple.get_address import get_address
                network = "Ripple"
                msg = RippleGetAddress(address_n=address_n)
            else:
                raise wire.DataError(f"Invalid path")

            resp = await get_address(ctx, msg)

            # show address
            from trezor.ui.layouts import show_airgap_address

            await show_airgap_address(ctx, resp.address, param.path, param.chain or network, param.chain_id)
        finally:
            from trezor.messages import EndSession
            from apps.base import handle_EndSession

            await handle_EndSession(ctx, EndSession())

## `getPublickey` handler
class GetPublickey:
    def __init__(self, request_id: str, xfp: str, params: List[Param]):
        self.request_id = request_id
        self.xfp = xfp
        self.params = params

    async def __call__(self, ctx: GenericContext):
        if len(self.params) < 1:
            raise wire.DataError('Invalid param')
        param = self.params[0]
        log.debug(__name__, f"param: {param}")
        try:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, EthereumGetPublicKey
            from apps.ethereum.get_public_key import get_public_key
            from trezor.ui.layouts import show_airgap_pubkey

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get public key
            address_n = parser_path(param.path)
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
            from storage import device

            coin_info = CoinInfo(COIN_TYPE_ETH, NETWORK_MAINNET)
            # pk
            key_data = resp.node.public_key
            # chain code
            chain_code = resp.node.chain_code
            # source fingerprint
            source_fingerprint = resp.node.fingerprint
            # path is m/44'/60'/0'/0/0
            # origin path, all hardened
            paths = PathComponent.parser_path(param.path)
            origin = KeyPath(paths[:3], master_key.node.fingerprint, 3)
            children = KeyPath(paths[3:])
            device_label = device.get_label()
            device_id = device.get_device_id()
            note = '|'.join([device_label, device_id])
            log.debug(__name__, f"HDKey note: {note}")
            hdkey = HDKey.derived(
                key_data,
                chain_code=chain_code,
                origin=origin,
                parent_fingerprint=source_fingerprint,
                children=children,
                use_info=coin_info,
                name="HyperMate",
                note=note,
            )

            # UR encode
            ur = UR(hdkey.type(), hdkey.cbor())
            pubkey_qrcode = UREncoder.encode(ur)
            await show_airgap_pubkey(ctx, pubkey_qrcode, param.path, param.chain)
        finally:
            from trezor.messages import EndSession
            from apps.base import handle_EndSession
            await handle_EndSession(ctx, EndSession())


class AddWallet:
    def __init__(self, request_id: str, params: List[Param]):
        self.request_id = request_id
        self.params = params

    async def __call__(self, ctx: GenericContext):
        if len(self.params) < 1:
            raise wire.DataError('Invalid param')

        paths = []
        # step 0: convert to list[`path`, `curve`]
        for param in self.params:
            log.debug(__name__, f"adding {param.chain} with path: {param.path}")
            # m / purpose' / coin_type' / account' / change / address_index
            address_n = parser_path(param.path)
            if len(address_n) < 3:
                raise wire.DataError(f"Invalid path")

            coin_type = address_n[1]

            if not (coin_type & 0x80000000):
                raise wire.DataError(f"Invalid path")

            coin_type = coin_type & 0x7fffffff

            if coin_type in (0, 60, 195):
                paths.append((param.path, "secp256k1"))
            elif coin_type == 501:
                paths.append((param.path, "ed25519"))
            else:
                raise wire.DataError(f"Invalid path")

        from trezor.ui.screen.home.connectapp.wallets import HpxAirgapWallet
        wallet = HpxAirgapWallet(paths)

        from trezor.ui.layouts import show_airgap_add_wallet
        await show_airgap_add_wallet(ctx, wallet)
