"""
# Airgap service

Airgap implements `wire.Interface` interface. We can trade Airgap as a virtual wire interface.

                               ▲
                               │
                               │
                               │
                        `setup` in wire
                               │
                               │
                               │
                               │
┌──────────────────────────────┼────────────────────────────────┐
│                                                               │
│                    `wire.Interface`                           │
│                                                               │
└───────────────────────────────────────────────────────────────┘
│                              │                                │
│           read               │             write              │
│                              │                                │
└──────────────────────────────┘────────────────────────────────┘
│                                                               │
│                                                               │
│                                                               │
│              Airgap Service                                   │
│                                                               │
│  1. Use `camera` as input, scan UR format QR code as `ur`     │
│                                                               │
│  2. Use `screen` as output, draw UR format QR code            │
│                                                               │
│  3. Implement `wire.Interface` as a virtual interface         │
│                                                               │
│  4. User can setup `airgap` service to `wire`                 │
│                                                               │
│                                                               │
│  The process in `airgap` service while scanned `ur`           │
│                                                               │
│  1. switch `ur` do                                            │
│     support?                                                  │
│     yes) convert to `PB` and report to wire                   │
│     no ) show error on screen                                 │
│                                                               │
│  The process in `airgap` servce when upper write data         │
│                                                               │
│  1. Encode data in `UR` or `MUR`, single QR or animation QR   │
│                                                               │
│  2. Draw encoded on screen                                    │
│                                                               │
└────────────▲───────────────────────────────────────┬──────────┘
             │                                       │
             │                                       │
  ┌──────────┼──────────┐                 ┌──────────▼────────┐
  │                     │                 │                   │
  │        Camera       │                 │      Screen       │
  │                     │                 │                   │
  └─────────────────────┘                 └───────────────────┘
  """

import gc
import binascii
from trezor import io, log, loop, utils

from . import event
from .ur.ur_decoder import URDecoder
from .rust_ur.rust_ur import RustDecodedUR
from .rust_ur.rust_ur import QRCodeType

def scan() -> loop.spawn:
    airgap = Airgap.instance()
    airgap.reset()
    return loop.spawn(handle_scanning(airgap))

async def scaning_event():
    return await Airgap.instance().event_hub.take()

class Airgap():
    _instance :'Airgap' = None

    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = Airgap()
        return cls._instance

    def __init__(self) -> None:
        self.camera = loop.wait(io.CAMERA)
        self.reading = loop.chan()
        self.writing = loop.chan()
        self.event_hub = loop.chan()

        self.decoder = URDecoder()

    async def scan(self) -> bytes|str:
        return await self.camera

    # impl wire.Interface
    async def read(self) -> bytes:
        return await self.reading.take()

    async def write(self, data: bytes) -> int:
        return await self.writing.put(data)

    def reset(self) -> None:
        """
        User can reset the airgap , discard scanning state.
        e.g. when scanning failed, or scanned invalid QR code
        """
        self.decoder = URDecoder()

async def receive_message(airgap: Airgap) -> RustDecodedUR|None:
    rust_ur = RustDecodedUR()
    while True:
        data = await airgap.scan()
        log.debug(__name__, f"scanned: {data}")
        if isinstance(data, bytes):
            data = data.decode()

        success = rust_ur.decoding(data)
        if success:
            log.debug(__name__, "ur decode success")
            if rust_ur.is_start_decoding:
                if rust_ur.is_multi_part:
                    ur = event.MultiUR(100, 0)
                else:
                    ur = event.SingleUR()
                # publish `ScanStart`
                airgap.event_hub.publish(event.ScanStart(ur))

            if rust_ur.is_multi_part:
                log.debug(__name__, f"multi part process: {rust_ur.process}")
                ur = event.MultiUR(100, rust_ur.process)
                airgap.event_hub.publish(event.Scanning(ur))

            if not rust_ur.is_complete:
                continue

            if rust_ur.is_multi_part:
                ur = event.MultiUR(100, 100)
                airgap.event_hub.publish(event.Scanning(ur))

            return rust_ur
        else:
            log.debug(__name__, "ur process failed, continue scanning")


async def handle_scanning(airgap: Airgap) -> None:
    rust_ur = await receive_message(airgap)
    if not rust_ur.is_complete:
        return
    await handle_ur_message(rust_ur)
    log.debug(__name__, "airgap process done")

async def handle_ur_message(msg: RustDecodedUR):
    log.debug(__name__, f"handle_ur_message type: {msg.ur_type()}")
    unimport_manager = utils.unimport()
    with unimport_manager:
        from trezor import wire
        from trezor.wire import DUMMY_CONTEXT
        from .handler import find_handler
        from trezor.ui.layouts import show_popup, show_error
        from trezor.ui import i18n
        from apps.airgap import MFPNotMatch
        h = find_handler(msg.ur_type())
        if not h:
            log.error(__name__, f"no handler for type: {msg.ur_type()}")
            await show_popup(i18n.Text.unknown_tx_type)
            return
        try:
            await h(DUMMY_CONTEXT, msg)
            Airgap.instance().event_hub.publish(4)
        except wire.ActionCancelled:
            # user canceled the action, do nothing
            Airgap.instance().event_hub.publish(3)
            return
        except wire.UnexpectedMessage as e :
            log.error(__name__, f"error: {e.message}")
            await show_error(DUMMY_CONTEXT, i18n.Text.invalid_ur, i18n.Title.error, i18n.Button.try_again)
            Airgap.instance().event_hub.publish(3)
        except wire.DataError as e :
            log.error(__name__, f"error: {e.message}")
            await show_error(DUMMY_CONTEXT, i18n.Text.invalid_ur, i18n.Title.invalid_data, i18n.Button.try_again)
            Airgap.instance().event_hub.publish(3)
        except MFPNotMatch as e:
            log.exception(__name__, e)
            await show_error(DUMMY_CONTEXT, i18n.Text.wallet_not_match_desc, i18n.Text.wallet_not_match, i18n.Button.try_again)
            Airgap.instance().event_hub.publish(3)
        except wire.Error as e:
            log.error(__name__, f"error: {e.message}")
            await show_popup(e.message)
            Airgap.instance().event_hub.publish(3)
        except Exception as e:
            log.exception(__name__, e)
            await show_popup(i18n.Text.internal_error)
            Airgap.instance().event_hub.publish(3)
        finally:
            gc.collect()

