from typing import TYPE_CHECKING

from trezor import io, loop, utils, wire
from trezor.messages import RebootToBoardloader, Success
from trezor.ui.layouts import confirm_action

if TYPE_CHECKING:
    from typing import NoReturn

    pass


async def reboot_to_boardloader(
    ctx: wire.Context, msg: RebootToBoardloader
) -> NoReturn:
    from trezor.ui import i18n

    await confirm_action(
        ctx,
        i18n.Title.entering_boardloader,
        i18n.Text.switch_to_boardloader,
    )
    await ctx.write(Success(message="Rebooting"))
    # make sure the outgoing USB buffer is flushed
    await loop.wait(ctx.wire.iface.iface_num() | io.POLL_WRITE)
    utils.reboot2boardloader()
    raise RuntimeError
