from typing import TYPE_CHECKING

from storage import device, io
from trezor import wire
from trezor.ui import i18n
from trezor.enums import ButtonRequestType, SafetyCheckLevel
from trezor.messages import Success
from trezor.strings import format_duration_ms
from trezor.ui.layouts import (
    confirm_action,
    confirm_set_homescreen,
    hold_confirm_action,
)

from apps.base import reload_settings_from_storage
from apps.common import safety_checks

if TYPE_CHECKING:
    from trezor.messages import ApplySettings
    from typing import Generator


def _wallpapers() -> Generator[str]:

    # system wallpapers
    SYS_WALLPAPER_PATH = "/res/wallpapers"
    for _, _, name in io.fs.dir_open(SYS_WALLPAPER_PATH):
        yield name

    USER_WALLPAPER_PATH = "/user/wallpapers"
    # user wallpapers
    for _, _, name in io.fs.dir_open(USER_WALLPAPER_PATH):
        yield name

def validate_homescreen(homescreen: bytes) -> None:
    homescreen = homescreen.decode()
    if not homescreen in _wallpapers():
        raise wire.DataError("Invalid homescreen")


async def apply_settings(ctx: wire.Context, msg: ApplySettings) -> Success:
    if not device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")
    if (
        msg.homescreen is None
        and msg.label is None
        and msg.use_passphrase is None
        and msg.passphrase_always_on_device is None
        and msg.display_rotation is None
        and msg.auto_lock_delay_ms is None
        and msg.safety_checks is None
        and msg.experimental_features is None
    ):
        raise wire.ProcessError("No setting provided")

    if msg.homescreen is not None:
        validate_homescreen(msg.homescreen)
        await confirm_set_homescreen(ctx)
        device.set_homescreen(f"A:/res/{msg.homescreen.decode()}")

    if msg.label is not None:
        if len(msg.label) > device.LABEL_MAXLENGTH:
            raise wire.DataError("Label too long")
        await require_confirm_change_label(ctx, msg.label)
        device.set_label(msg.label)

    if msg.use_passphrase is not None:
        await require_confirm_change_passphrase(ctx, msg.use_passphrase)
        device.set_passphrase_enabled(msg.use_passphrase)

    if msg.passphrase_always_on_device is not None:
        if not device.is_passphrase_enabled():
            raise wire.DataError("Passphrase is not enabled")
        # else:
        #     if not msg.passphrase_always_on_device:
        #         raise wire.DataError("Only support passphrase input on device")
        await require_confirm_change_passphrase_source(
            ctx, msg.passphrase_always_on_device
        )
        device.set_passphrase_always_on_device(msg.passphrase_always_on_device)

    if msg.auto_lock_delay_ms is not None:
        if msg.auto_lock_delay_ms < device.AUTOLOCK_DELAY_MINIMUM:
            raise wire.ProcessError("Auto-lock delay too short")
        if msg.auto_lock_delay_ms > device.AUTOLOCK_DELAY_MAXIMUM:
            raise wire.ProcessError("Auto-lock delay too long")
        await require_confirm_change_autolock_delay(ctx, msg.auto_lock_delay_ms)
        device.set_autolock_delay_ms(msg.auto_lock_delay_ms)

    if msg.safety_checks is not None:
        await require_confirm_safety_checks(ctx, msg.safety_checks)
        safety_checks.apply_setting(msg.safety_checks)

    if msg.display_rotation is not None:
        raise wire.ProcessError("Not support yet")
        # await require_confirm_change_display_rotation(ctx, msg.display_rotation)
        # device.set_rotation(msg.display_rotation)

    if msg.experimental_features is not None:
        await require_confirm_experimental_features(ctx, msg.experimental_features)
        device.set_experimental_features(msg.experimental_features)

    reload_settings_from_storage()

    return Success(message="Settings applied")


async def require_confirm_change_homescreen(ctx: wire.GenericContext) -> None:
    await confirm_action(
        ctx,
        "Set homescreen",
        description="Do you really want to change the homescreen image?",
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_label(ctx: wire.GenericContext, label: str) -> None:
    await confirm_action(
        ctx,
        i18n.Title.change_label,
        i18n.Text.change_label_to_x.format(label),
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_passphrase(
    ctx: wire.GenericContext, use: bool
) -> None:
    if use:
        title = i18n.Title.enable_passphrase
        msg = i18n.Text.enable_passphrase
    else:
        title = i18n.Title.disable_passphrase
        msg = i18n.Text.disable_passphrase
    await confirm_action(
        ctx,
        title,
        msg,
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_passphrase_source(
    ctx: wire.GenericContext, passphrase_always_on_device: bool
) -> None:
    title = i18n.Title.passphrase_source
    if passphrase_always_on_device:
        msg = i18n.Text.enable_passphrase_always
    else:
        msg = i18n.Text.revoke_enable_passphrase_always
    await confirm_action(
        ctx,
        title,
        msg,
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_display_rotation(
    ctx: wire.GenericContext, rotation: int
) -> None:
    if rotation == 0:
        label = "north"
    elif rotation == 90:
        label = "east"
    elif rotation == 180:
        label = "south"
    elif rotation == 270:
        label = "west"
    else:
        raise wire.DataError("Unsupported display rotation")
    await confirm_action(
        ctx,
        "Change rotation",
        description="Do you really want to change display rotation to {}?",
        description_param=label,
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_autolock_delay(
    ctx: wire.GenericContext, delay_ms: int
) -> None:
    await confirm_action(
        ctx,
        i18n.Setting.auto_lock,
        i18n.Text.auto_lock_x.format(format_duration_ms(delay_ms)),
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_safety_checks(
    ctx: wire.GenericContext, level: SafetyCheckLevel
) -> None:
    if level == SafetyCheckLevel.PromptAlways:
        await confirm_action(
            ctx,
            i18n.Title.disable_safety_checks,
            i18n.Text.disable_safety_checks,
            br_code=ButtonRequestType.ProtectCall,
        )
    elif level == SafetyCheckLevel.PromptTemporarily:
        await confirm_action(
            ctx,
            i18n.Title.disable_safety_checks,
            i18n.Text.disable_safety_checks,
            br_code=ButtonRequestType.ProtectCall,
        )
    elif level == SafetyCheckLevel.Strict:
        await confirm_action(
            ctx,
            i18n.Title.enable_safety_checks,
            i18n.Text.enable_safety_checks,
            br_code=ButtonRequestType.ProtectCall,
        )
    else:
        raise ValueError  # enum value out of range


async def require_confirm_experimental_features(
    ctx: wire.GenericContext, enable: bool
) -> None:
    if enable:
        await confirm_action(
            ctx,
            i18n.Title.experimental_mode,
            i18n.Text.enable_experimental_features,
            br_code=ButtonRequestType.ProtectCall,
        )
