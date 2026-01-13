from typing import TYPE_CHECKING

from .transaction import RawTransaction
from .account_address import AccountAddress
from ..common.bcs import Deserializer
from trezor.strings import format_amount, format_timestamp

if TYPE_CHECKING:
    from trezor import wire
    from .transaction import EntryFunction
    pass


from trezor.ui.layouts.aptos import (
    show_transaction_overview,
    confirm_transaction_detail,
    confirm_entry_function_overview,
    confirm_entry_function_detail,
)


def format_aptos_amount(amount: int) -> str:
    value = format_amount(amount, 8)
    return f"{value} APT"


async def confirm_transfer(ctx: wire.GenericContext, raw_tx: RawTransaction):
    payload: EntryFunction = raw_tx.payload.value
    # see: https://github.com/aptos-labs/aptos-core/blob/main/aptos-move/framework/aptos-framework/doc/aptos_account.md#0x1_aptos_account_transfer
    # [to:address, amount: u64]
    assert len(payload.args) == 2
    to = AccountAddress(payload.args[0])
    ser = Deserializer(payload.args[1])
    amount = ser.u64()
    amount = format_aptos_amount(amount)

    r = await show_transaction_overview(
        ctx, amount=amount, to=to.__str__(), network=ctx.name
    )

    if r:
        sender = raw_tx.sender
        gas_unit_price = format_aptos_amount(raw_tx.gas_unit_price)
        expiration_time = format_timestamp(raw_tx.expiration_timestamps_secs)

        await confirm_transaction_detail(
            ctx,
            amount=amount,
            to=to.__str__(),
            sender=sender.__str__(),
            sequence_number=raw_tx.sequence_number,
            max_gas_amount=raw_tx.max_gas_amount,
            gas_unit_price=gas_unit_price,
            expiration_time=expiration_time,
            chain_id=raw_tx.chain_id,
        )


async def confirm_entry_function(ctx: wire.GenericContext, raw_tx: RawTransaction):
    payload: EntryFunction = raw_tx.payload.value
    function = f"{payload.module}::{payload.function}"

    r = await confirm_entry_function_overview(ctx, function=function, args=payload.args)

    if r:
        sender = raw_tx.sender
        gas_unit_price = format_aptos_amount(raw_tx.gas_unit_price)
        expiration_time = format_timestamp(raw_tx.expiration_timestamps_secs)
        await confirm_entry_function_detail(
            ctx,
            sender=sender.__str__(),
            sequence_number=raw_tx.sequence_number,
            max_gas_amount=raw_tx.max_gas_amount,
            gas_unit_price=gas_unit_price,
            expiration_time=expiration_time,
            chain_id=raw_tx.chain_id,
            function=function,
            args=payload.args,
        )
