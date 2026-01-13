from typing import TYPE_CHECKING
from trezor.enums import ButtonRequestType
from .helper import raise_if_cancelled, interact

if TYPE_CHECKING:
    from trezor import wire

    pass

from trezor.ui.layouts.bitcoin import (
    # reuse
    confirm_output,
)


async def confirm_polkadot_balances(
    ctx: wire.GenericContext,
    amount: str,
    to: str,
    *,
    chain_name: str,
    sender: str,
    source: str | None = None,
    tip: str | None = None,
    keep_alive: str | None = None,
):
    from trezor.ui.screen.apps.polkadot import Balance
    icon = ctx.icon_path
    screen = Balance(
        amount=amount,
        to=to,
        chain_name=chain_name,
        sender=sender,
        source=source,
        tip=tip,
        keep_alive=keep_alive,
        icon = icon,
    )

    await screen.show()
    await raise_if_cancelled(
        interact(ctx, screen, ButtonRequestType.SignTx)
    )