from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Protocol, Literal, Optional, List, Tuple, Any, Union, TypeVar
    # from .item import Item

    class URType(Protocol):
        def tag(self) -> int:
            """Tag of the type."""
            ...

        def type(self) -> str:
            """Type of the type."""
            ...

        def cbor(self) -> bytes:
            """Encode type to cbor bytes."""

        @classmethod
        def from_cbor(cls, cbor: bytes) -> 'URType':
            """Decode cbor object from bytes"""
