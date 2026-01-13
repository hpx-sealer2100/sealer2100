from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha3_256
from trezor.messages import AptosSignedTx, AptosSignTx

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from . import ICON
from .helper import TRANSACTION_PREFIX
from .account_address import AccountAddress
from .transaction import RawTransaction
from trezor.ui.layouts.aptos import (
    confirm_transaction_detail,
    show_transaction_overview,
)


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: AptosSignTx, keychain: Keychain
) -> AptosSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = AccountAddress.from_key(pub_key_bytes).__str__()

    from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

    try:
        tx: RawTransaction = RawTransaction.from_bytes(msg.raw_tx)
        # from trezor import log
        # log.debug(__name__, f"tx: {tx}")
        from .well_known import find_confirm_handler
        confirm_handler = find_confirm_handler(tx)
        if tx.sender != AccountAddress.from_str(address):
            raise wire.DataError('Invalid sender, sender not match address in device')
    except:
        confirm_handler = None

    ctx.name = "APTOS"
    ctx.icon_path = ICON

    if confirm_handler:
        await confirm_handler(ctx, tx)
    else:
        await confirm_blind_sign_common(ctx, address, msg.raw_tx)

    prefix_bytes = RawTransaction.prehash()
    raw_tx = prefix_bytes + msg.raw_tx
    signature = ed25519.sign(node.private_key(), raw_tx)
    await confirm_final(ctx, "APTOS")
    return AptosSignedTx(public_key=pub_key_bytes, signature=signature)
