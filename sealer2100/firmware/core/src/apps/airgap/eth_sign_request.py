import ur_parser
import ure as re  # type: ignore[Import "ure" could not be resolved]

from . import parser_path, MFPNotMatch, get_mfp
from apps.common.seed import get_seed

from trezor import log, loop, messages
from trezor.wire import errors
from trezor.ui.layouts import show_airgap_qrcode
from trezor.airgap.rust_ur.rust_ur import *
from trezor.airgap.rust_ur import utils


from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from trezor.wire import GenericContext
    from trezor import protobuf
    from typing import Any, List, Optional, Awaitable
    from trezor.messages import EthereumTypedDataSignature
    pass


async def eth_sign_request(ctx: "GenericContext", rust_ur: RustDecodedUR):
    log.debug(__name__, "handling eth sign request")
    try:
        from apps.base import handle_Initialize
        from trezor.messages import Initialize
        #1.Initialize session
        init = Initialize(session_id=b"\x00")
        await handle_Initialize(ctx, init)
        from trezor.messages import EthereumGetPublicKey
        from apps.ethereum.get_public_key import get_public_key
        #2.Get path from ur, and convert to address_n
        path = ur_parser.eth_get_current_ur_path(rust_ur.data())
        log.debug(__name__, f"eth_get_current_ur_path: {path}")
        address_n = parser_path(path)
        #3.Get master key fingerprint
        mfp = await get_mfp(ctx)
        log.debug(
            __name__, f"master key fingerprint in device: {hex(mfp)}"
        )
        #4.Get current xpub
        msg = EthereumGetPublicKey(address_n=address_n)
        pubkey = await get_public_key(ctx, msg)
        log.debug(
            __name__, f"xpub in device: {pubkey.xpub}"
        )
        #5.Eth check
        check = ur_parser.eth_check(rust_ur.data(), mfp)
        if check != 0:
            raise MFPNotMatch()

        if rust_ur.view_type() == ViewType.EthTx:
            # 6.1 eth_parse_raw
            parsed_raw = ur_parser.eth_parse_raw(rust_ur.data(),pubkey.xpub)
            if parsed_raw.error_code() != 0:
                parsed_raw.free()
                raise errors.DataError("Invalid eth sign request data")
            if parsed_raw.tx_type() == 0:
                # 7.1 sign legacy transaction
                log.debug(__name__, "eth sign legacy transaction")
                resp = await eth_sign_tx(ctx, parsed_raw, address_n)
                v_bytes_length = utils.byte_length(resp.signature_v)
                signature = (
                    resp.signature_r
                    + resp.signature_s
                    + resp.signature_v.to_bytes(v_bytes_length, "big")
                )
            elif parsed_raw.tx_type() == 2:
                # 7.2 sign eip-1559 transaction
                log.debug(__name__, "eth sign eip-1559 transaction")
                resp = await eth_sign_tx_eip1559(ctx, parsed_raw, address_n)
                v_bytes_length = utils.byte_length(resp.signature_v)
                signature = (
                    resp.signature_r
                    + resp.signature_s
                    + resp.signature_v.to_bytes(v_bytes_length, "big")
                )
            else:
                parsed_raw.free()
                raise errors.DataError("Unknown eth sign request transaction type")
            parsed_raw.free()

        elif rust_ur.view_type() == ViewType.EthPersonalMessage:
            # 6.2 sign eth_parse_personal_message
            parsed_personal_message = ur_parser.eth_parse_personal_message(rust_ur.data(),pubkey.xpub)
            if parsed_personal_message.error_code() != 0:
                parsed_personal_message.free()
                raise errors.DataError("Invalid eth sign request data")
            log.debug(__name__, f"personal message: {parsed_personal_message.raw_message()}")
            resp = await eth_sign_message(ctx, parsed_personal_message, address_n)
            signature = resp.signature
            parsed_personal_message.free()

        elif rust_ur.view_type() == ViewType.EthTypedData:
            # 6.3 sign eth_parse_typed_data
            parsed_typed_data = ur_parser.eth_parse_typed_data(rust_ur.data(),pubkey.xpub)
            if parsed_typed_data.error_code() != 0:
                parsed_typed_data.free()
                raise errors.DataError("Invalid eth sign request data")
            log.debug(__name__, f"eip712 raw message: {parsed_typed_data.raw_message()}")
            resp = await eth_configm_typed_data(address_n, parsed_typed_data.raw_message())
            parsed_typed_data.free()
            signature = resp.signature
            # seed = await get_seed(ctx)
            # #7 #8 sign the data and encode signature to ur
            # ur_result = ur_parser.eth_sign_tx(rust_ur.data(),seed)
            # if ur_result.error_code() != 0:
            #     raise errors.DataError("Failed to sign eth typed data")
            # #9. Show the signature qr code
            # rust_encoded_ur = RustEncodedUR()
            # rust_encoded_ur.encoding(ur_result)
            # await show_airgap_qrcode(rust_encoded_ur)
            # return

        else:
            raise errors.DataError("Unknown eth sign request type")

        #8. Wrap the signature in EthSignature and encode to ur
        log.debug(__name__, f"signature: {utils.bytes_to_hex(signature)}")
        ur_result = ur_parser.eth_ur_encode_signature(rust_ur.data(), signature, origin)
        if ur_result.error_code() != 0:
            raise errors.DataError("Failed to encode eth signature")
        rust_encoded_ur = RustEncodedUR()
        rust_encoded_ur.encoding(ur_result)
        #9. Show the signature qr code

        await show_airgap_qrcode(rust_encoded_ur)

    finally:
        from trezor.messages import EndSession
        from apps.base import handle_EndSession

        await handle_EndSession(ctx, EndSession())

# === eth sign request ===
async def eth_sign_tx(ctx: "GenericContext", parsed_raw: ur_parser.EthParsedRaw,address_n: list):
    from trezor.messages import EthereumSignTx
    from apps.ethereum.sign_tx import sign_tx
    from apps.ethereum.helpers import address_from_bytes

    log.debug(__name__, f"chain_id: {parsed_raw.chain_id()}")
    log.debug(__name__, f"gas_price: {parsed_raw.gas_price()}")
    log.debug(__name__, f"gas_limit: {parsed_raw.gas_limit()}")
    log.debug(__name__, f"nonce: {parsed_raw.nonce()}")
    log.debug(__name__, f"to: {parsed_raw.to()}")
    log.debug(__name__, f"value: {parsed_raw.value()}")
    log.debug(__name__, f"data: {utils.bytes_to_hex(parsed_raw.input())}")

    chain_id = parsed_raw.chain_id()
    gas_price = utils.string_to_256bitBE(parsed_raw.gas_price())
    gas_limit = utils.string_to_256bitBE(parsed_raw.gas_limit())
    nonce = utils.uint32_to_256bitBE(parsed_raw.nonce())
    to = parsed_raw.to()
    value = utils.string_to_256bitBE(parsed_raw.value())
    data = utils.hex_to_bytes(parsed_raw.input())
    msg = EthereumSignTx(
        address_n=address_n,
        chain_id=chain_id,
        gas_price=gas_price,
        gas_limit=gas_limit,
        nonce=nonce,
        to=to,
        value=value,
        data_initial_chunk=data,
        data_length=len(data),
    )

    return await sign_tx(ctx, msg)


async def eth_sign_message(ctx: "GenericContext", parsed_raw: ur_parser.EthParsedPersonalMessage,address_n: list):
    from trezor.messages import EthereumSignMessage
    from apps.ethereum.sign_message import sign_message

    data = utils.hex_to_bytes(parsed_raw.raw_message())

    msg = EthereumSignMessage(address_n=address_n, data=data)
    return await sign_message(ctx, msg)


async def eth_sign_tx_eip1559(ctx: "GenericContext", parsed_raw: ur_parser.EthParsedRaw,address_n: list):
    from trezor.messages import EthereumSignTxEIP1559
    from apps.ethereum.sign_tx_eip1559 import sign_tx_eip1559
    from apps.ethereum.helpers import address_from_bytes

    chain_id = parsed_raw.chain_id()
    nonce = utils.uint32_to_256bitBE(parsed_raw.nonce())
    max_priority_fee_per_gas = utils.string_to_256bitBE(parsed_raw.max_priority_fee_per_gas())
    max_fee_per_gas = utils.string_to_256bitBE(parsed_raw.max_fee_per_gas())
    gas_limit = utils.string_to_256bitBE(parsed_raw.gas_limit())
    to = parsed_raw.to()
    value = utils.string_to_256bitBE(parsed_raw.value())
    data = utils.hex_to_bytes(parsed_raw.input())
    access_list = []


    msg = EthereumSignTxEIP1559(
        address_n=address_n,
        chain_id=chain_id,
        nonce=nonce,
        max_priority_fee=max_priority_fee_per_gas,
        max_gas_fee=max_fee_per_gas,
        gas_limit=gas_limit,
        data_length=len(data),
        data_initial_chunk=data,
        to=to,
        value=value,
        access_list=access_list,
    )
    return await sign_tx_eip1559(ctx, msg)

### helper methods, copy from trezorctl
def int_to_big_endian(value: int) -> bytes:
    return value.to_bytes((value.bit_length() + 7) // 8, "big")


def decode_hex(value: str) -> bytes:
    if value.startswith("0x") or value.startswith("0X"):
        return utils.hex_to_bytes(value[2:])
    else:
        return utils.hex_to_bytes(value)


def sanitize_typed_data(data: dict) -> dict:
    """Remove properties from a message object that are not defined per EIP-712."""
    REQUIRED_KEYS = ("types", "primaryType", "domain", "message")
    sanitized_data = {key: data[key] for key in REQUIRED_KEYS}
    sanitized_data["types"].setdefault("EIP712Domain", [])
    return sanitized_data

def is_array(type_name: str) -> bool:
    return type_name[-1] == "]"

def typeof_array(type_name: str) -> str:
    return type_name[: type_name.rindex("[")]


def parse_type_n(type_name: str) -> int:
    """Parse N from type<N>. Example: "uint256" -> 256."""
    match = re.search(r"\d+$", type_name)
    if match:
        return int(match.group(0))
    else:
        raise ValueError(f"Could not parse type<N> from {type_name}.")

def parse_array_n(type_name: str) -> Optional[int]:
    """Parse N in type[<N>] where "type" can itself be an array type."""
    # sign that it is a dynamic array - we do not know <N>
    if type_name.endswith("[]"):
        return None

    start_idx = type_name.rindex("[") + 1
    return int(type_name[start_idx:-1])


def get_byte_size_for_int_type(int_type: str) -> int:
    return parse_type_n(int_type) // 8

def get_field_type(type_name: str, types: dict) -> messages.EthereumFieldType:
    data_type = None
    size = None
    entry_type = None
    struct_name = None
    from trezor.enums import EthereumDataType

    if is_array(type_name):
        data_type = EthereumDataType.ARRAY
        size = parse_array_n(type_name)
        member_typename = typeof_array(type_name)
        entry_type = get_field_type(member_typename, types)
        # Not supporting nested arrays currently
        if entry_type.data_type == EthereumDataType.ARRAY:
            raise ValueError("Nested arrays are not supported")
    elif type_name.startswith("uint"):
        data_type = EthereumDataType.UINT
        size = get_byte_size_for_int_type(type_name)
    elif type_name.startswith("int"):
        data_type = EthereumDataType.INT
        size = get_byte_size_for_int_type(type_name)
    elif type_name.startswith("bytes"):
        data_type = EthereumDataType.BYTES
        size = None if type_name == "bytes" else parse_type_n(type_name)
    elif type_name == "string":
        data_type = EthereumDataType.STRING
    elif type_name == "bool":
        data_type = EthereumDataType.BOOL
    elif type_name == "address":
        data_type = EthereumDataType.ADDRESS
    elif type_name in types:
        data_type = EthereumDataType.STRUCT
        size = len(types[type_name])
        struct_name = type_name
    else:
        raise ValueError(f"Unsupported type name: {type_name}")

    return messages.EthereumFieldType(
        data_type=data_type,
        size=size,
        entry_type=entry_type,
        struct_name=struct_name,
    )

def encode_data(value: Any, type_name: str) -> bytes:
    if type_name.startswith("bytes"):
        return decode_hex(value)
    elif type_name == "string":
        return value.encode()
    elif type_name.startswith("int") or type_name.startswith("uint"):
        byte_length = get_byte_size_for_int_type(type_name)
        if isinstance(value, str):
            if value.startswith("0x") or value.startswith("0X"):
                value = value[2:]
                value = int(value, 16)
            elif value.isdigit():
                value = int(value)
            else:
                raise ValueError(f"Invalid int value - {value}")
        elif not isinstance(value, int):
            raise ValueError(f"Invalid int value - {value}")
        # mpy `to_bytes` method doesn’t implement signed parameter.
        return int(value).to_bytes(byte_length, "big")
    elif type_name == "bool":
        if not isinstance(value, bool):
            raise ValueError(f"Invalid bool value - {value}")
        return int(value).to_bytes(1, "big")
    elif type_name == "address":
        return decode_hex(value)

    # We should be receiving only atomic, non-array types
    raise ValueError(f"Unsupported data type for direct field encoding: {type_name}")

# airgap typed data sign ctx, a mock client
class AirgapTypedDataSignContext:
    def __init__(self, obj: dict):
        self.obj = obj
        pass

    async def call(self, msg: protobuf.MessageType, expected_type: type[protobuf.MessageType],) -> Any:

        if msg.MESSAGE_WIRE_TYPE == messages.ButtonRequest.MESSAGE_WIRE_TYPE:
            return messages.ButtonAck()
        elif msg.MESSAGE_WIRE_TYPE == messages.EthereumTypedDataStructRequest.MESSAGE_WIRE_TYPE:
            struct_name = msg.name
            types = self.obj["types"]
            members: List[messages.EthereumStructMember] = []
            for field in types[struct_name]:
                field_type = get_field_type(field["type"], types)
                struct_member = messages.EthereumStructMember(
                    type=field_type,
                    name=field["name"],
                )
                members.append(struct_member)
            resp = messages.EthereumTypedDataStructAck(members=members)
            return resp
        elif msg.MESSAGE_WIRE_TYPE == messages.EthereumTypedDataValueRequest.MESSAGE_WIRE_TYPE:
            types = self.obj["types"]
            root_index = msg.member_path[0]
            # Index 0 is for the domain data, 1 is for the actual message
            if root_index == 0:
                member_typename = "EIP712Domain"
                member_data = self.obj["domain"]
            elif root_index == 1:
                member_typename = self.obj["primaryType"]
                member_data = self.obj["message"]
            else:
                raise ValueError("Root index can only be 0 or 1")

            # It can be asking for a nested structure (the member path being [X, Y, Z, ...])
            # TODO: what to do when the value is missing (for example in recursive types)?
            for index in msg.member_path[1:]:
                if isinstance(member_data, dict):
                    member_def = types[member_typename][index]
                    member_typename = member_def["type"]
                    member_data = member_data[member_def["name"]]
                elif isinstance(member_data, list):
                    member_typename = typeof_array(member_typename)
                    member_data = member_data[index]

            # If we were asked for a list, first sending its length and we will be receiving
            # requests for individual elements later
            if isinstance(member_data, list):
                # Sending the length as uint16
                encoded_data = len(member_data).to_bytes(2, "big")
            else:
                encoded_data = encode_data(member_data, member_typename)
            log.debug(__name__, f"requesting: {msg.member_path}")
            log.debug(__name__, f"value: {member_data}")
            log.debug(__name__, f"sending type: {member_typename}, value: {utils.bytes_to_hex(encoded_data)}")
            request = messages.EthereumTypedDataValueAck(value=encoded_data)
            return request
    async def read(self, *argv: 'Any') -> None:
        pass

    async def write(self, *argv: 'Any') -> None:
        pass
    async def wait(self, *tasks: Awaitable) -> Any:
        return await loop.race(*tasks)

async def eth_configm_typed_data(address_n: List[int], eip712: str) -> EthereumTypedDataSignature:
    import ujson as json
    from apps.ethereum.sign_typed_data import sign_typed_data
    from trezor.messages import EthereumSignTypedData
    obj = json.loads(eip712)

    ctx = AirgapTypedDataSignContext(obj)
    req = EthereumSignTypedData(address_n=address_n, primary_type=obj["primaryType"], metamask_v4_compat=True)
    resp = await sign_typed_data(ctx, req)
    return resp

