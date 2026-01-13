from typing import TYPE_CHECKING

from storage import device
from trezor import config, wire
from trezor.messages import Success
from trezor.ui import i18n
from trezor.ui.layouts import (
    confirm_change_pin,
    confirm_remove_pin,
    show_success,
    show_error,
    confirm_device_protection_verify_retry,
)

from apps.common.request_pin import (
    request_pin,
    request_sd_salt,
    request_iris_match,
)

if TYPE_CHECKING:
    from typing import Awaitable

    from trezor.messages import ChangePin


async def change_pin(ctx: wire.Context, msg: ChangePin) -> Success:
    if not device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")

    if msg.remove:
        return await do_remove_pin(ctx)
    else:
        return await do_change_pin(ctx)

async def do_remove_pin(ctx: wire.Context) -> Success:
    protect_type = device.get_device_protect_type()

    if protect_type != device.DEVICE_PROTECT_TYPE_PIN_AND_IRIS:
        await show_error(ctx, "Not enable pin and iris.\n\nCan't remove.")
        return

    await confirm_remove_pin(ctx)

    salt = await request_sd_salt(ctx)

    pin = ""
    cur_protection: bytearray = None
    iris_data: bytes = None

    while True:
        # irirs first if enabled
        if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
            iris_data = await request_iris_match(ctx)

        if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
            # get old pin
            pin = await request_pin(ctx, i18n.Title.enter_pin)

        cur_protection = bytearray()
        if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
            cur_protection.extend(pin.encode())

        if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
            cur_protection.extend(iris_data)

        # verify pass
        if config.check_pin(cur_protection, salt):
            break
        # verify failed
        rem = config.get_pin_rem()
        if await confirm_device_protection_verify_retry(ctx, rem):
            continue

    # write into storage
    if not config.change_pin(cur_protection, iris_data, salt, salt):
        await show_error(ctx, i18n.Text.iris_changed_failed)
        return
    device.set_device_protect_type(device.DEVICE_PROTECT_TYPE_IRIS)
    await show_success(
        ctx,
        i18n.Text.pin_remove_success,
        title=i18n.Title.pin_removed,
        button=i18n.Button.done,
    )

    return Success(message="PIN remove success")


async def do_change_pin(ctx: wire.Context) -> Success:
    protect_type = device.get_device_protect_type()

    if not (protect_type & device.DEVICE_PROTECT_TYPE_PIN):
        await show_error(ctx, "Not enable PIN.\n\nCan't change.")
        return

    await confirm_change_pin(ctx)

    salt = await request_sd_salt(ctx)

    cur_protection: bytearray = None
    pin = ""
    iris_data = None

    while True:
        # iris first if enabled
        if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
            iris_data = await request_iris_match(ctx)

        if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
            # get old pin
            pin = await request_pin(ctx, i18n.Title.enter_old_pin)

        cur_protection = bytearray()
        if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
            cur_protection.extend(pin.encode())

        if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
            cur_protection.extend(iris_data)

        # verify pass
        if config.check_pin(cur_protection, salt):
            break
        # verify failed
        rem = config.get_pin_rem()
        if await confirm_device_protection_verify_retry(ctx, rem):
            # clear
            continue

    newpin = await request_pin(ctx, i18n.Title.enter_new_pin)

    new_protection = bytearray()
    if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
        new_protection.extend(newpin.encode())
    if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
        new_protection.extend(iris_data)

    # write into storage
    if not config.change_pin(cur_protection, new_protection, salt, salt):
        await show_error(ctx, i18n.Text.iris_changed_failed)
        return

    await show_success(
        ctx,
        i18n.Text.pin_changed,
        title=i18n.Title.pin_changed,
        button=i18n.Button.done,
    )
    return Success(message="PIN change successed")
