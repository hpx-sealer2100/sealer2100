from typing import TYPE_CHECKING

from trezor import io, loop, utils, wire
from trezor.messages import RebootToBootloader, Success
from trezor.ui.layouts import confirm_action

if TYPE_CHECKING:
    from typing import NoReturn

    pass


async def reboot_to_bootloader(ctx: wire.Context, msg: RebootToBootloader) -> NoReturn:
    from trezor.ui import i18n
    from trezor import io

    await confirm_action(
        ctx,
        i18n.Title.system_update,
        i18n.Text.switch_to_update_mode,
    )
    await ctx.write(Success(message="Rebooting"))
    # make sure the outgoing USB buffer is flushed
    await loop.wait(ctx.wire.iface.iface_num() | io.POLL_WRITE)
    # wait soemtime for ble send to phone
    await loop.sleep(500)
    # create a flag file
    with io.fatfs.open("0:/.stay_in_bootloader", "w"):
        pass
    utils.reboot_to_bootloader()
    raise RuntimeError
