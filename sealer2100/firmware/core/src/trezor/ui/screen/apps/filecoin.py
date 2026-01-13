import lvgl as lv

from .template import TransactionBase

from trezor.ui.screen import Modal
from trezor.ui import i18n, theme, Style, colors, font
from trezor.ui.component import VStack, LabeledText, DividerLine
from trezor.ui import Continue, Reject
from trezor.ui.constants import CONTENT_PAD
from trezor.ui.theme import Styles
from trezor import log
class FilecoinPayment(TransactionBase):
    def __init__(
        self,
        address_from,
        address_to,
        amount,
        fee_max=None,
        gasfeecap=None,
        total_amount=None,
        icon=None,
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
        labeled(i18n.Text.receiver, address_to)
        container.add(DividerLine)
        # from
        labeled(i18n.Text.from_, address_from)   
        container.add(DividerLine)
        # max fee
        labeled(i18n.Text.max_fee, fee_max)
        container.add(DividerLine)
        # gas price
        labeled(i18n.Text.gas_price, gasfeecap)
        container.add(DividerLine)
        # total
        labeled(i18n.Text.total, total_amount)