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

    async def do_wipe():
        if hasattr(ctx, "force_wipe") and ctx.force_wipe:
            # use <empty> pin to verify protection until lock device, make the device can be wiped
            from trezor import config, loop
            while config.get_pin_rem():
                config.unlock("", None)
                await loop.sleep(10)
        else:
            # verify user pin
            await verify_protection(ctx)

        await iris.wipe()
    # show tips
    # await confirm_wipe_device_tips(ctx)
    await wait_doing(i18n.Text.wiping_device, do_wipe())
    storage.wipe()
    reload_settings_from_storage()
    await confirm_wipe_device_success(ctx)
    if ctx == wire.DUMMY_CONTEXT:
        # if a dummy context which means a operation on device , we should restart the device
        from trezor import utils

        utils.reset()
    return Success(message="Device wiped")
