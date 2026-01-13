from typing import TYPE_CHECKING

import storage
from trezor import wire, iris
from trezor.ui import i18n
from trezor.messages import Success

from ..common.request_pin import verify_protection
from apps.base import reload_settings_from_storage

from trezor.ui.layouts import (
    confirm_wipe_device,
    confirm_wipe_device_tips,
    confirm_wipe_device_success,
    wait_doing,
)

if TYPE_CHECKING:
    from trezor.messages import WipeDevice

    pass


async def wipe_device(ctx: wire.GenericContext, msg: WipeDevice) -> Success:
    await confirm_wipe_device_tips(ctx)

    await confirm_wipe_device(ctx)

    # verify user pin
    await verify_protection(ctx)
    # show tips
    # await confirm_wipe_device_tips(ctx)
    await wait_doing(i18n.Text.wiping_device, iris.wipe())
    storage.wipe()
    reload_settings_from_storage()
    await confirm_wipe_device_success(ctx)
    if ctx == wire.DUMMY_CONTEXT:
        # if a dummy context which means a operation on device , we should restart the device
        from trezor import utils

        utils.reset()
    return Success(message="Device wiped")
