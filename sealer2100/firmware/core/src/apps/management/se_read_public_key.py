from typing import TYPE_CHECKING

from trezor import utils, wire
from trezor.messages import SEPublicKey

if TYPE_CHECKING:
    from trezor.messages import ReadSEPublicKey


async def se_read_public_key(ctx: wire.Context, msg: ReadSEPublicKey) -> SEPublicKey:
    if utils.EMULATOR:
        raise wire.ProcessError("Not support by emulator.")
    import thd89
    pk = thd89.get_public_key()
    return SEPublicKey(public_key=pk)
