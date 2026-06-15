#
# utils.py
#
# Copyright © 2020 Foundation Devices, Inc.
# Licensed under the "BSD-2-Clause Plus Patent License"
#

from .crc32 import crc32, crc32n, bit_length
import binascii

def crc32_bytes(buf):
    checksum = crc32n(buf)
    return checksum

def crc32_int(buf):
    return crc32(buf)

def data_to_hex(buf):
    return ''.join('{:02x}'.format(x) for x in buf)

def int_to_bytes(n):
    # return n.to_bytes((n.bit_length() + 7) // 8, 'big')
    c = (bit_length(n) + 7) // 8
    return n.to_bytes(c, 'big')

def bytes_to_int(buf):
    return int.from_bytes(buf, 'big')

def string_to_bytes(s):
    return bytes(s, 'utf8')

def is_ur_type(ch):
    if 'a' <= ch and ch <= 'z':
         return True
    if '0' <= ch and ch <= '9':
        return True
    if ch == '-':
        return True
    return False

def partition(s, n):
    for i in range(0, len(s), n):
        yield s[i:i + n]
    # return [s[i:i+n] for i in range(0, len(s), n)]

# Split the given sequence into two parts returned in a tuple
# The first entry in the tuple has the first `count` values.
# The second entry in the tuple has the remaining values.
def split(buf, count):
    return (buf[0:count], buf[count:])

def join_lists(lists):
    # return [y for x in lists for y in x]
    return sum(lists, [])

def join_bytes(list_of_ba):
    out = bytearray()
    for ba in list_of_ba:
        out.extend(ba)
    return out

def xor_into(target, source):
    count = len(target)
    assert(count == len(source)) # Must be the same length
    for i in range(count):
        target[i] ^= source[i]

def xor_with(a, b):
    target = a
    xor_into(target, b)
    return target

def take_first(s, count):
    return s[0:count]

def drop_first(s, count):
    return s[count:]




MAX_BITS_256 = 32  # 256 bits
def string_to_256bitBE(value):
    if not value:
        raise ValueError("invalid decimal string")

    buf = bytearray(MAX_BITS_256)
    length = 0

    for ch in value:
        if ch < '0' or ch > '9':
            raise ValueError("invalid decimal string")

        digit = ord(ch) - 48
        carry = digit

        i = MAX_BITS_256 - 1
        end = MAX_BITS_256 - length
        while i >= end:
            val = buf[i] * 10 + carry
            buf[i] = val & 0xFF
            carry = val >> 8
            i -= 1

        if carry:
            if length == MAX_BITS_256:
                raise ValueError("overflow > 256bit")
            buf[MAX_BITS_256 - 1 - length] = carry
            length += 1
        elif length == 0 and digit != 0:
            length = 1

    if length == 0:
        return b"\x00"

    start = MAX_BITS_256 - length
    while length > 1 and buf[start] == 0:
        start += 1
        length -= 1

    return bytes(buf[start:start + length])


def uint32_to_256bitBE(value):
    if value < 0 or value > 0xFFFFFFFF:
        raise ValueError("uint32 out of range")
    return string_to_256bitBE(str(value))



def hex_to_bytes(s):
    return binascii.unhexlify(s)

def bytes_to_hex(b):
    return binascii.hexlify(b).decode('utf-8')


def bit_length(n: int) -> int:
    """
    return bit count
    """
    return len(bin(n)) - 2

def byte_length(n: int) -> int:
    """
    return byte count
    """

    return (bit_length(n) + 7) // 8
