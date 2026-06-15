# Serialize TRON Format
from trezor.crypto import base58
from trezor.messages import TronSignTx, TronContract

from apps.common.writers import write_bytes_fixed

# PROTOBUF3 types
TYPE_VARINT = 0
TYPE_DOUBLE = 1
TYPE_STRING = 2
TYPE_GROUPS = 3
TYPE_GROUPE = 4
TYPE_FLOAT = 5


def add_field(w, fnumber, ftype):
    if fnumber > 15:
        w.append(fnumber << 3 | ftype)
        w.append(0x01)
    else:
        w.append(fnumber << 3 | ftype)


def write_varint(w, value):
    """
    Implements Base 128 variant
    See: https://developers.google.com/protocol-buffers/docs/encoding#varints
    """
    while True:
        byte = value & 0x7F
        value = value >> 7
        if value == 0:
            w.append(byte)
            break
        else:
            w.append(byte | 0x80)


def write_bytes_with_length(w, buf: bytes):
    write_varint(w, len(buf))
    write_bytes_fixed(w, buf, len(buf))


def pack_contract(contract: TronContract, owner_address):
    """
    Pack Tron Proto3 Contract
    See: https://github.com/tronprotocol/protocol/blob/master/core/Tron.proto
    and https://github.com/tronprotocol/protocol/blob/master/core/contract/smart_contract.proto
    and https://github.com/tronprotocol/protocol/blob/master/core/contract/asset_issue_contract.proto
    """
    retc = bytearray()
    add_field(retc, 1, TYPE_VARINT)
    # contract message
    cmessage = bytearray()
    api = ""
    if contract.transfer_contract:
        write_varint(retc, 1)
        api = "TransferContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        add_field(cmessage, 2, TYPE_STRING)
        write_bytes_with_length(
            cmessage, base58.decode_check(contract.transfer_contract.to_address)
        )
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.transfer_contract.amount)

    if contract.trigger_smart_contract:
        write_varint(retc, 31)
        api = "TriggerSmartContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        add_field(cmessage, 2, TYPE_STRING)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.trigger_smart_contract.contract_address),
        )
        if contract.trigger_smart_contract.call_value:
            add_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.call_value)

        # Contract data
        add_field(cmessage, 4, TYPE_STRING)
        write_bytes_with_length(cmessage, contract.trigger_smart_contract.data)

        if contract.trigger_smart_contract.call_token_value:
            add_field(cmessage, 5, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.call_token_value)
            add_field(cmessage, 6, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.asset_id)

    if contract.freeze_balance_contract:
        write_varint(retc, 11)
        api = "FreezeBalanceContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_contract.frozen_balance)
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_contract.frozen_duration)
        if contract.freeze_balance_contract.resource is not None:
            add_field(cmessage, 4, TYPE_VARINT)
            write_varint(cmessage, contract.freeze_balance_contract.resource)
        if contract.freeze_balance_contract.receiver_address is not None:
            add_field(cmessage, 5, TYPE_STRING)
            write_bytes_with_length(
                cmessage,
                base58.decode_check(contract.freeze_balance_contract.receiver_address),
            )

    if contract.unfreeze_balance_contract:
        write_varint(retc, 12)
        api = "UnfreezeBalanceContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        if contract.unfreeze_balance_contract.resource is not None:
            add_field(cmessage, 2, TYPE_VARINT)
            write_varint(cmessage, contract.unfreeze_balance_contract.resource)
        if contract.unfreeze_balance_contract.receiver_address is not None:
            add_field(cmessage, 3, TYPE_STRING)
            write_bytes_with_length(
                cmessage,
                base58.decode_check(
                    contract.unfreeze_balance_contract.receiver_address
                ),
            )

    if contract.withdraw_balance_contract:
        write_varint(retc, 13)
        api = "WithdrawBalanceContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

    if contract.freeze_balance_v2_contract:
        write_varint(retc, 54)
        api = "FreezeBalanceV2Contract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_v2_contract.frozen_balance)
        if contract.freeze_balance_v2_contract.resource is not None:
            add_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.freeze_balance_v2_contract.resource)

    if contract.unfreeze_balance_v2_contract:
        write_varint(retc, 55)
        api = "UnfreezeBalanceV2Contract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.unfreeze_balance_v2_contract.unfreeze_balance)
        if contract.unfreeze_balance_v2_contract.resource is not None:
            add_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.unfreeze_balance_v2_contract.resource)

    if contract.withdraw_expire_unfreeze_contract:
        write_varint(retc, 56)
        api = "WithdrawExpireUnfreezeContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

    if contract.delegate_resource_contract:
        write_varint(retc, 57)
        api = "DelegateResourceContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.delegate_resource_contract.resource)
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.delegate_resource_contract.balance)
        add_field(cmessage, 4, TYPE_STRING)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.delegate_resource_contract.receiver_address),
        )
        if contract.delegate_resource_contract.lock is not None:
            add_field(cmessage, 5, TYPE_VARINT)
            write_varint(cmessage, contract.delegate_resource_contract.lock)

    if contract.undelegate_resource_contract:
        write_varint(retc, 58)
        api = "UnDelegateResourceContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.undelegate_resource_contract.resource)
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.undelegate_resource_contract.balance)
        add_field(cmessage, 4, TYPE_STRING)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.undelegate_resource_contract.receiver_address),
        )
    if contract.transfer_asset_contract:
        write_varint(retc, 2)
        api = "TransferAssetContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, contract.transfer_asset_contract.asset_name.encode())
        add_field(cmessage, 2, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        add_field(cmessage, 3, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(contract.transfer_asset_contract.to_address))
        add_field(cmessage, 4, TYPE_VARINT)
        write_varint(cmessage, contract.transfer_asset_contract.amount)

    # write API
    capi = bytearray()
    add_field(capi, 1, TYPE_STRING)
    # write_bytes_with_length(capi, "type.googleapis.com/protocol." + api)
    write_bytes_with_length(capi, bytes("type.googleapis.com/protocol." + api, "ascii"))

    # extend to capi
    add_field(capi, 2, TYPE_STRING)
    write_bytes_with_length(capi, cmessage)

    # extend to contract
    add_field(retc, 2, TYPE_STRING)
    write_bytes_with_length(retc, capi)
    if contract.provider:
        add_field(retc, 3, TYPE_STRING)
        write_bytes_with_length(retc, contract.provider)
    if contract.contract_name:
        add_field(retc, 4, TYPE_STRING)
        write_bytes_with_length(retc, contract.contract_name)
    if contract.permission_id is not None:
        add_field(retc, 5, TYPE_VARINT)
        write_varint(retc, contract.permission_id)
    return retc


def serialize(transaction: TronSignTx, owner_address: str):
    # transaction parameters
    ret = bytearray()
    add_field(ret, 1, TYPE_STRING)
    write_bytes_with_length(ret, transaction.ref_block_bytes)
    add_field(ret, 4, TYPE_STRING)
    write_bytes_with_length(ret, transaction.ref_block_hash)
    add_field(ret, 8, TYPE_VARINT)
    write_varint(ret, transaction.expiration)
    if transaction.data is not None:
        add_field(ret, 10, TYPE_STRING)
        write_bytes_with_length(ret, bytes(transaction.data, "ascii"))

    # add Contract
    retc = pack_contract(transaction.contract, owner_address)

    add_field(ret, 11, TYPE_STRING)
    write_bytes_with_length(ret, retc)
    # add timestamp
    if transaction.timestamp:
        add_field(ret, 14, TYPE_VARINT)
        write_varint(ret, transaction.timestamp)
    # add fee_limit if any
    if transaction.fee_limit:
        add_field(ret, 18, TYPE_VARINT)
        write_varint(ret, transaction.fee_limit)

    return ret


# ── deserialize helpers ──────────────────────────────────────────────────────

def read_varint(data: bytes, offset: int):
    """Read a base-128 varint; returns (value, new_offset)."""
    result = 0
    shift = 0
    while True:
        b = data[offset]
        offset += 1
        result |= (b & 0x7F) << shift
        if not (b & 0x80):
            break
        shift += 7
    return result, offset


def read_bytes_with_length(data: bytes, offset: int):
    """Read length-prefixed bytes; returns (bytes, new_offset)."""
    length, offset = read_varint(data, offset)
    return data[offset:offset + length], offset + length


def read_field_tag(data: bytes, offset: int):
    """Read a protobuf field tag; returns (field_number, wire_type, new_offset)."""
    if offset >= len(data):
        return None, None, offset
    # Handle two-byte tags (field_number > 15)
    first = data[offset]
    offset += 1
    wire_type = first & 0x07
    field_number = first >> 3
    if field_number > 15 and offset < len(data):
        # skip the 0x01 marker byte written by add_field for fnumber > 15
        offset += 1
    return field_number, wire_type, offset


def parse_fields(data: bytes):
    """Parse all protobuf fields from data; returns list of (field_number, wire_type, value)."""
    fields = []
    offset = 0
    while offset < len(data):
        field_number, wire_type, offset = read_field_tag(data, offset)
        if field_number is None:
            break
        if wire_type == TYPE_VARINT:
            value, offset = read_varint(data, offset)
        elif wire_type == TYPE_STRING:
            value, offset = read_bytes_with_length(data, offset)
        else:
            break
        fields.append((field_number, wire_type, value))
        from trezor import log
        log.debug(__name__, "parse_fields: field_number=%d wire_type=%d value=%s", field_number, wire_type, value)
    return fields


def _fields_dict(data: bytes):
    """Parse fields into a dict {field_number: value} (last value wins)."""
    return {f: v for f, _, v in parse_fields(data)}


def unpack_contract(retc: bytes):
    """Inverse of pack_contract; returns (contract: TronContract, owner_address: str)."""
    from trezor.messages import (
        TronContract, TronTransferContract, TronTriggerSmartContract,
        TronFreezeBalanceContract, TronUnfreezeBalanceContract,
        TronWithdrawBalanceContract, TronFreezeBalanceV2Contract,
        TronUnfreezeBalanceV2Contract, TronWithdrawExpireUnfreezeContract,
        TronDelegateResourceContract, TronUnDelegateResourceContract,
        TronTransferAssetContract,
    )

    fields = _fields_dict(retc)
    contract_type = fields.get(1)          # varint: contract type
    capi_bytes = fields.get(2, b"")        # bytes: Any message
    provider = fields.get(3)
    contract_name = fields.get(4)
    permission_id = fields.get(5)

    # unpack Any (capi): field1=type_url, field2=value
    capi_fields = _fields_dict(capi_bytes)
    type_url = capi_fields.get(1, b"").decode("ascii")
    cmessage = capi_fields.get(2, b"")

    api = type_url.replace("type.googleapis.com/protocol.", "")
    msg_fields = _fields_dict(cmessage)

    owner_address = ""
    contract = TronContract()

    if api == "TransferContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        to_address = base58.encode_check(bytes(msg_fields[2]))
        amount = msg_fields[3]
        contract.transfer_contract = TronTransferContract(
            to_address=to_address, amount=amount
        )

    elif api == "TriggerSmartContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        contract_address = base58.encode_check(bytes(msg_fields[2]))
        call_value = msg_fields.get(3)
        data = bytes(msg_fields.get(4, b""))
        call_token_value = msg_fields.get(5)
        asset_id = msg_fields.get(6)
        contract.trigger_smart_contract = TronTriggerSmartContract(
            contract_address=contract_address,
            call_value=call_value,
            data=data,
            call_token_value=call_token_value,
            asset_id=asset_id,
        )

    elif api == "FreezeBalanceContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        frozen_balance = msg_fields[2]
        frozen_duration = msg_fields[3]
        resource = msg_fields.get(4)
        receiver_address = msg_fields.get(5)
        if receiver_address is not None:
            receiver_address = base58.encode_check(bytes(receiver_address))
        contract.freeze_balance_contract = TronFreezeBalanceContract(
            frozen_balance=frozen_balance,
            frozen_duration=frozen_duration,
            resource=resource,
            receiver_address=receiver_address,
        )

    elif api == "UnfreezeBalanceContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        resource = msg_fields.get(2)
        receiver_address = msg_fields.get(3)
        if receiver_address is not None:
            receiver_address = base58.encode_check(bytes(receiver_address))
        contract.unfreeze_balance_contract = TronUnfreezeBalanceContract(
            resource=resource,
            receiver_address=receiver_address,
        )

    elif api == "WithdrawBalanceContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        contract.withdraw_balance_contract = TronWithdrawBalanceContract()

    elif api == "FreezeBalanceV2Contract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        frozen_balance = msg_fields[2]
        resource = msg_fields.get(3)
        contract.freeze_balance_v2_contract = TronFreezeBalanceV2Contract(
            frozen_balance=frozen_balance,
            resource=resource,
        )

    elif api == "UnfreezeBalanceV2Contract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        unfreeze_balance = msg_fields[2]
        resource = msg_fields.get(3)
        contract.unfreeze_balance_v2_contract = TronUnfreezeBalanceV2Contract(
            unfreeze_balance=unfreeze_balance,
            resource=resource,
        )

    elif api == "WithdrawExpireUnfreezeContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        contract.withdraw_expire_unfreeze_contract = TronWithdrawExpireUnfreezeContract()

    elif api == "DelegateResourceContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        resource = msg_fields[2]
        balance = msg_fields[3]
        receiver_address = base58.encode_check(bytes(msg_fields[4]))
        lock = msg_fields.get(5)
        contract.delegate_resource_contract = TronDelegateResourceContract(
            resource=resource,
            balance=balance,
            receiver_address=receiver_address,
            lock=lock,
        )

    elif api == "UnDelegateResourceContract":
        owner_address = base58.encode_check(bytes(msg_fields[1]))
        resource = msg_fields[2]
        balance = msg_fields[3]
        receiver_address = base58.encode_check(bytes(msg_fields[4]))
        contract.undelegate_resource_contract = TronUnDelegateResourceContract(
            resource=resource,
            balance=balance,
            receiver_address=receiver_address,
        )

    elif api == "TransferAssetContract":
        asset_name = bytes(msg_fields[1]).decode("utf-8")
        owner_address = base58.encode_check(bytes(msg_fields[2]))
        to_address = base58.encode_check(bytes(msg_fields[3]))
        amount = msg_fields[4]
        contract.transfer_asset_contract = TronTransferAssetContract(
            asset_name=asset_name,
            to_address=to_address,
            amount=amount,
        )

    if provider is not None:
        contract.provider = provider
    if contract_name is not None:
        contract.contract_name = contract_name
    if permission_id is not None:
        contract.permission_id = permission_id

    return contract, owner_address


def deserialize(data: bytes):
    """Inverse of serialize; returns (transaction: TronSignTx, owner_address: str)."""
    from trezor.messages import TronSignTx

    fields = _fields_dict(data)

    ref_block_bytes = bytes(fields[1])
    ref_block_hash = bytes(fields[4])
    expiration = fields[8]
    raw_data = fields.get(10)
    transaction_data = raw_data.decode("ascii") if raw_data is not None else None
    retc = bytes(fields[11])
    timestamp = fields.get(14)
    fee_limit = fields.get(18)

    contract, owner_address = unpack_contract(retc)

    transaction = TronSignTx(
        ref_block_bytes=ref_block_bytes,
        ref_block_hash=ref_block_hash,
        expiration=expiration,
        data=transaction_data,
        contract=contract,
        timestamp=timestamp,
        fee_limit=fee_limit,
    )

    return transaction


