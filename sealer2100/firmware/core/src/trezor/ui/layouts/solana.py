from trezor import wire
from trezor.enums import ButtonRequestType
from .helper import raise_if_cancelled, interact

from trezor.ui.layouts.bitcoin import (
    # reuse
    confirm_output
)

async def confirm_sol_transfer(ctx: wire.GenericContext, from_addr: str, to_addr: str, amount: str) -> None:
    from trezor.ui.screen.apps.solana import TransactionDetail
    icon = ctx.icon_path
    screen = TransactionDetail(
        amount=amount,
        from_=from_addr,
        to=to_addr,
        total=None,
        icon=icon,
    )
    await screen.show()
    await raise_if_cancelled(
        interact(ctx, screen, ButtonRequestType.ProtectCall)
    )


async def confirm_sol_create_ata(
    ctx: wire.GenericContext,
    fee_payer: str,
    funding_account: str,
    associated_token_account: str,
    wallet_address: str,
    token_mint: str,
):
    from trezor.ui.screen.apps.solana import SPLCreateAssociatedTokenAccount
    icon = ctx.icon_path
    screen = SPLCreateAssociatedTokenAccount(
        fee_payer=fee_payer,
        funding_account=funding_account,
        associated_token_account=associated_token_account,
        wallet_address= wallet_address,
        token_mint= token_mint,
        icon=icon,
    )
    await screen.show()
    await raise_if_cancelled(
        interact(ctx, screen, ButtonRequestType.ProtectCall)
    )

async def confirm_sol_token_transfer(
    ctx: wire.GenericContext,
    from_addr: str,
    to_addr: str,
    amount: str,
    source_owner: str,
    fee_payer: str,
    token_mint: str = None,
):
    from trezor.ui.screen.apps.solana import SPLTokenTransactionDetail
    icon = ctx.icon_path
    screen = SPLTokenTransactionDetail(
        from_addr=from_addr,
        to_addr=to_addr,
        amount=amount,
        source_owner=source_owner,
        fee_payer=fee_payer,
        token_mint=token_mint,
        icon=icon
    )
    await screen.show()
    await raise_if_cancelled(
        interact(ctx, screen, ButtonRequestType.ProtectCall)
    )


async def confirm_sol_blinding_sign(
    ctx: wire.GenericContext, fee_payer: str, message_hex: str
) -> None:
    from trezor.ui.screen.confirm import SimpleConfirm
    from trezor.ui import i18n, colors

    screen = SimpleConfirm(i18n.Solana.blinding_sign_dsc)
    screen.set_title(i18n.Solana.blinding_sign)
    screen.text_color(colors.USER.DANGER)
    screen.mode('result')
    await screen.show()
    await raise_if_cancelled(
        interact(ctx, screen, ButtonRequestType.ProtectCall)
    )
    