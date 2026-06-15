import os
import hashlib

from typing import TYPE_CHECKING, Optional, Sequence
import logging
import click
import tqdm

from .. import fs, exceptions, messages, ui
from . import ChoiceType, with_client

if TYPE_CHECKING:
    from ..client import TrezorClient
    from . import TrezorConnection
    from ..protobuf import MessageType

DEFAULT_CHUNK_SIZE = 16 * 1024
MAX_CHUNK_SIZE = 16 * 1024

LOG = logging.getLogger(__name__)

# size in bytes -> human-readable string
def size_to_human(size: int) -> str:
    if size < 1024:
        return f"{size} B"
    elif size < 1024**2:
        return f"{size // 1024} KB"
    elif size < 1024**3:
        return f"{size // 1024**2} MB"
    elif size < 1024**4:
        return f"{size // 1024**3} GB"
    else:
        return f"{size // 1024**4} TB"

def size_from_human(size: str) -> int:
    size = size.upper()
    if size.endswith("KB"):
        return int(size[:-2]) * 1024
    elif size.endswith("MB"):
        return int(size[:-2]) * 1024**2
    elif size.endswith("GB"):
        return int(size[:-2]) * 1024**3
    elif size.endswith("TB"):
        return int(size[:-2]) * 1024**4
    elif size.endswith("B"):
        return int(size[:-1])
    elif size.isdigit():
        return int(size)
    else:
        click.echo(f"Invalid chunk size: {size}, use default 16KB")
        return DEFAULT_CHUNK_SIZE

def tqdm_bar_create(desc: str, total: int) -> tqdm.tqdm:
    import shutil
    term_width = shutil.get_terminal_size().columns
    if term_width < 80:
        ncols = None
        dynamic = True
    else:
        ncols = 80
        dynamic = False
    bar = tqdm.tqdm(desc=desc, total=total, unit='B', unit_scale=True, ncols=ncols, bar_format="{l_bar}{bar} {n_fmt}/{total_fmt}", dynamic_ncols=dynamic)
    return bar

@click.group(name="fs")
def cli() -> None:
    """File system management commands - read, write, etc."""


# stat
@cli.command()
# fmt: off
@click.argument("path")
# fmt: on
@with_client
def stat(client: "TrezorClient", path: str) -> dict:
    """Stat file or directory."""
    result = fs.stat(client, path)
    return {
        "path": path,
        "type": "file" if result.type == messages.FsType.FILE else "dir",
        "size": f"{size_to_human(result.size)} ({result.size} B)",
    }


# fs stat
@cli.command()
@with_client
def fs_stat(client: "TrezorClient") -> dict:
    """Stat file system."""
    result = fs.fs_stat(client)
    return {
        "block count": result.block_count,
        "block size": size_to_human(result.block_size),
        "block used": result.block_used,
        "total size": size_to_human(result.block_count * result.block_size),
        "used size": size_to_human(result.block_used * result.block_size),
        "free size": size_to_human(result.block_count * result.block_size - result.block_used * result.block_size),
        "used percent": f"{result.block_used / result.block_count * 100:.2f}%",
    }

# remove file or directory
@cli.command(name="rm")
# fmt: off
@click.argument("path")
# fmt: on
@with_client
def remove(client: "TrezorClient", path: str) -> None:
    """Remove file or directory."""
    fs.remove(client, path)

# mkdir
@cli.command(name="mkdir")
# fmt: off
@click.argument("path")
# fmt: on
@with_client
def mkdir(client: "TrezorClient", path: str) -> None:
    """Create directory."""
    fs.mkdir(client, path)

# read file
@cli.command()
# fmt: off
@click.option("-f", "--force", is_flag=True, help="Force read even if local file exists")
@click.option("-l", "--local", help="Local file path to save the file")
@click.option("-r", "--remote", help="Remote file path to read from on device")
@click.option("-cs", "--chunk-size", default=str(DEFAULT_CHUNK_SIZE), help="Chunk size in bytes to read")
# fmt: on
@with_client
def read(client: "TrezorClient", force: bool, local: str, remote: str, chunk_size: str) -> None:
    """Read file from device."""
    chunk_size = size_from_human(chunk_size)
    if chunk_size > MAX_CHUNK_SIZE:
        click.echo(f"Chunk size {chunk_size} is too large, max is {size_to_human(MAX_CHUNK_SIZE)}")
        exit(1)

    LOG.debug(f"Read file {remote} to {local} with chunk size {size_to_human(chunk_size)}")

    if not force and os.path.exists(local):
        click.echo(f"Local file {local} already exists, use --force to overwrite")
        exit(1)

    stat = fs.stat(client, remote)
    if stat.type != messages.FsType.FILE:
        click.echo(f"Remote path {remote} is not a file")
        exit(1)
    bar = tqdm_bar_create(f"reading {remote}", stat.size)
    offset = 0
    with open(local, "wb") as f:
        while offset < stat.size:
            LOG.debug(f"Reading chunk {offset} to {offset + chunk_size} from {remote}")
            chunk = fs.read(client, remote, offset, chunk_size)
            LOG.debug(f"Readed chunk size: {size_to_human(len(chunk.data))}")
            # verify data hash if available
            if chunk.data_hash:
                LOG.debug(f"Verify data hash {chunk.data_hash.hex()}")
                hash = hashlib.sha256(chunk.data).digest()
                if hash != chunk.data_hash:
                    click.echo(f"Data hash verification failed, expected {chunk.data_hash.hex()}, got {hash.hex()}")
                    bar.close()
                    exit(1)
            f.write(chunk.data)
            bar.update(len(chunk.data))
            offset += chunk_size
        bar.close()
    click.echo(f"File {local} read from {remote} done")
    # show file digest
    with open(local, "rb") as f:
        data = f.read()
        hash = hashlib.sha256(data).digest()
        click.echo(f"File digest: {hash.hex()}")

# write file
@cli.command()
# fmt: off
@click.option("-l", "--local", required=True, help="Local file path to read from")
@click.option("-r", "--remote", required=True, help="Remote file path to write to on device")
@click.option("-cs", "--chunk-size", default=str(DEFAULT_CHUNK_SIZE), help="Chunk size in bytes to write")
# fmt: on
@with_client
def write(client: "TrezorClient", local: str, remote: str, chunk_size: str) -> None:
    """Overwrite file to device."""

    chunk_size = size_from_human(chunk_size)
    if chunk_size > MAX_CHUNK_SIZE:
        click.echo(f"Chunk size {chunk_size} is too large, max is {size_to_human(MAX_CHUNK_SIZE)}")
        exit(1)


    bar = tqdm_bar_create(f"writing {remote}", os.path.getsize(local))
    offset = 0
    with open(local, "rb") as f:
        total = os.path.getsize(local)
        while offset < total:
            data = f.read(chunk_size)
            fs.write(client, remote, total, offset, data)
            bar.update(len(data))
            offset += len(data)

        bar.close()

    click.echo(f"File {local} written to {remote} done")

# resource files checksum
@cli.command(name="checksums")
# fmt: off
@click.argument("path", required=False)
# fmt: on
@with_client
def checksums(client: "TrezorClient", path: str | None = None) -> None:
    """Generate checksums for resource files.

    This command will generate checksums for all resource files in the device.
    Optionally, it can fetch the checksum file from the device and save it
    locally if PATH is provided.
    """
    fs.checksums(client)

    if path:
        read.callback(True, path, "/res/checksums.txt", str(DEFAULT_CHUNK_SIZE))

# fread
@cli.command(name="fread", deprecated=True)
# fmt: off
@click.option("-f", "--force", is_flag=True, help="Force read even if local file exists")
@click.option("-l", "--local", help="Local file path to save the file")
@click.option("-r", "--remote", help="Remote file path to read from on device")
@click.option("-cs", "--chunk-size", default=str(DEFAULT_CHUNK_SIZE), help="Chunk size in bytes to read")
# fmt: on
@with_client
def fread(client: "TrezorClient", force: bool, local: str, remote: str, chunk_size: str) -> None:
    """Read file from device.

    This command is deprecated, please use `read` command instead.
    """
    chunk_size = size_from_human(chunk_size)
    if chunk_size > MAX_CHUNK_SIZE:
        click.echo(f"Chunk size {chunk_size} is too large, max is {size_to_human(MAX_CHUNK_SIZE)}")
        exit(1)

    LOG.debug(f"Read file {remote} to {local} with chunk size {size_to_human(chunk_size)}")

    if not force and os.path.exists(local):
        click.echo(f"Local file {local} already exists, use --force to overwrite")
        exit(1)

    stat = fs.stat(client, remote)
    if stat.type != messages.FsType.FILE:
        click.echo(f"Remote path {remote} is not a file")
        exit(1)
    bar = tqdm_bar_create(f"reading {remote}", stat.size)
    offset = 0

    with open(local, "wb") as f:
        while offset < stat.size:
            LOG.debug(f"Reading chunk {offset} to {offset + chunk_size} from {remote}")
            chunk = fs.file_read(client, remote, offset, chunk_size)
            LOG.debug(f"Readed chunk size: {size_to_human(len(chunk.data))}")
            # verify data hash if available
            if chunk.data_hash:
                LOG.debug(f"Verify data hash {chunk.data_hash.hex()}")
                hash = hashlib.sha256(chunk.data).digest()
                if hash != chunk.data_hash:
                    click.echo(f"Data hash verification failed, expected {chunk.data_hash.hex()}, got {hash.hex()}")
                    bar.close()
                    exit(1)
            f.write(chunk.data)
            bar.update(len(chunk.data))
            offset += chunk_size
        bar.close()

    click.echo(f"File {local} read from {remote} done")
    # show file digest
    with open(local, "rb") as f:
        data = f.read()
        hash = hashlib.sha256(data).digest()
        click.echo(f"File digest: {hash.hex()}")

@cli.command(name="fwrite", deprecated=True)
# fmt: off
@click.option("-l", "--local", required=True, help="Local file path to read from")
@click.option("-r", "--remote", required=True, help="Remote file path to write to on device")
@click.option("-cs", "--chunk-size", default=str(DEFAULT_CHUNK_SIZE), help="Chunk size in bytes to write")
# fmt: on
@with_client
def fwrite(client: "TrezorClient", local: str, remote: str, chunk_size: str) -> None:
    """Overwrite file to device.

    This command is deprecated, please use `write` command instead.
    """

    chunk_size = size_from_human(chunk_size)
    if chunk_size > MAX_CHUNK_SIZE:
        click.echo(f"Chunk size {chunk_size} is too large, max is {size_to_human(MAX_CHUNK_SIZE)}")
        exit(1)


    bar = tqdm_bar_create(f"writing {remote}", os.path.getsize(local))
    offset = 0
    with open(local, "rb") as f:
        total = os.path.getsize(local)
        while offset < total:
            data = f.read(chunk_size)
            processed = (offset + len(data))
            fs.file_write(client, remote, offset, data, processed * 100//total)
            bar.update(len(data))
            offset += len(data)
        bar.close()

    click.echo(f"File {local} written to {remote} done")
