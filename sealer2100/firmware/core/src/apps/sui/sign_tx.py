from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import blake2b
from trezor.messages import SuiSignedTx, SuiSignTx

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from .helper import INTENT_BYTES, sui_address_from_pubkey
from . import ICON
from ..ethereum.layout import (
    require_show_overview_ton,
    require_confirm_fee_ton
)

@auto_keychain(__name__)
async def sign_tx(ctx: wire.Context, msg: SuiSignTx, keychain: Keychain) -> SuiSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = sui_address_from_pubkey(pub_key_bytes)

    from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

    intent = msg.raw_tx[:3]
    if INTENT_BYTES != intent:
        raise wire.DataError("Invalid raw tx")
    ctx.name = 'SUI'
    ctx.icon_path = ICON
    from apps.common.signverify import decode_message
    #格式化打印下message
    # from trezor.utils import dump_protobuf_lines
    # print("Sui_message","\n".join(dump_protobuf_lines(msg)))
    # await confirm_blind_sign_common(ctx, address, msg.raw_tx)
    await require_show_overview_ton(
        ctx,
        "SUI",
        msg.destination,
        msg.sui_amount,
        -10,
        None,
        False,
    )
    await require_confirm_fee_ton(
        ctx,
        msg.sui_amount,
        0,
        1,
        -10,
        token=None,
        from_address=address,
        to_address=msg.destination,
        contract_addr=None,
        token_id=None,
        evm_chain_id=None,
        raw_data=None,
    )
    await confirm_final(ctx, "SUI")
    signature = ed25519.sign(
        node.private_key(), blake2b(data=msg.raw_tx, outlen=32).digest()
    )

    return SuiSignedTx(public_key=pub_key_bytes, signature=signature)
