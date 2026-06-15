#!/usr/bin/env python3
import click

from trezorlib import cosi, firmware, _ed25519
from trezorlib._internal import image

from typing import List, Tuple

# =========================== signing =========================


def sign_with_privkeys(digest: bytes, privkeys: List[bytes]) -> List[bytes]:
    pubkeys = [_ed25519.publickey_unsafe(sk) for sk in privkeys]
    for i, pk in enumerate(pubkeys):
        click.echo("Signing with public key #{}: {}".format(i + 1, pk.hex()))

    sigs = [
        _ed25519.signature_unsafe(digest, sk, pk)
        for sk, pk in zip(privkeys, pubkeys)
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
    "-r",
    "--remote",
    metavar="IPADDR",
    multiple=True,
    help="IP address of remote signer. Can be repeated.",
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
    remote,
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
        fw = image.parse(firmware_data)
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
