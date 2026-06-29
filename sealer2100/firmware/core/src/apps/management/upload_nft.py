from micropython import const
from typing import TYPE_CHECKING

from storage import device
from trezor import io, wire, loop, log
from trezor.crypto.hashlib import sha256
from trezor.enums import NftRequestType
from trezor.messages import NftRequest, NftAck, Success
from trezor.ui.layouts import confirm_collect_nft
from trezor.ui.screen.home.nft import (
    NFT_DIR,
    NftDesc,
    NftSource,
    nfts_sources
)

import ujson as json

if TYPE_CHECKING:
    from trezor.messages import NftUpload

SUPPORTED_EXTS = ("jpg", "png", "jpeg")

SUPPORTED_MAX_RESOURCE_SIZE = {
    "jpg": const(1024 * 1024),
    "jpeg": const(1024 * 1024),
    "png": const(1024 * 1024),
}

MAX_NFT_COUNT = const(32)

REQUEST_CHUNK_SIZE = const(16 * 1024)
REQUEST_TIMEOUT = const(10 * 1000) # 10s

async def wait_timeout(timeout: int):
    await loop.sleep(timeout)
    await wire.signal_ack()
async def request_nft_with_timeout(ctx: wire.Context, req: NftRequest, timeout: int) -> NftAck:
    request = ctx.call(req, NftAck)
    wait = wait_timeout(timeout)
    return await loop.race(request, wait)

async def upload_nft(ctx: wire.Context, msg: NftUpload) -> Success:
    ext = msg.extension

    # check supported
    if ext not in SUPPORTED_EXTS:
        raise wire.DataError("Not supported nft extension")

    metadata = msg.metadata

    # ask user to confirm
    await confirm_collect_nft(ctx, f"{metadata.name}#{metadata.id}")


    from trezor.ui.screen.management.upload_res import LoadingResource
    loading: LoadingResource|None = None
    try:
        # try mkdir
        io.fs.mkdir(NFT_DIR)
        io.fs.mkdir(f'{NFT_DIR}/{metadata.token}')
        io.fs.mkdir(f'{NFT_DIR}/{metadata.token}/desc')
        io.fs.mkdir(f'{NFT_DIR}/{metadata.token}/image')
        io.fs.mkdir(f'{NFT_DIR}/{metadata.token}/thumbnail')
        io.fs.mkdir(f'{NFT_DIR}/{metadata.token}/wallpaper')

        # save description
        content = {
            'id': metadata.id,
            'name': metadata.name,
            'network': metadata.network,
            'owner': metadata.owner,
            'token': metadata.token,
            'ext': msg.extension,
        }
        desc_path = f'{NFT_DIR}/{metadata.token}/desc/{metadata.id}.json'
        with io.fs.open(desc_path, "w") as f:
            f.truncate()
            f.write(json.dumps(content))

        # setup a loading screen
        desc = NftDesc(desc_path)
        source = NftSource(desc)
        total = msg.image_size + msg.thumbnail_size + msg.wallpaper_size
        loading = LoadingResource(f"{metadata.name}#{metadata.id}", total)
        await loading.show()

        # save image
        size = msg.image_size
        with io.fs.open(source.source('FS'), "w") as f:
            f.truncate()
            offset = 0
            while size > 0:
                length = min(REQUEST_CHUNK_SIZE, size)
                req = NftRequest(offset=offset, data_length=length, type=NftRequestType.IMAGE)
                ack = await request_nft_with_timeout(ctx, req, timeout=REQUEST_TIMEOUT)
                if ack.hash:
                    digest = sha256(ack.chunk).digest()
                    if digest != ack.hash:
                        raise wire.DataError("Date digest is inconsistent")
                f.write(ack.chunk)
                offset += length
                size -= length
                loading.update(length)
                f.sync()

        # save thumbnail
        size = msg.thumbnail_size
        with io.fs.open(source.thumbnail('FS'), "w") as f:
            f.truncate()
            offset = 0
            while size > 0:
                length = min(REQUEST_CHUNK_SIZE, size)
                req = NftRequest(offset=offset, data_length=length, type=NftRequestType.THUMBNAIL)
                ack = await request_nft_with_timeout(ctx, req, timeout=REQUEST_TIMEOUT)
                if ack.hash:
                    digest = sha256(ack.chunk).digest()
                    if digest != ack.hash:
                        raise wire.DataError("Date digest is inconsistent")
                f.write(ack.chunk)
                offset += length
                size -= length
                loading.update(length)
                f.sync()

        # save wallpaper
        size = msg.wallpaper_size
        with io.fs.open(source.wallpaper('FS'), "w") as f:
            f.truncate()
            offset = 0
            while size > 0:
                length = min(REQUEST_CHUNK_SIZE, size)
                req = NftRequest(offset=offset, data_length=length, type=NftRequestType.WALLPAPER)
                ack = await request_nft_with_timeout(ctx, req, timeout=REQUEST_TIMEOUT)
                if ack.hash:
                    digest = sha256(ack.chunk).digest()
                    if digest != ack.hash:
                        raise wire.DataError("Date digest is inconsistent")
                f.write(ack.chunk)
                offset += length
                size -= length
                loading.update(length)
                f.sync()

    except BaseException as e:
        log.exception(__name__, f'{e}')
        raise wire.FirmwareError(f"Failed to write file with error code {e}")
    finally:
        if loading:
            loading.close(None)
    from trezor.ui.screen import manager
    from trezor.ui import events
    manager.publish(events.NFT_CHANGED)
    return Success(message="Success")
