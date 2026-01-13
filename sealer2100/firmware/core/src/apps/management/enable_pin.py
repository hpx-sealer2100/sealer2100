from typing import TYPE_CHECKING

from storage import device
from trezor import config, wire
from trezor.messages import Success
from trezor.ui import i18n
from trezor.ui.layouts import (
    confirm_enable_pin,
    show_success,
    show_error,
    confirm_device_protection_verify_retry,
)

from trezor.ui.layouts.management import (
    show_pin_security_tip,
)

from apps.common.request_pin import (
    request_pin,
    request_sd_salt,
    request_iris_match,
)

if TYPE_CHECKING:
    from typing import Awaitable

    from trezor.messages import ChangePin


async def enable_pin(ctx: wire.Context) -> Success:
    if not device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")
    protect_type = device.get_device_protect_type()

    if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
        await show_error(ctx, "Enabled PIN.\n\nCan't enable.")
        return

    await confirm_enable_pin(ctx)

    salt = await request_sd_salt(ctx)

    cur_protection: bytearray = None
    iris_data: bytes = None

    while True:
        # irirs first if enabled       
        if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
            iris_data = await request_iris_match(ctx)

        cur_protection = bytearray()

        if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
            cur_protection.extend(iris_data)

        # verify pass
        if config.check_pin(cur_protection, salt):
            break
        # verify failed
        rem = config.get_pin_rem()
        if await confirm_device_protection_verify_retry(ctx, rem):
            continue

    await show_pin_security_tip(ctx)
    new_protection = bytearray()
    newpin = await request_pin(ctx, i18n.Title.enter_new_pin)
    new_protection.extend(newpin.encode())
    new_protection.extend(iris_data)
    # write into storage
    if not config.change_pin(cur_protection, new_protection, salt, salt):
        await show_error(ctx, i18n.Text.iris_changed_failed)
        return
    device.set_device_protect_type(device.DEVICE_PROTECT_TYPE_PIN_AND_IRIS)
    await show_success(
        ctx,
        i18n.Text.pin_enable_success,
        title=i18n.Title.pin_enabled,
        button=i18n.Button.done,
    )

    return Success(message="PIN remove success")