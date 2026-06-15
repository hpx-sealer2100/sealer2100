from typing import TYPE_CHECKING

from storage import device
from trezor import wire
from trezor.ui import i18n
from trezor.enums import ButtonRequestType, TronResourceCode
from trezor.strings import format_amount
from trezor.ui.layouts import show_defi_lock, confirm_blob
from trezor.ui.layouts.tron import confirm_address, confirm_output
from trezor.ui.layouts.tron import confirm_total
from trezor.utils import chunks

from . import tokens

if TYPE_CHECKING:
    from typing import Awaitable
    from trezor.wire import Context

def _get_resource_description(resource: TronResourceCode) -> str|None:
    if resource is TronResourceCode.BANDWIDTH:
        return i18n.Text.bandwidth
    elif resource == TronResourceCode.ENERGY:
        return i18n.Text.energy
    else:
        return None

def require_confirm_data(ctx: Context, data: bytes, data_total: int) -> Awaitable[None]:
    from trezor.ui.layouts import confirm_data

    return confirm_data(
        ctx,
        title=i18n.Title.view_data,
        description=i18n.Text.bytes_.format(data_total),
        data=data,
        br_code = ButtonRequestType.SignTx,
    )


def require_confirm_tx(
    ctx: Context,
    to: str,
    value: int,
) -> Awaitable[None]:
    to_str = to
    return confirm_output(
        ctx,
        address=to_str,
        amount=format_amount_trx(value, None),
    )


def require_confirm_trigger_trc20(
    ctx: Context,
    verified: bool,
    contract_address: str,
    amount: int,
    token: tokens.TokenInfo,
    toAddress: str,
) -> Awaitable[None]:
    if verified:
        return confirm_output(
            ctx,
            address=toAddress,
            amount=format_amount_trx(amount, token),
        )

    # Unknown token
    return confirm_address(
        ctx,
        i18n.Title.unknown_token,
        contract_address,
        description=i18n.Text.contract,
    )


def require_confirm_fee(
    ctx: Context,
    token: tokens.TokenInfo | None = None,
    from_address: str | None = None,
    to_address: str | None = None,
    value: int = 0,
    fee_limit: int|None = None,
    network: str | None = None,
) -> Awaitable[None]:
    if token is None:
        if fee_limit is None:
            fee_limit = 0
        total_amount = format_amount_trx(value + fee_limit, None)
    else:
        total_amount = None
    return confirm_total(
        ctx,
        from_address,
        to_address,
        format_amount_trx(value, token),
        format_amount_trx(fee_limit, None),
        total_amount,
    )


def require_confirm_freeze(
    ctx: Context,
    signer: str,
    frozen_balance: int | None = None,
    frozen_duration: int | None = None,
    resource: int | None = None,
    receiver_address: str | None = None,
) -> Awaitable[None]:
    from trezor.ui.layouts.tron import confirm_freeze

    return confirm_freeze(
        ctx,
        signer,
        _get_resource_description(resource),
        format_amount_trx(frozen_balance, None) if frozen_balance is not None else None,
        str(frozen_duration) if frozen_duration is not None else None,
        receiver_address,
    )


def require_confirm_unfreeze(
    ctx: Context,
    signer: str,
    resource: int | None = None,
    receiver_address: str | None = None,
    unfrozen_balance: int | None = None,
) -> Awaitable[None]:
    from trezor.ui.layouts.tron import confirm_unfreeze

    return confirm_unfreeze(
        ctx,
        signer,
        _get_resource_description(resource),
        format_amount_trx(unfrozen_balance, None)
        if str(unfrozen_balance) is not None else None,
        receiver_address,
    )


def require_confirm_unfreeze_v2(
    ctx: Context,
    signer: str,
    resource: int | None = None,
    unfrozen_balance: int | None = None,
) -> Awaitable[None]:
    from trezor.ui.layouts.tron import confirm_unfreeze

    return confirm_unfreeze(
        ctx,
        signer,
        _get_resource_description(resource),
        format_amount_trx(unfrozen_balance, None)
        if str( unfrozen_balance ) is not None else None,
    )


def require_confirm_delegate(
    ctx: Context,
    signer: str,
    resource: int | None = None,
    balance: int | None = None,
    receiver_address: str | None = None,
    lock: bool | None = None,
) -> Awaitable[None]:
    from trezor.ui.layouts.tron import confirm_delegate

    return confirm_delegate(
        ctx,
        signer,
        _get_resource_description(resource),
        format_amount_trx(balance, None) if balance is not None else None,
        receiver_address,
        str(lock) if lock is not None else None,
    )


def require_confirm_undelegate(
    ctx: Context,
    signer: str,
    resource: int | None = None,
    balance: int | None = None,
    receiver_address: str | None = None,
    lock: bool | None = None,
) -> Awaitable[None]:
    from trezor.ui.layouts.tron import confirm_undelegate

    return confirm_undelegate(
        ctx,
        signer,
        _get_resource_description(resource),
        format_amount_trx(balance, None) if balance is not None else None,
        receiver_address,
        str(lock) if lock is not None else None,
    )

def confirm_tron_common(ctx: Context, message: str):
    return confirm_blob(ctx, "Tron", "", description="Contract", blob=message)

def require_confirm_transfer_asset(
    ctx: Context,
    verified: bool,
    assert_name: str,
    amount: int,
    token: tokens.TokenInfo,
    toAddress: str,
) -> Awaitable[None]:
    if verified:
        return confirm_output(
            ctx,
            address=toAddress,
            amount=format_amount_trx(amount, token),
        )

    # Unknown token
    return confirm_address(
        ctx,
        i18n.Title.unknown_token,
        assert_name,
        description=i18n.Text.contract,
    )

def format_amount_trx(value: int, token: tokens.TokenInfo | None) -> str:
    if token:
        suffix = token.symbol
        decimals = token.decimals
    else:
        suffix = "TRX"
        decimals = 6

    return f"{format_amount(value, decimals)} {suffix}"


def split_address(address):
    return chunks(address, 16)


def split_text(text):
    return chunks(text, 18)

async def check_defi_lock(ctx):
    if device.get_defi_lock():
        await show_defi_lock(ctx)
        raise wire.DataError("Forbidden transaction type")
