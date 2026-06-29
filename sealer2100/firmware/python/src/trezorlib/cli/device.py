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

import os
import sys
import re
import time
import json
import pathlib
from typing import TYPE_CHECKING, Optional, Sequence

import click

from .. import debuglink, device, exceptions, messages, ui
from . import ChoiceType, with_client

if TYPE_CHECKING:
    from ..client import TrezorClient
    from . import TrezorConnection
    from ..protobuf import MessageType

RECOVERY_TYPE = {
    "scrambled": messages.RecoveryDeviceType.ScrambledWords,
    "matrix": messages.RecoveryDeviceType.Matrix,
}

BACKUP_TYPE = {
    "single": messages.BackupType.Bip39,
    "shamir": messages.BackupType.Slip39_Basic,
    "advanced": messages.BackupType.Slip39_Advanced,
}

SD_PROTECT_OPERATIONS = {
    "on": messages.SdProtectOperationType.ENABLE,
    "off": messages.SdProtectOperationType.DISABLE,
    "refresh": messages.SdProtectOperationType.REFRESH,
}

BL_REBOOT_TYPE = {
    "firmware": messages.RebootType.Normal,
    "boardloader": messages.RebootType.Boardloader,
    "bootloader": messages.RebootType.BootLoader,
    }


@click.group(name="device")
def cli() -> None:
    """Device management commands - setup, recover seed, wipe, etc."""


@cli.command()
@with_client
def self_test(client: "TrezorClient") -> str:
    """Perform a factory self-test.

    Only available on PRODTEST firmware.
    """
    return debuglink.self_test(client)


@cli.command()
@click.option(
    "-b",
    "--bootloader",
    help="Wipe device in bootloader mode. This also erases the firmware.",
    is_flag=True,
)
@with_client
def wipe(client: "TrezorClient", bootloader: bool) -> str:
    """Reset device to factory defaults and remove all private data."""
    if bootloader:
        if not client.features.bootloader_mode:
            click.echo("Please switch your device to bootloader mode.")
            sys.exit(1)
        else:
            click.echo("Wiping user data and firmware!")
    else:
        if client.features.bootloader_mode:
            click.echo(
                "Your device is in bootloader mode. This operation would also erase firmware."
            )
            click.echo(
                'Specify "--bootloader" if that is what you want, or disconnect and reconnect device in normal mode.'
            )
            click.echo("Aborting.")
            sys.exit(1)
        else:
            click.echo("Wiping user data!")

    try:
        return device.wipe(client)
    except exceptions.TrezorFailure as e:
        click.echo("Action failed: {} {}".format(*e.args))
        sys.exit(3)


@cli.command()
@click.option("-m", "--mnemonic", multiple=True)
@click.option("-p", "--pin", default="")
@click.option("-r", "--passphrase-protection", is_flag=True)
@click.option("-l", "--label", default="")
@click.option("-i", "--ignore-checksum", is_flag=True)
@click.option("-s", "--slip0014", is_flag=True)
@click.option("-b", "--needs-backup", is_flag=True)
@click.option("-n", "--no-backup", is_flag=True)
@with_client
def load(
    client: "TrezorClient",
    mnemonic: Sequence[str],
    pin: str,
    passphrase_protection: bool,
    label: str,
    ignore_checksum: bool,
    slip0014: bool,
    needs_backup: bool,
    no_backup: bool,
) -> str:
    """Upload seed and custom configuration to the device.

    This functionality is only available in debug mode.
    """
    if slip0014 and mnemonic:
        raise click.ClickException("Cannot use -s and -m together.")

    if slip0014:
        mnemonic = [" ".join(["all"] * 12)]
        if not label:
            label = "SLIP-0014"

    try:
        return debuglink.load_device(
            client,
            mnemonic=list(mnemonic),
            pin=pin,
            passphrase_protection=passphrase_protection,
            label=label,
            language="en-US",
            skip_checksum=ignore_checksum,
            needs_backup=needs_backup,
            no_backup=no_backup,
        )
    except exceptions.TrezorFailure as e:
        if e.code == messages.FailureType.UnexpectedMessage:
            raise click.ClickException(
                "Unrecognized message. Make sure your Trezor is using debug firmware."
            )
        else:
            raise


@cli.command()
@click.option("-w", "--words", type=click.Choice(["12", "18", "24"]), default="24")
@click.option("-e", "--expand", is_flag=True)
@click.option("-p", "--pin-protection", is_flag=True)
@click.option("-r", "--passphrase-protection", is_flag=True)
@click.option("-l", "--label")
@click.option("-u", "--u2f-counter", default=None, type=int)
@click.option(
    "-t", "--type", "rec_type", type=ChoiceType(RECOVERY_TYPE), default="scrambled"
)
@click.option("-d", "--dry-run", is_flag=True)
@with_client
def recover(
    client: "TrezorClient",
    words: str,
    expand: bool,
    pin_protection: bool,
    passphrase_protection: bool,
    label: Optional[str],
    u2f_counter: int,
    rec_type: messages.RecoveryDeviceType,
    dry_run: bool,
) -> "MessageType":
    """Start safe recovery workflow."""
    if rec_type == messages.RecoveryDeviceType.ScrambledWords:
        input_callback = ui.mnemonic_words(expand)
    else:
        input_callback = ui.matrix_words
        click.echo(ui.RECOVERY_MATRIX_DESCRIPTION)

    return device.recover(
        client,
        word_count=int(words),
        passphrase_protection=passphrase_protection,
        pin_protection=pin_protection,
        label=label,
        u2f_counter=u2f_counter,
        language="en-US",
        input_callback=input_callback,
        type=rec_type,
        dry_run=dry_run,
    )


@cli.command()
@click.option("-e", "--show-entropy", is_flag=True)
@click.option("-t", "--strength", type=click.Choice(["128", "192", "256"]))
@click.option("-r", "--passphrase-protection", is_flag=True)
@click.option("-p", "--pin-protection", is_flag=True)
@click.option("-l", "--label")
@click.option("-u", "--u2f-counter", default=0)
@click.option("-s", "--skip-backup", is_flag=True)
@click.option("-n", "--no-backup", is_flag=True)
@click.option("-b", "--backup-type", type=ChoiceType(BACKUP_TYPE), default="single")
@with_client
def setup(
    client: "TrezorClient",
    show_entropy: bool,
    strength: Optional[int],
    passphrase_protection: bool,
    pin_protection: bool,
    label: Optional[str],
    u2f_counter: int,
    skip_backup: bool,
    no_backup: bool,
    backup_type: messages.BackupType,
) -> str:
    """Perform device setup and generate new seed."""
    if strength:
        strength = int(strength)

    if (
        backup_type == messages.BackupType.Slip39_Basic
        and messages.Capability.Shamir not in client.features.capabilities
    ) or (
        backup_type == messages.BackupType.Slip39_Advanced
        and messages.Capability.ShamirGroups not in client.features.capabilities
    ):
        click.echo(
            "WARNING: Your Trezor device does not indicate support for the requested\n"
            "backup type. Traditional single-seed backup may be generated instead."
        )

    return device.reset(
        client,
        display_random=show_entropy,
        strength=strength,
        passphrase_protection=passphrase_protection,
        pin_protection=pin_protection,
        label=label,
        language="en-US",
        u2f_counter=u2f_counter,
        skip_backup=skip_backup,
        no_backup=no_backup,
        backup_type=backup_type,
    )


@cli.command()
@with_client
def backup(client: "TrezorClient") -> str:
    """Perform device seed backup."""
    return device.backup(client)


@cli.command()
@click.argument("operation", type=ChoiceType(SD_PROTECT_OPERATIONS))
@with_client
def sd_protect(
    client: "TrezorClient", operation: messages.SdProtectOperationType
) -> str:
    """Secure the device with SD card protection.

    When SD card protection is enabled, a randomly generated secret is stored
    on the SD card. During every PIN checking and unlocking operation this
    secret is combined with the entered PIN value to decrypt data stored on
    the device. The SD card will thus be needed every time you unlock the
    device. The options are:

    \b
    on - Generate SD card secret and use it to protect the PIN and storage.
    off - Remove SD card secret protection.
    refresh - Replace the current SD card secret with a new one.
    """
    if client.features.model == "1":
        raise click.ClickException("Trezor One does not support SD card protection.")
    return device.sd_protect(client, operation)


@cli.command()
@click.pass_obj
def reboot_to_bootloader(obj: "TrezorConnection") -> str:
    """Reboot device into bootloader mode.
    """
    # avoid using @with_client because it closes the session afterwards,
    # which triggers double prompt on device
    with obj.client_context() as client:
        return device.reboot(client)

@cli.command()
@click.pass_obj
def reboot_to_boardloader(obj: "TrezorConnection") -> str:
    """Reboot device into boardloader mode.

    Currently only supported on Trezor Model T.
    """
    # avoid using @with_client because it closes the session afterwards,
    # which triggers double prompt on device
    with obj.client_context() as client:
        if client.features.model != "T":
            click.echo(f"Reboot to boardloader is not support on hypermate {client.features.model}")
        return device.reboot(client, False)

@cli.command()
@click.pass_obj
@click.argument("cert")
def se_write_cert(obj: "TrezorConnection", cert: str) -> None:
    """Get device se cert.

    Used in device verify.
    """
    with obj.client_context() as client:
        cert_bytes = device.se_write_cert(client, cert.strip('"'))
        return cert_bytes


@cli.command()
@click.pass_obj
def se_read_cert(obj: "TrezorConnection") -> bytes:
    """Get device se cert.

    Used in device verify.
    """
    with obj.client_context() as client:
        cert_bytes = device.se_read_cert(client).public_cert
        return cert_bytes

@cli.command()
@click.pass_obj
def se_read_public_key(obj: "TrezorConnection") -> dict:
    """Get device se public key.

    Used in device verify.
    """
    with obj.client_context() as client:
        pk = device.se_read_public_key(client).public_key
        return {
            "public key": pk.hex()
        }

@cli.command()
@click.pass_obj
@click.argument("message")
def se_sign_message(obj: "TrezorConnection", message: str) -> dict:
    """Get device se public key.

    Used in device verify.
    """
    with obj.client_context() as client:
        sig = device.se_sign_message(client, message.encode()).signature
        return {
            "signature": sig.hex()
        }

@cli.command()
# fmt: off
@click.option("-d", "--dir", help="The full path of the nft dir to upload", type=click.Path(exists=True))
# fmt: on
@with_client
def upload_nft(
    client: "TrezorClient",
    dir: click.Path,
) -> None:
    """Upload nft to device."""
    dir_structure = """
    NFT dir structure is as follows:
    # ├── {nft}
    #   ├── desc/
    #   │   ├── {id}.json
    #   ├── thumbnail
    #   │   ├── {id}.png
    #   ├── image/
    #   │   ├── {id}.png
    #   ├── wallpaper/
    #   │   ├── {id}.png
    """

    # check dir structure
    d = pathlib.Path(dir)
    if not (d / "desc").exists():
        click.echo(f"Dir {dir} does not contain desc dir")
        click.echo(dir_structure)
        sys.exit(3)
    if not (d / "thumbnail").exists():
        click.echo(f"Dir {dir} does not contain thumbnail dir")
        click.echo(dir_structure)
        sys.exit(3)
    if not (d / "image").exists():
        click.echo(f"Dir {dir} does not contain image dir")
        click.echo(dir_structure)
        sys.exit(3)
    if not (d / "wallpaper").exists():
        click.echo(f"Dir {dir} does not contain wallpaper dir")
        click.echo(dir_structure)
        sys.exit(3)

    descs = [f for f in (d / "desc").iterdir() if f.suffix == ".json"]
    for desc in descs:
        # get id from desc file
        id1 = desc.stem
        meta = json.load(desc.open(mode="rb"))
        id2 = meta["id"]
        if id1 != id2:
            # check if id in desc file and meta are the same
            click.echo(f"Desc file {desc} has id {id1}, but meta has id {id2}")
            sys.exit(3)
        ext = meta["extension"]
        # check files exist
        if not (d / "thumbnail" / f"{id1}.{ext}").exists():
            click.echo(f"Thumbnail file {id1}.{ext} does not exist")
            click.echo(dir_structure)
            sys.exit(3)
        if not (d / "image" / f"{id1}.{ext}").exists():
            click.echo(f"Image file {id1}.{ext} does not exist")
            click.echo(dir_structure)
            sys.exit(3)
        if not (d / "wallpaper" / f"{id1}.{ext}").exists():
            click.echo(f"Wallpaper file {id1}.{ext} does not exist")
            click.echo(dir_structure)
            sys.exit(3)
        image = (d / "image" / f"{id1}.{ext}").read_bytes()
        thumbnail = (d / "thumbnail" / f"{id1}.{ext}").read_bytes()
        wallpaper = (d / "wallpaper" / f"{id1}.{ext}").read_bytes()
        try:
            click.echo("Please confirm the action on your Trezor device")

            click.echo("Uploading...\r", nl=False)
            bar = click.progressbar(
                label="Uploading", length=len(image) + len(thumbnail) + len(wallpaper), show_eta=False
            )
            device.upload_nft(
                client,
                meta,
                image=image,
                thumbnail=thumbnail,
                wallpaper=wallpaper,
                progress=bar.update,
            )
        except exceptions.Cancelled:
            click.echo("Upload aborted on device.")
        except exceptions.TrezorException as e:
            click.echo(f"Upload failed: {e}")
            sys.exit(3)

@cli.command()
# fmt: off
@click.option("-f", "--fullpath", help="The full path of the file to update")
# fmt: on
@with_client
def update_res(
    client: "TrezorClient",
    fullpath: str,
) -> None:
    """Update internal static resource(internal icons)."""
    if fullpath:
        file_name = fullpath.split("/")[-1]
        with open(fullpath, "rb") as f:
            data = f.read()
        try:
            click.echo("Uploading...\r", nl=False)
            with click.progressbar(
                label="Uploading", length=len(data), show_eta=False
            ) as bar:
                device.update_res(client, file_name, data, progress_update=bar.update)
        except exceptions.Cancelled:
            click.echo("Upload aborted on device.")
        except exceptions.TrezorException as e:
            click.echo(f"Update failed: {e}")
            sys.exit(3)

@cli.command()
@click.argument("enable", type=ChoiceType({"on": True, "off": False}), required=False)
@click.option(
    "-e",
    "--expiry",
    type=int,
    help="Dialog expiry in seconds.",
)
@with_client
def set_busy(
    client: "TrezorClient", enable: Optional[bool], expiry: Optional[int]
) -> str:
    """Show a "Do not disconnect" dialog."""
    if enable is False:
        return device.set_busy(client, None)

    if expiry is None:
        raise click.ClickException("Missing option '-e' / '--expiry'.")

    if expiry <= 0:
        raise click.ClickException(
            f"Invalid value for '-e' / '--expiry': '{expiry}' is not a positive integer."
        )

    return device.set_busy(client, expiry * 1000)

# new feautres only available in bootloader

# helper functions
def header_printer(header:str) -> None:
    print('====================================')
    print(header)
    print('------------------------------------')

def print_buffer(buffer:bytes) -> None:
    print(''.join(format(x, '02x') for x in buffer))

# firmware update
@cli.command()
# fmt: off
@click.option("-t", "--reboot_type", type=ChoiceType(BL_REBOOT_TYPE), default="firmware", help="Reboot type")
# fmt: on
@with_client
def bl_reboot(client: "TrezorClient", reboot_type: messages.RebootType) -> str:
    result = device.bl_reboot(client, reboot_type=reboot_type)
    return result
# FirmwareUpdateEmmc
@cli.command()
# fmt: off
@click.option("-p", "--path", required=True, help="Remote firmware file path (e.g. /updates/fw.bin)")
@click.option("-r", "--reboot_on_success", required=False, is_flag=True, help="Reboot on success")
# fmt: on
@with_client
def firmware_update(client: "TrezorClient",path:str, reboot_on_success:bool) -> None:
    result = device.firmware_update(client, path_file=path, reboot_on_success=reboot_on_success)
    if isinstance(result, messages.Success):
        print(result.message)
    elif isinstance(result, messages.Failure):
        print(result.code)
        print(result.message)
    else:
        raise RuntimeError(f"Unexpected message {result}")

# write-sn
@cli.command()
# fmt: off
@click.argument("sn")
# fmt: on
@with_client
def write_sn(client: "TrezorClient", sn: str) -> None:
    device.write_sn(client, sn)

# se-initialize
@cli.command()
@with_client
def se_initialize(client: "TrezorClient") -> None:
    device.se_initialize(client)

# se-initialize-done
@cli.command()
@with_client
def se_initialize_done(client: "TrezorClient") -> None:
    device.se_initialize_done(client)

@cli.command()
@click.pass_obj
@click.argument("csr")
def se_sign_cert_request(obj: "TrezorConnection", csr: str) -> dict:
    csr = bytes.fromhex(csr)
    with obj.client_context() as client:
        sig = device.se_sign_message(client, csr).signature
        return {
            "signature": sig.hex()
        }

# se-back-to-rom-boot
@cli.command()
@with_client
def se_back_to_rom_boot(client: "TrezorClient") -> None:
    device.se_back_to_rom_boot(client)

# se-wipe-user-storage
@cli.command()
@with_client
def se_wipe_user_storage(client: "TrezorClient") -> None:
    device.se_wipe_user_storage(client)
