from micropython import const

import binascii
from trezor import loop, io, log
from trezorio import Iris as IrisDevice

from typing import TYPE_CHECKING

IRIS = IrisDevice()

if TYPE_CHECKING:
    from typing import Literal, NewType, Optional, Union, Awaitable, Callable, TypeVar

    MsgType = Literal[0, 1, 2]

    MsgId = NewType("MsgId", int)

    T = TypeVar("T")
    TimeoutedResult = Union[T, TimeoutError]

_iris_version: Optional[str] = None
_iris_sn: Optional[str] = None

REGIST_TIMEOUT = const(30)
MATCH_TIMEOUT = const(30)

MSG_TYPE_REQUEST = const(0)
MSG_TYPE_REPLY = const(1)
MSG_TYPE_NOTE = const(2)

## msg id
MSG_ID_REGIST = const(0x01)
MSG_ID_REGIST_FACE = const(0x02)
MSG_ID_CANCEL_OPERATE = const(0x03)
MSG_ID_MATCH = const(0x04)
MSG_ID_USER_GET_COUNT = const(0x05)
MSG_ID_USER_GET_ID_ALL = const(0x06)
MSG_ID_USER_GET_BY_ID = const(0x07)
MSG_ID_DEVICE_GET_STATUS = const(0x08)
MSG_ID_USER_DEL_BY_ID = const(0x09)
MSG_ID_USER_DEL_ALL = const(11)
MSG_ID_DEVICE_GET_SERIAL_CODE = const(12)
MSG_ID_DEVICE_GET_VERSION = const(13)
MSG_ID_USER_ADD = const(14)
MSG_ID_DEVICE_RESET = const(20)
MSG_ID_DEVICE_REBOOT = const(21)
MSG_ID_DEVICE_POWERDOWN = const(22)
MSG_ID_DEVICE_REBOOT_LOADER = const(26)
MSG_ID_NOTE_MODULE_STATUS = const(50)
MSG_ID_NOTE_REGIST_PROGRESS = const(51)
MSG_ID_NOTE_IRIS_STATUS = const(52)
MSG_ID_NOTE_VIDEO_STREAM = const(53)
MSG_ID_NOTE_OTA_STATUS = const(54)

# 在注册、识别虹膜时，虹膜的距离状态 0: 正常, 1:太近, 2:太远, 3:未找到
IRIS_DISTANCE_STATE_NORMAL = const(0)
IRIS_DISTANCE_STATE_CLOSE = const(1)
IRIS_DISTANCE_STATE_FAR = const(2)
IRIS_DISTANCE_STATE_NONE = const(3)

# 模组启动成功（ready)
MODULE_STATUS_READY = const(1)
# 模组启动识别（match）
MODULE_STATUS_MATCH = const(2)
# 模组启动注册（enroll）
MODULE_STATUS_ENROLL = const(3)
# 模组绑定状态（bind）
MODULE_STATUS_BIND = const(4)

# user defined
# 模组正在启动
MODULE_STATUS_IRIS_STARTING = const(0x80)
# 模组启动超时
MODULE_STATUS_IRIS_STARTING_TIMEOUT = const(0x81)
# 注册或识别完成
MODULE_STATUS_DONE = const(0x82)

IRIS_HANDSHAKE_PENDING = const(1)

class TimeoutError(Exception):
    pass


# helper decorator
def with_timeout(timeout: int):
    def decorator(func: Callable[..., Awaitable[T]], *args, **kwargs) -> Callable[..., Awaitable[TimeoutedResult]]:
        async def wrapper(*args, **kwargs):
            timeout_task = loop.sleep(timeout)
            main_task = func(*args, **kwargs)
            racer = loop.race(main_task, timeout_task)
            result = await racer
            if timeout_task in racer.finished:
                raise TimeoutError()
            return result
        return wrapper
    return decorator

# Message 分为两种 Request 和 Response
# Request app 向虹膜模组发送的消息
# Response app 从虹膜模组收到的消息

class Request:
    """
    向模组发送请求
    """
    payload: Optional[bytes]

    def __init__(self, id: MsgId):
        self.id = id
        self.type = MSG_TYPE_REQUEST
        self.payload = None

    def to_bytes(self) -> bytes:
        payload = bytearray([self.id, self.type])
        payload_len = len(self.payload) if self.payload else 0
        payload.extend(payload_len.to_bytes(2, "little"))
        if self.payload is not None:
            payload.extend(self.payload)
        return payload

class Regist(Request):
    """
    注册请求
    """
    def __init__(self, time_out: int):
        super().__init__(MSG_ID_REGIST)
        self.payload = bytearray([
            3, # left and right
            time_out, # timeout
            1, # auto generate user id
        ])
        self.payload.extend(b'\x00'*32)

class Powerdown(Request):
    """
    关机请求
    """
    def __init__(self):
        super().__init__(MSG_ID_DEVICE_POWERDOWN)

class Match(Request):
    """
    识别请求
    """
    def __init__(self):
        super().__init__(MSG_ID_MATCH)
        self.payload = bytes([
            2,
            MATCH_TIMEOUT,
        ])

class Response:
    """
    从模组收到的消息
    """

    @staticmethod
    def from_bytes(data: bytes) -> Union['Reply', 'Note']:
        if len(data) < 4:
            raise ValueError("Message data too short")
        type: MsgType = data[1]
        if type == 1:
            return Reply.from_bytes(data)
        elif type == 2:
            return Note.from_bytes(data)
        else:
            raise ValueError("Unknown message type")

class Reply(Response):
    def __init__(self, id: MsgId, payload: bytes):
        self.id = id
        self.type = MSG_TYPE_REPLY
        self.code = payload[0]
        self.data = payload[1:]

    def error_msg(self) -> str:
        return error_code_str(self.code)

    @staticmethod
    def from_bytes(data: bytes) -> "Reply":
        if len(data) < 5:
            raise ValueError("Message data too short")
        id: MsgId = data[0]
        _type: MsgType = data[1]
        payload_len = int.from_bytes(data[2:4], "little")
        if len(data) != 4 +payload_len:
            raise ValueError("Message data length mismatch")
        payload = data[4:] if payload_len > 0 else None
        return Reply(id, payload)

    def is_success(self) -> bool:
        return self.code == 0

    def is_timeout(self) -> bool:
        return self.code == 3

    def __str__(self):
        if self.data:
            return f"{{id: {self.id}, type: {self.type}, code: {self.code}, data: {binascii.hexlify(self.data)}}}"
        else:
            return f"{{id: {self.id}, type: {self.type}, code: {self.code}}}"

class Note(Response):
    def __init__(self, id: MsgId, payload: bytes):
        self.id = id
        self.type = MSG_TYPE_NOTE
        self.payload = payload

    @staticmethod
    def from_bytes(data: bytes) -> "Note":
        if len(data) < 4:
            raise ValueError("Message data too short")

        id: MsgId = data[0]
        _type: MsgType = data[1]
        payload_len = int.from_bytes(data[2:4], "little")
        if len(data) != 4 + payload_len:
            raise ValueError("Message data length mismatch")

        payload = data[4:] if payload_len > 0 else None
        if id == MSG_ID_NOTE_MODULE_STATUS:
            return ModuleStatus(id, payload)
        elif id == MSG_ID_NOTE_REGIST_PROGRESS:
            return RegistProgress(id, payload)
        elif id == MSG_ID_NOTE_IRIS_STATUS:
            return IrisStatus(id, payload)
        else:
            return None

class ModuleStatus(Note):
    '''
    模组启动后，将会主动发该消息。
    '''
    def __init__(self, id, payload):
        super().__init__(id, payload)
        self.status = self.payload[0]

    def __str__(self):
        if self.status < 1 or self.status > 3:
            return ""
        from trezor.ui import i18n
        return getattr(i18n.Text, f'iris_note_module_status_{self.status}')

class RegistProgress(Note):
    """
    在注册进度变化时发送。
    """
    def __init__(self, id, payload):
        super().__init__(id, payload)
        self.left = self.payload[0]
        self.right = self.payload[1]

    def __str__(self):
        from trezor.ui import i18n
        return i18n.Text.iris_note_regist_progress.format(self.left, self.right)

class IrisStatus(Note):
    """
    在注册或识别时，提示用户当前的虹膜状态

    距离位置：合适，太近，太远。在注册或识别时，100ms 主动发送一次。
    """
    def __init__(self, id, payload):
        super().__init__(id, payload)
        self.status = self.payload[0]
        self.distance = int.from_bytes(self.payload[1:], 'little')
        # mm -> cm
        self.distance //= 10

    def __str__(self):
        if self.status < 0 or self.status > 3:
            return ""
        from trezor.ui import i18n
        status = getattr(i18n.Text, f'iris_note_iris_status_{self.status}')
        return f"{status}: {self.distance} cm"

def error_code_str(code: int) -> str:
    from trezor.ui import i18n
    if code < 0 or code > 15:
        # remap
        code = 1
    return getattr(i18n.Text, f'iris_code_{code}')

def send_request(req: Request) -> None:
    msg = req.to_bytes()
    log.debug(__name__, f"write: {binascii.hexlify(msg)}")
    IRIS.write(msg)

async def wait_response() -> Union[Reply, Note]:
    data = await loop.wait(io.IRIS | io.POLL_READ)
    log.debug(__name__,f"read: {binascii.hexlify(data)}")
    resp = Response.from_bytes(data)
    log.debug(__name__,f"resp: {resp}")
    return resp

@with_timeout(1000)
async def wait_response_with_timeout() -> Union[Reply, Note]:
    """
    Wait for response with timeout
    """
    try:
        return await wait_response()
    except TimeoutError:
        return None

@with_timeout(1500)
async def power_up() -> ModuleStatus:
    """
    Power up iris module
    """
    if IRIS.is_opened():
        return

    log.debug(__name__, "Power up iris ...")
    IRIS.open()
    resp = await wait_response()

    if not isinstance(resp, ModuleStatus):
        log.exception(__name__, f"Invalid response: {resp}")
        raise RuntimeError("IRIS open failed")
    log.debug(__name__, "Power up success success")
    return resp

@with_timeout(1000)
async def get_module_status() -> ModuleStatus:
    """
    Get module status
    """
    req = Request(MSG_ID_DEVICE_GET_STATUS)
    send_request(req)
    resp = await wait_response()
    if not isinstance(resp, ModuleStatus):
        raise RuntimeError("IRIS get module status failed")
    return resp

async def do_sec_channel_open() -> None:
    """
    Do sec channel open
    """
    while True:
        ret: int|None = IRIS.sec_channel_open()
        if ret is None:
            log.debug(__name__, "sec channel open success")
            return
        if ret == IRIS_HANDSHAKE_PENDING:
            await loop.sleep(10)

async def open() -> None:
    """
    Open iris Module

    Power up iris module and connect to it.
    """
    log.debug(__name__, "Open iris ...")
    if IRIS.is_opened():
        return
    try:
        resp = await power_up()
    except TimeoutError:
        log.debug(__name__, "Power up iris timeout, get module status")
        resp = await get_module_status()

    if resp.status == MODULE_STATUS_BIND:
        await do_sec_channel_open()
    elif resp.status != MODULE_STATUS_READY:
        raise RuntimeError("IRIS open failed")
    log.debug(__name__, "Open success")

    return resp

@with_timeout(1000)
async def send_power_down() -> None:
    """
    Send power down command
    """
    log.debug(__name__, "Power down iris ...")
    req = Powerdown()
    send_request(req)
    resp = await wait_response()
    if not isinstance(resp, Reply):
        raise RuntimeError("IRIS power down failed")
    if resp.code != 0:
        raise RuntimeError("IRIS power down failed")
    log.debug(__name__, "Power down command success")


async def close():
    """
    Close iris Module

    Power down iris module and disconnect from it.
    """
    try:
        # send power down command to iris module
        await send_power_down()
    except Exception as e:
        # close iris module whatever happens
        log.exception(__name__, e)
    # wait a moment to make sure iris module power down
    await loop.sleep(50)
    log.debug(__name__, "iris close")
    IRIS.close()

@with_timeout(1000)
async def cancel():
    """
    Cancel iris operation
    """
    req = Request(MSG_ID_CANCEL_OPERATE)
    send_request(req)
    await wait_response()

@with_timeout(1000)
async def regist():
    """
    Start iris registration

    return a wait object for caller to get the result or status
    """
    req = Regist(REGIST_TIMEOUT)
    send_request(req)
    resp = await wait_response()
    if not isinstance(resp, ModuleStatus):
        raise RuntimeError("IRIS start regist failed")
    if resp.status != MODULE_STATUS_ENROLL:
        raise RuntimeError("IRIS start regist failed")

    log.debug(__name__, "Start regist success")

    return resp

@with_timeout(1000)
async def match():
    """
    Start iris match

    return a wait object for caller to get the result or status
    """
    req = Match()
    send_request(req)
    resp = await wait_response()

    if not isinstance(resp, ModuleStatus):
        raise RuntimeError("IRIS start match failed")
    if resp.status != MODULE_STATUS_MATCH:
        raise RuntimeError("IRIS start match failed")

    log.debug(__name__, "Start match success")

    return resp

@with_timeout(1000)
async def del_user_by_id(id: bytes):
    """
    Delete user by id
    """
    req = Request(MSG_ID_USER_DEL_BY_ID)
    req.payload = id
    send_request(req)
    resp = await wait_response()
    if not isinstance(resp, Reply):
        raise RuntimeError("IRIS delete user by id failed")
    if resp.code != 0:
        raise RuntimeError("IRIS delete user by id failed")
    log.debug(__name__, "Delete user by id success")


@with_timeout(1000)
async def do_wipe():
    req = Request(MSG_ID_USER_DEL_ALL)
    send_request(req)
    resp = await wait_response()
    if not isinstance(resp, Reply):
        raise RuntimeError("IRIS wipe failed")
    if resp.code != 0:
        raise RuntimeError("IRIS wipe failed")
    log.debug(__name__, "Wipe success")

async def wipe():
    """
    Delete all users
    """
    log.debug(__name__, "iris wipeing...")
    from trezor import utils
    if utils.EMULATOR:
        await loop.sleep(800)
        return
    # wait a moment to make sure iris module has power down
    # when only enable iris verification, before wipe, need `match` first
    # IRIS will power up and match then power down, so need to wait a moment
    await loop.sleep(1000)
    await open()
    await loop.sleep(200)
    await do_wipe()
    await close()

@with_timeout(1000)
async def get_version() -> str:
    """
    Get iris module version
    """
    global _iris_version
    if _iris_version is not None:
        log.debug(__name__, f"version (cached): {_iris_version}")
        return _iris_version

    req = Request(MSG_ID_DEVICE_GET_VERSION)
    send_request(req)
    resp = await wait_response()
    if not isinstance(resp, Reply):
        raise RuntimeError("IRIS get version failed")
    _iris_version = resp.data.decode("utf-8").rstrip('\x00')
    log.debug(__name__, f"version: {_iris_version}")
    return _iris_version

async def get_sn() -> str:
    """
    Get iris module serial number
    """
    global _iris_sn
    if _iris_sn is not None:
        return _iris_sn

    req = Request(MSG_ID_DEVICE_GET_SERIAL_CODE)
    send_request(req)
    resp = await wait_response()
    if not isinstance(resp, Reply):
        raise RuntimeError("IRIS get serial number failed")
    _iris_sn = resp.data.decode("utf-8")
    return _iris_sn

async def refresh_iris_version():
    from trezor import utils
    if utils.EMULATOR:
        version = "iris-emulator"
        utils.IRIS_VERSION = version
        return
    try:
        await open()
        version = await get_version()
        utils.IRIS_VERSION = version
    except Exception as e:
        log.exception(__name__, e)
    finally:
        await close()

