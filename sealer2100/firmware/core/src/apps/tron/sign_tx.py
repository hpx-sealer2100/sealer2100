from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha256
from trezor.messages import TronSignedTx, TronSignTx
from trezor.ui.layouts import confirm_final

from storage import device
from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain
from apps.tron.address import _address_base58, get_address_from_public_key
from apps.tron.serialize import serialize

from . import ICON, PRIMARY_COLOR, layout, tokens
from ..ethereum.layout import (
    check_defi_lock,
)

@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: TronSignTx, keychain: Keychain
) -> TronSignedTx:
    """Parse and sign TRX transaction"""

    # if hasattr(msg, "data") and msg.data in ("\x00", b"\x00"):
    #     msg.data = None

    validate(msg)
    address_n = msg.address_n or ()
    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(address_n)

    seckey = node.private_key()
    public_key = secp256k1.publickey(seckey, False)
    address = get_address_from_public_key(public_key[:65])
    ctx.icon_path = ICON
    try:
        await _require_confirm_by_type(ctx, msg, address)
    except AttributeError as e:
        from trezor import log
        log.exception(__name__, e)
        raise wire.DataError("The transaction has invalid asset data field")

    raw_data = serialize(msg, address)
    data_hash = sha256(raw_data).digest()

    signature = secp256k1.sign(seckey, data_hash, False)

    signature = signature[1:65] + bytes([(~signature[0] & 0x01) + 27])
    return TronSignedTx(signature=signature, serialized_tx=raw_data)


async def _require_confirm_by_type(ctx, transaction: TronSignTx, owner_address: str):
    # Confirm transaction
    contract = transaction.contract
    if contract.transfer_contract:
        if contract.transfer_contract.amount is None:
            raise wire.DataError("Invalid Tron transfer amount")
        await layout.require_confirm_tx(
            ctx,
            contract.transfer_contract.to_address,
            contract.transfer_contract.amount,
        )
        await layout.require_confirm_fee(
            ctx,
            token = None,
            from_address=owner_address,
            to_address=contract.transfer_contract.to_address,
            value=contract.transfer_contract.amount,
            fee_limit=transaction.fee_limit,
            network='TRON',
        )
    elif contract.trigger_smart_contract:
        # check if TRC20 transfer/approval
        data = contract.trigger_smart_contract.data
        action = None

        if data is None:
            raise wire.DataError("Invalid Tron contract call data")

        if (
            data[:16]
            == b"\xa9\x05\x9c\xbb\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        ):
            action = "Transfer"
        elif (
            data[:16]
            == b"\x09\x5e\xa7\xb3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        ):
            action = "Approve"

        if action == "Transfer":
            token = tokens.token_by_address(
                "trc20", contract.trigger_smart_contract.contract_address
            )
            recipient = _address_base58(b"\x41" + data[16:36])
            value = int.from_bytes(data[36:68], "big")

            await layout.require_confirm_trigger_trc20(
                ctx,
                False if token is tokens.UNKNOWN_TOKEN else True,
                contract.trigger_smart_contract.contract_address,
                value,
                token,
                recipient,
            )
            if transaction.fee_limit:
                await layout.require_confirm_fee(
                    ctx,
                    token,
                    from_address=owner_address,
                    to_address=recipient,
                    value=value,
                    fee_limit=transaction.fee_limit,
                    network="TRON",
                )
        else:
            await check_defi_lock(ctx)
            from trezor.ui.layouts import confirm_blind_sign_common
            await confirm_blind_sign_common(ctx, owner_address, data)
    elif device.get_defi_lock():
        await check_defi_lock(ctx)
    elif contract.freeze_balance_contract:
        await layout.require_confirm_freeze(
            ctx,
            owner_address,
            contract.freeze_balance_contract.frozen_balance,
            contract.freeze_balance_contract.frozen_duration,
            contract.freeze_balance_contract.resource,
            contract.freeze_balance_contract.receiver_address,
        )
    elif contract.unfreeze_balance_contract:
        await layout.require_confirm_unfreeze(
            ctx,
            owner_address,
            contract.unfreeze_balance_contract.resource,
            contract.unfreeze_balance_contract.receiver_address,
            None,
        )
    elif contract.withdraw_balance_contract:
        await layout.confirm_tron_common(ctx, "Withdraw Balance Contract")
    elif contract.freeze_balance_v2_contract:
        await layout.require_confirm_freeze(
            ctx,
            owner_address,
            contract.freeze_balance_v2_contract.frozen_balance,
            None,
            contract.freeze_balance_v2_contract.resource,
            None,
        )
    elif contract.unfreeze_balance_v2_contract:
        await layout.require_confirm_unfreeze_v2(
            ctx,
            owner_address,
            contract.unfreeze_balance_v2_contract.resource,
            contract.unfreeze_balance_v2_contract.unfreeze_balance,
        )
    elif contract.withdraw_expire_unfreeze_contract:
        await layout.confirm_tron_common(ctx, "Withdraw Expire Unfreeze Contract")
    elif contract.delegate_resource_contract:
        await layout.require_confirm_delegate(
            ctx,
            owner_address,
            contract.delegate_resource_contract.resource,
            contract.delegate_resource_contract.balance,
            contract.delegate_resource_contract.receiver_address,
            contract.delegate_resource_contract.lock,
        )
    elif contract.undelegate_resource_contract:
        await layout.require_confirm_undelegate(
            ctx,
            owner_address,
            contract.undelegate_resource_contract.resource,
            contract.undelegate_resource_contract.balance,
            contract.undelegate_resource_contract.receiver_address,
            None,
        )
    elif contract.transfer_asset_contract:
        asset_name = contract.transfer_asset_contract.asset_name
        token = tokens.token_by_address('trc10', asset_name)

        await layout.require_confirm_transfer_asset(
            ctx, not token is tokens.UNKNOWN_TOKEN,
            asset_name,
            contract.transfer_asset_contract.amount,
            token,
            contract.transfer_asset_contract.to_address,
        )

        await layout.require_confirm_fee(
            ctx,
            token = token,
            from_address=owner_address,
            to_address=contract.transfer_asset_contract.to_address,
            value=contract.transfer_asset_contract.amount,
            fee_limit=transaction.fee_limit,
            network='TRON',
        )
    else:
        raise wire.DataError("Invalid transaction type")

    # Confirm extra data if exist
    if transaction.data:
        await layout.require_confirm_data(ctx, transaction.data, len(transaction.data))

    await confirm_final(ctx, "TRON")


def validate(msg: TronSignTx):
    if None in (msg.contract,):
        raise wire.ProcessError("Some of the required fields are missing (contract)")
