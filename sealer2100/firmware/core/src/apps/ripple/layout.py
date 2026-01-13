from typing import TYPE_CHECKING

from trezor.strings import format_amount
from trezor.ui.layouts.tron import confirm_output

from . import helpers

if TYPE_CHECKING:
    from typing import Awaitable
    from trezor.wire import Context


def require_confirm_fee(
    ctx: Context,
    from_address: str | None = None,
    to_address: str | None = None,
    fee: int = 0,
    value: int = 0,
    tag: int = None,
) -> Awaitable[None]:
    from trezor.ui.layouts.ripple import confirm_ripple_payment

    return confirm_ripple_payment(
        ctx,
        from_address,
        to_address,
        format_amount(value, helpers.DECIMALS) + " XRP",
        format_amount(fee, helpers.DECIMALS) + " XRP",
        format_amount(value + fee, helpers.DECIMALS) + " XRP",
        str(tag) if tag is not None else None,
    )


def require_confirm_tx(ctx: Context, to: str, value: int) -> Awaitable[None]:
    return confirm_output(ctx, to, f"{format_amount(value, helpers.DECIMALS)} XRP")
