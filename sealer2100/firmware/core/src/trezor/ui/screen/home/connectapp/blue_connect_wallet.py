import lvgl as lv

from . import *
from trezor.ui.component import IndexedItem, Url
from trezor.ui import i18n, Style, font, colors, events
from trezor.ui.screen import Navigation, VStack
from trezor.ui.constants import urls

class ConnectWallet(Navigation):
    def __init__(self, connect_type):
        import trezor.ui.screen
        # trezor.ui.screen.TITLE_LOCALTION = 'center'
        super().__init__()

        self._connect_type = connect_type
        if self._connect_type == "hpx":
            self.title.set_text(i18n.Text.hpx_wallet)
        else:
            self.title.set_text(i18n.Text.okx_wallet)
        self.subtitle.set_text("IOS & Android")

        self.btn_left.set_text(i18n.Button.learn_more)

        self.btn_right.set_text(i18n.Button.done)

        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)
        self.content.spacing(8)
        self.set_scroll(True)

        if self._connect_type == "hpx":
            connects = i18n.Text.blue_connect_hpx_wallet
        else:
            connects = i18n.Text.blue_connect_okx_wallet

        # the first is download
        download = connects[0]

        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.add_style(Styles.board, lv.PART.MAIN)
        # title
        title = download.get("title")
        obj = IndexedItem(group, 1, title)
        obj.label.set_style_text_font(font.medium, lv.PART.MAIN)
        obj.label.set_style_text_color(colors.USER.WHITE, lv.PART.MAIN)

        message = download.get('message')
        url = group.add(Url)
        url.set_style_text_line_space(4, lv.PART.MAIN)
        url.set_width(lv.pct(100))
        url.set_lable(message)
        url.set_url(urls.URL_DOWNLOAD_APP)

        for index, connect in enumerate(connects[1:]):
            Item(self.content, index+2, connect)

        self.btn_right.add_event_cb(self.on_done, lv.EVENT.CLICKED, None)
        self.btn_left.add_event_cb(self.on_more, lv.EVENT.CLICKED, None)
        trezor.ui.screen.TITLE_LOCALTION = None

    def on_done(self, e: lv.event_t):
        lv.event_send(self, events.NAVIGATION_BACK, None)

    def on_more(self, e: lv.event_t):
        from .learn_more import LearnMore

        if self._connect_type == "hpx":
            workflow.spawn(LearnMore("A:/res/hp/hpx-help.png").show())
        else:
            workflow.spawn(LearnMore("A:/res/hp/ic_qr_okx.png").show())

class Item(VStack):
    def __init__(self, parent, index: int, connect: dict):
        super().__init__(parent)
        self.add_style(Styles.container, lv.PART.MAIN)
        self.add_style(Styles.board, lv.PART.MAIN)

        title = connect.get('title')
        if title:
            obj = IndexedItem(self, index, title)
            obj.label.set_style_text_font(font.medium, lv.PART.MAIN)
            obj.label.set_style_text_color(colors.USER.WHITE, lv.PART.MAIN)

        message = connect.get('message')
        if isinstance(message, str):
            obj = self.add(lv.label)
            obj.set_width(lv.pct(100))
            obj.set_style_text_line_space(4, lv.PART.MAIN)
            obj.set_style_text_color(colors.USER.LIGHT_GRAY, lv.PART.MAIN)
            obj.set_style_text_font(font.small, lv.PART.MAIN)
            obj.set_text(message)
        elif isinstance(message, list):
            for msg in message:
                if isinstance(msg, str):
                    obj = self.add(lv.label)
                    obj.set_width(lv.pct(100))
                    obj.set_style_text_line_space(4, lv.PART.MAIN)
                    obj.set_style_text_color(colors.USER.LIGHT_GRAY, lv.PART.MAIN)
                    obj.set_recolor(True)
                    obj.set_style_text_font(font.small, lv.PART.MAIN)
                    obj.set_text(msg)
                elif isinstance(msg, dict):
                    group = lv.spangroup(self)
                    group.set_width(lv.pct(100))
                    group.set_height(lv.SIZE.CONTENT)
                    group.set_mode(lv.SPAN_MODE.BREAK)
                    group.set_style_text_font(font.small, lv.PART.MAIN)
                    group.set_style_text_color(colors.USER.LIGHT_GRAY, lv.PART.MAIN)
                    value: str = msg.get('value')
                    color: int = msg.get('color')
                    if value:
                        span = group.new_span()
                        span.set_text(value)

                    if color is not None:
                        span.style.set_text_color(lv.color_hex(color))
