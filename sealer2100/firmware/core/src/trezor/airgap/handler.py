from typing import TYPE_CHECKING
from trezor import log
from trezor.airgap.ur.ur import UR
from trezor.wire import errors

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
    Handler = Callable[["GenericContext", bytes], HandlerTask]

handlers: dict[str, Handler] = {}

def register_handler(name: str, handler: Handler) -> None:
    handlers[name] = handler

def find_handler(type: str) -> Handler:
    log.debug(__name__, f"finding handler: {type}")
    return handlers.get(type)

# === handlers ===

from apps.airgap.eth_sign_request import eth_sign_request
from apps.airgap.hpx_app_call_device import app_call_device
register_handler("eth-sign-request", eth_sign_request)
register_handler('hpx-app-call-device', app_call_device)
