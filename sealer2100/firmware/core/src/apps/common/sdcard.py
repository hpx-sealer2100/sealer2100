import storage.sd_salt
from storage.sd_salt import SD_CARD_HOT_SWAPPABLE
from trezor import io, sdcard, ui, wire

async def request_sd_salt(
    ctx: wire.GenericContext = wire.DUMMY_CONTEXT,
) -> bytearray | None:
    if not storage.sd_salt.is_enabled():
        return None

    raise NotImplementedError()

