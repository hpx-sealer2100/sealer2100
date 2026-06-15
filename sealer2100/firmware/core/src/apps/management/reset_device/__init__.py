from typing import TYPE_CHECKING

import storage
import storage.device
from storage.device import (
    DEVICE_PROTECT_TYPE_PIN,
    DEVICE_PROTECT_TYPE_IRIS,
    DEVICE_PROTECT_TYPE_PIN_AND_IRIS,
)
from trezor import config, loop, utils, wire, iris
from trezor.crypto import bip39, hashlib, random
from trezor.enums import BackupType
from trezor.messages import EntropyAck, EntropyRequest, Success
from trezor.ui.layouts.management import (
    confirm_wallet_ready,
    confirm_reset_device,
    confirm_device_protection,
    request_strength,
)
from trezor.ui.layouts import show_loading, show_app_guide, wait_doing
from apps.common.request_pin import request_device_protection
from .. import backup_types
from . import layout
from trezor.ui import i18n
from trezor.ui import Redo

if __debug__:
    import storage.debug

if TYPE_CHECKING:
    from trezor.messages import ResetDevice

_DEFAULT_BACKUP_TYPE = BackupType.Bip39

async def reset_storage():
    """
    wipe `iris` `storage` and refresh iris version
    """
    storage.reset()
    await loop.sleep(100)
    await iris.wipe()
    await loop.sleep(100)
    await iris.refresh_iris_version()

async def reset_device(ctx: wire.Context, msg: ResetDevice) -> Success:
    # validate parameters and device state
    _validate_reset_device(msg)

    if msg.language is not None:
        i18n.change_language(msg.language)
    await wait_doing(i18n.Text.wiping_device, reset_storage())
    if msg.language is not None:
        storage.device.set_language(msg.language)

    # make sure user knows they're setting up a new wallet
    if msg.backup_type == BackupType.Slip39_Basic:
        # prompt = "Create a new wallet\nwith Shamir Backup?"
        raise wire.ProcessError("Shamir Backup not supported")
    elif msg.backup_type == BackupType.Slip39_Advanced:
        # prompt = "Create a new wallet\nwith Super Shamir?"
        raise wire.ProcessError("Super Shamir not supported")

    if isinstance(ctx, wire.DummyContext):
        utils.play_dead()

    try:
        if isinstance(ctx, wire.DummyContext):
            # on device reset, we need to ask for a new strength to override the default  value 12
            msg.strength = await request_strength(ctx)

        # request and set new PIN
        protect_type = await confirm_device_protection(ctx, "")
        protect_type = await request_device_protection(ctx, protect_type)
        storage.device.set_device_protect_type(protect_type)

        # If either of skip_backup or no_backup is specified, we are not doing backup now.
        # Otherwise, we try to do it.
        perform_backup = not msg.no_backup and not msg.skip_backup
        await confirm_wallet_ready(ctx, True)

        # generate and display internal entropy
        int_entropy = random.bytes(32)
        if __debug__:
            storage.debug.reset_internal_entropy = int_entropy
        if msg.display_random:
            await layout.show_internal_entropy(ctx, int_entropy)

        # request external entropy and compute the master secret
        entropy_ack = await ctx.call(EntropyRequest(), EntropyAck)
        ext_entropy = entropy_ack.entropy if entropy_ack else b""
        # For SLIP-39 this is the Encrypted Master Secret
        secret = _compute_secret_from_entropy(
            int_entropy, ext_entropy, msg.strength
        )

        # Check backup type, perform type-specific handling
        if msg.backup_type == BackupType.Bip39:
            # in BIP-39 we store mnemonic string instead of the secret
            secret = bip39.from_data(secret).encode()
        else:
            # Unknown backup type.
            raise RuntimeError

        # generate and display backup information for the master secret
        if perform_backup:
            await backup_seed(ctx, msg.backup_type, secret)
        # write settings and master secret into storage
        if msg.label is not None:
            storage.device.set_label(msg.label)

        storage.device.set_passphrase_enabled(bool(msg.passphrase_protection))

        storage.device.store_mnemonic_secret(
            secret,  # for SLIP-39, this is the EMS
            msg.backup_type,
            needs_backup=not perform_backup,
            no_backup=bool(msg.no_backup),
        )

        # if we backed up the wallet, show success message
        # if perform_backup:
        #     await layout.show_backup_success(ctx)
        if isinstance(ctx, wire.DummyContext):
            utils.make_show_app_guide(utils.SHOW_APP_GUIDE_RESET)
        else:
            await show_app_guide()

    except BaseException as e:
        raise e
    else:
        if isinstance(ctx, wire.DummyContext):
            loop.clear()
        return Success(message="Initialized")


def _validate_reset_device(msg: ResetDevice) -> None:
    msg.backup_type = msg.backup_type or _DEFAULT_BACKUP_TYPE
    if msg.backup_type not in (
        BackupType.Bip39,
        BackupType.Slip39_Basic,
        BackupType.Slip39_Advanced,
    ):
        raise wire.ProcessError("Backup type not implemented.")
    if backup_types.is_slip39_backup_type(msg.backup_type):
        if msg.strength not in (128, 256):
            raise wire.ProcessError("Invalid strength (has to be 128 or 256 bits)")
    else:  # BIP-39
        if msg.strength not in (128, 192, 256):
            raise wire.ProcessError("Invalid strength (has to be 128, 192 or 256 bits)")
    if msg.display_random and (msg.skip_backup or msg.no_backup):
        raise wire.ProcessError("Can't show internal entropy when backup is skipped")
    if storage.device.is_initialized():
        raise wire.UnexpectedMessage("Already initialized")


def _compute_secret_from_entropy(
    int_entropy: bytes, ext_entropy: bytes, strength_in_bytes: int
) -> bytes:
    # combine internal and external entropy
    ehash = hashlib.sha256()
    ehash.update(int_entropy)
    ehash.update(ext_entropy)
    entropy = ehash.digest()
    # take a required number of bytes
    strength = strength_in_bytes // 8
    secret = entropy[:strength]
    return secret


async def backup_seed(
    ctx: wire.Context, backup_type: BackupType, mnemonic_secret: bytes
) -> None:
    if backup_type == BackupType.Slip39_Basic:
        raise wire.ProcessError("Shamir Backup not supported")
    elif backup_type == BackupType.Slip39_Advanced:
        raise wire.ProcessError("Super Shamir not supported")
    else:
        return await layout.bip39_show_and_confirm_mnemonic(
            ctx, mnemonic_secret.decode()
        )
