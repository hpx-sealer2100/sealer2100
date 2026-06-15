import os
import time
import hashlib

from typing import TYPE_CHECKING, Any, Callable, Optional, Sequence

from . import messages
from .exceptions import Cancelled, TrezorException
from .tools import Address, expect, session

if TYPE_CHECKING:
    from .client import TrezorClient
    from .protobuf import MessageType


# stat
@expect(messages.FsInfo)
def stat(client: "TrezorClient", path: str) -> "MessageType":
    resp = client.call(messages.FsStat(path=path))
    return resp

# fs stat
@expect(messages.FsFsInfo)
def fs_stat(client: "TrezorClient") -> "MessageType":
    resp = client.call(messages.FsFsStat())
    return resp

# remove file or directory
@expect(messages.Success)
def remove(client: "TrezorClient", path: str) -> "MessageType":
    resp = client.call(messages.FsRemove(path=path))
    return resp

# mkdir
@expect(messages.Success)
def mkdir(client: "TrezorClient", path: str) -> "MessageType":
    resp = client.call(messages.FsMkdir(path=path))
    return resp

# read file
@expect(messages.FsChunk)
def read(client: "TrezorClient", path: str, offset: int, size: int) -> "MessageType":
    resp = client.call(messages.FsRead(path=path, offset=offset, size=size))
    return resp

@expect(messages.Success)
def write(client: "TrezorClient", path: str, total: int, offset: int, data: bytes) -> "MessageType":
    data_hash = hashlib.sha256(data).digest()
    chunk = messages.FsChunk(data=data, data_hash=data_hash)
    resp = client.call(messages.FsWrite(path=path, total=total, offset=offset, chunk=chunk))
    return resp

@expect(messages.Success)
def checksums(client: "TrezorClient") -> "MessageType":
    resp = client.call(messages.FsChecksums())
    return resp

@expect(messages.FsFile)
def file_read(client: "TrezorClient", path: str, offset: int, size: int) -> "MessageType":
    file = messages.FsFile(path = path, offset=offset, len = size)
    resp = client.call(messages.FsFileRead(file=file))
    return resp

@expect(messages.FsFile)
def file_write(client: "TrezorClient", path: str, offset: int, data: bytes, pct: int) -> "MessageType":
    file = messages.FsFile(path = path, offset=offset, len = len(data), data=data)
    resp = client.call(messages.FsFileWrite(file=file, overwrite=True, append=False, ui_percentage=pct))
    return resp
