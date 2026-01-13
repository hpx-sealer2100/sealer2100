import lvgl as lv

from .template import TransactionBase

from trezor.ui import i18n, theme, colors, font
from trezor.ui import Continue, Reject
from trezor.ui.component import VStack, LabeledText, DividerLine
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles
from trezor.ui.constants import CONTENT_PAD

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Literal
    AssetMode = Literal['freeze', 'unfreeze', 'delegate', 'undelegate']


class Asset(TransactionBase):
    def __init__(
        self,
        sender: str,
        *,
        mode: AssetMode,
        resource: str|None = None,
        balance: str|None = None,
        duration: str|None = None,
        receiver: str|None = None,
        lock: str|None = None,
        icon: str|None = None,
    ):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Title.asset)

        self.content.add_style(theme.Styles.group, lv.PART.MAIN)

        item = self.add(lv.label)
        if mode == 'freeze':
            item.set_text(i18n.Text.you_are_freezing)
        elif mode == 'unfreeze':
            item.set_text(i18n.Text.you_are_unfreezing)
        elif mode == 'delegate':
            item.set_text(i18n.Text.you_are_delegating)
        elif mode == 'undelegate':
            item.set_text(i18n.Text.you_are_undelegating)
        self.add(DividerLine)

        labeled = lambda label, text: self.labeled(self.content, label, text)

        # sender
        labeled(i18n.Text.sender, sender)

        if resource is not None:
            self.add(DividerLine)
            item = labeled(i18n.Text.resource, resource)

        if balance is not None:
            if mode == 'freeze':
                label = i18n.Text.frozen_balance
            elif mode == 'unfreeze':
                label = i18n.Text.unfrozen_balance
            elif mode == 'delegate':
                label = i18n.Text.delegated_balance
            elif mode == 'undelegate':
                label = i18n.Text.undelegated_balance
            else:
                label = i18n.Text.amount
            self.add(DividerLine)
            labeled(label, balance)

        if duration is not None:
            self.add(DividerLine)
            item = labeled(i18n.Text.duration, duration)

        if receiver is not None:
            self.add(DividerLine)
            item = labeled(i18n.Text.receiver, receiver)

        if lock is not None:
            self.add(DividerLine)
            item = labeled(i18n.Text.lock, lock)

class TransactionDetail(TransactionBase):
    def __init__(
        self,
        amount: str,
        from_: str,
        to: str,
        fee_max: str,
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

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)

        # receiver
        labeled(i18n.Text.receiver, to)
        container.add(DividerLine)
         # from
        labeled(i18n.Text.from_, from_)
        container.add(DividerLine)

        # max fee
        labeled(i18n.Text.max_fee, fee_max)

        # total
        if total is not None:
            container.add(DividerLine)
            labeled(i18n.Text.total, total)

    def show_contract_address(self, address: str):
        item = self.add(LabeledText)
        item.set_label(i18n.Text.contract)
        item.set_text(address)

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
