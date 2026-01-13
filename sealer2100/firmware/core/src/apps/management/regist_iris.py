from typing import TYPE_CHECKING

from storage import device
from trezor import config, wire, iris, utils
from trezor.messages import Success
from trezor.ui import i18n
from trezor.ui.layouts import (
    confirm_regist_iris,
    show_iris_regist_tip,
    show_iris_regist_note,
    show_error,
    request_iris_regist,
    confirm_device_protection_verify_retry,
)

from apps.common.request_pin import (
    request_sd_salt,
    request_pin,
    iris_regist_success,
)

if TYPE_CHECKING:
    from trezor.messages import ChangePin
    pass


async def regist_iris(ctx: wire.Context) -> Success:
    if not device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")

    protect_type = device.get_device_protect_type()
    if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
        await show_error(ctx, "Iris have enabled.\n\nCan't enable.")
        return

    await confirm_regist_iris(ctx)

    salt = await request_sd_salt(ctx)
    cur_protection : bytearray = None
    pin = ""
    while True:

        if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
            # get old pin
            pin = await request_pin(ctx, i18n.Title.enter_pin)

        cur_protection = bytearray()
        if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
            cur_protection.extend(pin.encode())

        # verify pass
        if config.check_pin(cur_protection, salt):
            break
        # verify failed
        rem = config.get_pin_rem()
        if await confirm_device_protection_verify_retry(ctx, rem):
            continue

    await show_iris_regist_tip(ctx)
    await show_iris_regist_note(ctx)
    iris_data_new = await request_iris_regist(ctx)

    new_protection = bytearray()
    if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
        new_protection.extend(pin.encode())

    new_protection.extend(iris_data_new)

    # write into storage
    if not config.change_pin(cur_protection, new_protection, salt, salt):
        await show_error(ctx, i18n.Text.iris_changed_failed)
        return

    device.set_device_protect_type(device.DEVICE_PROTECT_TYPE_PIN_AND_IRIS)
    await iris_regist_success(ctx)
    return Success(message="iris regist successed")
