import lvgl as lv

from typing import TYPE_CHECKING

from ubinascii import hexlify

from .template import TransactionBase

from trezor.ui import i18n, Style, colors, font
from trezor.ui import Detail
from trezor.ui.component import VStack, LabeledText, DividerLine, HStack
from trezor.ui.theme import Styles
from trezor.ui.constants import CONTENT_PAD

if TYPE_CHECKING:
    from typing import List
    pass

class TransactionDetail(TransactionBase):
    def __init__(
        self,
        amount: str,
        to: str,
        sender: str,
        sequence_number: int,
        max_gas_amount: int,
        gas_unit_price: str,
        expiration_time: str,
        chain_id: int,
        icon: str,
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
        labeled(i18n.Text.from_, sender)
        container.add(DividerLine)
        # # amount
        labeled(i18n.Text.amount, amount)
        container.add(DividerLine)

        # max gas amount
        labeled(container, i18n.Text.max_gas_limit, str(max_gas_amount))
        container.add(DividerLine)
        # gas price
        labeled(container, i18n.Text.gas_unit_price, str(gas_unit_price))
        container.add(DividerLine)
        # sequence number
        labeled(container, i18n.Text.sequence_number, str(sequence_number))
        container.add(DividerLine)
        # expiration time
        labeled(container, i18n.Text.expiration_time, str(expiration_time))
        container.add(DividerLine)
        # chain id
        labeled(container, i18n.Text.chain_id, str(chain_id))

class EntryFunctionOverview(TransactionBase):
    def __init__(self, network: str, icon: str, function: str, args: List[bytes]):
        super().__init__()
        self.set_icon(icon)

        # function name
        item = self.add(LabeledText)
        item.set_label(i18n.Text.unknown_function)
        item.set_text(function)
        item.add_style(Styles.group, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)

        if args:
            for index, arg in enumerate(args):
                arg = "0x" + hexlify(arg).decode()
                labeled(i18n.Text.argument_x.format(index), arg)
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


class EntryFunctionDetail(TransactionBase):
    def __init__(
        self,
        sender: str,
        sequence_number: int,
        max_gas_amount: int,
        gas_unit_price: str,
        expiration_time: str,
        chain_id: int,
        icon: str,
        function: str,
        args: List[bytes]
    ):
        super().__init__()
        self.set_icon(icon)

        # function name
        item = self.add(LabeledText)
        item.set_label(i18n.Text.unknown_function)
        item.set_text(function)
        item.add_style(Styles.group, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)

        if args:
            for index, arg in enumerate(args):
                arg = "0x" + hexlify(arg).decode()
                labeled(i18n.Text.argument_x.format(index), arg)
                container.add(DividerLine)


        # second group
        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)
        container.set_height(lv.SIZE.CONTENT)
        labeled = lambda label, text: self.labeled(container, label, text)

        # from
        labeled(i18n.Text.from_, sender)
        container.add(DividerLine)

        # max gas amount
        labeled(i18n.Text.max_gas_limit, str(max_gas_amount))
        container.add(DividerLine)
        # gas price
        labeled(i18n.Text.gas_unit_price, str(gas_unit_price))
        container.add(DividerLine)
        # sequence number
        labeled(i18n.Text.sequence_number, str(sequence_number))
        container.add(DividerLine)
        # expiration time
        labeled(i18n.Text.expiration_time, str(expiration_time))
        container.add(DividerLine)
        # chain id
        labeled(i18n.Text.chain_id, str(chain_id))