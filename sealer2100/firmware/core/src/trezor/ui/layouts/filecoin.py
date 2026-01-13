from trezor import ui, wire
from .helper import raise_if_cancelled, interact
from trezor.enums import ButtonRequestType
async def confirm_filecoin_payment(
    ctx: wire.GenericContext,
    sender: str,
    receiver: str,
    amount: str,
    fee_max: str,
    gasfeecap: str,
    total_amount: str,
) -> None:
    from trezor.ui.screen.apps.filecoin import FilecoinPayment

    screen = FilecoinPayment(
        address_from=sender,
        address_to=receiver,
        amount=amount,
        fee_max=fee_max,
        gasfeecap=gasfeecap,
        total_amount=total_amount,
        icon=ctx.icon_path,
    )

    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.SignTx))
