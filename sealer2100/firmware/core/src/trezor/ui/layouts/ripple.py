from trezor import ui, wire
from .helper import raise_if_cancelled, interact
from trezor.enums import ButtonRequestType
async def confirm_ripple_payment(
    ctx: wire.GenericContext,
    sender: str | None = None,
    receiver: str | None = None,
    amount: str = 0,
    fee: str = 0,
    total: str | None = None,
    tag: str | None = None,
) -> None:
    from trezor.ui.screen.apps.ripple import RipplePayment

    screen = RipplePayment(
        sender,
        receiver,
        amount,
        fee,
        total,
        tag,
        icon=ctx.icon_path,
    )
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.SignTx))