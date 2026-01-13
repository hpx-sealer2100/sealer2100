from typing import TYPE_CHECKING

from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.messages import EthereumMessageSignature
from trezor.ui.layouts import confirm_sign_message
from trezor.utils import HashWriter

from apps.common import paths
from apps.common.helpers import validate_message
from apps.common.signverify import decode_message

from . import networks
from .helpers import address_from_bytes, get_icon, get_display_network_name
from .keychain import PATTERNS_ADDRESS, with_keychain_from_path

if TYPE_CHECKING:
    from trezor.messages import EthereumSignMessage
    from trezor.wire import Context

    from apps.common.keychain import Keychain
    from .definitions import Definitions


def message_digest(message: bytes) -> bytes:
    h = HashWriter(sha3_256(keccak=True))
    signed_message_header = b"\x19Ethereum Signed Message:\n"
    h.extend(signed_message_header)
    h.extend(str(len(message)).encode())
    h.extend(message)
    return h.get_digest()


@with_keychain_from_path(*PATTERNS_ADDRESS)
async def sign_message(
    ctx: Context, msg: EthereumSignMessage, keychain: Keychain, defs: Definitions
) -> EthereumMessageSignature:
    validate_message(msg.message)
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    address = address_from_bytes(node.ethereum_pubkeyhash())

    network = defs.network
    ctx.icon_path = get_icon(
        network.chain_id if network else None
    )
    chain_id = None if network is not networks.UNKNOWN_NETWORK else network.chain_id
    await confirm_sign_message(
        ctx,
        get_display_network_name(network),
        decode_message(msg.message),
        address = address,
        chain_id = chain_id
    )

    signature = secp256k1.sign(
        node.private_key(),
        message_digest(msg.message),
        False,
        secp256k1.CANONICAL_SIG_ETHEREUM,
    )

    return EthereumMessageSignature(
        address=address,
        signature=signature[1:] + bytearray([signature[0]]),
    )
