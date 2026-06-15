from trezor import io, wire
from trezor.messages import FileInfo, FileInfoList, ListResDir


async def list_dir(ctx: wire.Context, msg: ListResDir) -> FileInfoList:
    try:
        files = []
        for size, attrs, name in io.fs.dir_open(msg.path):
            if attrs[1] != "h":
                files.append(FileInfo(name=name, size=size))
    except io.fs.FSError as e:
        raise wire.DataError(f"Fs error {e}")

    return FileInfoList(files=files)
