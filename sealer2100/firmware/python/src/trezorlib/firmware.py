# This file is part of the Trezor project.
#
# Copyright (C) 2012-2022 SatoshiLabs and contributors
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the License along with this library.
# If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.

import hashlib
from enum import Enum
from typing import Any, List, Tuple

import construct as c

from . import _ed25519

# production ed25519 public keys
KEYS = [
    bytes.fromhex(k) for k in [
        "5a4b5aa99b2979560d6ef8746ac25ac67d41ec32f718e5405c1b7e4060d79d8a",
        "d4823213e87540687ffae6f6e5ddb12d9b16e40b4aceffaa8dd8122e7441c156",
        "24d37906e2e1444da4e099b2de7a7e2887770410994a61cb8f05e2518e7c5a7d",
    ]
]

# development ed25519 public keys
KEYS_DEV = [
    bytes.fromhex(k) for k in [
        "29e5833a915a6429a4e3a7948475c338ef436eb82be89c92f059704403db9d55",
        "0d7550754e0800a5d237eef5826035766b9b3e5a15868a940ab289958788e3b0",
        "e734ea6c2b6257de72355e472aa05a4c487e6b463c029ed306df2f01b5636b58",
    ]
]

KEYS_M = 2
KEYS_N = 3
SIG_SLOTS = KEYS_N

class FirmwareIntegrityError(Exception):
    pass


class InvalidSignatureError(FirmwareIntegrityError):
    pass


class Unsigned(FirmwareIntegrityError):
    pass

class HeaderType(Enum):
    FIRMWARE = b"HPTF"
    BOOTLOADER = b"HPTB"

class EnumAdapter(c.Adapter):
    def __init__(self, subcon: Any, enum: Any) -> None:
        self.enum = enum
        super().__init__(subcon)

    def _encode(self, obj: Any, ctx: Any, path: Any):
        return obj.value

    def _decode(self, obj: Any, ctx: Any, path: Any):
        try:
            return self.enum(obj)
        except ValueError:
            return obj

# the version is stored as a 4-byte little-endian number
Version = c.Struct(
    "build" / c.Int8ul,
    "patch" / c.Int8ul,
    "minor" / c.Int8ul,
    "major" / c.Int8ul,
)

ImageHeader = c.Struct(
    "magic" / EnumAdapter(c.Bytes(4), HeaderType),
    "version" / Version,
    "header_size" / c.Int32ul,
    "code_size" / c.Int32ul,
    # "code_size" / c.Rebuild(
    #     c.Int32ul,
    #     lambda this:
    #         len(this._.code) if hasattr(this._, "code")
    #         else this.code_size

    # ),
    "required_sig_count" / c.Int32ul,
    "_reserved" / c.Bytes(0x80 - 20),
    "digest" / c.Bytes(32),
    "sig_mask" / c.Int32ul,
    "sigs" / c.Bytes(64)[SIG_SLOTS],
    "padding" / c.Padding(0x380 - 32 - 4 - SIG_SLOTS * 64),
)

FirmwareImage = c.Struct(
    "header" / ImageHeader,
    "_code_offset" / c.Tell,
    "code" / c.GreedyBytes,
)

BootloaderImage = c.Struct(
    "vector_table" / c.Bytes(0x400),
    "header" / ImageHeader,
    "_code_offset" / c.Tell,
    "code" / c.GreedyBytes,
)
# fmt: on


class FirmwareFormat(Enum):
    TREZOR_ONE = 1
    TREZOR_T = 2
    TREZOR_ONE_V2 = 3
    TREZOR_HP = 4

ParsedFirmware = Tuple[FirmwareFormat, c.Container]

def parse(data: bytes) -> ParsedFirmware:
    if data[0x400:0x404] == b"HPTB":
        version = FirmwareFormat.TREZOR_HP
        cls = BootloaderImage
    elif data[:4] == b"HPTF":
        version = FirmwareFormat.TREZOR_HP
        cls = FirmwareImage
    else:
        raise ValueError("Unrecognized firmware image type")

    try:
        fw = cls.parse(data)
        # update code size
        # fw.header.code_size = len(fw.code)
    except Exception as e:
        raise FirmwareIntegrityError("Invalid firmware image") from e
    return version, fw

def header_digest(header: c.Container, hasher: 'hashlib._Hash') -> None:

    stripped_header = header.copy()
    stripped_header.digest = b"\x00" * 32
    stripped_header.sig_mask = 0x00
    stripped_header.sigs = [b"\x00" * 64] * SIG_SLOTS
    stripped_header.padding = b"\x00" * (0x380 - 32 - 4 - SIG_SLOTS * 64)
    header_bytes = ImageHeader.build(stripped_header)
    hasher.update(header_bytes)

def digest_hp(fw: c.Container) -> bytes:
    hasher = hashlib.sha256()
    if fw.header.magic == HeaderType.BOOTLOADER:
        hasher.update(fw.vector_table)
    header_digest(fw.header, hasher)
    hasher.update(fw.code)
    return hasher.digest()

def validate_hp(fw: c.Container, keys: List[bytes], allow_unsigned: bool = False) -> None:
    if fw.header.digest == b"\x00" * 32:
        if allow_unsigned:
            return
        raise Unsigned("Firmware is unsigned.")

    digest = digest_hp(fw)
    if digest != fw.header.digest:
        raise FirmwareIntegrityError("Digest in header does not match.")

    if fw.header.required_sig_count < 1:
        raise FirmwareIntegrityError("At least one signer must be specified.")

    sig_mask = fw.header.sig_mask
    if sig_mask == 0:
        if allow_unsigned:
            return
        raise Unsigned("Firmware is unsigned.")

    if sig_mask.bit_count() > len(keys):
        raise FirmwareIntegrityError("Sigmask specifies more public keys than provided.")

    sig_count = 0
    for i in range(SIG_SLOTS):
        present = sig_mask & (1 << i) != 0
        if not present:
            continue
        sig_count += 1
        key = keys[i]
        sig = fw.header.sigs[i]
        _ed25519.checkvalid(sig, digest, key)

    if sig_count == 0:
        if allow_unsigned:
            return
        raise Unsigned("Firmware is unsigned.")
    if sig_count < fw.header.required_sig_count:
        raise FirmwareIntegrityError(f"Not enough signatures. sig_count={sig_count}, required={fw.header.required_sig_count}")

def digest(version: FirmwareFormat, fw: c.Container) -> bytes:
    if version == FirmwareFormat.TREZOR_HP:
        return digest_hp(fw)
    else:
        raise ValueError("Unrecognized firmware version")

def validate(
    version: FirmwareFormat, fw: c.Container, keys: List[bytes], allow_unsigned: bool = False
) -> None:
    if version == FirmwareFormat.TREZOR_HP:
        return validate_hp(fw, keys, allow_unsigned)
    else:
        raise ValueError("Unrecognized firmware version")
