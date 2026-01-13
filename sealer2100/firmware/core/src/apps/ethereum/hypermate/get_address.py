from typing import TYPE_CHECKING

from trezor.messages import EthereumAddressHypermate as EthereumAddress
from trezor.ui.layouts import show_address

from apps.common import paths

from .. import networks
from ..helpers import address_from_bytes, get_display_network_name
from .keychain import PATTERNS_ADDRESS, with_keychain_from_path

if TYPE_CHECKING:
    from trezor.messages import EthereumGetAddressHypermate as EthereumGetAddress
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@with_keychain_from_path(*PATTERNS_ADDRESS)
async def get_address(
    ctx: Context, msg: EthereumGetAddress, keychain: Keychain
) -> EthereumAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)

    if msg.chain_id:
        network = networks.by_chain_id(msg.chain_id)
    else:
        if len(msg.address_n) > 1:  # path has slip44 network identifier
            network = networks.by_slip44(msg.address_n[1] & 0x7FFF_FFFF)
        else:
            network = networks.UNKNOWN_NETWORK
    address = address_from_bytes(node.ethereum_pubkeyhash(), network)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        await show_address(
            ctx,
            address=address,
            path=path,
            network=get_display_network_name(network),
            chain_id=msg.chain_id
            if network is networks.UNKNOWN_NETWORK
            else None,
        )

    return EthereumAddress(address=address)
