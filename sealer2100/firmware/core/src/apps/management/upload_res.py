from micropython import const
from typing import TYPE_CHECKING

from storage import device
from trezor import io, wire
from trezor.crypto.hashlib import blake2s
from trezor.enums import ResourceType
from trezor.messages import ResourceAck, ResourceRequest, Success, ZoomRequest
from trezor.ui.layouts import confirm_collect_nft, confirm_set_homescreen

import ujson as json
import ure as re  # type: ignore[Import "ure" could not be resolved]

if TYPE_CHECKING:
    from trezor.messages import ResourceUpload

SUPPORTED_EXTS = (("jpg", "png", "jpeg"), ("jpg", "jpeg", "png", "mp4"))

SUPPORTED_MAX_RESOURCE_SIZE = {
    "jpg": const(1024 * 1024),
    "jpeg": const(1024 * 1024),
    "png": const(1024 * 1024),
    "mp4": const(10 * 1024 * 1024),
}
# FILE_PATH_COMPONENTS = (("wallpapers", "wp"), ("nfts", "nft"))
NFT_METADATA_ALLOWED_KEYS = ("header", "subheader", "network", "owner")
REQUEST_CHUNK_SIZE = const(1 * 1024)

MAX_WP_COUNTER = const(5)
MAX_NFT_COUNTER = const(24)

# a more precise version should be ^(nft|wp)-[0-9,a-f]{8}-\\d{13,}$, but micropython not support limit range {}
PATTERN = re.compile(r"^(nft|wp)-[0-9a-f]+-\d+$")


async def upload_res(ctx: wire.Context, msg: ResourceUpload) -> Success:
    raise wire.Error("upload_res unsupported now")
