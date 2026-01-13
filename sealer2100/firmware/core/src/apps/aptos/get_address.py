from typing import TYPE_CHECKING

from trezor.messages import AptosAddress
from trezor.ui.layouts import show_address

from apps.common import paths, seed
from apps.common.keychain import auto_keychain

from . import ICON
from .account_address import AccountAddress

if TYPE_CHECKING:
    from trezor.messages import AptosGetAddress
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: AptosGetAddress, keychain: Keychain
) -> AptosAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = AccountAddress.from_key(pub_key_bytes).__str__()
    ctx.icon_path = ICON
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        await show_address(
            ctx,
            address=address,
            path=path,
            network="APTOS",
        )

    return AptosAddress(address=address)
