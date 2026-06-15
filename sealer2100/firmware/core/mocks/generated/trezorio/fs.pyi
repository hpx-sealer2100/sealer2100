from typing import *
FS_ERR_OK: int                  # (0) No error
FS_ERR_IO: int                  # (-5) Error during device operation
FS_ERR_CORRUPT: int             # (-84) Corrupted
FS_ERR_NOENT: int               # (-2) No directory entry
FS_ERR_EXIST: int               # (-17) Entry already exists
FS_ERR_NOTDIR: int              # (-20) Entry is not a dir
FS_ERR_ISDIR: int               # (-21) Entry is a dir
FS_ERR_NOTEMPTY: int            # (-39)  Dir is not empty
FS_ERR_BADF: int                # (-9) Bad file number
FS_ERR_FBIG: int                # (-27) File too large
FS_ERR_INVAL: int               # (-22) Invalid parameter
FS_ERR_NOSPC: int               # (-28) No space left on device
FS_ERR_NOMEM: int               # (-12) No more memory available
FS_ERR_NOATTR: int              # (-61) No data/attr available
FS_ERR_NAMETOOLONG: int         # (-36) File name too long
FS_ERR_NOSYS: int               # (-38) Function not implemented


# extmod/modtrezorio/modtrezorio-fs.h
class FSError(OSError):
    pass


# extmod/modtrezorio/modtrezorio-fs.h
class FSFile:
    """
    Class encapsulating file
    """

    def __enter__(self) -> FSFile:
        """
        Return an open file object
        """
    from types import TracebackType

    def __exit__(
        self, type: type[BaseException] | None,
        value: BaseException | None,
        traceback: TracebackType | None,
    ) -> None:
        """
        Close an open file object
        """

    def close(self) -> None:
        """
        Close an open file object
        """

    def read(self, data: bytearray) -> int:
        """
        Read data from the file
        """

    def write(self, data: bytes | bytearray) -> int:
        """
        Write data to the file
        """

    def seek(self, offset: int) -> None:
        """
        Move file pointer of the file object
        """

    def truncate(self, size: int = None) -> None:
        """
        Truncate the file
        :param size: New size of the file, if not set, truncate to current position
        """

    def sync(self) -> None:
        """
        Flush cached data of the writing file
        """


# extmod/modtrezorio/modtrezorio-fs.h
class FSDir(Iterator[tuple[int, str, str]]):
    """
    Class encapsulating directory
    """

    def close(self) -> None:
        """
        Close the directory
        """

    def __next__(self) -> tuple[int, str, str]:
        """
        Read an entry in the directory
        """


# extmod/modtrezorio/modtrezorio-fs.h
def open(path: str, flags: str) -> FSFile:
    """
    Open or create a file
    """


# extmod/modtrezorio/modtrezorio-fs.h
def dir_open(path: str) -> FSDir:
    """
    Open a directory (return generator)
    """


# extmod/modtrezorio/modtrezorio-fs.h
def mkdir(path: str) -> None:
    """
    Create a sub directory
    """


# extmod/modtrezorio/modtrezorio-fs.h
def remove(path: str) -> None:
    """
    Delete an existing file or directory
    """


# extmod/modtrezorio/modtrezorio-fs.h
def stat(path: str) -> tuple[int, str, str]:
    """
    Get file status
    """


# extmod/modtrezorio/modtrezorio-fs.h
def rename(oldpath: str, newpath: str) -> None:
    """
    Rename/Move a file or directory
    """


# extmod/modtrezorio/modtrezorio-fs.h
def mount() -> None:
    """
    Mount the filesystem.
    """


# extmod/modtrezorio/modtrezorio-fs.h
def unmount() -> None:
    """
    Unmount the filesystem.
    """


# extmod/modtrezorio/modtrezorio-fs.h
def is_mounted() -> bool:
   """
   Check if the filesystem is mounted.
   """


# extmod/modtrezorio/modtrezorio-fs.h
def format() -> None:
    """
    Format the filesystem.
    """
