from typing import TYPE_CHECKING

from .account_address import AccountAddress
from ..common.bcs import Deserializable, Serializable
from ..common.bcs import Deserializer, Serializer
from .type_tag import TypeTag

if TYPE_CHECKING:
    from __future__ import annotations
    from typing import Any, List

    pass


class RawTransaction(Deserializable, Serializable):
    # Sender's address
    sender: AccountAddress
    # Sequence number of this transaction. This must match the sequence number in the sender's
    # account at the time of execution.
    sequence_number: int
    # The transaction payload, e.g., a script to execute.
    payload: "TransactionPayload"
    # Maximum total gas to spend for this transaction
    max_gas_amount: int
    # Price to be paid per gas unit.
    gas_unit_price: int
    # Expiration timestamp for this transaction, represented as seconds from the Unix epoch.
    expiration_timestamps_secs: int
    # Chain ID of the Aptos network this transaction is intended for.
    chain_id: int

    def __init__(
        self,
        sender: AccountAddress,
        sequence_number: int,
        payload: "TransactionPayload",
        max_gas_amount: int,
        gas_unit_price: int,
        expiration_timestamps_secs: int,
        chain_id: int,
    ):
        self.sender = sender
        self.sequence_number = sequence_number
        self.payload = payload
        self.max_gas_amount = max_gas_amount
        self.gas_unit_price = gas_unit_price
        self.expiration_timestamps_secs = expiration_timestamps_secs
        self.chain_id = chain_id

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, RawTransaction):
            return NotImplemented
        return (
            self.sender == other.sender
            and self.sequence_number == other.sequence_number
            and self.payload == other.payload
            and self.max_gas_amount == other.max_gas_amount
            and self.gas_unit_price == other.gas_unit_price
            and self.expiration_timestamps_secs == other.expiration_timestamps_secs
            and self.chain_id == other.chain_id
        )

    def __str__(self):
        return f"""RawTransaction:
    sender: {self.sender}
    sequence_number: {self.sequence_number}
    payload: {self.payload}
    max_gas_amount: {self.max_gas_amount}
    gas_unit_price: {self.gas_unit_price}
    expiration_timestamps_secs: {self.expiration_timestamps_secs}
    chain_id: {self.chain_id}
"""

    @staticmethod
    def prehash() -> bytes:
        # hasher = sha3_256()
        # hasher.update(b"APTOS::RawTransaction")
        # return hasher.digest()
        # fmt:off
        digest = bytes(
            [
                0xb5, 0xe9, 0x7d, 0xb0, 0x7f, 0xa0, 0xbd, 0x0e, 0x55, 0x98, 0xaa, 0x36, 0x43, 0xa9, 0xbc, 0x6f,
                0x66, 0x93, 0xbd, 0xdc, 0x1a, 0x9f, 0xec, 0x9e, 0x67, 0x4a, 0x46, 0x1e, 0xaa, 0x00, 0xb1, 0x93,
            ]
        )
        # fmt:on
        return digest

    @staticmethod
    def deserialize(deserializer: Deserializer) -> RawTransaction:
        return RawTransaction(
            AccountAddress.deserialize(deserializer),
            deserializer.u64(),
            TransactionPayload.deserialize(deserializer),
            deserializer.u64(),
            deserializer.u64(),
            deserializer.u64(),
            deserializer.u8(),
        )

    def serialize(self, serializer: Serializer) -> None:
        self.sender.serialize(serializer)
        serializer.u64(self.sequence_number)
        self.payload.serialize(serializer)
        serializer.u64(self.max_gas_amount)
        serializer.u64(self.gas_unit_price)
        serializer.u64(self.expiration_timestamps_secs)
        serializer.u8(self.chain_id)


class TransactionPayload:
    SCRIPT: int = 0
    MODULE_BUNDLE: int = 1
    SCRIPT_FUNCTION: int = 2

    variant: int
    value: Any

    def __init__(self, payload: Any):
        if isinstance(payload, Script):
            self.variant = TransactionPayload.SCRIPT
        elif isinstance(payload, ModuleBundle):
            self.variant = TransactionPayload.MODULE_BUNDLE
        elif isinstance(payload, EntryFunction):
            self.variant = TransactionPayload.SCRIPT_FUNCTION
        else:
            raise Exception("Invalid type")
        self.value = payload

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, TransactionPayload):
            return NotImplemented
        return self.variant == other.variant and self.value == other.value

    def __str__(self) -> str:
        return self.value.__str__()

    @staticmethod
    def deserialize(deserializer: Deserializer) -> TransactionPayload:
        variant = deserializer.uleb128()

        if variant == TransactionPayload.SCRIPT:
            payload: Any = Script.deserialize(deserializer)
        elif variant == TransactionPayload.MODULE_BUNDLE:
            payload = ModuleBundle.deserialize(deserializer)
        elif variant == TransactionPayload.SCRIPT_FUNCTION:
            payload = EntryFunction.deserialize(deserializer)
        else:
            raise Exception("Invalid type")

        return TransactionPayload(payload)

    def serialize(self, serializer: Serializer) -> None:
        serializer.uleb128(self.variant)
        self.value.serialize(serializer)


class ModuleBundle:
    def __init__(self):
        raise NotImplementedError

    @staticmethod
    def deserialize(deserializer: Deserializer) -> ModuleBundle:
        raise NotImplementedError

    def serialize(self, serializer: Serializer) -> None:
        raise NotImplementedError


class Script:
    code: bytes
    ty_args: List[TypeTag]
    args: List[ScriptArgument]

    def __init__(self, code: bytes, ty_args: List[TypeTag], args: List[ScriptArgument]):
        self.code = code
        self.ty_args = ty_args
        self.args = args

    @staticmethod
    def deserialize(deserializer: Deserializer) -> Script:
        code = deserializer.to_bytes()
        ty_args = deserializer.sequence(TypeTag.deserialize)
        args = deserializer.sequence(ScriptArgument.deserialize)
        return Script(code, ty_args, args)

    def serialize(self, serializer: Serializer) -> None:
        serializer.to_bytes(self.code)
        serializer.sequence(self.ty_args, Serializer.struct)
        serializer.sequence(self.args, Serializer.struct)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Script):
            return NotImplemented
        return (
            self.code == other.code
            and self.ty_args == other.ty_args
            and self.args == other.args
        )

    def __str__(self):
        return f"<{self.ty_args}>({self.args})"


class ScriptArgument:
    U8: int = 0
    U64: int = 1
    U128: int = 2
    ADDRESS: int = 3
    U8_VECTOR: int = 4
    BOOL: int = 5
    U16: int = 6
    U32: int = 7
    U256: int = 8

    variant: int
    value: Any

    def __init__(self, variant: int, value: Any):
        if variant < 0 or variant > 5:
            raise Exception("Invalid variant")

        self.variant = variant
        self.value = value

    @staticmethod
    def deserialize(deserializer: Deserializer) -> ScriptArgument:
        variant = deserializer.u8()
        if variant == ScriptArgument.U8:
            value: Any = deserializer.u8()
        elif variant == ScriptArgument.U16:
            value = deserializer.u16()
        elif variant == ScriptArgument.U32:
            value = deserializer.u32()
        elif variant == ScriptArgument.U64:
            value = deserializer.u64()
        elif variant == ScriptArgument.U128:
            value = deserializer.u128()
        elif variant == ScriptArgument.U256:
            value = deserializer.u256()
        elif variant == ScriptArgument.ADDRESS:
            value = AccountAddress.deserialize(deserializer)
        elif variant == ScriptArgument.U8_VECTOR:
            value = deserializer.to_bytes()
        elif variant == ScriptArgument.BOOL:
            value = deserializer.bool()
        else:
            raise Exception("Invalid variant")
        return ScriptArgument(variant, value)

    def serialize(self, serializer: Serializer) -> None:
        serializer.u8(self.variant)
        if self.variant == ScriptArgument.U8:
            serializer.u8(self.value)
        elif self.variant == ScriptArgument.U16:
            serializer.u16(self.value)
        elif self.variant == ScriptArgument.U32:
            serializer.u32(self.value)
        elif self.variant == ScriptArgument.U64:
            serializer.u64(self.value)
        elif self.variant == ScriptArgument.U128:
            serializer.u128(self.value)
        elif self.variant == ScriptArgument.U256:
            serializer.u256(self.value)
        elif self.variant == ScriptArgument.ADDRESS:
            serializer.struct(self.value)
        elif self.variant == ScriptArgument.U8_VECTOR:
            serializer.to_bytes(self.value)
        elif self.variant == ScriptArgument.BOOL:
            serializer.bool(self.value)
        else:
            raise Exception(f"Invalid ScriptArgument variant {self.variant}")

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, ScriptArgument):
            return NotImplemented
        return self.variant == other.variant and self.value == other.value

    def __str__(self):
        return f"[{self.variant}] {self.value}"


class EntryFunction:
    module: ModuleId
    function: str
    ty_args: List[TypeTag]
    args: List[bytes]

    def __init__(
        self, module: ModuleId, function: str, ty_args: List[TypeTag], args: List[bytes]
    ):
        self.module = module
        self.function = function
        self.ty_args = ty_args
        self.args = args

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, EntryFunction):
            return NotImplemented

        return (
            self.module == other.module
            and self.function == other.function
            and self.ty_args == other.ty_args
            and self.args == other.args
        )

    def __str__(self):
        return f"{self.module}::{self.function}::<{self.ty_args}>({self.args})"

    @staticmethod
    def deserialize(deserializer: Deserializer) -> EntryFunction:
        module = ModuleId.deserialize(deserializer)
        function = deserializer.str()
        ty_args = deserializer.sequence(TypeTag.deserialize)
        args = deserializer.sequence(Deserializer.to_bytes)
        return EntryFunction(module, function, ty_args, args)

    def serialize(self, serializer: Serializer) -> None:
        self.module.serialize(serializer)
        serializer.str(self.function)
        serializer.sequence(self.ty_args, Serializer.struct)
        serializer.sequence(self.args, Serializer.to_bytes)


class ModuleId:
    address: AccountAddress
    name: str

    def __init__(self, address: AccountAddress, name: str):
        self.address = address
        self.name = name

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, ModuleId):
            return NotImplemented
        return self.address == other.address and self.name == other.name

    def __str__(self) -> str:
        return f"{self.address}::{self.name}"

    @staticmethod
    def from_str(module_id: str) -> ModuleId:
        split = module_id.split("::")
        return ModuleId(AccountAddress.from_str(split[0]), split[1])

    @staticmethod
    def deserialize(deserializer: Deserializer) -> ModuleId:
        addr = AccountAddress.deserialize(deserializer)
        name = deserializer.str()
        return ModuleId(addr, name)

    def serialize(self, serializer: Serializer) -> None:
        self.address.serialize(serializer)
        serializer.str(self.name)
