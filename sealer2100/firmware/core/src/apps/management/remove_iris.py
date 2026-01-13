from typing import TYPE_CHECKING

from storage import device
from trezor import config, wire, iris, utils
from trezor.messages import Success
from trezor.ui import i18n
from trezor.ui.layouts import (
    wait_doing,
    confirm_remove_iris,
    show_success,
    show_error,
    request_iris_match,
    show_verify_origin_iris,
    confirm_device_protection_verify_retry,
)

from apps.common.request_pin import (
    request_sd_salt,
    request_pin,
)

if TYPE_CHECKING:
    from trezor.messages import ChangePin
    pass


async def remove_iris(ctx: wire.Context) -> Success:
    if not device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")

    protect_type = device.get_device_protect_type()
    if protect_type != device.DEVICE_PROTECT_TYPE_PIN_AND_IRIS:
        await show_error(ctx, "Not enable pin and iris.\n\nCan't remove.")
        return

    # confirm that user wants to change the pin
    await confirm_remove_iris(ctx)

    salt = await request_sd_salt(ctx)
    cur_protection: bytearray
    pin = ""
    while True:

        # iris first if enabled
        if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
            await show_verify_origin_iris(ctx)
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
    if not config.change_pin(cur_protection, pin, salt, salt):
        await show_error(ctx, i18n.Text.iris_removed_failed)
        return

    device.set_device_protect_type(device.DEVICE_PROTECT_TYPE_PIN)
    if not utils.EMULATOR:
        async def do_remove_iris():
            await iris.open()
            await iris.wipe()
            iris.close()
        await wait_doing(i18n.Text.iris_removing_old, do_remove_iris())

    await show_success(
        ctx,
        i18n.Text.iris_removed_ok,
        title=i18n.Text.iris_removed,
        icon= "A:/res/hp/ic_hongmo_yichu.png",
        button=i18n.Button.done,
    )
    return Success(message="iris remove successed")
