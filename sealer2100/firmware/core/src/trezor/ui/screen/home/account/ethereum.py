from typing import TYPE_CHECKING
import lvgl as lv

from .helper import parser_path

from trezor import utils, workflow, loop, log
from trezor.ui import i18n, Style, theme, colors
from trezor.ui.screen import Navigation
from trezor.ui.component import VStack, LabeledText
from trezor.ui.constants import *

if TYPE_CHECKING:
    from typing import Literal, List

    AddressState = Literal["receive", "airgap"]

class Ethereum(Navigation):
    @staticmethod
    def get_name() -> str:
        return "ETH"

    @staticmethod
    def get_icon() -> str:
        return "A:/res/evm-eth.png"

    @staticmethod
    def get_path() -> str:
        return "m/44'/60'/0'/0/0"

    def __init__(self):
        super().__init__()
        # cache `receive_address` and `airgap_address`, lazy loaded
        self._receive_address = None
        self._airgap_address = None

        self.title.set_text(self.get_name())

        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Style().pad_left(16).pad_right(16), 0)

        # default is `receive` state
        self.state = "receive"

        # qr code
        self._qrcode_view: lv.qrcode = None

        # a tip
        self.tip = self.add(lv.label)
        self.tip.set_width(440)
        self.tip.set_long_mode(lv.label.LONG.WRAP)
        self.tip.set_text(i18n.Text.tap_switch_to_airgap)

        # address
        self.address_view = self.add(LabeledText)
        self.address_view.set_label(i18n.Text.address)
        self.address_view.set_text("")
        # set_text later

        # path
        path_view = self.add(LabeledText)
        path_view.set_label(i18n.Text.path)
        path_view.set_text(self.get_path())

    def on_loaded(self):
        super().on_loaded()
        # default is `receive` address
        async def load_address():
            # wait a while, not block the UI
            await loop.sleep(300)
            await self.do_update_receive_address()
        workflow.spawn(load_address())

    def on_click_qrcode(self, e):
        if self.state == "airgap":
            self.state = "receive"
            self.tip.set_text(i18n.Text.tap_switch_to_airgap)
            workflow.spawn(self.do_update_receive_address())
        else:
            self.state = "airgap"
            self.tip.set_text(i18n.Text.tap_switch_to_receive)
            workflow.spawn(self.do_update_airgap_address())

    @property
    def qrcode_view(self) -> lv.qrcode:
        if self._qrcode_view:
            return self._qrcode_view
        self._qrcode_view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        self._qrcode_view.add_style(theme.Styles.qrcode, lv.PART.MAIN)
        self._qrcode_view.center()
        self._qrcode_view.add_flag(lv.obj.FLAG.CLICKABLE)
        self._qrcode_view.add_event_cb(self.on_click_qrcode, lv.EVENT.CLICKED, None)
        return self._qrcode_view

    async def do_update_receive_address(self):
        if self._receive_address is not None:
            # use cached address
            self.qrcode_view.update(self._receive_address, len(self._receive_address))
            return

        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, EthereumGetAddress
            from apps.ethereum.get_address import get_address

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get address
            req = EthereumGetAddress(address_n=parser_path(self.get_path()))
            resp = await get_address(ctx, req)

            # cache address
            self._receive_address = resp.address

            # step 3: update qr code
            self.qrcode_view.update(self._receive_address, len(self._receive_address))
            # step 4: update address
            self.address_view.set_text(self._receive_address)

    async def do_update_airgap_address(self):
        if self._airgap_address is not None:
            # use cached address
            self.qrcode_view.update(self._airgap_address, len(self._airgap_address))
            return

        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, EthereumGetPublicKey
            from apps.ethereum.get_public_key import get_public_key

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get public key
            # path is m/44'/60'/0'/0/0
            # we hope app can watch m/44'/60'/0'/0/*'
            address_n = parser_path(self.get_path())
            address_n = address_n[:3]
            req = EthereumGetPublicKey(address_n=address_n)
            resp = await get_public_key(ctx, req)

            # step 2: get master key fingerprint
            address_n = address_n[:1]
            req = EthereumGetPublicKey(address_n=address_n)
            master_key = await get_public_key(ctx, req)

            # step 3: generate airgap address, aka HDKey
            from trezor.airgap.bc_types.coininfo import (
                CoinInfo,
                COIN_TYPE_ETH,
                NETWORK_MAINNET,
            )
            from trezor.airgap.bc_types.hdkey import HDKey
            from trezor.airgap.bc_types.keypath import KeyPath, PathComponent

            coin_info = CoinInfo(COIN_TYPE_ETH, NETWORK_MAINNET)
            # pk
            key_data = resp.node.public_key
            # chain code
            chain_code = resp.node.chain_code
            # source fingerprint
            source_fingerprint = resp.node.fingerprint
            # path is m/44'/60'/0'/0/0
            # we hope app can watch m/44'/60'/0'/0/*'
            # origin path, all hardened
            paths = PathComponent.parser_path(self.get_path())
            origin = KeyPath(paths[:3], master_key.node.fingerprint, 3)
            children = KeyPath([paths[3], PathComponent()], depth=0)
            hdkey = HDKey.derived(
                key_data,
                chain_code=chain_code,
                origin=origin,
                parent_fingerprint=source_fingerprint,
                children=children,
                use_info=coin_info,
                name="Sealer2001",
                note="eth.account.standard",
            )

            # UR encode
            from trezor.airgap.ur.ur import UR
            from trezor.airgap.ur.ur_encoder import UREncoder
            ur = UR(hdkey.type(), hdkey.cbor())
            data = UREncoder.encode(ur)

            # cache address, upper case
            self._airgap_address = data.upper()

            # step 4: update qr code
            self.qrcode_view.update(self._airgap_address, len(self._airgap_address))
