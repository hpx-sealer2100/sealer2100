import lvgl as lv
from trezor import log, wire, workflow, loop
from trezor import ui, wire
from trezor.enums import ButtonRequestType
from trezor.messages import ButtonAck, ButtonRequest
from trezor.ui import i18n, colors
from .common import button_request, interact2, raise_if_cancelled

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Any, Awaitable, TypeVar, Sequence
    from storage.device import DeviceProtectType

    T = TypeVar("T")
    LayoutType = Awaitable[Any]

from .helper import *


async def confirm_reset_device(
    ctx: wire.GenericContext, prompt: str, recovery: bool = False
) -> None:
    from . import confirm_action
    title = i18n.Title.restore_wallet if recovery else i18n.Title.create_wallet
    await confirm_action(ctx, title, prompt)


async def confirm_wallet_ready(ctx: wire.GenericContext, backup: bool = None) -> bool:
    from trezor.ui.screen.initialize.ready import Ready

    screen = Ready(backup)
    await screen.show()
    await interact(
        ctx,
        screen,
        ButtonRequestType.ResetDevice,
    )


async def request_word_count(
    ctx: wire.GenericContext, dry_run: bool
) -> int:
    from trezor.ui.screen.initialize.wordcount import WordcountScreen

    screen = WordcountScreen()
    if dry_run:
        screen.mode('check')
    else:
        screen.mode('recover')
    await screen.show()

    r = await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.MnemonicWordCount))
    return int(r)


async def request_mnemonic(ctx, word_count: int) -> str:
    # confirm screen
    from trezor.ui.screen.initialize.recover_wallet import EnterMnemonic
    confirm_screen = EnterMnemonic()
    await confirm_screen.show()


    import storage.recovery

    while True:
        if not await interact(ctx, confirm_screen):
            confirm_screen.close(None)
            raise wire.ActionCancelled()

        if storage.recovery.is_dry_run():
            confirm_screen.close(None)

        # input screen
        from trezor.ui.screen.initialize.mnemonic import MnemonicInput
        screen = MnemonicInput(word_count)
        await screen.show()

        if not await interact(ctx, screen, ButtonRequestType.MnemonicInput):
            if not storage.recovery.is_dry_run():
                continue
            confirm_screen.close(None)
            raise wire.ActionCancelled()

        confirm_screen.close(None)
        words = screen.mnemonics
        return " ".join(words)


async def request_strength(ctx) -> int:
    word_cnt_strength_map = {
        12: 128,
        18: 192,
        24: 256,
    }

    from trezor.ui.screen.initialize.wordcount import WordcountScreen

    screen = WordcountScreen()
    await screen.show()
    count = await interact(ctx, screen, ButtonRequestType.MnemonicWordCount)
    if not isinstance(count, int):
        raise wire.ActionCancelled()
    return word_cnt_strength_map[count]


async def confirm_device_protection(
    ctx: wire.GenericContext, prompt: str, recovery: bool = False
) -> DeviceProtectType:
    from trezor.ui.screen.initialize.enable_protection import EnableProtectionScreen

    screen = EnableProtectionScreen()
    await screen.show()

    # wait user choose protect type
    return await raise_if_cancelled(interact(ctx, screen))


async def show_start_backup(ctx: wire.GenericContext):
    from trezor.ui.screen.initialize.create_wallet import StartBackup
    screen = StartBackup()
    await screen.show()

    await raise_if_cancelled(
        interact(
            ctx,
            screen,
            ButtonRequestType.ResetDevice,
        )
    )

async def show_almost_complete(ctx: wire.GenericContext):
    from trezor.ui.screen.initialize.create_wallet import Completion
    screen = Completion()
    await screen.show()

    await raise_if_cancelled(
        interact(
            ctx,
            screen,
            ButtonRequestType.ResetDevice,
        )
    )


async def show_words(
    ctx: wire.GenericContext,
    share_words: Sequence[str],
) -> None:
    from trezor.ui.screen.initialize.mnemonic import MnemonicDisplay
    log.debug(__name__, f"words: {share_words}")
    screen = MnemonicDisplay(len(share_words))
    screen.update_mnemonics(share_words)
    await screen.show()
    # confirm the share
    return await raise_if_cancelled(
        interact(
            ctx,
            screen,
            ButtonRequestType.ResetDevice,
        )
    )

async def confirm_words(ctx: wire.GenericContext, share_words: Sequence[str]) -> bool:
    from trezor.ui.screen.initialize.mnemonic import  MnemonicCheck

    screen = MnemonicCheck(share_words)
    await screen.show()

    return await interact(ctx, screen, ButtonRequestType.Other)

async def show_check_pass(ctx: wire.GenericContext) -> None:
    from trezor.ui.screen.message import Success
    screen = Success(i18n.Text.verification_passed, i18n.Text.verification_passed_desc, "A:/res/hp/icon_done_3.png")
    await screen.show()
    await screen

async def show_check_failed(ctx: wire.GenericContext) -> None:
    from trezor.ui.screen.message import Error
    screen = Error(i18n.Text.word_error, i18n.Text.word_error_desc, "A:/res/hp/icon_error_3.png")
    screen.btn.mode('retry')
    screen.btn.set_text(i18n.Button.try_again)
    await screen.show()
    await screen

async def confirm_recover_wallet_from(ctx: wire.GenericContext) -> int:
    from trezor.ui.screen.initialize.recover_wallet import RecoverWallet
    screen = RecoverWallet()
    await screen.show()
    return await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def confirm_input_mnemonic(ctx: wire.GenericContext):
    from trezor.ui.screen.initialize.recover_wallet import EnterMnemonic
    screen = EnterMnemonic()
    await screen.show()
    return await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def show_pin_security_tip(ctx: wire.GenericContext):
    from trezor.ui.screen.pin import EnablePinScreen
    screen = EnablePinScreen()
    await screen.show()
    await interact(ctx, screen)

async def show_iris_regist_tip(ctx: wire.GenericContext):
    from trezor.ui.screen.iris import IrisRegistTip
    screen = IrisRegistTip()
    await screen.show()
    await interact(ctx, screen)

async def show_iris_regist_note(ctx: wire.GenericContext):
    from trezor.ui.screen.iris import IrisRegistTipAgain
    screen = IrisRegistTipAgain()
    await screen.show()
    await interact(ctx, screen)

async def suggest_enable_iris(ctx: wire.GenericContext):
    from trezor.ui.screen.confirm import SimpleConfirm
    screen = SimpleConfirm(i18n.Text.suggest_enable_iris)
    screen.set_icon("A/res/hp/ic_hongmo_1.png")
    screen.set_title(i18n.Text.register_iris)
    screen.mode('result')
    screen.btn_confirm.set_text(i18n.Button.iris_entry)
    screen.btn_cancel.set_text(i18n.Button.iris_not_now)
    await screen.show()
    return await interact(ctx, screen)
