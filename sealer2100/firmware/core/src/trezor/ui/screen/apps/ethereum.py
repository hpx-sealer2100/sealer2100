import lvgl as lv

from typing import TYPE_CHECKING

from .template import TransactionBase

from trezor import log
from trezor.ui import i18n, theme, colors, font, Style
from trezor.ui import Confirm, Reject, Continue, Cancel, More, Detail
from trezor.ui.component import VStack, LabeledText, HStack, DividerLine, ItemImg
from trezor.ui.screen import Modal
from trezor.ui.screen.confirm import HolderConfirm
from trezor.ui.constants import CONTENT_PAD
from trezor.ui.theme import Styles

# import TransactionOverview in `ethereum` namespace
from trezor.ui.screen.apps.template import TransactionOverview

if TYPE_CHECKING:
    from typing import Literal, Iterable

    pass


class TypedHash(TransactionBase):
    def __init__(self, domain_hash: str, message_hash: str, *, coin: str|None = None):
        super().__init__()
        self.set_title(i18n.Title.typed_hash.format(coin or ""))

        self.content.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(self.content, label, text)
        # domain hash
        labeled(i18n.Text.domain_hash, domain_hash)
        self.add(DividerLine)

        # message hash
        labeled(i18n.Text.message_hash, message_hash)


class Eip712(TransactionBase):
    def __init__(self, title: str, **kwargs):
        super().__init__()
        self.set_title(title)

        self.content.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(self.content, label, text)

        line = None
        for k in ("name", "version", "chainId", "verifyingContract", "salt"):
            if (v := kwargs.get(k)) is not None:
                labeled(f"{k} :", v)
                line = self.add(DividerLine)

        if line:
            line.delete()


class ShowMore(TransactionBase):
    def __init__(self, title: str, param: Iterable[str]):
        super().__init__()
        self.set_title(title)

        self.content.add_style(Styles.group, lv.PART.MAIN)

        param = iter(param)
        # use first as label
        self.btn_more_label = next(param)
        # use remained as content
        content = "\n".join(param)
        item = self.add(LabeledText)
        item.set_label(self.btn_more_label)
        item.set_text(content)
        self.add(DividerLine)

        # view detail button
        self.btn_more = self.add(HStack)
        self.btn_more.set_height(lv.SIZE.CONTENT)
        self.btn_more.add_style(Styles.container, lv.PART.MAIN)
        self.btn_more.add_flag(lv.obj.FLAG.CLICKABLE)

        img = self.btn_more.add(lv.img)
        img.set_src("A:/res/hp/down.png")

        self.btn_more_label = self.btn_more.add(lv.label)
        self.btn_more_label.set_text(i18n.Button.view_more)
        self.btn_more_label.add_style(
            Style()
            .text_font(font.small)
            .text_color(colors.USER.WHITE),
            lv.PART.MAIN
        )

        self.btn_more.add_event_cb(lambda e: self.close(More()), lv.EVENT.CLICKED, None)

    def btn_more_text(self, text: str):
        self.btn_more_label.set_text(text)

class TransactionDetail(TransactionBase):
    def __init__(
        self,
        amount: str,
        from_: str,
        to: str,
        fee_max: str,
        gas_price: str,
        total: str | None,
        icon: str | None,
    ):
        super().__init__()

        self.set_icon(icon)
        self.set_title(i18n.Text.send)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(amount)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        self.container = self.add(VStack)
        self.container.add_style(Styles.container, lv.PART.MAIN)
        self.container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(self.container, label, text)
        # to
        labeled(i18n.Text.receiver, to)
        self.container.add(DividerLine)

        # from
        labeled(i18n.Text.from_, from_)
        self.container.add(DividerLine)

        # max fee
        labeled(i18n.Text.max_fee, fee_max)
        self.container.add(DividerLine)

        # gas price
        item = labeled(i18n.Text.gas_price, gas_price)
        self.container.add(DividerLine)

        # total
        if total is not None:
            item = labeled(i18n.Text.total, total)
            item.label.set_style_text_font(font.bold, lv.PART.MAIN)

    def show_contract_address(self, address: str):
        self.container.add(DividerLine)
        self.labeled(self.container, i18n.Text.contract, address)

    def show_token_id(self, id: int):
        self.container.add(DividerLine)
        self.labeled(self.container, "Token ID:", str(id))

    def show_chain_id(self, id: int):
        self.container.add(DividerLine)
        self.labeled(self.container, "Chain ID:", str(id))

    def show_raw_data(self, data: bytes):
        from ubinascii import hexlify
        self.container.add(DividerLine)
        self.labeled(self.container, "Raw:", hexlify(data).decode())

class TransactionDetail1559(TransactionBase):
    def __init__(
        self,
        amount: str,
        from_: str,
        to: str,
        fee_max: str,
        max_priority_fee_per_gas: str,
        max_fee_per_gas: str,
        total: str,
        icon: str | None,
    ):
        super().__init__()

        self.set_icon(icon)
        self.set_title(i18n.Text.send)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(amount)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)
        # to
        labeled(i18n.Text.receiver, to)
        container.add(DividerLine)
        # from
        labeled(i18n.Text.from_, from_)
        container.add(DividerLine)

        labeled(i18n.Text.max_fee, fee_max)
        container.add(DividerLine)

        # max priority fee per gas
        labeled(i18n.Text.max_priority_fee_per_gas, max_priority_fee_per_gas)
        container.add(DividerLine)

        # max fee per gas
        labeled(i18n.Text.max_fee_per_gas, max_fee_per_gas)

        # total
        if total is not None:
            container.add(DividerLine)
            labeled(i18n.Text.total, total)

    def show_contract_address(self, address: str):
        item = self.add(LabeledText)
        item.set_label(i18n.Text.contract)
        item.set_text(address)
        #设置颜色为白色
        item.set_style_text_color(colors.STD.WHITE, lv.PART.MAIN)

    def show_token_id(self, id: int):
        item = self.add(LabeledText)
        item.set_label("Token ID:")
        item.set_text(str(id))

    def show_chain_id(self, id: int):
        item = self.add(LabeledText)
        item.set_label("Chain ID:")
        item.set_text(str(id))

    def show_raw_data(self, data: bytes):
        item = self.add(LabeledText)
        item.set_label("Raw:")
        from ubinascii import hexlify
        item.set_text(hexlify(data).decode())

class ApproveOverview(TransactionBase):
    def __init__(
        self,
        approve_to: str,
        allowance: str,
        contract: str,
    ):
        super().__init__()

        self.set_icon("A:/res/approve-confirm.png")
        self.set_title(i18n.Title.approve_confirm)

        # desc 
        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)
        container.set_style_border_color(colors.USER.GREEN, lv.PART.MAIN)

        item = ItemImg(container, i18n.Text.approve_confirm_desc, "A:/res/hp/ic_infotip-green.png","left")
        item.label.set_style_text_color(colors.USER.GREEN, lv.PART.MAIN)
        item.set_width(lv.pct(100))

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)

        # approve to
        labeled(i18n.Text.approve_to, approve_to)
        container.add(DividerLine)

        # allowance
        labeled(i18n.Text.allowance, allowance)
        container.add(DividerLine)
        # contract
        labeled(i18n.Text.contract, contract)
        container.add(DividerLine)

        # view detail button
        self.btn_detail = container.add(HStack)
        self.btn_detail.set_height(lv.SIZE.CONTENT)
        self.btn_detail.add_style(Styles.container, lv.PART.MAIN)
        self.btn_detail.add_flag(lv.obj.FLAG.CLICKABLE)

        img = self.btn_detail.add(lv.img)
        img.set_src("A:/res/hp/down.png")

        label = self.btn_detail.add(lv.label)
        label.set_text(i18n.Text.detail)
        label.add_style(
            Style()
            .text_font(font.small)
            .text_color(colors.USER.WHITE),
            lv.PART.MAIN
        )

        self.btn_detail.add_event_cb(self.on_click_detail, lv.EVENT.CLICKED, None)

    def on_click_detail(self, e):
        self.close(Detail())


class ApproveDetail(TransactionBase):
    def __init__(
        self,
        spender: str,
        myself: str,
        max_fee: str,
        contract_addr: str,
    ):
        super().__init__()

        self.set_icon("A:/res/approve-confirm.png")
        self.set_title(i18n.Title.approve_confirm)

        # desc 
        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)
        container.set_style_border_color(colors.USER.GREEN, lv.PART.MAIN)

        item = ItemImg(container, i18n.Text.approve_confirm_desc, "A:/res/hp/ic_infotip-green.png","left")
        item.label.set_style_text_color(colors.USER.GREEN, lv.PART.MAIN)
        item.set_width(lv.pct(100))

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)

        # approve to
        labeled(i18n.Text.approve_to, spender)
        container.add(DividerLine)

        # myself, sender
        labeled(i18n.Text.sender, myself)
        container.add(DividerLine)

        # max fee
        labeled(i18n.Text.max_fee, max_fee)
        container.add(DividerLine)

        # contract
        labeled(i18n.Text.contract, contract_addr)

class Eip712Message(TransactionBase):
    def __init__(self, network: str, icon: str, message: str, address: str):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Title.eip712_sign_message.format(network))

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)
        
        # message
        self.labeled(container, i18n.Text.message, message)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)
        # address
        self.labeled(container, i18n.Text.address, address)

class ContractCall(TransactionBase):
    def __init__(self, icon: str, myself: str, contract: str, chain_id: int, fee_max: str):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Title.contract_call)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        # receiver
        self.labeled(container, i18n.Text.receiver, contract)
        container.add(DividerLine)

        # sender, myself
        self.labeled(container, i18n.Text.sender, myself)
        container.add(DividerLine)

        # chain id
        self.labeled(container, i18n.Text.chain_id, str(chain_id))
        container.add(DividerLine)

        # max fee
        self.labeled(container, i18n.Text.max_fee, fee_max)
