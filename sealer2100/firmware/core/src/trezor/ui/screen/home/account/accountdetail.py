import lvgl as lv

from typing import TYPE_CHECKING

from trezor import loop, workflow ,log
from trezor.ui import i18n, Style, colors,font
from trezor.ui.component import VStack,LabeledText,Item
from trezor.ui.screen import Navigation,Modal
from trezor.ui.constants import *
from trezor.ui.theme import Styles

class AccountDetail(Navigation):
    def __init__(self, title: str,path:str,icon:str):
        super().__init__()
        self.set_scroll(True)
        self._path = path
        self._icon = icon
        self.title.set_text(title)
        self.create_content(VStack)
        self.content: VStack
        self.content.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.content.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self._qrcode_view: lv.qrcode = None

    def on_loaded(self):
        super().on_loaded()
        # default is `receive` address
        async def load_address():
            # wait a while, not block the UI
            await loop.sleep(300)
            await self.do_update_address()
        workflow.spawn(load_address())

    @property
    def qrcode_view(self) -> lv.qrcode:
        if self._qrcode_view:
            return self._qrcode_view
        self._qrcode_view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        self._qrcode_view.add_style(Styles.qrcode, lv.PART.MAIN)
        return self._qrcode_view

    async def do_update_address(self):
        #TODO get address
        address = await self.get_address()
        self.qrcode_view.update(address, len(address))
        self.qrcode_view.set_style_pad_left(30, lv.PART.MAIN)
        group = self.add(VStack)
        group.add_style(Styles.board, lv.PART.MAIN)
        group.add_style(
            Style()
            .width(432)
            .height(lv.SIZE.CONTENT)
            .pad_row(0)
            .pad_all(0),
            lv.PART.MAIN
        )
        group.set_scroll_dir(lv.DIR.TOP | lv.DIR.BOTTOM)
        group.align_to(self.content,lv.ALIGN.TOP_MID,0,408)
        self.address_view = LabeledText(group)
        self.address_view.set_label(i18n.Text.address)

        self.address_view.set_text1(address)
        self.address_view.label.set_style_text_color(lv.color_hex(0x666666), lv.PART.MAIN)
        self.address_view.desc.set_style_text_color(colors.USER.BLACK, lv.PART.MAIN)

        self.path_view = LabeledText(group)
        self.path_view.set_style_pad_top(0, lv.PART.MAIN)
        self.path_view.set_label(i18n.Text.path)
        self.path_view.set_text1(self._path)

        self.path_view.label.add_style(
            Style()
            .pad_top(CONTENT_PAD)
            .border_width(2)
            .border_opa(lv.OPA.COVER)
            .border_color(lv.color_hex(0xE1E6EE))
            .border_side(lv.BORDER_SIDE.TOP),
            lv.PART.MAIN
        )
        self.path_view.label.set_style_text_color(lv.color_hex(0x666666), lv.PART.MAIN)
        self.path_view.desc.set_style_text_color(colors.USER.BLACK, lv.PART.MAIN)
class AccountAddress(Modal):
    def __init__(self,title:str,path:str,address,icon:str):
        super().__init__()
        self.set_scroll(True)
        self._path = path
        self._icon = icon
        self._address = address
        self.title.set_text(title)
        self.subtitle.set_text(i18n.Subtitle.account_qr_address)
        self.btn_left.set_text(i18n.Button.address)
        self.btn_left.mode("info")
        self.btn_right.set_text(i18n.Button.done)
        self.create_content(VStack)
        self.content: VStack
        self.content.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        # self.content.set_style_pad_left(CONTENT_PAD, lv.PART.MAIN)
        self.content.set_style_pad_bottom(CONTENT_PAD, lv.PART.MAIN)
        self._qrcode_view: lv.qrcode = None

        self.address_view = Item(self.content,address,None)
        self.address_view.align(lv.ALIGN.BOTTOM_MID, 0, CONTENT_PAD)
        self.address_view.add_flag(lv.obj.FLAG.HIDDEN)
        self.address_view.label.set_style_text_font(font.Regular.PF28, lv.PART.MAIN)

        self.path_view = LabeledText(self.content)
        self.path_view.set_label7(i18n.Text.path)
        self.path_view.set_text1(path)
        self.path_view.add_style(
            Style()
            .radius(16)
            .bg_color(colors.USER.WHITE)
            .bg_opa(255)
            .border_width(0)
            .width(432)
            .height(lv.SIZE.CONTENT)
            .pad_all(CONTENT_PAD),
            0,
        )
        self.path_view.add_flag(lv.obj.FLAG.HIDDEN)

        self.path_view.label.set_style_text_color(lv.color_hex(0x666666), lv.PART.MAIN)
        self.path_view.desc.set_style_text_color(colors.USER.BLACK, lv.PART.MAIN)
        self.btn_left.add_event_cb(self.on_change, lv.EVENT.CLICKED, None)
        self.btn_right.add_event_cb(self.on_done, lv.EVENT.CLICKED, None)

    def on_loaded(self):
        super().on_loaded()
        # default is `receive` address
        async def load_address():
            # wait a while, not block the UI
            await loop.sleep(300)
            await self.do_update_address()
        workflow.spawn(load_address())

    @property
    def qrcode_view(self) -> lv.qrcode:
        if self._qrcode_view:
            return self._qrcode_view
        self._qrcode_view = lv.qrcode(self.content, QRCODE_SIZE, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        self._qrcode_view.add_style(Styles.qrcode, lv.PART.MAIN)
        self._qrcode_view.center()
        return self._qrcode_view

    async def do_update_address(self):
        self.qrcode_view.update(self._address, len(self._address))
        self.qrcode_view.set_style_pad_left(30, lv.PART.MAIN)

    def on_change(self, _: lv.event_t):
        if self.path_view.has_flag(lv.obj.FLAG.HIDDEN):
           self.path_view.clear_flag(lv.obj.FLAG.HIDDEN)
           self.address_view.clear_flag(lv.obj.FLAG.HIDDEN)
           self._qrcode_view.add_flag(lv.obj.FLAG.HIDDEN)
           self.subtitle.add_flag(lv.obj.FLAG.HIDDEN)
           self.btn_left.set_text(i18n.Button.qr_code)
           self.content.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        else:
           self.btn_left.set_text(i18n.Button.address)
           self._qrcode_view.clear_flag(lv.obj.FLAG.HIDDEN)
           self.subtitle.clear_flag(lv.obj.FLAG.HIDDEN)
           self.path_view.add_flag(lv.obj.FLAG.HIDDEN)
           self.address_view.add_flag(lv.obj.FLAG.HIDDEN)
           self.content.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

    def on_done(self, _: lv.event_t):
        log.debug(__name__, f"event: click done ")

