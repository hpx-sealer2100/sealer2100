from typing import TYPE_CHECKING
from trezor import log
from trezor.airgap.ur.ur import UR
from trezor.wire import errors
from .rust_ur.rust_ur import QRCodeType
from .rust_ur.rust_ur import RustDecodedUR

if TYPE_CHECKING:
    from trezor.airgap.bc_types.keypath import KeyPath, PathComponent
    from typing import (
        Any,
        Callable,
        Coroutine,
        List,
    )
    from trezor.wire import GenericContext

    HandlerTask = Coroutine[Any, Any, None]
    Handler = Callable[["GenericContext", "RustDecodedUR"], HandlerTask]

handlers: dict[int, Handler] = {}

def register_handler(name: int, handler: Handler) -> None:
    handlers[name] = handler

def find_handler(ur_type: int) -> Handler:
    log.debug(__name__, f"finding handler: {ur_type}")
    return handlers.get(ur_type)

# === handlers ===

from apps.airgap.eth_sign_request import eth_sign_request
from apps.airgap.hpx_app_call_device import app_call_device
from apps.airgap.solana_sign_request import solana_sign_request
from apps.airgap.btc_sign_request import psbt_sign_request
from apps.airgap.tron_sign_request import tron_sign_request
register_handler(QRCodeType.EthSignRequest, eth_sign_request)
register_handler(QRCodeType.HpxAppCallDevice, app_call_device)
register_handler(QRCodeType.SolSignRequest, solana_sign_request)
register_handler(QRCodeType.CryptoPSBT, psbt_sign_request)
register_handler(QRCodeType.KeystoneSignRequest, tron_sign_request)