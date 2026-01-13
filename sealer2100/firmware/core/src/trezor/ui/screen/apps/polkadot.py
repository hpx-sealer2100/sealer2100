import lvgl as lv

from .template import TransactionBase

from trezor.ui import i18n, theme, font, colors
from trezor.ui import Reject, Continue
from trezor.ui.screen import Modal
from trezor.ui.component import VStack, LabeledText, DividerLine
from trezor.ui.constants import CONTENT_PAD
from trezor.ui.theme import Styles

class Balance(TransactionBase):
    def __init__(
        self,
        amount: str,
        to: str,
        chain_name: str,
        sender: str,
        source: str | None = None,
        tip: str | None = None,
        keep_alive: str | None = None,
        icon: str | None = None,
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

        # sender
        labeled(container, i18n.Text.sender, sender)
        container.add(DividerLine)

        # amount
        labeled(container, i18n.Text.amount, amount)

        # source
        if source:
            container.add(DividerLine)
            labeled(container, i18n.Text.source, source)

        # tip
        if tip:
            container.add(DividerLine)
            labeled(container, i18n.Text.tip, tip)

        # keep alive
        if keep_alive:
            container.add(DividerLine)
            labeled(container, i18n.Text.keep_alive, keep_alive)