from micropython import const
from typing import TYPE_CHECKING

from storage.cache import show_update_res_confirm
from trezor import io, loop, utils, wire
from trezor.crypto.hashlib import blake2s
from trezor.messages import ResourceAck, ResourceRequest, Success
from trezor.ui.layouts import confirm_update_res

if TYPE_CHECKING:
    from trezor.messages import ResourceUpdate

REQUEST_CHUNK_SIZE = const(16 * 1024)

async def update_res(ctx: wire.Context, msg: ResourceUpdate) -> Success:
    if show_update_res_confirm():
        await confirm_update_res(ctx)
    res_size = msg.data_length
    initial_data = msg.initial_data_chunk
    if blake2s(initial_data).digest() != msg.hash:
        raise wire.DataError("Date digest is inconsistent")

    file_path = f"/res/{msg.file_name}"
    data_left = res_size - REQUEST_CHUNK_SIZE
    offset = REQUEST_CHUNK_SIZE
    try:
        with io.fs.open(file_path, "w") as f:
            f.write(initial_data)

            while data_left > 0:
                request = ResourceRequest(data_length=REQUEST_CHUNK_SIZE, offset=offset)
                ack: ResourceAck = await ctx.call(request, ResourceAck)
                data = ack.data_chunk
                digest = blake2s(data).digest()
                if digest != ack.hash:
                    raise wire.DataError("Date digest is inconsistent")
                f.write(data)
                offset += (
                    REQUEST_CHUNK_SIZE if data_left > REQUEST_CHUNK_SIZE else data_left
                )
                data_left -= REQUEST_CHUNK_SIZE
            # force refresh to disk
            f.sync()
    except BaseException as e:
        raise wire.FirmwareError(f"Failed to write file with error code {e}")
    return Success(message="Success")
