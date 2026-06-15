from typing import TYPE_CHECKING
from ubinascii import hexlify
import ujson as json


from . import networks
from trezor.messages import EthereumNetworkInfo, EthereumTokenInfo

if TYPE_CHECKING:
    from trezor.messages import EthereumFieldType

RSKIP60_NETWORKS = (30, 31)


def address_from_bytes(
    address_bytes: bytes, network: EthereumNetworkInfo = networks.UNKNOWN_NETWORK
) -> str:
    """
    Converts address in bytes to a checksummed string as defined
    in https://github.com/ethereum/EIPs/blob/master/EIPS/eip-55.md
    """
    from trezor.crypto.hashlib import sha3_256

    if network.chain_id in RSKIP60_NETWORKS:
        # rskip60 is a different way to calculate checksum
        prefix = str(network.chain_id) + "0x"
    else:
        prefix = ""

    address_hex = hexlify(address_bytes).decode()
    digest = sha3_256((prefix + address_hex).encode(), keccak=True).digest()

    def _maybe_upper(i: int) -> str:
        """Uppercase i-th letter only if the corresponding nibble has high bit set."""
        digest_byte = digest[i // 2]
        hex_letter = address_hex[i]
        if i % 2 == 0:
            # even letter -> high nibble
            bit = 0x80
        else:
            # odd letter -> low nibble
            bit = 0x08
        if digest_byte & bit:
            return hex_letter.upper()
        else:
            return hex_letter

    return "0x" + "".join(_maybe_upper(i) for i in range(len(address_hex)))


def bytes_from_address(address: str) -> bytes:
    from ubinascii import unhexlify
    from trezor import wire

    if len(address) == 40:
        return unhexlify(address)

    elif len(address) == 42:
        if address[0:2] not in ("0x", "0X"):
            raise wire.ProcessError("Ethereum: invalid beginning of an address")
        return unhexlify(address[2:])

    elif len(address) == 0:
        return bytes()

    raise wire.ProcessError("Ethereum: Invalid address length")


def get_type_name(field: EthereumFieldType) -> str:
    """Create a string from type definition (like uint256 or bytes16)."""
    from trezor.enums import EthereumDataType

    data_type = field.data_type
    size = field.size

    TYPE_TRANSLATION_DICT = {
        EthereumDataType.UINT: "uint",
        EthereumDataType.INT: "int",
        EthereumDataType.BYTES: "bytes",
        EthereumDataType.STRING: "string",
        EthereumDataType.BOOL: "bool",
        EthereumDataType.ADDRESS: "address",
    }

    if data_type == EthereumDataType.STRUCT:
        assert field.struct_name is not None  # validate_field_type
        return field.struct_name
    elif data_type == EthereumDataType.ARRAY:
        assert field.entry_type is not None  # validate_field_type
        type_name = get_type_name(field.entry_type)
        if size is None:
            return f"{type_name}[]"
        else:
            return f"{type_name}[{size}]"
    elif data_type in (EthereumDataType.UINT, EthereumDataType.INT):
        assert size is not None  # validate_field_type
        return TYPE_TRANSLATION_DICT[data_type] + str(size * 8)
    elif data_type == EthereumDataType.BYTES:
        if size:
            return TYPE_TRANSLATION_DICT[data_type] + str(size)
        else:
            return TYPE_TRANSLATION_DICT[data_type]
    else:
        # all remaining types can use the name directly
        # if the data_type is left out, this will raise KeyError
        return TYPE_TRANSLATION_DICT[data_type]

def decode_typed_data(data: bytes, type_name: str) -> str:
    """Used by sign_typed_data module to show data to user."""
    if type_name.startswith("bytes"):
        return hexlify(data).decode()
    elif type_name == "string":
        return data.decode()
    elif type_name == "address":
        return address_from_bytes(data)
    elif type_name == "bool":
        return "true" if data == b"\x01" else "false"
    elif type_name.startswith("uint"):
        return str(int.from_bytes(data, "big"))
    elif type_name.startswith("int"):
        # Micropython does not implement "signed" arg in int.from_bytes()
        return str(_from_bytes_bigendian_signed(data))

    raise ValueError  # Unsupported data type for direct field decoding


def _from_bytes_bigendian_signed(b: bytes) -> int:
    negative = b[0] & 0x80
    if negative:
        neg_b = bytearray(b)
        for i in range(len(neg_b)):
            neg_b[i] = ~neg_b[i] & 0xFF
        result = int.from_bytes(neg_b, "big")
        return -result - 1
    else:
        return int.from_bytes(b, "big")


def get_icon(chain_id: int | None):
    if chain_id is None:
        return "A:/res/evm-unkn.png"
    network: EthereumNetworkInfo | None = networks.by_chain_id(chain_id)
    if network and network is not networks.UNKNOWN_NETWORK:
        from trezor import io
        try:
            io.fs.stat(f"/res/{network.icon}")
        except:
            return "A:/res/evm-unkn.png"
        return f"A:/res/{network.icon}"
    else:
        return "A:/res/evm-unkn.png"


def get_display_network_name(network: EthereumNetworkInfo | None):
    if network is networks.UNKNOWN_NETWORK or network is None:
        return "EVM"
    # elif len(network.name) <= 8:
    #     return network.name
    else:
        return network.name

def store_definition(network: EthereumNetworkInfo, token: EthereumTokenInfo | None) -> bool:
    from trezor import io

    chain_id = network.chain_id
    # verify chain_id
    if token is not None and token.chain_id != chain_id:
        return False

    io.fs.mkdir(f"/user/ethereum/{chain_id}")

    # store network
    if not _store_network(network):
        return False

    if not token:
        return True

    return token and _store_token(token)

def _store_network(network: EthereumNetworkInfo) -> bool:
    from . import networks
    from trezor import io
    chain_id = network.chain_id

    if networks.is_buildin(chain_id):
        # buildin network, no need to store
        return True

    # store network
    path = f"/user/ethereum/{chain_id}/network.json"
    mata = {
        "chain_id": chain_id,
        "symbol": network.symbol,
        "name": network.name,
        "slip44": network.slip44,
    }
    data = json.dumps(mata)
    try:
        with io.fs.open(path, "w+") as f:
            f.truncate()
            f.write(data.encode())
    except:
        return False

    return True

def _store_token(token: EthereumTokenInfo) -> bool:
    from . import tokens
    from trezor import io
    chain_id = token.chain_id

    if tokens.is_buildin(chain_id, token.address):
        # buildin token, no need to store
        return True

    # store token
    address = address_from_bytes(token.address)
    path = f"/user/ethereum/{chain_id}/{address}.json"
    mata = {
        "address": address,
        "chain_id": chain_id,
        "symbol": token.symbol,
        "decimals": token.decimals,
        "name": token.name,
    }
    data = json.dumps(mata)
    try:
        with io.fs.open(path, "w+") as f:
            f.truncate()
            f.write(data.encode())
    except:
        return False

    return True

def get_network_from_fs(chain_id: int) -> EthereumNetworkInfo:
    from . import networks
    from trezor import io

    path = f"/user/ethereum/{chain_id}/network.json"
    try:
        with io.fs.open(path, "r") as f:
            data = bytearray(256)
            f.read(data)
            network: dict = json.loads(data)
            # verify items
            if not all(
                key in network and network[key] is not None
                for key in ("chain_id", "symbol", "name", "slip44")
            ):
                # invalid network data
                return networks.UNKNOWN_NETWORK

            return networks.EthereumNetworkInfo(
                chain_id=network["chain_id"],
                symbol=network["symbol"],
                name=network["name"],
                slip44=network["slip44"],
            )
    except Exception as e:
        return networks.UNKNOWN_NETWORK

def get_token_from_fs(chain_id: int, address: bytes) -> EthereumTokenInfo:
    from . import tokens
    from trezor import io
    address = address_from_bytes(address)
    path = f"/user/ethereum/{chain_id}/{address}.json"
    try:
        with io.fs.open(path, "r") as f:
            data = bytearray(256)
            f.read(data)
            token: dict = json.loads(data)
            # verify items
            if not all(
                key in token and token[key] is not None
                for key in ("address", "chain_id", "symbol", "decimals", "name")
            ):
                # invalid token data
                return tokens.UNKNOWN_TOKEN
            return tokens.EthereumTokenInfo(
                address=bytes_from_address(token["address"]),
                chain_id=token["chain_id"],
                name=token["name"],
                symbol=token["symbol"],
                decimals=token["decimals"],
            )
    except Exception as e:
        return tokens.UNKNOWN_TOKEN
