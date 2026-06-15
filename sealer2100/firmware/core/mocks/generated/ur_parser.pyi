from typing import *


# extmod/modairgapped/modairgapped.c
class ResultContext:
    """The context of the parsed result."""


# extmod/modairgapped/modairgapped.c
class EncodeResultContext:
    """The context of the encode result."""


# extmod/modairgapped/modairgapped.c
class DecoderContext:
    """The context of the decoder."""


# extmod/modairgapped/modairgapped.c
class EncoderContext:
    """The context of the encoder."""


# extmod/modairgapped/modairgapped.c
class URParseResult:
    def close(self) -> None:
        """Close the URParseResult object and free the memory."""
        ...
    def free(self) -> None:
        """Close the URParseResult object and free the memory."""
        ...
    def is_multi_part(self) -> bool:
        """Return True if the URParseResult object is multi-part."""
        ...
    def progress(self) -> int:
        """Return multi-part progress in [0, 100]"""
        ...
    def t(self) -> int:
        """The `ViewType` of the URParseResult object."""
        ...
    def ur_type(self) -> int:
        """The `QRCodeType` of the URParseResult object."""
        ...
    def data(self) -> 'ResultContext':
        """The parsed result.
        """
    def decoder(self) -> 'DecoderContext':
        """The decoder used to parse the UR."""
        ...
    def error_code(self) -> int:
        """The error code of the URParseResult object."""
        ...
    def error_message(self) -> str:
        """The error message of the URParseResult object."""
        ...


# extmod/modairgapped/modairgapped.c
class URParseMultiResult:
    def close(self) -> None:
        """Close the URParseMultiResult object and free the memory."""
        ...
    def free(self) -> None:
        """Close the URParseMultiResult object and free the memory."""
        ...
    def is_complete(self) -> bool:
        """Return True if the URParseMultiResult object is complete."""
        ...
    def t(self) -> int:
        """The `ViewType` of the URParseMultiResult object."""
        ...
    def ur_type(self) -> int:
        """The `QRCodeType` of the URParseMultiResult object."""
        ...
    def progress(self) -> int:
        """Return multi-part progress in [0, 100]"""
        ...
    def data(self) -> 'ResultContext':
        """The parsed result."""
        ...
    def error_code(self) -> int:
        """The error code of the parsed result."""
        ...
    def error_message(self) -> str:
        """The error message of the parsed result."""
        ...


# extmod/modairgapped/modairgapped.c
def parse_ur(ur: str) -> URParseResult:
    """Parse the UR code."""
    ...


# extmod/modairgapped/modairgapped.c
def receive(ur: str, decoder: DecoderContext) -> URParseMultiResult:
    """Receive multi-part UR code."""
    ...


# extmod/modairgapped/modairgapped.c
class UREncodeResult:
    """The result of encoding UR code."""
    def close(self) -> None:
        """Close the encode result and free the memory."""
        ...
    def free(self) -> None:
        """Free the memory of the encode result."""
        ...
    def is_multi_part(self) -> bool:
        """Check if the encode result is multi-part."""
        ...
    def data(self) -> EncodeResultContext:
        """The encoder context of the encode result."""
        ...
    def encoder(self) -> EncoderContext:
        """The encoder context of the encode result."""
        ...
    def error_code(self) -> int:
        """The error code of the encode result."""
        ...
    def error_message(self) -> str:
        """The error message of the encode result."""
        ...


# extmod/modairgapped/modairgapped.c
class UREncodeMultiResult:
    """The result of encoding UR code."""
    def close(self) -> None:
        """Close the encode result and free the memory."""
        ...
    def free(self) -> None:
        """Free the memory of the encode result."""
        ...
    def data(self) -> EncodeResultContext:
        """The data of the encode result."""
        ...
    def error_code(self) -> int:
        """The error code of the encode result."""
        ...
    def error_message(self) -> str:
        """The error message of the encode result."""
        ...


# extmod/modairgapped/modairgapped.c
def get_next_part(self) -> UREncodeMultiResult:
    """Get the next part of the encode result."""
    ...


# extmod/modairgapped/modairgapped.c
def get_connect_metamask_ur(mfp: int, public_keys: list, origin: str) -> UREncodeMultiResult:
    """Get the connect metamask UR code."""
    ...


# extmod/modairgapped/modairgapped.c
def get_connect_hpx_ur(mfp: int, public_keys: list, origin: str, *, device_id: str, label: str, sn: str, passphrase: bool) -> UREncodeMultiResult:
    """Get the connect HPX UR code."""
    ...


# extmod/modairgapped/modairgapped.c
class EthParsedRaw:
    """The parsed Ethereum transaction."""
    def close(self):
        """Close the parsed Ethereum transaction and free the memory."""
        ...
    def free(self):
        """Free the parsed Ethereum transaction and close the memory."""
        ...
    def value(self) -> int:
        """The value of the transaction."""
        ...
    def max_txn_fee(self) -> int:
        """The maximum transaction fee of the transaction."""
        ...
    def gas_price(self) -> int:
        """The gas price of the transaction."""
        ...
    def gas_limit(self) -> int:
        """The gas limit of the transaction."""
        ...
    def to(self) -> str:
        """The receiver of the transaction."""
        ...
    def nonce(self) -> int:
        """The nonce of the transaction."""
        ...
    def input(self) -> str:
        """The input of the transaction."""
        ...
    def max_fee(self) -> str:
        """The maximum transaction fee of the transaction."""
        ...
    def max_priority(self) -> str:
        """The maximum priority of the transaction."""
        ...
    def max_fee_per_gas(self) -> str:
        """The maximum fee per gas of the transaction."""
        ...
    def max_priority_fee_per_gas(self) -> str:
        """The maximum priority fee per gas of the transaction."""
        ...
    def chain_id(self) -> int:
        """The chain ID of the transaction."""
        ...
    def tx_type(self) -> int:
        """The transaction type of the transaction."""
        ...
    def error_code(self) -> int:
        """The error code of the transaction."""
        ...
    def error_message(self) -> str:
        """The error message of the transaction."""
        ...


# extmod/modairgapped/modairgapped.c
def eth_parse_raw(ur: ResultContext, xpub: str) -> EthParsedRaw:
    """Parse the Ethereum transaction."""
    ...


# extmod/modairgapped/modairgapped.c
def eth_check(ur: ResultContext, mfp: int) -> int:
    """Check the Ethereum transaction."""
    ...


# extmod/modairgapped/modairgapped.c
class EthParsedPersonalMessage:
    """The parsed Ethereum personal message."""
    def close(self) -> None:
        """Close the parsed Ethereum personal message and free the memory."""
        ...
    def free(self) -> None:
        """Free the parsed Ethereum personal message."""
        ...
    def raw_message(self) -> str:
        """Get the raw message."""
        ...
    def utf8_message(self) -> str:
        """Get the UTF-8 message."""
        ...
    def error_code(self) -> int:
        """Get the error code."""
        ...
    def error_message(self) -> str:
        """Get the error message."""
        ...


# extmod/modairgapped/modairgapped.c
def eth_parse_personal_message(ur: ResultContext, xpub: str) -> EthParsedPersonalMessage:
    """Parse the Ethereum personal message."""
    ...


# extmod/modairgapped/modairgapped.c
class EthParsedTypedData:
    """The parsed Ethereum typed data."""
    def close(self) -> None:
        """Close the parsed Ethereum typed data and free the memory."""
        ...
    def free(self) -> None:
        """Free the parsed Ethereum typed data."""
        ...
    def name(self) -> str:
        """Get the name of the typed data."""
        ...
    def version(self) -> str:
        """Get the version of the typed data."""
        ...
    def chain_id(self) -> int:
        """Get the chain ID of the typed data."""
        ...
    def verifying_contract(self) -> str:
        """Get the verifying contract of the typed data."""
        ...
    def salt(self) -> str:
        """Get the salt of the typed data."""
        ...
    def primary_type(self) -> str:
        """Get the primary type of the typed data."""
        ...
    def message(self) -> str:
        """Get the UTF-8 message of the typed data."""
        ...
    def domain_hash(self) -> str:
        """Get the domain hash of the typed data."""
        ...
    def message_hash(self) -> str:
        """Get the message hash of the typed data."""
        ...
    def safe_tx_hash(self) -> str:
        """Get the safe transaction hash of the typed data."""
        ...
    def error_code(self) -> int:
        """Get the error code of the typed data."""
        ...
    def error_message(self) -> str:
        """Get the error message of the typed data."""
        ...
    def raw_message(self) -> str:
        """Get the raw message of the typed data."""
        ...


# extmod/modairgapped/modairgapped.c
def eth_parse_typed_data(ur: ResultContext, xpub: str) -> EthParsedTypedData:
    """Parse the Ethereum typed data."""
    ...


# extmod/modairgapped/modairgapped.c
def eth_ur_encode_signature(ur: EncodeResultContext, signature: str, origin: str) -> UREncodeResult:
    """Encode the signature signature."""
    ...


# extmod/modairgapped/modairgapped.c
def eth_get_current_ur_path(ur: ResultContext) -> str:
    """Get the current UR path."""
    ...


# extmod/modairgapped/modairgapped.c
def hpx_app_call_get_payload(ur: ResultContext) -> str:
    """Get the payload of the HPX app call."""
    ...


# extmod/modairgapped/modairgapped.c
def eth_sign_tx(ur: ResultContext, seed: bytes) -> UREncodeResult:
    """Sign the transaction."""
    ...


# extmod/modairgapped/modairgapped.c
def solana_check(ur: ResultContext, mfp: int) -> int:
    """Check the Solana transaction."""
    ...


# extmod/modairgapped/modairgapped.c
def solana_get_path(ur: ResultContext) -> str:
    """Get the Solana path."""
    ...


# extmod/modairgapped/modairgapped.c
def solana_parse_tx_raw(ur: ResultContext) -> str:
    """Parse the Solana transaction raw."""
    ...


# extmod/modairgapped/modairgapped.c
def solana_ur_encode_signature(ur: EncodeResultContext, signature: bytes) -> UREncodeResult:
    """Encode the signature."""
    ...


# extmod/modairgapped/modairgapped.c
class DisplayTxDetailOutput:
    """Display the BTC output detail."""
    def address(self) -> str:
        """Get the address."""
        ...
    def amount(self) -> str:
        """Get the amount."""
        ...
    def is_mine(self) -> bool:
        """Get the is mine."""
        ...
    def is_external(self) -> bool:
        """Get the is external."""
        ...


# extmod/modairgapped/modairgapped.c
class DisplayTxDetailInput:
    """Display the BTC input detail."""
    def address(self) -> str:
        """Get the address."""
        ...
    def amount(self) -> str:
        """Get the amount."""
        ...
    def is_mine(self) -> bool:
        """Get the is mine."""
        ...
    def is_external(self) -> bool:
        """Get the is external."""
        ...


# extmod/modairgapped/modairgapped.c
class DisplayTxDetail:
    """Display the BTC transaction detail."""
    def total_input_amount(self) -> str:
        """Get the total input amount."""
        ...
    def total_output_amount(self) -> str:
        """Get the total output amount."""
        ...
    def fee_amount(self) -> str:
        """Get the fee amount."""
        ...
    def inputs(self) -> list[DisplayTxDetailInput]:
        """Get the inputs."""
        ...
    def outputs(self) -> list[DisplayTxDetailOutput]:
        """Get the outputs."""
        ...
    def network(self) -> str:
        """Get the network."""
        ...
    def total_input_sat(self) -> str:
        """Get the total input satoshis."""
        ...
    def total_output_sat(self) -> str:
        """Get the total output satoshis."""
        ...
    def fee_sat(self) -> str:
        """Get the fee satoshis."""
        ...
    def sign_status(self) -> str:
        """Get the sign status."""
        ...
    def error_code(self) -> int:
        """Get the error code."""
        ...
    def error_message(self) -> str:
        """Get the error message."""
        ...


# extmod/modairgapped/modairgapped.c
def btc_parse_psbt(ur_in: ResultContext, mfp: int, public_keys: list[tuple[str, str]]) -> DisplayTxDetail:
    """Parse the PSBT."""
    ...


# extmod/modairgapped/modairgapped.c
def btc_check_psbt(ur_in: ResultContext, mfp: int, public_keys: list[tuple[str, str]]) -> bool:
    """Check the PSBT."""
    ...


# extmod/modairgapped/modairgapped.c
def btc_sign_psbt(ur_in: ResultContext, seed: bytes, mfp: int) -> str:
    """Sign the PSBT."""
    ...


# extmod/modairgapped/modairgapped.c
def tron_parse_keystone_path(ur_in: ResultContext) -> str:
    """Parse the keystone path."""
    ...


# extmod/modairgapped/modairgapped.c
def tron_check_keystone(ur_in: ResultContext, mfp: int, xpub: str) -> bool:
    """Check the keystone."""
    ...


# extmod/modairgapped/modairgapped.c
def tron_parse_keystone_raw(ur_in: ResultContext) -> str:
    """Parse the keystone raw."""
    ...


# extmod/modairgapped/modairgapped.c
def tron_ur_encode_signature(ur_in: ResultContext, signature: str) -> EncodeResult:
    """Encode the signature."""
    ...
