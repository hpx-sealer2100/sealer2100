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

import struct
from enum import Enum
from hashlib import blake2s
from typing import Any, List, Optional

import click
import construct as c

from .. import cosi, firmware

SYM_OK = click.style("\u2714", fg="green")
SYM_MISSED = click.style("\u2b55", fg="yellow")
SYM_FAIL = click.style("\u274c", fg="red")


class Status(Enum):
    VALID = click.style("VALID", fg="green", bold=True)
    INVALID = click.style("INVALID", fg="red", bold=True)
    MISSING = click.style("MISSING", fg="blue", bold=True)
    DEVEL = click.style("DEVEL", fg="yellow", bold=True)

    def is_ok(self) -> bool:
        return self is Status.VALID or self is Status.DEVEL

class ImageType(Enum):
    VENDOR_HEADER = 0
    BOOTLOADER = 1
    FIRMWARE = 2


def _make_dev_keys(*key_bytes: bytes) -> List[bytes]:
    return [k * 32 for k in key_bytes]

def all_zero(data: bytes) -> bool:
    return all(b == 0 for b in data)


def _check_signature_any(
    fw: c.Container, pubkeys: List[bytes], is_devel: bool
) -> Status:
    header = fw.header
    if all_zero((b for sig in header.sigs for b in sig)) and header.sig_mask == 0:
        return Status.MISSING
    try:
        firmware.validate(firmware.FirmwareFormat.TREZOR_HP, fw, pubkeys)
        return Status.VALID if not is_devel else Status.DEVEL
    except Exception:
        return Status.INVALID


# ====================== formatting functions ====================


class LiteralStr(str):
    pass


def _format_container(
    pb: c.Container,
    indent: int = 0,
    sep: str = " " * 4,
    truncate_after: Optional[int] = 64,
    truncate_to: Optional[int] = 32,
) -> str:
    def mostly_printable(bytes: bytes) -> bool:
        if not bytes:
            return True
        printable = sum(1 for byte in bytes if 0x20 <= byte <= 0x7E)
        return printable / len(bytes) > 0.8

    def pformat(value: Any, indent: int) -> str:
        level = sep * indent
        leadin = sep * (indent + 1)

        if isinstance(value, LiteralStr):
            return value

        if isinstance(value, list):
            # short list of simple values
            if not value or isinstance(value[0], (int, bool, Enum)):
                return repr(value)

            # long list, one line per entry
            lines = ["[", level + "]"]
            lines[1:1] = [leadin + pformat(x, indent + 1) for x in value]
            return "\n".join(lines)

        if isinstance(value, dict):
            lines = ["{"]
            for key, val in value.items():
                if key.startswith("_"):
                    continue
                if val is None or val == []:
                    continue
                lines.append(leadin + key + ": " + pformat(val, indent + 1))
            lines.append(level + "}")
            return "\n".join(lines)

        if isinstance(value, (bytes, bytearray)):
            length = len(value)
            suffix = ""
            if truncate_after and length > truncate_after:
                suffix = "..."
                value = value[: truncate_to or 0]
            if mostly_printable(value):
                output = repr(value)
            else:
                output = value.hex()
            return f"{length} bytes {output}{suffix}"

        if isinstance(value, Enum):
            return str(value)

        return repr(value)

    return pformat(pb, indent)


def _format_version(version: c.Container) -> str:
    version_str = ".".join(
        str(version[k]) for k in ("major", "minor", "patch") if k in version
    )
    if "build" in version:
        version_str += f" build {version.build}"
    return version_str


# =========================== functionality implementations ===============


class SignableImage:
    NAME = "Unrecognized image"
    DEV_KEYS = _make_dev_keys(b"\xA5", b"\x5A", b'\xAA')
    DEV_KEY_SIGMASK = 0b111

    def __init__(self, fw: c.Container) -> None:
        self.fw = fw
        #default to production keys
        self.public_keys = firmware.KEYS

    def set_devel(self, is_devel: bool) -> None:
        self.public_keys = firmware.KEYS_DEV if is_devel else firmware.KEYS

    def identify_dev_keys(self) -> None:
        # try checking signature with dev keys first
        self.public_keys = firmware.KEYS_DEV
        if not self.check_signature().is_ok():
            # validation with dev keys failed, use production keys
            self.public_keys = firmware.KEYS

    def digest(self) -> bytes:
        return firmware.digest(firmware.FirmwareFormat.TREZOR_HP, self.fw)

    def check_signature(self) -> Status:
        return _check_signature_any(
            self.fw, self.public_keys, self.public_keys == firmware.KEYS_DEV
        )

    def rehash(self) -> None:
        self.fw.header.digest = self.digest()

    def insert_signature(self, signature: bytes, index: int) -> None:
        if index < 0 or index >= firmware.SIG_SLOTS:
            raise ValueError("Invalid signature index")
        if len(signature) != 64:
            raise ValueError("Signature must be 64 bytes")
        self.fw.header.sigs[index] = signature
        self.fw.header.sig_mask |= 1 << index

    def dump(self) -> bytes:
        if self.fw.header.magic == firmware.HeaderType.BOOTLOADER:
            return firmware.BootloaderImage.build(self.fw)
        elif self.fw.header.magic == firmware.HeaderType.FIRMWARE:
            return firmware.FirmwareImage.build(self.fw)
        else:
            raise ValueError("Unrecognized firmware image type")

    def format(self, verbose: bool) -> str:
        return _format_container(self.fw)

class BinImage(SignableImage):

    def format(self, verbose: bool = False) -> str:
        header_out = self.fw.header.copy()

        if not verbose:
            for key in self.fw.header:
                if "version" in key:
                    header_out[key] = LiteralStr(_format_version(self.fw.header[key]))

        all_ok = SYM_OK
        hash_status = Status.VALID
        sig_status = Status.VALID

        expected = self.fw.header.digest
        actual = self.digest()
        if all_zero(self.fw.header.digest):
            hash_status = Status.MISSING
            status = SYM_MISSED
        else:
            hash_status = Status.VALID if expected == actual else Status.INVALID
            status = SYM_OK if hash_status.is_ok() else SYM_FAIL

        header_out["digest"] = LiteralStr(f"{status} {actual.hex()}")

        sig_status = self.check_signature()
        all_ok = SYM_OK if hash_status.is_ok() and sig_status.is_ok() else SYM_FAIL

        output = [
            "Firmware Header " + _format_container(header_out),
            f"Fingerprint: {click.style(self.digest().hex(), bold=True)}",
            f"{all_ok} Signature is {sig_status.value}, digest is {hash_status.value}",
        ]

        return "\n".join(output)


class FirmwareImage(BinImage):
    NAME = "firmware"

class BootloaderImage(BinImage):
    NAME = "bootloader"

def parse(image: bytes) -> SignableImage:
    _, fw = firmware.parse(image)
    if fw.header.magic == firmware.HeaderType.BOOTLOADER:
        return BootloaderImage(fw)
    if fw.header.magic == firmware.HeaderType.FIRMWARE:
        return FirmwareImage(fw)
    raise ValueError("Unrecognized image type")
