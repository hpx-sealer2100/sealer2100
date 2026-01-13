from collections import namedtuple

from trezor import wire

from apps.solana.publickey import PublicKey

#     """Create memo transaction params."""
#     signer: PublicKey
#     """Signing account."""
#     message: bytes
#     """Memo message in bytes."""


MemoParams = namedtuple("MemoParams", ["signer", "message"])


async def parse(ctx: wire.Context, accounts: list[PublicKey], data: bytes) -> None:
    """Parse memo instruction params."""
    from trezor.ui import i18n

    params = MemoParams(signer=accounts[0], message=data)
    memo = params.message.decode("utf-8")
    from trezor.ui.layouts import confirm_text
    await confirm_text(ctx, i18n.Title.memo, memo, description=i18n.Text.message)

