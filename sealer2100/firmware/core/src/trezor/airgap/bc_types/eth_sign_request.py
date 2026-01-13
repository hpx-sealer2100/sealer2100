"""
; Metadata for the signing request for Ethereum.
;
sign-data-type = {
    type: int .default 1 transaction data; the unsigned data type
}

eth-transaction-data = 1; legacy transaction rlp encoding of unsigned transaction data
eth-typed-data = 2; EIP-712 typed signing data
eth-raw-bytes=3;   for signing message usage, like EIP-191 personal_sign data
eth-typed-transaction=4; EIP-2718 typed transaction of unsigned transaction data

; Metadata for the signing request for Ethereum.
; request-id: the identifier for this signing request.
; sign-data: the unsigned data
; data-type: see sign-data-type definition
; chain-id: chain id definition see https://github.com/ethereum-lists/chains for detail
; derivation-path: the key path of the private key to sign the data
; address: Ethereum address of the signing type for verification purposes which is optional

eth-sign-request = (
    sign-data: sign-data-bytes, ; sign-data is the data to be signed by offline signer, currently it can be unsigned transaction or typed data
    data-type: #3.401(sign-data-type),
    chain-id: int .default 1,
    derivation-path: #5.304(crypto-keypath), ;the key path for signing this request
    ?request-id: uuid, ; the uuid for this signing request
    ?address: eth-address-bytes,            ;verification purpose for the address of the signing key
    ?origin: text  ;the origin of this sign request, like wallet name
)
request-id = 1
sign-data = 2
data-type = 3
chain-id = 4 ;it will be the chain id of ethereum related blockchain
derivation-path = 5
address = 6
origin = 7
eth-address-bytes = bytes .size 20
sign-data-bytes = bytes ; for unsigned transactions it will be the rlp encoding for unsigned transaction data and ERC 712 typed data it will be the bytes of json string.
"""

from .__typing import *
from .keypath import KeyPath
from apps.common.cbor import encode, decode, OrderedMap, Tagged

# legacy transaction
ETH_TRANSACTION_DATA = 1
# eip-712 message
ETH_TYPED_DATA = 2
# eip-191 personal
ETH_RAW_BYTES = 3
# typed transaction, eip-2718
ETH_TYPED_TRANSACTION = 4


class EthSignRequest(OrderedMap):
    def __init__(
        self,
        sign_data: bytes,
        data_type: int,
        derivation_path: KeyPath,
        chain_id: int = 1,
        request_id: Optional[bytes]=None,
        address: Optional[bytes]=None,
        origin: Optional[str]=None,
    ):
        super().__init__()
        self.sign_data = sign_data
        self.data_type = data_type
        self.chain_id = chain_id
        self.derivation_path = derivation_path
        self.request_id = request_id
        self.address = address
        self.origin = origin

    # URType protocol
    def tag(self) -> int:
        # not found document
        raise NotImplementedError

    def type(self) -> int:
        return 'eth-sign-request'

    def build(self) -> None:
        # rebuild
        self._internal_list.clear()

        # 1: #6.37 uuid?
        if self.request_id:
            self[1] = Tagged(37, self.request_id)

        # 2: sign-data
        self[2] = self.sign_data

        # 3: data-type
        self[3] = self.data_type

        # 4: chain-id
        self[4] = self.chain_id

        # 5: derivation-path
        self.derivation_path.build()
        self[5] = Tagged(self.derivation_path.tag(), self.derivation_path)

        # 6: address?
        if self.address:
            self[6] = self.address

        # 7: origin?
        if self.origin:
            self[7] = self.origin

    def cbor(self) -> bytes:
        self.build()
        return encode(self)

    @classmethod
    def from_cbor(cls, cbor: bytes) -> 'EthSignRequest':
        obj = decode(cbor)
        return cls.from_object(obj)

    @classmethod
    def from_object(cls, obj: dict) -> 'EthSignRequest':
        if not isinstance(obj, dict):
            raise TypeError('Unexpected type, expected dict')

        # 1: #6.37 uuid?
        request_id = obj.get(1)

        if request_id is not None:
            if not isinstance(request_id, Tagged) or request_id.tag != 37:
                raise TypeError('request-id must be #6.37 uuid')
            request_id = request_id.value
            if not isinstance(request_id, bytes):
                raise TypeError('request-id must be bytes')

        # 2: sign-data
        sign_data = obj.get(2)
        if not isinstance(sign_data, bytes):
            raise TypeError('sign-data must be bytes')

        # 3: data-type
        data_type = obj.get(3)
        if not isinstance(data_type, int):
            raise TypeError('data-type must be int')
        if data_type > ETH_TYPED_TRANSACTION or data_type < ETH_TRANSACTION_DATA:
            raise TypeError(f'Unexpected data-type: {data_type}')

        # 4: chain-id
        chain_id = obj.get(4)
        if not isinstance(chain_id, int):
            raise TypeError('chain-id must be int')

        # 5: derivation-path
        derivation_path = obj.get(5)
        if not isinstance(derivation_path, Tagged) or derivation_path.tag != 304:
            raise TypeError('derivation-path must be #5.304(crypto-keypath)')
        derivation_path = derivation_path.value
        derivation_path = KeyPath.from_object(derivation_path)

        # 6: address?
        address = obj.get(6)
        if address is not None:
            if not isinstance(address, bytes):
                raise TypeError('address must be bytes')
            if len(address) != 20:
                raise TypeError('address must be 20 bytes')

        # 7: origin?
        origin = obj.get(7)
        if origin is not None and not isinstance(origin, str):
            raise TypeError('origin must be string')

        return cls(sign_data, data_type, derivation_path, chain_id, request_id, address, origin)
