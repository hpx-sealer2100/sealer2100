"""
; Metadata for the complete or partial derivation path of a key.
;
; `source-fingerprint`, if present, is the fingerprint of the
; ancestor key from which the associated key was derived.
;
; If `components` is empty, then `source-fingerprint` MUST be a fingerprint of
; a master key.
;
; `depth`, if present, represents the number of derivation steps in
; the path of the associated key, regardless of whether steps are present in the `components` element
; of this structure.

crypto-keypath = {
    components: [path-component], ; If empty, source-fingerprint MUST be present
    ? source-fingerprint: uint32 .ne 0 ; fingerprint of ancestor key, or master key if components is empty
    ? depth: uint8 ; 0 if this is a public key derived directly from a master key
}

path-component = (
    child-index-component /     ; A single child, possibly hardened
    child-range-component /		; A specific range of children, all possibly hardened
    child-wildcard-component /  ; An inspecific range of children, all possibly hardened
    child-pair-component        ; Used in output descriptors,
                                ; see https://github.com/bitcoin/bitcoin/pull/22838
)

uint32 = uint .size 4
uint31 = uint32 .lt 0x80000000
child-index-component = (child-index, is-hardened)
child-range-component = ([child-index, child-index], is-hardened) ; [low, high] where low < high
child-wildcard-component = ([], is-hardened)
child-pair-component = [
    child-index-component,	; Child to use for external addresses, possibly hardened
    child-index-component	; Child to use for internal addresses, possibly hardened
]

child-index = uint31
is-hardened = bool

components = 1
source-fingerprint = 2
depth = 3
"""

from .__typing import *
from apps.common.cbor import encode, OrderedMap

class KeyPath(OrderedMap):

    def __init__(
        self,
        components: List["PathComponent"],
        source_fingerprint: Optional[int] = None,
        depth: Optional[int] = None,
    ):
        super().__init__()
        self.version: Literal[1, 2] = 1
        self.components = components
        self.source_fingerprint = source_fingerprint
        self.depth = depth

    @property
    def path(self) -> str:
        components = (str(c) for c in components)
        return '/'.join(components)


    # URType
    def tag(self) -> int:
        # 304 for version 1
        # 40304 for version 2
        tag = 304 if self.version == 1 else 40304
        return tag

    def type(self) -> str:
        # crypto-keypath for version 1
        # keypath for version 2
        type = "crypto-keypath" if self.version == 1 else "keypath"
        return type

    def build(self) -> None:
        # rebuild
        self._internal_list.clear()

        # 1: components
        components = [item for c in self.components for item in c.items()]
        self[1] = components

        # 2: source-fingerprint?
        if self.source_fingerprint is not None:
            self[2] = self.source_fingerprint
        # 3: depth?
        if self.depth is not None:
            self[3] = self.depth

    def cbor(self) -> bytes:
        self.build()
        return encode(self)

    @classmethod
    def from_cbor(cls, cbor: bytes):
        return cls.from_object(encode(cbor))

    @classmethod
    def from_object(cls, obj: dict):
        if not isinstance(obj, dict):
            raise TypeError('Unexpected type, expected dict')

        # 1: components
        components = obj.get(1)
        if not isinstance(components, list):
            raise TypeError('components must be list')
        components = PathComponent.from_list(components)

        # 2: source-fingerprint?
        source_fingerprint = obj.get(2)
        # may be None
        if source_fingerprint is not None and not isinstance(source_fingerprint, int):
            raise TypeError('source-fingerprint must be int')

        # 3: depth?
        depth = obj.get(3)
        # may be None
        if depth is not None and not isinstance(depth, int):
            raise TypeError('depth must be int')

        return cls(components, source_fingerprint, depth)

class PathComponent:
    def __init__(self, index: Optional[int] = None, hardened: bool = False):
        self.index = index
        self.hardened = hardened

    @property
    def wildcard(self) -> bool:
        return self.index is None

    @classmethod
    def from_tuple(cls, item: Tuple[int, bool]):
        index, hardened = item
        if not isinstance(index, int):
            raise TypeError('index must be int')
        if not isinstance(hardened, bool):
            raise TypeError('hardened must be bool')
        return cls(index, hardened)

    @staticmethod
    def from_list(items: List[Tuple[int, bool]]):
        paired = all(isinstance(item, tuple) for item in items)
        # user can pass paired items
        if paired:
            return [PathComponent.from_tuple(item) for item in items]

        # or unpaired
        # [44, true, 60, true, 0, true]
        if len(items) % 2 != 0:
            raise ValueError('items must be paired')

        def chunks():
            for i in range(0, len(items), 2):
                yield items[i : i + 2]

        return [PathComponent.from_tuple((index, hardened)) for index, hardened in chunks()]

    @staticmethod
    def parser_path(path: str) -> List['PathComponent']:
        items = path.split('/')
        if items[0] == 'm':
            items.remove('m')

        def convert(item: str) -> PathComponent:
            hardened = item.endswith("'")
            if hardened:
                item = item[:-1]
            if item == '*':
                index = None
            else:
                index = int(item)
            return PathComponent(index, hardened)
        return list(convert(item) for item in items)

    def items(self) -> Tuple[Union[list, int], bool]:
        if self.wildcard:
            return [], self.hardened
        return self.index, self.hardened

    def __str__(self):
        if self.wildcard:
            return "*"
        if self.hardened:
            return f"{self.index}'"
        return str(self.index)

