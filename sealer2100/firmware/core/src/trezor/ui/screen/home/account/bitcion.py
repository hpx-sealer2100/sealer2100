import lvgl as lv
from typing import TYPE_CHECKING

from storage import device
from trezor import utils, workflow, log, loop
from trezor.ui import colors, i18n
from trezor.ui.screen import Navigation, manager
from trezor.ui.component import VStack, HStack, LabeledText
from trezor.ui.component.item import ItemWithtwoImg
from trezor.ui.theme import Styles
from trezor.ui.constants import *
from trezor.enums import InputScriptType
from .helper import parser_path
from .detail import Detail

if TYPE_CHECKING:
    from typing import List
    pass

# register a id for observer btc type changed
_BTC_ADDRESS_TYPE_CHANGED = lv.event_register_id()

# a `CoinProtocol` class
class Bitcoin:
    
    def get_name(self) -> str:
        return "Bitcoin"
    def get_icon(self) -> str:
        return "A:/res/account/btc-btc.png"
    def get_path(self) -> str:
        type = device.get_btc_address_type()
        return type.path()

    async def get_address(self) -> str:
        import_manager = utils.unimport()
        with import_manager:
            from trezor.wire import DUMMY_CONTEXT as ctx
            from apps.base import handle_Initialize
            from trezor.messages import Initialize, GetAddress
            from apps.bitcoin.get_address import get_address

            # step 1: initialize
            init = Initialize(session_id=b"\x00")
            await handle_Initialize(ctx, init)

            # step 2: get address
            type = device.get_btc_address_type()
            req = GetAddress(address_n=parser_path(self.get_path()), script_type=type.script_type())
            resp = await get_address(ctx, req)

            return resp.address

class BTCDetail(Detail):
    def __init__(self, coin):
        super().__init__(coin)

    def address_type_view(self, parent):
        type = device.get_btc_address_type()
        self._address_type = ItemWithtwoImg(parent, type.name(), "A:/res/btc-address-type.png", "A:/res/arrow-right_3.png")
        self._address_type.spacing(8)
        self._address_type.add_style(Styles.container, lv.PART.MAIN)
        self._address_type.add_style(Styles.board, lv.PART.MAIN)

        self._address_type.add_flag(lv.obj.FLAG.CLICKABLE)

        self._address_type.add_event_cb(self.on_address_type_changed, _BTC_ADDRESS_TYPE_CHANGED, None)
        self._address_type.add_event_cb(self.on_click_address_type, lv.EVENT.CLICKED, None)

    def on_address_type_changed(self, e: lv.event_t):
        type = device.get_btc_address_type()
        self._address_type.label.set_text(type.name())
        # reset cached address
        self._address = None

        async def update_view():
            address = await self.get_address()
            if self._address_view:
                self._address_view.set_text(address)
            if self._qrcode_view:
                data = address.encode()
                self._qrcode_view.update(data, len(data))

        workflow.spawn(update_view())

    def on_click_address_type(self, e: lv.event_t):
        screen = ChangeAddressType()
        screen.subscribers.append(self._address_type)
        if self._address_view:
            screen.subscribers.append(self._address_view)
        if self._qrcode_view:
            screen.subscribers.append(self._qrcode_view)
        workflow.spawn(screen.show())

    def address_view(self):
        self.content.clean()
        # reset qrcode view
        self._qrcode_view = None

        container = self.add(VStack)
        container.spacing(8)
        self.address_type_view(container)

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
        # reset address view
        self._address_view = None

        container = self.add(VStack)
        container.spacing(32)
        container.items_center()

        self.address_type_view(container)

        self._qrcode_view = lv.qrcode(container, 300, colors.USER.QRCODE_BG, colors.USER.QRCODE_FG)
        self._qrcode_view.add_style(Styles.qrcode, lv.PART.MAIN)
        data = self._address.encode()
        self._qrcode_view.update(data, len(data))

class ChangeAddressType(Navigation):
    def __init__(self):
        super().__init__()
        self.set_title(i18n.Title.select_derivation_path)
        self.subscribers: List[lv.obj] = []

        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Styles.container, lv.PART.MAIN)
        self.content.spacing(8)

        self.options = [AddressTypeOption(self.content, o) for o in device.BTCAddressType.suported()]

        self.group = lv.group_create()
        for item in self.options:
            self.group.add_obj(item)
        
        self.group.set_focus_cb(self.on_group_focus_changed)

    def on_loaded(self):
        super().on_loaded()
        # find the current and set focus
        current = self.current()

        item = utils.first(self.options, lambda item: item.option == current)
        lv.group_focus_obj(item)

    def dismiss(self):
        # 页面类型从Modal换成了Navigation
        # 在用户完成交易时，需要关闭当前页面
        super().dismiss()
        loop.spawn(manager.pop(self))

    @staticmethod
    def current() -> device.BTCAddressType:
        return device.get_btc_address_type()

    @staticmethod
    def save_option(option: device.BTCAddressType):
        device.set_btc_address_type(option)
    
    def on_deleting(self):
        super().on_deleting()
        self.group._del()

    def on_group_focus_changed(self, group: lv._group_t):
        obj = group.get_focused()
        item = utils.first(self.options, lambda item: obj == item)
        log.debug(__name__, f"user clicked: {item.option}")

        # 由于每次点击都会发送focused事件，这里判断下点击的地址类型时当前类型则不处理
        current = self.current()
        if item.option == current:
            log.debug(__name__, f"current: {current}, no need save")
            return

        self.save_option(item.option)

        for subscriber in self.subscribers:
            lv.event_send(subscriber, _BTC_ADDRESS_TYPE_CHANGED, None)

        self.close(None)

class AddressTypeOption(HStack):
    
    def __init__(self, parent, option: device.BTCAddressType):
        super().__init__(parent)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.add_style(Styles.board, lv.PART.MAIN)

        self.option = option

        obj = self.add(LabeledText)
        obj.set_width(lv.SIZE.CONTENT)
        obj.set_flex_grow(1)
        obj.set_label(self.option.name())
        obj.set_text(self.option.path())

        # state
        self.state = self.add(lv.img)
        self.state.set_src("A:/res/hp/ic_done.png")
        self.state.add_flag(lv.obj.FLAG.HIDDEN)

        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.add_event_cb(self.on_event, lv.EVENT.ALL, None)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def on_event(self, event: lv.event_t):
        code = event.code
        if code == lv.EVENT.CLICKED:
            # 当点击事件发生时，手动触发focus事件
            # obj是一个容器，测试发现在点击obj内部元素时，无法正确触发group的focus事件
            # 感觉时触发了lv.EVENT.FOCUSED/DEFOCUSED,但是没有触发到group的fouce事件
            # 只有点击self的pad的地方才可以触发group的focuse事件
            # 并且lvgl是发送自身的focused，再发送click事件
            # 所以此时的state已经时显示状态
            # if not self.state.has_flag(lv.obj.FLAG.HIDDEN):
            #     return
            lv.group_focus_obj(self)
        elif code == lv.EVENT.FOCUSED:
            self.state.clear_flag(lv.obj.FLAG.HIDDEN)
        elif code == lv.EVENT.DEFOCUSED:
            self.state.add_flag(lv.obj.FLAG.HIDDEN)
        