import storage.device
from trezor import wire
from trezor.ui import i18n
from trezor.enums import ButtonRequestType
from trezor.messages import SetU2FCounter, Success
from trezor.ui.layouts import confirm_action


async def set_u2f_counter(ctx: wire.Context, msg: SetU2FCounter) -> Success:
    if not storage.device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")
    if msg.u2f_counter is None:
        raise wire.ProcessError("No value provided")

    await confirm_action(
        ctx,
        i18n.Title.set_u2f_counter,
        i18n.Text.set_u2f_counter_x.format(msg.u2f_counter),
        br_code=ButtonRequestType.ProtectCall,
    )

    storage.device.set_u2f_counter(msg.u2f_counter)

    return Success(message="U2F counter set")
