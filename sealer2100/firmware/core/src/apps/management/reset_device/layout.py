from trezor import ui, wire, log
from trezor.ui import i18n
from trezor.ui.layouts import show_success, show_warning
from trezor.ui.layouts.management import (  # noqa: F401
    confirm_words,
    show_words,
    show_start_backup,
    show_almost_complete,
    show_check_pass,
    show_check_failed,
)

async def show_internal_entropy(ctx: wire.GenericContext, entropy: bytes) -> None:
    return


async def _show_confirmation_success(
    ctx: wire.GenericContext,
) -> None:
    title = i18n.Title.verified
    text = i18n.Text.backup_verified
    return await show_success(
        ctx,
        text,
        title,
        button=i18n.Button.continue_,
    )


async def _show_confirmation_failure(ctx: wire.GenericContext) -> None:
    title = i18n.Title.invalid_mnemonic
    text = i18n.Text.backup_invalid

    await show_warning(
        ctx,
        text,
        title,
        button=i18n.Button.try_again,
    )


async def show_backup_success(ctx: wire.GenericContext) -> None:

    title = i18n.Title.wallet_is_ready
    text = i18n.Text.create_success
    await show_success(
        ctx, text, title, button=i18n.Button.continue_
    )


# BIP39
# ===


async def bip39_show_and_confirm_mnemonic(
    ctx: wire.GenericContext, mnemonic: str
):
    # warn user about mnemonic safety
    await show_start_backup(ctx)
    words = mnemonic.split()
    while True:
        # display paginated mnemonic on the screen
        await show_words(ctx, share_words=words)
        await show_almost_complete(ctx)
        r = await confirm_words(ctx, words)
        if r: 
            log.debug(__name__, "words check paessed")
            await show_check_pass(ctx)
            return True
        else:
            log.debug(__name__, "words check failed")
            await show_check_failed(ctx)
            continue

