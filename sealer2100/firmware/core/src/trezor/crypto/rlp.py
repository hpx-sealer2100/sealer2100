from micropython import const
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from trezor.utils import Writer

    # The intention below is basically:
    # RLPItem = int | bytes | list[RLPItem]
    # That will not typecheck though. Type `list` is invariant in its parameter, meaning
    # that we cannot pass list[bytes] into a list[RLPItem] parameter (what if the
    # function wanted to append an int?). We do want to enforce that it's a `list`, not
    # a generic `Sequence` (because we do isinstance checks for a list). We are however
    # only reading from the list and passing into things that consume a RLPItem. Hence
    # we have to enumerate single-type lists as well as the universal list[RLPItem].
    RLPList = list[int] | list[bytes] | list["RLPItem"]
    RLPItem = RLPList | bytes | int


STRING_HEADER_BYTE = const(0x80)
LIST_HEADER_BYTE = const(0xC0)


def _byte_size(x: int) -> int:
    if x < 0:
        raise ValueError  # only unsigned ints are supported
    for exp in range(64):
        if x < 0x100**exp:
            return exp

    raise ValueError  # int is too large


def int_to_bytes(x: int) -> bytes:
    return x.to_bytes(_byte_size(x), "big")

def bytes_to_int(value: bytes) -> int:
    return int.from_bytes(value, "big")

def write_header(
    w: Writer,
    length: int,
    header_byte: int,
    data_start: bytes | None = None,
) -> None:
    if length == 1 and data_start is not None and data_start[0] <= 0x7F:
        # no header when encoding one byte below 0x80
        pass

    elif length <= 55:
        w.append(header_byte + length)

    else:
        encoded_length = int_to_bytes(length)
        w.append(header_byte + 55 + len(encoded_length))
        w.extend(encoded_length)


def header_length(length: int, data_start: bytes | None = None) -> int:
    if length == 1 and data_start is not None and data_start[0] <= 0x7F:
        # no header when encoding one byte below 0x80
        return 0

    if length <= 55:
        return 1

    return 1 + _byte_size(length)


def length(item: RLPItem) -> int:
    data: bytes | None = None
    if isinstance(item, int):
        data = int_to_bytes(item)
        item_length = len(data)
    elif isinstance(item, (bytes, bytearray)):
        data = item
        item_length = len(item)
    elif isinstance(item, list):
        item_length = sum(length(i) for i in item)
    else:
        raise TypeError

    return header_length(item_length, data) + item_length


def write_string(w: Writer, string: bytes) -> None:
    write_header(w, len(string), STRING_HEADER_BYTE, string)
    w.extend(string)


def write_list(w: Writer, lst: RLPList) -> None:
    payload_length = sum(length(item) for item in lst)
    write_header(w, payload_length, LIST_HEADER_BYTE)
    for item in lst:
        write(w, item)


def write(w: Writer, item: RLPItem) -> None:
    if isinstance(item, int):
        write_string(w, int_to_bytes(item))
    elif isinstance(item, (bytes, bytearray)):
        write_string(w, item)
    elif isinstance(item, list):
        write_list(w, item)
    else:
        raise TypeError

SHORT_STRING = 128 + 56
def consume_length_prefix(rlp, start):
    b0 = rlp[start]
    if b0 < 128:  # single byte
        return (b"", bytes, 1, start)
    elif b0 < SHORT_STRING:  # short string
        if b0 - 128 == 1 and rlp[start + 1] < 128:
            raise ValueError("Encoded as short string although single byte was possible")
        return (rlp[start : start + 1], bytes, b0 - 128, start + 1)
    elif b0 < 192:  # long string
        ll = b0 - 183  # - (128 + 56 - 1)
        if rlp[start + 1 : start + 2] == b"\x00":
            raise ValueError("Length starts with zero bytes")
        len_prefix = rlp[start + 1 : start + 1 + ll]
        l = bytes_to_int(len_prefix)  # noqa: E741
        if l < 56:
            raise ValueError("Long string prefix used for short string")
        return (rlp[start : start + 1] + len_prefix, bytes, l, start + 1 + ll)
    elif b0 < 192 + 56:  # short list
        return (rlp[start : start + 1], list, b0 - 192, start + 1)
    else:  # long list
        ll = b0 - 192 - 56 + 1
        if rlp[start + 1 : start + 2] == b"\x00":
            raise ValueError("Length starts with zero bytes")
        len_prefix = rlp[start + 1 : start + 1 + ll]
        l = bytes_to_int(len_prefix)  # noqa: E741
        if l < 56:
            raise ValueError("Long list prefix used for short list")
        return (rlp[start : start + 1] + len_prefix, list, l, start + 1 + ll)


def consume_payload(rlp, prefix, start, type_, length):
    if type_ is bytes:
        item = rlp[start : start + length]
        return (item,  start + length)
    elif type_ is list:
        items = []
        next_item_start = start
        end = next_item_start + length
        while next_item_start < end:
            p, t, l, s = consume_length_prefix(rlp, next_item_start)
            item, next_item_start = consume_payload(rlp, p, s, t, l)
            # When the item returned above is a single element, item_rlp will also
            # contain a single element, but when it's a list, the first element will be
            # the RLP of the whole List, which is what we want here.
            items.append(item)
        if next_item_start > end:
            raise ValueError( "List length prefix announced a too small " "length")
        return (items, next_item_start)
    else:
        raise TypeError("Type must be either list or bytes")


def consume_item(rlp, start):
    p, t, l, s = consume_length_prefix(rlp, start)
    return consume_payload(rlp, p, s, t, l)

def _decode(item):
    try:
        result, end = consume_item(item, 0)
    except IndexError:
        raise ValueError("RLP string too short")
    if end != len(item):
        msg = f"RLP string ends with {len(item) - end} superfluous bytes"
        raise ValueError(msg)
    return result

def decode(rlp: bytes, offset: int = 0) -> RLPItem:
    item = _decode(rlp[offset:])
    return item
