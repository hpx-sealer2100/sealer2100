"""
eth-signature  = (
    request-id: uuid,
    signature: eth-signature-bytes,
    ? origin: text, ; The device info for providing this signature
)

request-id = 1
signature = 2
origin = 3

eth-signature-bytes = bytes .size 65; the signature of the signing request (r,s,v)
"""

from .__typing import *
from apps.common.cbor import encode, decode, OrderedMap, Tagged

class EthSignature(OrderedMap):
    def __init__(self,signature: bytes, request_id: Optional[bytes]=None, origin: Optional[str]=None):
        super().__init__()
        self.request_id = request_id
        self.signature = signature
        self.origin = origin

    # URType protocol
    def tag(self) -> int:
        # not found document
        raise NotImplementedError

    def type(self) -> int:
        return "eth-signature"

    def build(self):
        # rebuild
        self._internal_list.clear()

        # 1: #6.37 uuid
        if self.request_id:
            self[1] = Tagged(37, self.request_id)

        # 2: signature
        self[2] = self.signature

        # 3: origin
        if self.origin:
            self[3] = self.origin


    def cbor(self) -> bytes:
        self.build()
        return encode(self)

    @classmethod
    def from_cbor(cls, cbor: bytes):
        obj = decode(cbor)
        return cls.from_object(obj)
    @classmethod
    def from_object(cls, obj: dict):
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

        # 2: signature
        signature = obj.get(2)
        if not isinstance(signature, bytes):
            raise TypeError('signature must be bytes')

        # 3: origin?
        origin = obj.get(3)
        if origin is not None and not isinstance(origin, str):
                raise TypeError('origin must be str')

        return cls(signature, request_id, origin)
