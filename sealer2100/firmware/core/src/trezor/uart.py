import ustruct
from micropython import const
from typing import TYPE_CHECKING

from storage import device
from trezor import config, io, log, loop, motor, utils
from trezor.ui.screen.statusbar import StatusBar
from trezor.ui.screen.bluetooth import BluetoothPairing
from trezor.ui import display

from apps import base

_PREFIX = const(42330)  # 0xA55A
_FORMAT = ">HHB"
_HEADER_LEN = const(5)
# fmt: off
_CMD_BLE_NAME = _PRESS_SHORT = _USB_STATUS_PLUG_IN = _BLE_STATUS_CONNECTED = _BLE_PAIR_SUCCESS = const(1)
_PRESS_LONG = _USB_STATUS_PLUG_OUT = _BLE_STATUS_DISCONNECTED = _BLE_PAIR_FAILED = _CMD_BLE_STATUS = const(2)
# fmt: on
_BLE_STATUS_OPENED = _POWER_STATUS_CHARGING = _CMD_BLE_PAIR_CODE = const(3)
_BLE_STATUS_CLOSED = _CMD_BLE_PAIR_RES = _POWER_STATUS_CHARGING_FINISHED = const(4)
_CMD_NRF_VERSION = const(5)  # ble firmware version
_CMD_DEVICE_CHARGING_STATUS = const(8)
_CMD_BATTERY_STATUS = const(9)
_CMD_SIDE_BUTTON_PRESS = const(10)
SCREEN: BluetoothPairing | None = None
BLE_ENABLED: bool | None = None
BLE_CTRL = io.BLE()

## TODO: split handlers to `trezor/tasks`

async def safe_reloop():
    from trezor import wire
    await wire.signal_ack()


async def handle_uart():
    fetch_all()
    while True:
        try:
            await process_push()
        except Exception as exec:
            if __debug__:
                log.exception(__name__, exec)
            loop.clear()
            return  # pylint: disable=lost-exception


async def handle_ble_info():
    while True:
        fetch_ble_info()
        await loop.sleep(500)


async def process_push() -> None:

    uart = loop.wait(io.UART | io.POLL_READ)

    response = await uart
    header = response[:_HEADER_LEN]
    prefix, length, cmd = ustruct.unpack(_FORMAT, header)
    if prefix != _PREFIX:
        # unexpected prefix, ignore directly
        return
    value = response[_HEADER_LEN:][: length - 2]
    log.debug(__name__, f"cmd == {cmd} with value {value} ")
    if cmd == _CMD_BLE_STATUS:
        # 1 connected 2 disconnected 3 opened 4 closed
        await _deal_ble_status(value)
    elif cmd == _CMD_BLE_PAIR_CODE:
        # show six bytes pair code as string
        await _deal_ble_pair(value)
    elif cmd == _CMD_BLE_PAIR_RES:
        # paring result 1 success 2 failed
        await _deal_pair_res(value)
    elif cmd == _CMD_BLE_NAME:
        # retrieve ble name has format: ^T[0-9]{4}$
        _retrieve_ble_name(value)
    elif cmd == _CMD_NRF_VERSION:
        # retrieve nrf version
        _retrieve_nrf_version(value)
    else:
        log.debug(__name__, f"unknown or not care command:{cmd}")


async def _deal_ble_pair(value):
    pair_codes = value.decode("utf-8")
    # pair_codes = "".join(list(map(lambda c: chr(c), ustruct.unpack(">6B", value))))
    utils.turn_on_lcd_if_possible()


    async def paring():
        if not device.is_initialized():
            with utils.unimport():
                from trezor.ui.screen.message import Message
                screen = Message("Wallet Setup Required", "To pair your device via Bluetooth, please create a wallet on your hardware device first. This is necessary for secure connection")
                await screen.show()
                await screen
                return

        if not config.is_unlocked():
            with utils.unimport():
                from apps.base import unlock_device
                try:
                    await unlock_device()
                except:
                    return

        global SCREEN
        # user maybe click button on screen, or user cancel pairing
        SCREEN = BluetoothPairing(pair_codes)
        await SCREEN.show()
        # 1. user click, delete the screen and unload it
        # 2. user cancel, `_deal_pair_res` will dismiss the screen, and unload it
        await SCREEN
        SCREEN = None

    # show pairing screen in another thread
    loop.spawn(paring())

async def _deal_pair_res(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if SCREEN:
        # cancel pairing
        SCREEN.dismiss()
    if res == _BLE_PAIR_FAILED:
        from trezor.ui.layouts import show_pairing_error
        await show_pairing_error()

async def _deal_ble_status(value: bytes) -> None:
    global BLE_ENABLED
    res = ustruct.unpack(">B", value)[0]
    if res == _BLE_STATUS_CONNECTED:
        utils.BLE_CONNECTED = True
        # show icon in status bar
        StatusBar.instance().show_ble(StatusBar.BLE_STATE_CONNECTED)
    elif res == _BLE_STATUS_DISCONNECTED:
        utils.BLE_CONNECTED = False
        if not BLE_ENABLED:
            return
        StatusBar.instance().show_ble(StatusBar.BLE_STATE_ENABLED)
        from trezor.ui.screen.lockscreen import set_busy
        set_busy(False)
        await safe_reloop()
    elif res == _BLE_STATUS_OPENED:
        BLE_ENABLED = True
        if utils.BLE_CONNECTED:
            return
        StatusBar.instance().show_ble(StatusBar.BLE_STATE_ENABLED)
        if config.is_unlocked():
            device.set_ble_status(enable=True)
    elif res == _BLE_STATUS_CLOSED:
        BLE_ENABLED = False
        StatusBar.instance().show_ble(StatusBar.BLE_STATE_DISABLED)
        if config.is_unlocked():
            device.set_ble_status(enable=False)


def _retrieve_ble_name(value: bytes) -> None:
    if value != b"":
        utils.BLE_NAME = value.decode("utf-8")
        # if config.is_unlocked():
        #     device.set_ble_name(utils.BLE_NAME)


def _retrieve_nrf_version(value: bytes) -> None:
    if value != b"":
        utils.BLE_VERSION = value.decode("utf-8")
        # if config.is_unlocked():
        #     device.set_ble_version(utils.BLE_VERSION)


def _request_ble_name():
    """Request ble name."""
    BLE_CTRL.ctrl(0x83, 0x01)


def _request_ble_version():
    """Request ble version."""
    BLE_CTRL.ctrl(0x83, 0x02)


def _request_battery_level():
    """Request battery level."""
    BLE_CTRL.ctrl(0x82, 0x04)


def _request_ble_status():
    """Request current ble status."""
    BLE_CTRL.ctrl(0x81, 0x04)

def fetch_all():
    """Request some important data."""
    _request_ble_name()
    _request_ble_version()
    _request_ble_status()
    _request_battery_level()


def fetch_ble_info():
    if not utils.BLE_NAME:
        BLE_CTRL.ctrl(0x83, 0x01)

    if not utils.BLE_VERSION:
        BLE_CTRL.ctrl(0x83, 0x02)

    global BLE_ENABLED
    if BLE_ENABLED is None:
        BLE_CTRL.ctrl(0x81, 0x04)


def ctrl_ble_enabled(enable: bool) -> None:
    """Request to open or close ble.
    @param enable: True to open, False to close
    """
    if not device.ble_enabled() and enable:
        BLE_CTRL.ctrl(0x81, 0x01)
    elif device.ble_enabled() and not enable:
        BLE_CTRL.ctrl(0x81, 0x02)

def ctrl_ble_disconnect():
    BLE_CTRL.ctrl(0x81, 0x03)

# def ctrl_power_off() -> None:
#     """Request to power off the device."""
#     BLE_CTRL.ctrl(0x82, 0x01)


def get_ble_name() -> str:
    """Get ble name."""
    return utils.BLE_NAME or ""


def get_ble_version() -> str:
    """Get ble version."""
    return utils.BLE_VERSION or ""


def is_ble_opened() -> bool:
    return BLE_ENABLED if BLE_ENABLED is not None else True
