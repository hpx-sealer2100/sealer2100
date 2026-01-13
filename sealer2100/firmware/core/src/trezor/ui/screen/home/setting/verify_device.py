import lvgl as lv

from . import *
from trezor.ui import i18n
from trezor.ui.screen import Navigation,VStack
from trezor.ui.component import IndexedItem, Url
from trezor.ui.constants import urls

class VerifyDevice(Navigation):
    def __init__(self):        
        super().__init__()
        self.title.set_text(i18n.Setting.verify_device)
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)
        self.content.spacing(CONTENT_PAD)

        descs = i18n.Text.verify_device_desc
        cnt = self.add(VStack)
        cnt.spacing(8)
        # the first is download
        download = descs[0]

        group = cnt.add(VStack)
        group.add_style(Styles.group, lv.PART.MAIN)
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

        from trezor.ui.screen.home.connectapp.blue_connect_wallet import Item
        for index, connect in enumerate(descs[1:]):
            Item(cnt, index+2, connect)
            
        view = self.add(lv.label)
        view.set_text(i18n.Text.scan_learn_more)
        view.set_style_text_font(font.medium, lv.PART.MAIN)
        
        cnt = self.add(lv.obj)
        cnt.add_style(Styles.container, lv.PART.MAIN)
        img = lv.img(cnt)
        img.add_style(Styles.group, lv.PART.MAIN)
        img.set_src("A:/res/hp/hpx-help.png")
        img.center()