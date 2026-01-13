from typing import TYPE_CHECKING

from storage import device
from trezor import config, wire, iris, utils
from trezor.messages import Success
from trezor.ui import i18n
from trezor.ui.layouts import (
    wait_doing,
    confirm_change_iris,
    show_success,
    show_error,
    request_iris_match,
    request_iris_regist,
    show_verify_origin_iris,
    show_regist_new_iris,
    confirm_device_protection_verify_retry,
    show_iris_duplicate,
)

from apps.common.request_pin import (
    request_sd_salt,
    request_pin,
)

if TYPE_CHECKING:
    from trezor.messages import ChangePin
    pass


async def change_iris(ctx: wire.Context) -> Success:
    if not device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")

    protect_type = device.get_device_protect_type()
    if not (protect_type & device.DEVICE_PROTECT_TYPE_IRIS):
        await show_error(ctx, "Not enable iris.\n\nCan't remove.")
        return

    # confirm that user wants to change the pin
    await confirm_change_iris(ctx)

    salt = await request_sd_salt(ctx)
    cur_protection: bytearray = None
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

    await show_regist_new_iris(ctx)

    iris_data_new = await request_iris_regist(ctx)

    if iris_data_new == iris_data:
        await show_iris_duplicate(ctx)
        return

    new_protection = bytearray()
    if protect_type & device.DEVICE_PROTECT_TYPE_PIN:
        new_protection.extend(pin.encode())

    if protect_type & device.DEVICE_PROTECT_TYPE_IRIS:
        new_protection.extend(iris_data_new)

    # write into storage
    if not config.change_pin(cur_protection, new_protection, salt, salt):
        await show_error(ctx, i18n.Text.iris_changed_failed)
        return

    if not utils.EMULATOR:
        async def do_del_iris_by_id():
            from trezor import loop
            await loop.sleep(400)
            await iris.open()
            await loop.sleep(50)
            await iris.del_user_by_id(iris_data)
            await loop.sleep(50)
            iris.close()
        await wait_doing(i18n.Text.iris_removing_old, do_del_iris_by_id())

    await show_success(
        ctx,
        i18n.Text.iris_changed_ok,
        title=i18n.Text.iris_changed,
        icon="A:/res/hp/ic_done566.png",
        button=i18n.Button.done,
    )
    return Success(message="iris change successed")
