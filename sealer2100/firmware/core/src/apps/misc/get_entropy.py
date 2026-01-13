from typing import TYPE_CHECKING

from trezor.crypto import random
from trezor.enums import ButtonRequestType
from trezor.messages import Entropy
from trezor.ui.layouts import confirm_action

if TYPE_CHECKING:
    from trezor.wire import Context
    from trezor.messages import GetEntropy


async def get_entropy(ctx: Context, msg: GetEntropy) -> Entropy:
    from trezor.ui import i18n

    await confirm_action(
        ctx,
        i18n.Title.confirm_entropy,
        i18n.Text.confirm_entropy,
        br_code=ButtonRequestType.ProtectCall,
    )

    size = min(msg.size, 1024)
    entropy = random.bytes(size)

    return Entropy(entropy=entropy)
