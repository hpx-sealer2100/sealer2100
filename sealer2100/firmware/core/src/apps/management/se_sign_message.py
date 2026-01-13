from typing import TYPE_CHECKING

from trezor import utils, wire
from trezor.messages import SEMessageSignature

if TYPE_CHECKING:
    from trezor.messages import SESignMessage


async def se_sign_message(ctx: wire.Context, msg: SESignMessage) -> SEMessageSignature:
    if utils.EMULATOR:
        raise wire.ProcessError("Not support by emulator.")

    from trezor.ui.layouts import confirm_verify_device

    await confirm_verify_device(ctx)

    import thd89

    sig = thd89.sign_message(msg.message)
    return SEMessageSignature(signature=sig)
