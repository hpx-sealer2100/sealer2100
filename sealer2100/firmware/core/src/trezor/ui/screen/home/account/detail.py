import lvgl as lv

from trezor import loop, workflow
from trezor.ui import i18n, font, colors
from trezor.ui.screen import Navigation
from trezor.ui.component import VStack, LabeledText
from trezor.ui.theme import Styles
from trezor.ui.constants import *
from .protocol import *

class Detail(Navigation):
    def __init__(self, coin: CoinProtocol):
        super().__init__()
        self.coin = coin
        title = i18n.Title.address.format(self.coin.get_name())
        self.set_title(title)
        self.set_subtitle("Account ##1")

        self.content: lv.obj
        self._address: str|None = None
        self._address_view: lv.label|None = None
        self._qrcode_view: lv.qrcode|None = None

        self.btn_left.set_text(i18n.Button.address)
        self.btn_left.mode('switch')
        self.btn_left.add_state(lv.STATE.DISABLED)

        self.btn_right.set_text(i18n.Button.qr_code)
        self.btn_right.mode('switch')

        self.btn_left.add_event_cb(self.on_click_btn_address, lv.EVENT.CLICKED, None)
        self.btn_right.add_event_cb(self.on_click_btn_qrcode, lv.EVENT.CLICKED, None)
        # default is address view
        self.address_view()

    def on_click_btn_address(self, e: lv.event_t):
        self.btn_left.add_state(lv.STATE.DISABLED)
        self.btn_right.clear_state(lv.STATE.DISABLED)

        self.address_view()

    def on_click_btn_qrcode(self, e: lv.event_t):
        self.btn_left.clear_state(lv.STATE.DISABLED)
        self.btn_right.add_state(lv.STATE.DISABLED)

        self.qrcode_view()

    def address_view(self):
        self.content.clean()

        container = self.add(VStack)
        container.spacing(8)

        self._address_view = container.add(lv.label)
        self._address_view.set_width(lv.pct(100))
        self._address_view.add_style(Styles.board, lv.PART.MAIN)
        # update address later
        self._address_view.set_text(' ')

        async def do_update_address():
            address = await self.get_address()
            self._address_view.set_text(address)

        workflow.spawn(do_update_address())

    def qrcode_view(self):
        self.content.clean()

        self._qrcode_view = lv.qrcode(self.content, 300, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        self._qrcode_view.add_style(Styles.qrcode, lv.PART.MAIN)
        self._qrcode_view.align(lv.ALIGN.TOP_MID, 0, 0)
        data = self._address.encode()
        self._qrcode_view.update(data, len(data))

    async def get_address(self) -> str:
        if self._address:
            return self._address

        await loop.sleep(300)
        from trezor.wire import ActionCancelled
        try: 
            address = await self.coin.get_address()
            self._address = address
            return self._address
        except ActionCancelled:
            self.on_nav_back(None)