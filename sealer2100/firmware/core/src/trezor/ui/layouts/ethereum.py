from typing import TYPE_CHECKING

from .helper import interact, raise_if_cancelled

from trezor import wire, log
from trezor.enums import ButtonRequestType
from trezor.ui import Reject, Detail, i18n

from trezor.ui.screen.apps.ethereum import (
    TransactionDetail,
    TransactionDetail1559,
    TransactionOverview,
)

if TYPE_CHECKING:
    from typing import Iterable

    pass


async def show_transaction_overview(
    ctx: wire.GenericContext,
    amount: str,
    to: str,
    network: str,
):
    screen = TransactionOverview(network, amount, to, ctx.icon_path)
    await screen.show()
    r = await interact(ctx, screen, ButtonRequestType.SignTx)
    if isinstance(r, Reject):
        raise wire.ActionCancelled()
    elif isinstance(r, Detail):
        return True
    else:
        return False
    
async def confirm_transaction_detail(
    ctx: wire.GenericContext,
    amount: str,
    gas_price: str,
    fee_max: str,
    from_address: str,
    to_address: str,
    total_amount: str,
    contract_addr: str | None,
    token_id: int | None,
    evm_chain_id: int | None,
    raw_data: bytes | None = None,
):
    screen = TransactionDetail(
        amount,
        from_address,
        to_address,
        fee_max,
        gas_price,
        total_amount,
        ctx.icon_path,
    )
    if contract_addr is not None:
        screen.show_contract_address(contract_addr)

    if token_id is not None:
        screen.show_token_id(token_id)

    if evm_chain_id is not None:
        screen.show_chain_id(evm_chain_id)

    # if raw_data is not None:
    #     screen.show_raw_data(raw_data)

    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.SignTx))


async def confirm_transaction_detail_eip1559(
    ctx: wire.GenericContext,
    amount: str,
    max_priority_fee_per_gas,
    max_fee_per_gas,
    fee_max: str,
    from_address: str,
    to_address: str,
    total_amount: str,
    contract_addr: str | None,
    token_id: int | None,
    evm_chain_id: int | None,
    raw_data: bytes | None,
):
    screen = TransactionDetail1559(
        amount,
        from_address,
        to_address,
        fee_max,
        max_priority_fee_per_gas,
        max_fee_per_gas,
        total_amount,
        ctx.icon_path,
    )

    if contract_addr is not None:
        screen.show_contract_address(contract_addr)

    if token_id is not None:
        screen.show_token_id(token_id)

    if evm_chain_id is not None:
        screen.show_chain_id(evm_chain_id)

    # if raw_data is not None:
    #     screen.show_raw_data(raw_data)

    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.SignTx))


async def confirm_address(
    ctx: wire.GenericContext,
    title: str,
    address: str,
    *,
    description: str = i18n.Text.address
):
    from trezor.ui.screen.apps.template import Blob

    screen = Blob(title, "", label=description, blob=address)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))


async def confirm_output(ctx: wire.GenericContext, address: str, amount: str):
    raise NotImplementedError


async def confirm_sign_typed_hash(
    ctx: wire.GenericContext, domain_hash: str, message_hash: str
):
    from trezor.ui.screen.apps.ethereum import TypedHash

    screen = TypedHash(domain_hash, message_hash, coin=ctx.name)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))


async def should_show_more(
    ctx: wire.GenericContext,
    title: str,
    param: Iterable[str],
    *,
    button_str: str | None = None
):
    from trezor.ui import Confirm, More
    from trezor.ui.screen.apps.ethereum import ShowMore

    screen = ShowMore(title, param)
    if button_str:
        screen.btn_more_text(button_str)

    await screen.show()
    r = await raise_if_cancelled(interact(ctx, screen))
    assert isinstance(r, (Confirm, More))

    return isinstance(r, More)


async def confirm_domain(ctx: wire.GenericContext, **kwargs):
    from trezor.ui.screen.apps.ethereum import Eip712

    title = i18n.Title.typed_data.format(ctx.name)
    screen = Eip712(title, **kwargs)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))
