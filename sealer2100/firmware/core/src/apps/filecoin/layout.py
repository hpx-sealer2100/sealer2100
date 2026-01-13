from typing import TYPE_CHECKING

from trezor.enums import ButtonRequestType
from trezor.ui.layouts.tron import confirm_output


if TYPE_CHECKING:
    from typing import Awaitable
    from trezor.wire import Context


def require_confirm_tx(
    ctx: Context,
    to: str,
    value: str,
) -> Awaitable[None]:
    return confirm_output(
        ctx,
        address=to,
        amount=value
    )