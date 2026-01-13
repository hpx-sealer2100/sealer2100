# from trezor import utils

from typing import TYPE_CHECKING
import lvgl as lv
from trezor import log, wire, workflow, loop
from trezor import ui, wire
from trezor.enums import ButtonRequestType
from trezor.messages import ButtonAck, ButtonRequest
from trezor.ui import i18n, colors
from .common import button_request, interact2, raise_if_cancelled

if TYPE_CHECKING:
    from typing import Any, Awaitable, TypeVar, Sequence
    from trezor.loop import AwaitableTask

    T = TypeVar("T")
    LayoutType = Awaitable[Any]

from .helper import *

# import `show` function for easy use
from .bitcoin import show_xpub, show_pubkey

# import `show` function for easy use
from .management import show_iris_regist_tip, show_iris_regist_note


# messages functions
async def show_loading(timeout_ms: int = 2000):
    from trezor.ui.screen.popup import Loading

    screen = Loading()
    await screen.show()
    screen.auto_close_timeout = timeout_ms
    await screen

async def show_popup(
    operation: str,
    timeout_ms: int = 3000,
    icon: str | None = None,
) -> None:
    from trezor.ui.screen.popup import Popup

    screen = Popup(operation, icon)
    await screen.show()
    screen.auto_close_timeout = timeout_ms
    await screen

async def wait_doing(msg: str, task: AwaitableTask):
    from trezor.ui.screen.popup import Doing
    screen = Doing(msg)
    await screen.show()
    racer = loop.race(screen, task, exit_others=False)
    r = await racer
    screen.close(None)
    return r

async def show_pairing_error() -> None:
    await show_popup(
        i18n.Text.bluetooth_pair_failed,
        timeout_ms=2000,
        icon="A:/res/error.png",
    )


async def show_app_guide() -> None:
    from trezor.ui.screen.initialize.down_app import DownAppScreen
    from trezor.ui.screen import Screen
    screen = DownAppScreen()
    await screen.show()
    while True:
        if not isinstance(screen, Screen):
            return
        screen = await screen


async def show_airgap_signature(sig: str):
    from trezor.ui.screen.airgap import EthSignature

    screen = EthSignature(sig)
    await screen.show()
    await screen

async def show_success(
    ctx: wire.GenericContext,
    msg: str,
    title: str = "Success",
    icon: str|None = None,
    button: str = i18n.Button.done,
) -> Awaitable[None]:
    from trezor.ui.screen.message import Success

    screen = Success(title, msg, icon)
    screen.button_text(button)
    await screen.show()
    return await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Success))


async def show_warning(
    ctx: wire.GenericContext,
    msg: str,
    title: str = "Warning",
    button: str = i18n.Button.try_again,
) -> Awaitable[None]:
    from trezor.ui.screen.message import Warning

    screen = Warning(title, msg)
    screen.button_text(button)
    await screen.show()
    return await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Warning))


async def show_error(
    ctx: wire.GenericContext,
    msg: str,
    title: str = "Error",
    button: str = i18n.Button.try_again,
) -> Awaitable[None]:
    from trezor.ui.screen.message import Error

    screen = Error(title, msg)
    screen.button_text(button)
    screen.btn.mode('retry')
    await screen.show()
    return await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Warning))


async def show_address(
    ctx: wire.GenericContext,
    address: str,
    path: str,
    network: str,
    chain_id: int | None = None,
):
    from trezor.ui.screen.apps.template import Address

    screen = Address(address, path, network, chain_id)
    await screen.show()
    return await interact(ctx, screen, ButtonRequestType.Address)

async def show_airgap_address(
    ctx: wire.GenericContext,
    address: str,
    path: str,
    network: str,
    chain_id: int | None = None,
):
    from trezor.ui.screen.apps.template import Address

    screen = Address(address, path, network, chain_id)
    screen.mode('airgap-verify')
    # default show qrcode
    screen.on_click_toggle(None)
    await screen.show()
    return await interact(ctx, screen, ButtonRequestType.Address)

async def show_airgap_pubkey(
    ctx: wire.GenericContext,
    pubkey: str,
    path: str,
    network: str,
):
    from trezor.ui.screen.airgap import AirgapPublicKey
    screen = AirgapPublicKey(pubkey, path, network)
    await screen.show()
    await interact(ctx, screen)

async def show_pin_reach_limit(ctx: wire.GenericContext):
    from trezor.ui.screen.management import PinLimitError
    screen = PinLimitError()
    await screen.show()
    await ctx.wait(screen)

async def show_verify_origin_iris(ctx: wire.GenericContext):
    from trezor.ui.screen.message import Message
    screen = Message(i18n.Text.iris_verification, i18n.Text.iris_verification_desc,"A:/res/hp/ic_hongmo_1.png")
    screen.btn.set_text(i18n.Button.continue_)
    await screen.show()
    await screen

async def show_regist_new_iris(ctx: wire.GenericContext):
    from trezor.ui.screen.message import Message
    screen = Message(i18n.Text.new_iris_registration, i18n.Text.new_iris_registration_desc,"A:/res/hp/icon_done_3.png")
    screen.btn.set_text(i18n.Button.continue_)
    await screen.show()
    await screen

async def show_iris_duplicate(ctx: wire.GenericContext):
    from trezor.ui.screen.message import Message
    screen = Message(i18n.Text.iris_duplicate, i18n.Text.iris_duplicate_dsc,"A:/res/hp/ic_hongmo_1.png")
    screen.btn.set_text(i18n.Button.continue_)
    await screen.show()
    await screen

async def show_defi_lock(ctx: wire.GenericContext):
        # only tip when enabled
        icon = "A:/res/hp/ic_cuowu555.png"
        title = i18n.Text.defi_disable
        msg = i18n.Text.defi_disable_desc
        btn = i18n.Button.close
        from trezor.ui.screen.message import Message
        screen = Message(title, msg, icon)
        screen.btn.mode('close')
        screen.btn.set_text(i18n.Button.close)
        await screen.show()
        await interact(ctx, screen)

# confirm functions
async def hold_confirm_action(
    ctx: wire.GenericContext, title: str, msg: str, br_code=ButtonRequestType.Other
):
    from trezor.ui.screen.apps.template import HoldConfirmAction

    screen = HoldConfirmAction(msg)
    icon = None
    if hasattr(ctx, "icon_path"):
        icon = ctx.icon_path
    screen.set_title(title, icon)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, br_code))


async def confirm_action(
    ctx: wire.GenericContext, title: str, msg: str, br_code=ButtonRequestType.Other
):
    from trezor.ui.screen.confirm import SimpleConfirm

    screen = SimpleConfirm(msg)
    screen.title.set_text(title)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, br_code))


async def confirm_final(ctx: wire.GenericContext, chain_name: str) -> None:
    from trezor.ui.screen.apps.template import TransactionConfirm
    screen = TransactionConfirm(i18n.Title.confirm_transaction, i18n.Subtitle.confirm_transaction)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))
    # show success
    await show_popup(
        i18n.Text.transaction_signed_desc, timeout_ms=3000, icon="A:/res/hp/ic_done566.png"
    )


async def confirm_set_homescreen(ctx, replace: bool = False):
    msg = i18n.Text.set_as_homescreen
    if replace:
        msg = i18n.Text.replace_homescreen
    await confirm_action(
        ctx,
        i18n.Title.set_as_homescreen,
        msg,
    )

async def confirm_collect_nft(ctx, replace: bool = False):
    msg = i18n.Text.collect_nft
    if replace:
        msg = i18n.Text.replace_nft
    await confirm_action(
        ctx,
        i18n.Title.collect_nft,
        msg,
    )

async def confirm_verify_device(ctx):
    await confirm_action(ctx, i18n.Title.verify_device, i18n.Text.verify_device)

async def confirm_update_res(ctx, update_boot:bool):
    if update_boot:
        title = i18n.Title.update_bootloader
        msg = i18n.Text.update_bootloader
    else:
        title = i18n.Title.update_resource
        msg = i18n.Text.update_resource

    from trezor.ui.screen.confirm import SimpleConfirm
    screen = SimpleConfirm(msg)
    screen.title.set_text(title)
    screen.btn_right.set_text(i18n.Button.update)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))

async def confirm_device_protection_verify_retry(ctx, rem: int):
    from trezor.ui.screen.management import DeviceProtectionVerifyRetry
    screen = DeviceProtectionVerifyRetry(rem)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))

## wipe device
async def confirm_wipe_device(ctx: wire.GenericContext):
    from trezor.ui.screen.management.wipe_device import WipeDeviceConfirm

    screen = WipeDeviceConfirm()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.WipeDevice))


async def confirm_wipe_device_tips(ctx: wire.GenericContext):
    from trezor.ui.screen.management.wipe_device import WipeDeviceTip

    screen = WipeDeviceTip()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.WipeDevice))


async def confirm_wipe_device_success(ctx: wire.GenericContext):
    from trezor.ui.screen.popup import Popup

    screen = Popup(i18n.Text.wipe_device_success, "A:/res/hp/ic_done566.png")
    await screen.show()
    return await interact(ctx, screen, ButtonRequestType.WipeDevice)

async def request_pin_on_device(
    ctx: wire.GenericContext,
    prompt: str,
    attempts_remaining: int | None,
    allow_cancel: bool,
) -> str:
    await button_request(ctx, code=ButtonRequestType.PinEntry)
    from trezor.ui.screen.pin import InputPinScreen

    screen = InputPinScreen(prompt)
    await screen.show()
    result = await ctx.wait(screen)
    if not result:
        if not allow_cancel:
            loop.clear()
        raise wire.PinCancelled
    assert isinstance(result, str)
    return result

async def request_iris_regist(ctx: wire.GenericContext):
    from trezor.ui.screen.iris import IrisRegist
    screen = IrisRegist()
    await screen.show()
    return await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.ProtectCall))

async def request_iris_match(ctx: wire.GenericContext):
    await button_request(ctx, code=ButtonRequestType.IrisEntry)
    from trezor.ui.screen.iris import IrisMatch
    screen = IrisMatch()
    await screen.show()
    result = await ctx.wait(screen)
    if not result:
        raise wire.ActionCancelled
    return result

async def confirm_check_recovery_mnemonic(ctx: wire.GenericContext):
    from trezor.ui.screen.message import Info

    title = i18n.Title.check_recovery_mnemonic
    text = i18n.Text.check_recovery_mnemonic
    screen = Info(title, text)
    screen.button_text(i18n.Button.continue_)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.ProtectCall))

async def confirm_enable_pin(ctx: wire.GenericContext):
    from trezor.ui.screen.management import PinEnableConfirm
    screen = PinEnableConfirm()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def confirm_change_pin(ctx: wire.GenericContext):
    from trezor.ui.screen.management import PinChangeConfirm
    screen = PinChangeConfirm()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def confirm_remove_pin(ctx: wire.GenericContext):
    from trezor.ui.screen.management import PinRemoveConfirm
    screen = PinRemoveConfirm()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def confirm_regist_iris(ctx: wire.GenericContext):
    from trezor.ui.screen.management import IrisRegistConfirm
    screen = IrisRegistConfirm()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def confirm_change_iris(ctx: wire.GenericContext):
    from trezor.ui.screen.management import IrisChangeConfirm
    screen = IrisChangeConfirm()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def confirm_remove_iris(ctx: wire.GenericContext):
    from trezor.ui.screen.management import IrisRemoveConfirm
    screen = IrisRemoveConfirm()
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, ButtonRequestType.Other))

async def confirm_sign_message(
    ctx: wire.GenericContext,
    coin: str,
    message: str,
    *,
    address: str,
    chain_id: int | None = None,
):
    from trezor.ui.screen.apps.template import SignMessage

    title = i18n.Title.sign_message.format(coin)
    screen = SignMessage(title, message, address=address, chain_id=chain_id)
    screen.set_mode("sign")
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))
    # show success
    await show_popup(
        i18n.Text.transaction_signed, timeout_ms=3000, icon="A:/res/success.png"
    )


async def confirm_verify_message(
    ctx: wire.GenericContext,
    coin: str,
    message: str,
    *,
    address: str,
    chain_id: int | None = None,
):
    from trezor.ui.screen.apps.template import SignMessage

    title = i18n.Title.verify_message.format(coin)
    screen = SignMessage(title, message, address=address, chain_id=chain_id)
    screen.set_mode("verify")
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen))


async def confirm_blob(
    ctx: wire.GenericContext,
    title: str,
    message: str,
    *,
    description: str,
    blob: bytes,
    br_code=ButtonRequestType.Other,
):
    from trezor.ui.screen.apps.template import Blob

    screen = Blob(title, message, label=description, blob=blob)
    await screen.show()
    await raise_if_cancelled(interact(ctx, screen, br_code=br_code))

async def confirm_metadata(
    ctx: wire.GenericContext,
    title: str,
    message: str,
    *,
    description: str,
    data: bytes,
    br_code=ButtonRequestType.Other,
):
    return await confirm_blob(
        ctx, title, message, blob=data, description=description, br_code=br_code
    )

async def confirm_data(
    ctx: wire.GenericContext,
    title: str,
    data: bytes,
    *,
    description: str,
    br_code: ButtonRequestType,
):
    return await confirm_blob(
        ctx, title, "", blob=data, description=description, br_code=br_code
    )

def confirm_address(
    ctx: wire.GenericContext,
    title: str,
    address: str,
    description: str | None = "Address:",
    br_type: str = "confirm_address",
    br_code: ButtonRequestType = ButtonRequestType.Other,
) -> Awaitable[None]:
    return confirm_blob(
        ctx, title, "", blob=address.encode(), description=description or "", br_code=br_code
    )

async def should_show_details(
    ctx: wire.GenericContext,
    address: str,
    title: str,
    br_code: ButtonRequestType = ButtonRequestType.ConfirmOutput,
) -> bool:
    from trezor.lvglui.scrs.template import TransactionOverview

    res = await interact2(
        ctx,
        TransactionOverview(
            title,
            address,
            primary_color=lv.color_hex(0x0098EA),
            icon_path="A:/res/chain-ton.png",
            has_details=True,
        ),
        "confirm_output",
        br_code,
    )
    if not res:
        from trezor import loop

        await loop.sleep(300)
        raise wire.ActionCancelled()
    elif res == 2:  # show more
        return True
    else:  # confirm
        return False

async def confirm_text(
    ctx: wire.GenericContext, title: str, text: str, *, description: str
):
    return await confirm_blob(ctx, title, "", blob=text, description=description)


async def confirm_blind_sign_common(
    ctx: wire.Context, signer: str, raw_message: bytes
) -> None:
    return await confirm_blob(ctx, i18n.Text.do_sign_this_transaction.format(ctx.name), signer, blob=raw_message,description="Data:")


## passphrase

async def request_passphrase_on_device(ctx: wire.Context, len: int) -> str:
    from trezor.ui.result import NavigationBack
    from trezor.ui.screen.passphrase import PassphraseInput
    screen = PassphraseInput(len)
    await screen.show()
    r = await interact(ctx, screen, br_code=ButtonRequestType.PassphraseEntry)
    await screen.wait_unloaded()
    if isinstance(r, NavigationBack):
        # await loop.sleep(100)
        raise wire.ActionCancelled()
    return r
async def require_confirm_passphrase(ctx: wire.Context, passphrase: str):
    from trezor.ui.screen.passphrase import PassphraseConfirm
    screen = PassphraseConfirm(passphrase)
    await screen.show()
    return await raise_if_cancelled(interact(ctx, screen, br_code=ButtonRequestType.PassphraseEntry))
