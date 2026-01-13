from typing import TYPE_CHECKING

from trezor import log
from .layout import confirm_transfer
from .transaction import ModuleId
from .transaction import RawTransaction, TransactionPayload

if TYPE_CHECKING:
    from typing import List, Dict, Callable, Awaitable
    from trezor import wire

    ConfirmHandler = Callable[[wire.GenericContext, RawTransaction], Awaitable[None]]


class WellKnown:
    module: ModuleId
    func: str
    confirm_handler: ConfirmHandler

    def __init__(self, module: ModuleId, func: str, confirm_handler: ConfirmHandler):
        self.module = module
        self.func = func
        self.confirm_handler = confirm_handler

    def __eq__(self, other: "WellKnown"):
        return self.module == other.module and self.func == other.func


WELL_KNOWN: List[WellKnown] = [
    (WellKnown(
        module=ModuleId.from_str("0x1::aptos_account"),
        func="transfer",
        confirm_handler=confirm_transfer,
    )),
]

def find_confirm_handler(raw_tx: RawTransaction) -> ConfirmHandler | None:
    payload = raw_tx.payload
    if payload.variant == TransactionPayload.SCRIPT_FUNCTION:
        from .transaction import EntryFunction
        value: EntryFunction = payload.value
        for known in WELL_KNOWN:
            if value.module == known.module and value.function == known.func:
                return known.confirm_handler
        log.debug(__name__, "Unknown entry function")
        from .layout import confirm_entry_function
        return confirm_entry_function
    elif payload.variant == TransactionPayload.MODULE_BUNDLE:
        return None
    elif payload.variant == TransactionPayload.SCRIPT:
        return None
    else:
        return None






