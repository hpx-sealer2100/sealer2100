"""
; An hd-key is either a master key or a derived key.
hd-key = {
    master-key / derived-key
}

; A master key is always private, has no use or derivation information,
; and always includes a chain code.
master-key = (
    is-master: true,
    key-data: key-data-bytes,
    chain-code: chain-code-bytes
)

; A derived key may be private or public, has an optional chain code, and
; may carry additional metadata about its use and derivation.
; To maintain isomorphism with [BIP32] and allow keys to be derived from
; this key `chain-code`, `origin`, and `parent-fingerprint` must be present.
; If `origin` contains only a single derivation step and also contains `source-fingerprint`,
; then `parent-fingerprint` MUST be identical to `source-fingerprint` or may be omitted.
derived-key = (
    ? is-private: bool .default false,   ; true if key is private, false if public
    key-data: key-data-bytes,
    ? chain-code: chain-code-bytes       ; omit if no further keys may be derived from this key
    ? use-info: #6.305(crypto-coininfo), ; How the key is to be used
    ? origin: #6.304(crypto-keypath),    ; How the key was derived
    ? children: #6.304(crypto-keypath),  ; What children should/can be derived from this
    ? parent-fingerprint: uint32 .ne 0,  ; The fingerprint of this key's direct ancestor, per [BIP32]
    ? name: text,                        ; A short name for this key.
    ? note: text                         ; An arbitrary amount of text describing the key.
)

; If the `use-info` field is omitted, defaults (mainnet BTC key) are assumed.
; If `cointype` and `origin` are both present, then per [BIP44], the second path
; component's `child-index` must match `cointype`.

; The `children` field may be used to specify what set of child keys should or can be derived from this key. This may include `child-index-range` or `child-index-wildcard` as its last component. Any components that specify hardened derivation will require the key be private.

is-master = 1
is-private = 2
key-data = 3
chain-code = 4
use-info = 5
origin = 6
children = 7
parent-fingerprint = 8
name = 9
note = 10

uint8 = uint .size 1
key-data-bytes = bytes .size 33
chain-code-bytes = bytes .size 32
"""

# Version 1 of crypto-hdkey types.
# Tags in version 1 not yet registered in IANA, and tag name is `Crypto-HDKey`.

# https://eips.ethereum.org/EIPS/eip-4527
# https://github.com/BlockchainCommons/Research/blob/3b36962000eb8a4c7afab5018a2e012dffeca1d2/papers/bcr-2020-007-hdkey.md

from .__typing import *

from .coininfo import CoinInfo
from .keypath import KeyPath
from apps.common.cbor import encode, decode, OrderedMap, Tagged

class HDKey(OrderedMap):
    def __init__(self):
        super().__init__()
        self.version: Literal[1, 2] = 1
        # master
        self.is_master: Optional[bool] = None
        self.key_data: Optional[bytes] = None
        self.chain_code: Optional[bytes] = None

        # derived
        self.is_private: Optional[bool] = None
        self.key_data: Optional[bytes] = None
        self.chain_code: Optional[bytes] = None
        self.use_info: Optional[CoinInfo] = None
        self.origin: Optional[KeyPath] = None
        self.children: Optional[KeyPath] = None
        self.parent_fingerprint: Optional[int] = None
        self.name: Optional[str] = None
        self.note: Optional[str] = None

    @classmethod
    def master(cls, key_data: bytes, chain_code: bytes):
        self = cls()
        self.is_master = True
        self.key_data = key_data
        self.chain_code = chain_code
        return self

    @classmethod
    def derived(
        cls,
        key_data: bytes,
        chain_code: bytes,
        origin: KeyPath,
        parent_fingerprint: int,
        private: bool = False,
        use_info: Optional[CoinInfo] = None,
        children: Optional[KeyPath] = None,
        name: Optional[str] = None,
        note: Optional[str] = None,

    ):
        self = cls()
        self.is_private = private
        self.key_data = key_data
        self.chain_code = chain_code
        self.use_info = use_info
        self.origin = origin
        self.children = children
        self.parent_fingerprint = parent_fingerprint
        self.name = name
        self.note = note
        return self

    # URType
    def tag(self) -> int:
        # 306 for version 1
        # 40306 for version 2
        tag = 306 if self.version == 1 else 40306
        return tag

    def type(self) -> str:
        # crypto-hdkey for version 1
        # hdkey for version 2
        type = "crypto-hdkey" if self.version == 1 else "hdkey"
        return type

    def build_master(self) -> None:
        # master-key
        # 1: is-master
        self[1] = True
        # 2: key-data-bytes
        self[3] = self.key_data
        # 4: chain-code-bytes
        self[4] = self.chain_code

    def build_driver(self) -> None:
        # derived-key
        # 2: is-private?
        self[2] = bool(self.is_private)
        # 3: key-data-bytes
        self[3] = self.key_data
        # 4: chain-code-bytes
        self[4] = self.chain_code
        # 5: use-info?
        if self.use_info is not None:
            self.use_info.build()
            self[5] = Tagged(self.use_info.tag(), self.use_info)
        # 6: origin
        self.origin.build()
        self[6] = Tagged(self.origin.tag(), self.origin)
        # 7: children?
        if self.children is not None:
            self.children.build()
            self[7] = Tagged(self.children.tag(), self.children)
        # 8: parent-fingerprint
        self[8] = self.parent_fingerprint
        # 9: name?
        if self.name:
            self[9] = self.name
        # 10: note?
        if self.note:
            self[10] = self.note

    def build(self) -> None:
        # rebuild
        self._internal_list.clear()
        if self.is_master:
            self.build_master()
        else:
            self.build_driver()

    def cbor(self) -> bytes:
        self.build()
        return encode(self)

    @classmethod
    def from_cbor(cls, cbor: bytes):
        obj = decode(cbor)
        return cls.from_object(obj)

    @classmethod
    def form_object(cls, obj: dict):
        if not isinstance(obj, dict):
            raise TypeError('Unexpected type, expected dict')
        # 1: is-master
        is_master = obj.get(1)
        if is_master is not None:
            if not isinstance(is_master, bool):
                raise TypeError('is-master must be bool')
            if is_master:
                # master-key
                return cls.__obj_to_master(obj)

        # derived-key
        return cls.__obj_to_driver(obj)

    @classmethod
    def __obj_to_master(cls, obj: dict):
        # 2: key-data-bytes
        key_data = obj.get(3)
        if not isinstance(key_data, bytes):
            raise TypeError('key-data-bytes must be bytes')
        # 4: chain-code-bytes
        chain_code = obj.get(4)
        if not isinstance(chain_code, bytes):
            raise TypeError('chain-code-bytes must be bytes')

        return cls.master(key_data, chain_code)

    @classmethod
    def __obj_to_driver(cls, obj: dict):
        # 2: is-private?
        is_private = obj.get(2)
        if is_private is not None:
            if not isinstance(is_private, bool):
                raise TypeError('is-private must be bool')
        # 3: key-data-bytes
        key_data = obj.get(3)
        if not isinstance(key_data, bytes):
            raise TypeError('key-data-bytes must be bytes')
        # 4: chain-code-bytes
        chain_code = obj.get(4)
        if not isinstance(chain_code, bytes):
            raise TypeError('chain-code-bytes must be bytes')

        # 5: use-info?
        use_info = obj.get(5)
        if use_info is not None:
            if not isinstance(use_info, Tagged):
                raise TypeError('use-info must be tagged')
            if use_info.tag != 305 or use_info.tag != 40305:
                raise TypeError('use-info tag must be 305 or 40305')
            version = 1 if use_info.tag == 305 else 2
            use_info = use_info.value
            use_info = CoinInfo.from_object(use_info)
            use_info.version = version

        # 6: origin
        origin = obj.get(6)
        if not isinstance(origin, Tagged):
            raise TypeError('origin must be tagged')
        if origin.tag != 304 or origin.tag != 40304:
            raise TypeError('origin tag must be 304 or 40304')
        version = 1 if origin.tag == 304 else 2
        origin = origin.value
        origin = KeyPath.from_object(origin)
        origin.version = version

        # 7: children?
        children = obj.get(7)
        if children is not None:
            if not isinstance(children, Tagged):
                raise TypeError('children must be tagged')
            if children.tag != 304 or children.tag != 40304:
                raise TypeError('children tag must be 304 or 40304')
            version = 1 if children.tag == 304 else 2
            children = children.value
            children = KeyPath.from_object(children)
            children.version = version

        # 8: parent-fingerprint
        parent_fingerprint = obj.get(8)
        if not isinstance(parent_fingerprint, int):
            raise TypeError('parent-fingerprint must be int')

        # 9: name?
        name = obj.get(9)
        if name is not None:
            if not isinstance(name, str):
                raise TypeError('name must be str')

        # 10: note?
        note = obj.get(10)
        if note is not None:
            if not isinstance(note, str):
                raise TypeError('note must be str')

        return cls.derived(
            key_data,
            chain_code,
            origin,
            parent_fingerprint,
            private=is_private,
            use_info=use_info,
            children=children,
            name=name,
            note=note,
        )
