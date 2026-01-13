from trezor import wire

from trezor.ui.components.common.webauthn import ConfirmInfo
from .helper import interact

async def confirm_webauthn(
    ctx: wire.GenericContext | None,
    info: ConfirmInfo,
) -> bool:
    raise NotImplementedError()

async def confirm_webauthn_reset(

):
    raise NotImplementedError()
