import sys
import utime
from micropython import const
from typing import TYPE_CHECKING
from .utils import EMULATOR
if TYPE_CHECKING:
    from typing import Any
    pass


NOTSET = const(0)
DEBUG = const(10)
INFO = const(20)
WARNING = const(30)
ERROR = const(40)
CRITICAL = const(50)

_leveldict = {
    DEBUG: ("DEBUG", "32"),
    INFO: ("INFO", "36"),
    WARNING: ("WARNING", "33"),
    ERROR: ("ERROR", "31"),
    CRITICAL: ("CRITICAL", "1;31"),
}

level = DEBUG
color = True

if EMULATOR:
    _SECONDS_1970_TO_2000 = const(946684800)
    def _format_timestamp(ms: int):
        s = ms //1000
        ms = ms % 1000
        d = utime.gmtime2000(s - _SECONDS_1970_TO_2000)
        return f"{d[0]}-{d[1]:02d}-{d[2]:02d} {d[3]:02d}:{d[4]:02d}:{d[5]:02d} {ms:03d}"

def _log(name: str, mlevel: int, msg: str, *args: Any) -> None:
    if __debug__ and mlevel >= level:
        from trezor import utils
        if color:
            fmt = (
                "%s \x1b[35m%s\x1b[0m \x1b["
                + _leveldict[mlevel][1]
                + "m%s\x1b[0m "
                + msg
            )
        else:
            fmt = "%s %s %s " + msg

        if EMULATOR:
            t = _format_timestamp(utime.ticks_ms())
        else:
            t = f"{utime.sleep_us}"
        print(fmt % ((t, name, _leveldict[mlevel][0]) + args))


def debug(name: str, msg: str, *args: Any) -> None:
    _log(name, DEBUG, msg, *args)


def info(name: str, msg: str, *args: Any) -> None:
    _log(name, INFO, msg, *args)


def warning(name: str, msg: str, *args: Any) -> None:
    _log(name, WARNING, msg, *args)


def error(name: str, msg: str, *args: Any) -> None:
    _log(name, ERROR, msg, *args)


def critical(name: str, msg: str, *args: Any) -> None:
    _log(name, CRITICAL, msg, *args)


def exception(name: str, exc: BaseException) -> None:
    # we are using `__class__.__name__` to avoid importing ui module
    # we also need to instruct typechecker to ignore the missing argument
    # in ui.Result exception
    if exc.__class__.__name__ == "Result":
        _log(
            name,
            DEBUG,
            "ui.Result: %s",
            exc.value,  # type: ignore[Cannot access member "value" for type "BaseException"]
        )
    elif exc.__class__.__name__ == "Cancelled":
        _log(name, DEBUG, "ui.Cancelled")
    else:
        _log(name, ERROR, "exception:")
        # since mypy 0.770 we cannot override sys, so print_exception is unknown
        sys.print_exception(exc)  # type: ignore ["print_exception" is not a known member of module]
