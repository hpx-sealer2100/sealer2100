import lvgl as lv

from .template import TransactionBase

from trezor.ui import i18n, theme, colors, font
from trezor.ui import Continue, Reject
from trezor.ui.component import VStack, LabeledText, DividerLine
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles
from trezor.ui.constants import *


class TransactionDetail(TransactionBase):
    def __init__(
        self,
        amount: str,
        from_: str,
        to: str,
        total: str | None,
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
        # from
        labeled(i18n.Text.from_, from_)  

        # total
        if total is not None:
            container.add(DividerLine)
            labeled(i18n.Text.total, total)


class SPLTokenTransactionDetail(TransactionBase):
    def __init__(
        self,
        from_addr: str,
        to_addr: str,
        amount: str,
        source_owner: str,
        fee_payer: str,
        token_mint: str = None,
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
        labeled(i18n.Solana.ata_reciver, to_addr)
        container.add(DividerLine)

        # from
        labeled(i18n.Solana.ata_sender, from_addr)
        container.add(DividerLine)

        # source_owner
        labeled(i18n.Solana.source_owner, source_owner)
        container.add(DividerLine)

        # fee_payer
        labeled(i18n.Solana.fee_payer, fee_payer)
        container.add(DividerLine)


class SPLCreateAssociatedTokenAccount(TransactionBase):
    def __init__(
        self,
        fee_payer: str,
        funding_account: str,
        associated_token_account: str,
        wallet_address: str,
        token_mint: str,
        icon: str = None
    ):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Title.spl_create_ata)

        self.content.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(self.content, label, text)

        # associated token account
        labeled(i18n.Solana.associated_token_account, associated_token_account)
        self.add(DividerLine)

        # wallet address
        labeled(i18n.Solana.owner, wallet_address)
        self.add(DividerLine)

        # token mint
        labeled(i18n.Solana.mint_address, token_mint)
        self.add(DividerLine)

        # fund account
        labeled(i18n.Solana.found_by, funding_account)
        self.add(DividerLine)

        # fee_payer
        labeled(i18n.Solana.fee_payer, fee_payer)
        self.add(DividerLine)