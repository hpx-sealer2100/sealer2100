import lvgl as lv

from . import *
from trezor.ui.component.item import IndexedItem
from trezor.ui import i18n, Done, font, colors
from trezor.ui.theme import Styles
from trezor.ui.screen import Modal, VStack
from trezor import workflow, utils


class ConnectWalletScreen(Modal):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.link_wallet)
        self.create_content(VStack)
        self.content: VStack

        texts = [
            i18n.Text.connect_wallet_1,
            i18n.Text.connect_wallet_2.format(utils.BLE_NAME),
            i18n.Text.connect_wallet_3,
        ]

        for index, connect_text in enumerate(texts):
            item = IndexedItem(self.content, index + 1, connect_text)
            item.add_style(Styles.board, lv.PART.MAIN)
            item.label.set_style_text_font(font.medium, lv.PART.MAIN)

        self.btn_left.set_text(i18n.Button.back)
        self.btn_left.add_event_cb(self.on_back, lv.EVENT.CLICKED, None)

        self.btn_right.set_text(i18n.Button.continue_)
        self.btn_right.add_event_cb(self.on_click_next, lv.EVENT.CLICKED, None)

    def on_back(self, _: lv.event_t):
        from .down_app import DownAppScreen

        screen = DownAppScreen()
        workflow.spawn(screen.show())
        self.close(screen)

    def on_click_next(self, _: lv.event_t):
        utils.make_show_app_guide(0)
        self.close(Done())
