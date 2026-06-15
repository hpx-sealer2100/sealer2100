#!/usr/bin/env python3

import click
import hashlib

import construct as c

from enum import Enum
from trezorlib import _ed25519
from typing import List, Tuple, Any, Literal

from trezorlib.firmware import (
    KEYS,
    KEYS_DEV,
    KEYS_M,
    KEYS_N,
    SIG_SLOTS,
    InvalidSignatureError,
    FirmwareIntegrityError,
    Unsigned,
)

from trezorlib._internal.image import (
    SYM_OK,
    SYM_FAIL,
    SYM_MISSED,
    Status,
    _make_dev_keys,
    all_zero,
    _format_container,
    _format_version,
    LiteralStr,
)

# =========================== image =========================

IRIS_OTA_MAGIC = b"update_ota.tar"


class HeaderType(Enum):
    IRIS = b"IRIS"


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


Version = c.Struct(
    "build" / c.Int32ul,
    "patch" / c.Int32ul,
    "minor" / c.Int32ul,
    "major" / c.Int32ul,
)

ImageHeader = c.Struct(
    "magic" / EnumAdapter(c.Bytes(4), HeaderType),
    "version" / Version,
    "header_size" / c.Int32ul,
    "code_size" / c.Int32ul,
    "required_sig_count" / c.Int32ul,
    "_reserved" / c.Bytes(0x80 - 32),
    "digest" / c.Bytes(32),
    "sig_mask" / c.Int32ul,
    "sigs" / c.Bytes(64)[SIG_SLOTS],
    "padding" / c.Padding(0x380 - 32 - 4 - SIG_SLOTS * 64),
)

IrisImage = c.Struct(
    "header" / ImageHeader,
    "code" / c.GreedyBytes,
)


def ota_default_header(code_size: int) -> c.Container:
    return c.Container(
        magic=HeaderType.IRIS,
        version=c.Container(build=0, patch=0, minor=0, major=0),
        header_size=ImageHeader.sizeof(),
        code_size=code_size + ImageHeader.sizeof(),
        required_sig_count=KEYS_M,
        _reserved=b"\x00" * (0x80 - 32),
        digest=b"\x00" * 32,
        sig_mask=0,
        sigs=[b"\x00" * 64 for _ in range(SIG_SLOTS)],
    )


def parse(data: bytes) -> c.Container:
    if data[:4] == HeaderType.IRIS.value:
        # signed OTA image
        return IrisImage.parse(data)
    elif data[:14] == IRIS_OTA_MAGIC:
        # original OTA image
        # wrap it with default header
        header = ota_default_header(len(data))
        return c.Container(
            header=header,
            code=data,
        )
    else:
        raise ValueError("Unrecognized image type")


def header_digest(header: c.Container, hasher: "hashlib._Hash") -> None:

    stripped_header = header.copy()
    stripped_header.digest = b"\x00" * 32
    stripped_header.sig_mask = 0x00
    stripped_header.sigs = [b"\x00" * 64] * SIG_SLOTS
    stripped_header.padding = b"\x00" * (0x380 - 32 - 4 - SIG_SLOTS * 64)
    header_bytes = ImageHeader.build(stripped_header)
    hasher.update(header_bytes)


def digest(fw: c.Container) -> bytes:
    hasher = hashlib.sha256()
    header_digest(fw.header, hasher)
    hasher.update(fw.code)
    return hasher.digest()


def validate(fw: c.Container, keys: List[bytes], allow_unsigned: bool = False) -> None:
    if fw.header.digest == b"\x00" * 32:
        if allow_unsigned:
            return
        raise Unsigned("Firmware is unsigned.")

    d = digest(fw)
    if d != fw.header.digest:
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
        _ed25519.checkvalid(sig, d, key)

    if sig_count == 0:
        if allow_unsigned:
            return
        raise Unsigned("Firmware is unsigned.")
    if sig_count < fw.header.required_sig_count:
        raise FirmwareIntegrityError(f"Not enough signatures. sig_count={sig_count}, required={fw.header.required_sig_count}")


def _check_signature_any(
    fw: c.Container, pubkeys: List[bytes], is_devel: bool
) -> Status:
    header = fw.header
    if all_zero((b for sig in header.sigs for b in sig)) and header.sig_mask == 0:
        return Status.MISSING
    try:
        validate(fw, pubkeys)
        return Status.VALID if not is_devel else Status.DEVEL
    except Exception:
        return Status.INVALID
class SignableImage:
    NAME = "Unrecognized image"
    DEV_KEYS = _make_dev_keys(b"\xA5", b"\x5A", b'\xAA')
    DEV_KEY_SIGMASK = 0b111

    def __init__(self, fw: c.Container) -> None:
        self.fw = fw
        #default to production keys
        self.public_keys = KEYS

    def set_devel(self, is_devel: bool) -> None:
        self.public_keys = KEYS_DEV if is_devel else KEYS

    def identify_dev_keys(self) -> None:
        # try checking signature with dev keys first
        self.public_keys = KEYS_DEV
        if not self.check_signature().is_ok():
            # validation with dev keys failed, use production keys
            self.public_keys = KEYS

    def digest(self) -> bytes:
        return digest(self.fw)

    def check_signature(self) -> Status:
        return _check_signature_any(
            self.fw, self.public_keys, self.public_keys == KEYS_DEV
        )

    def rehash(self) -> None:
        self.fw.header.digest = self.digest()

    def insert_signature(self, signature: bytes, index: int) -> None:
        if index < 0 or index >= SIG_SLOTS:
            raise ValueError("Invalid signature index")
        if len(signature) != 64:
            raise ValueError("Signature must be 64 bytes")
        self.fw.header.sigs[index] = signature
        self.fw.header.sig_mask |= 1 << index

    def dump(self) -> bytes:
        if self.fw.header.magic == HeaderType.IRIS:
            return IrisImage.build(self.fw)
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


class SignableIrisImage(BinImage):
    NAME = "iris"

    @staticmethod
    def parse(data: bytes) -> "SignableIrisImage":
        fw = parse(data)
        return SignableIrisImage(fw)
# =========================== signing =========================


def sign_with_privkeys(digest: bytes, privkeys: List[bytes]) -> List[bytes]:
    pubkeys = [_ed25519.publickey_unsafe(sk) for sk in privkeys]
    for i, pk in enumerate(pubkeys):
        click.echo("Signing with public key #{}: {}".format(i + 1, pk.hex()))

    sigs = [
        _ed25519.signature_unsafe(digest, sk, pk) for sk, pk in zip(privkeys, pubkeys)
    ]

    try:
        for sig, pk in zip(sigs, pubkeys):
            _ed25519.checkvalid(sig, digest, pk)
    except Exception as e:
        raise click.ClickException("Failed to produce valid signature.") from e

    return sigs

def parse_privkey_args(privkey_data: List[str]) -> Tuple[int, List[bytes]]:
    privkeys = []
    sigmask = 0
    for key in privkey_data:
        try:
            idx, key_hex = key.split(":", maxsplit=1)
            privkeys.append(bytes.fromhex(key_hex))
            sigmask |= 1 << (int(idx) - 1)
        except ValueError:
            click.echo(f"Could not parse key: {key}")
            click.echo("Keys must be in the format: <key index>:<hex-encoded key>")
            raise click.ClickException("Unrecognized key format.")
    return sigmask, privkeys

# ===================== CLI actions =========================

def parse_version(version: str) -> Tuple[int, int, int]:
    """
    Parse version string into major, minor, patch.

    :param version: Version string in the format "major.minor.patch", e.g. "3.260602.1249"
    """
    major, minor, patch = version.split(".")
    return int(major), int(minor), int(patch)

@click.command()
@click.option("-n", "--dry-run", is_flag=True, help="Do not save changes.")
@click.option("-h", "--rehash", is_flag=True, help="Force recalculate hashes.")
@click.option("-v", "--verbose", is_flag=True, help="Show verbose info about headers.")
@click.option("--verify", "verify", is_flag=True, help="Verify signed bin")
@click.option(
    "-S",
    "--sign-private",
    "privkey_data",
    metavar="INDEX:PRIVKEY_HEX",
    multiple=True,
    help="Private key to use for signing. Can be repeated.",
)
@click.option(
    "-D", "--sign-dev-keys", is_flag=True, help="Sign with development header keys."
)
@click.option(
    "-s",
    "--signature",
    "insert_signature",
    nargs=2,
    metavar="INDEX:INDEX:INDEX... SIGNATURE_HEX",
    help="Insert external signature.",
)
@click.option(
    "-d",
    "--digest",
    "print_digest",
    is_flag=True,
    help="Only output header digest for signing and exit.",
)
@click.option(
    "--set-version",
    "set_version",
    help="Set version to the specified value.",
    type=click.STRING,
)
@click.argument("firmware_file", type=click.File("rb+"))
def cli(
    firmware_file,
    verbose,
    verify,
    rehash,
    dry_run,
    privkey_data,
    sign_dev_keys,
    insert_signature,
    print_digest,
    set_version,
):
    """Manage trezor-core firmware headers.

    This tool supports three types of files: raw vendor headers (TRZV), bootloader
    images (TRZB), and firmware images which are prefixed with a vendor header
    (TRZV+TRZF).

    Run with no options on a file to dump information about that file.

    Run with -d to print the header digest and exit. This works correctly regardless of
    whether code hashes have been filled.

    Run with -h to recalculate and fill in code hashes.

    To insert an external signature:

      ./headertool.py firmware.bin -s 1:2:3 ABCDEF<...signature in hex format>

    The string "1:2:3" is a list of 1-based indexes of keys used to generate the signature.

    To sign with local private keys:

    \b
      ./headertool.py firmware.bin -S 1:ABCDEF<...hex private key> -S 2:1234<..hex private key>

    Each instance of -S is in the form "index:privkey", where index is the same as
    above. Instead of specifying the keys manually, use -D to substitue known
    development keys.

    Signature validity is not checked in either of the two cases.

    To sign with remote participants:

      ./headertool.py firmware.bin -r 10.24.13.11 -r 10.24.13.190 ...

    Each participant must be running keyctl-proxy configured on the same file. Signers'
    public keys must be in the list of known signers and are matched to indexes
    automatically.
    """
    firmware_data = firmware_file.read()

    try:
        fw = SignableIrisImage.parse(firmware_data)
    except Exception as e:
        import traceback

        traceback.print_exc()
        magic = firmware_data[:4]
        raise click.ClickException(
            "Could not parse file (magic bytes: {!r})".format(magic)
        ) from e

    if verify:
        fw.identify_dev_keys()
        click.echo(fw.format(True))
        return

    if not set_version:
        raise click.ClickException("Version must be specified for signing.")

    major, minor, patch = parse_version(set_version)
    fw.fw.header.version = c.Container(build=0, patch=patch, minor=minor, major=major)

    digest = fw.digest()
    if print_digest:
        click.echo(digest.hex())
        return

    if rehash:
        fw.rehash()

    if sign_dev_keys:
        fw.set_devel(True)
        privkeys = fw.DEV_KEYS
        sigmask = fw.DEV_KEY_SIGMASK
    else:
        fw.set_devel(False)
        sigmask, privkeys = parse_privkey_args(privkey_data)

    signatures = None

    if privkeys:
        click.echo("Signing with local private keys...", err=True)
        signatures = sign_with_privkeys(digest, privkeys)

    if insert_signature:
        click.echo("Inserting external signature...", err=True)
        sigmask_str, signatures = insert_signature
        signatures = bytes.fromhex(signatures)
        sigmask = 0
        for bit in sigmask_str.split(":"):
            sigmask |= 1 << (int(bit) - 1)

    if signatures:
        fw.rehash()
        for sig in signatures:
            valid = False
            for i, pk in enumerate(fw.public_keys):
                try:
                    _ed25519.checkvalid(sig, digest, pk)
                    fw.insert_signature(sig, i)
                    valid = True
                    break
                except Exception:
                    pass

            if not valid:
                click.echo("Signature {} is invalid".format(sig.hex()), err=True)

    click.echo(fw.format(verbose))

    updated_data = fw.dump()
    if updated_data == firmware_data:
        click.echo("No changes made", err=True)
    elif dry_run:
        click.echo("Not saving changes", err=True)
    else:
        firmware_file.seek(0)
        firmware_file.truncate(0)
        firmware_file.write(updated_data)


if __name__ == "__main__":
    cli()
