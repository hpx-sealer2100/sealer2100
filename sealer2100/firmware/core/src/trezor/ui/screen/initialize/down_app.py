import lvgl as lv

from . import *

from trezor.ui import i18n, Style, font, colors, log
from trezor.ui.component import VStack, Url
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles
from trezor.ui.constants import BOTTOM_BUTTON_HEIGHT, urls
from trezor import log, workflow, motor


class DownAppScreen(Modal):
    def __init__(self):
        super().__init__()

        self.title.set_text(i18n.Title.download_app)
        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.spacing(32)
        group.items_center()

        url = group.add(Url)
        url.set_width(lv.pct(100))
        url.set_lable(i18n.Text.download_app)
        url.set_url(urls.URL_DOWNLOAD_APP)

        app_img = group.add(lv.img)
        app_img.add_style(Styles.group, lv.PART.MAIN)
        app_img.set_src("A:/res/hp/down_app.png")

        self.btn.set_text(i18n.Button.continue_)
        self.btn.add_event_cb(self.on_next, lv.EVENT.CLICKED, None)

    def on_next(self, _: lv.event_t):
        from .connect_wallet import ConnectWalletScreen

        screen = ConnectWalletScreen()
        workflow.spawn(screen.show())
        self.close(screen)
