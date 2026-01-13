from typing import TYPE_CHECKING

from trezor import wire
from trezor.ui import Reject, Detail, i18n
from trezor.enums import ButtonRequestType
from .helper import raise_if_cancelled, interact
# reuse overview in ethereum
from .ethereum import show_transaction_overview

if TYPE_CHECKING:
    from typing import List
    pass

async def confirm_transaction_detail(
    ctx: wire.GenericContext,
    amount: str,
    to: str,
    sender: str,
    sequence_number: int,
    max_gas_amount: int,
    gas_unit_price: str,
    expiration_time: str,
    chain_id: int,
):
    from trezor.ui.screen.apps.aptos import TransactionDetail

    screen = TransactionDetail(
        amount=amount,
        to=to,
        sender=sender,
        sequence_number=sequence_number,
        max_gas_amount=max_gas_amount,
        gas_unit_price=gas_unit_price,
        expiration_time=expiration_time,
        chain_id=chain_id,
        icon=ctx.icon_path,
    )

    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.SignTx))


async def confirm_entry_function_overview(
    ctx: wire.GenericContext,
    function: str,
    args: List[bytes]
):
    from trezor.ui.screen.apps.aptos import EntryFunctionOverview
    screen = EntryFunctionOverview(
        function=function,
        network=ctx.name,
        icon=ctx.icon_path,
        args = args,
    )
    await screen.show()
    r = await interact(ctx, screen, ButtonRequestType.SignTx)
    if isinstance(r, Reject):
        raise wire.ActionCancelled()
    elif isinstance(r, Detail):
        return True
    else:
        return False

async def confirm_entry_function_detail(
    ctx: wire.GenericContext,
    sender: str,
    sequence_number: int,
    max_gas_amount: int,
    gas_unit_price: str,
    expiration_time: str,
    chain_id: int,
    function: str,
    args: List[bytes]
):
    from trezor.ui.screen.apps.aptos import EntryFunctionDetail
    screen = EntryFunctionDetail(
        sender=sender,
        sequence_number=sequence_number,
        max_gas_amount=max_gas_amount,
        gas_unit_price=gas_unit_price,
        expiration_time=expiration_time,
        chain_id=chain_id,
        icon=ctx.icon_path,
        function=function,
        args=args,
    )
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.SignTx))
