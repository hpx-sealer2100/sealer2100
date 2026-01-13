import utime
from typing import Any, NoReturn, TYPE_CHECKING

from trezor import iris, utils
import storage.cache
import storage.sd_salt
from trezor import config, loop, wire, log

from storage.device import (
    DEVICE_PROTECT_TYPE_PIN,
    DEVICE_PROTECT_TYPE_IRIS,
    DEVICE_PROTECT_TYPE_PIN_AND_IRIS,
)

from .sdcard import request_sd_salt

from trezor.ui import i18n
from trezor.ui.layouts import (
    wait_doing,
    request_iris_regist,
    request_iris_match,
    show_pin_reach_limit,
    confirm_device_protection_verify_retry,
)
from trezor.ui.layouts.management import (
    show_pin_security_tip,
    show_iris_regist_tip,
    show_iris_regist_note,
    suggest_enable_iris,
)

if TYPE_CHECKING:
    from storage.device import DeviceProtectType

    pass


def can_lock_device() -> bool:
    """Return True if the device has a PIN set or SD-protect enabled."""
    return config.has_pin() or storage.sd_salt.is_enabled()


async def request_pin(
    ctx: wire.GenericContext,
    prompt: str = "",
    attempts_remaining: int | None = None,
    allow_cancel: bool = True,
    **kwargs: Any,
) -> str:
    from trezor.ui.layouts import request_pin_on_device

    return await request_pin_on_device(ctx, prompt, attempts_remaining, allow_cancel)


async def request_pin_confirm(ctx: wire.Context, *args: Any, **kwargs: Any) -> str:
    while True:
        pin1 = await request_pin(ctx, i18n.Title.enter_new_pin, *args, **kwargs)
        pin2 = await request_pin(ctx, i18n.Title.enter_pin_again, *args, **kwargs)
        if pin1 == pin2:
            return pin1

        await pin_mismatch(ctx)


async def request_device_protection(
    ctx: wire.Context, protect: DeviceProtectType
) -> DeviceProtectType:
    protection = bytearray()

    if protect & DEVICE_PROTECT_TYPE_PIN:
        log.debug(__name__, "enable device protected by PIN")
        await show_pin_security_tip(ctx)
        newpin = await request_pin_confirm(ctx)
        protection.extend(newpin.encode())

    if protect == DEVICE_PROTECT_TYPE_PIN:
        await pin_have_enabled(ctx)

    if not (protect & DEVICE_PROTECT_TYPE_IRIS):
        if await suggest_enable_iris(ctx):
            protect |= DEVICE_PROTECT_TYPE_IRIS

    if protect & DEVICE_PROTECT_TYPE_IRIS:
        log.debug(__name__, "enable device protected by iris")
        await show_iris_regist_tip(ctx)
        await show_iris_regist_note(ctx)
        iris_data = await request_iris_regist(ctx)
        protection.extend(iris_data)
        await iris_regist_success(ctx)
    if not config.change_pin("", protection, None, None):
        raise wire.ProcessError("Failed to set PIN")

    return protect


async def pin_mismatch(ctx) -> None:
    from trezor.ui.layouts import show_error

    title = i18n.Title.pin_not_match
    msg = i18n.Text.pin_not_match

    await show_error(
        ctx,
        title=title,
        msg=msg,
        button=i18n.Button.try_again,
    )

async def pin_have_enabled(ctx) -> None:
    from trezor.ui.layouts import show_success
    title = i18n.Title.pin_enabled
    msg = i18n.Text.pin_enable_success
    await show_success(
        ctx, 
        title = title, 
        msg = msg,
        button=i18n.Button.done,
    )

async def request_pin_and_sd_salt(
    ctx: wire.Context,
    prompt: str = "",
    allow_cancel: bool = True,
) -> tuple[str, bytearray | None]:
    if config.has_pin():
        pin = await request_pin(ctx, prompt, config.get_pin_rem(), allow_cancel)
        config.ensure_not_wipe_code(pin)
    else:
        pin = ""

    salt = await request_sd_salt(ctx)

    return pin, salt


def _set_last_unlock_time() -> None:
    now = utime.ticks_ms()
    storage.cache.set_int(storage.cache.APP_COMMON_REQUEST_PIN_LAST_UNLOCK, now)


def _get_last_unlock_time() -> int:
    return storage.cache.get_int(storage.cache.APP_COMMON_REQUEST_PIN_LAST_UNLOCK) or 0


async def verify_protection(
    ctx: wire.GenericContext = wire.DUMMY_CONTEXT,
    cache_time_ms: int = 0,
) -> None:

    last_unlock = _get_last_unlock_time()
    if (
        cache_time_ms
        and last_unlock
        and utime.ticks_ms() - last_unlock <= cache_time_ms
        and config.is_unlocked()
    ):
        return

    protect_type = storage.device.get_device_protect_type()
    log.debug(__name__, f"protect type: {protect_type}")

    salt = await request_sd_salt(ctx)

    while True:
        protection = bytearray()

        if not config.has_pin():
            protection = ""
        else:

            # iris first if enabled
            if protect_type & DEVICE_PROTECT_TYPE_IRIS:
                iris_data = await request_iris_match(ctx)

            if protect_type & DEVICE_PROTECT_TYPE_PIN:
                # request pin
                pin = await request_pin(ctx, i18n.Title.enter_pin)

            if protect_type & DEVICE_PROTECT_TYPE_PIN:
                protection.extend(pin.encode())
            if protect_type & DEVICE_PROTECT_TYPE_IRIS:
                protection.extend(iris_data)

        if config.unlock(protection, salt):
            _set_last_unlock_time()
            return
        elif not config.has_pin():
            # failed when not have pin
            raise RuntimeError

        # error
        rem = config.get_pin_rem()

        # locked
        if rem == 0:
            await show_pin_reach_limit(ctx)
            await wait_doing(i18n.Text.wiping_device, iris.wipe())
            storage.wipe()
            utils.reset()
            return

        await confirm_device_protection_verify_retry(ctx, rem)

    raise wire.PinInvalid


async def error_pin_invalid(ctx: wire.Context) -> NoReturn:
    from trezor.ui.layouts import show_error

    await show_error(
        ctx,
        title=i18n.Title.wrong_pin,
        msg=i18n.Text.wrong_pin,
    )
    raise wire.PinInvalid


async def error_pin_matches_wipe_code(ctx: wire.Context) -> NoReturn:
    from trezor.ui.layouts import show_error

    await show_error(
        ctx,
        title="Invalid PIN",
        msg="The new PIN must be different from your\nwipe code.",
    )
    raise wire.PinInvalid


async def iris_regist_success(ctx: wire.Context):
    from trezor.ui.layouts import show_success

    title = i18n.Text.register_iris_ok
    msg = i18n.Text.iris_ok_desc
    await show_success(
        ctx,
        title=title,
        msg=msg,
        icon="A:/res/hp/ic_done34.png",
        button=i18n.Button.continue_,
    )
