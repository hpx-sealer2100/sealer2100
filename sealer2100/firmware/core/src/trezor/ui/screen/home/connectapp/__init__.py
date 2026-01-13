#from trezor.ui.component.item import ItemImgLeft
from trezor.ui.component.item import ItemWithtwoImg
import lvgl as lv

from trezor import  workflow
from trezor.ui import i18n, colors, font, Style
from trezor.ui.component import VStack, HStack
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor import workflow, motor
class ConnectApp(Navigation):
    def __init__(self):
        super().__init__()
        self.set_icon("A:/res/hp/ic_qianbao1.png")
        self.title.set_text(i18n.App.connect_app)
        self.subtitle.set_text(i18n.Subtitle.choose_connection_method)
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)
        self.content.spacing(8)

        blue_view = ItemWithtwoImg(self.content, i18n.Title.blue_connect, "A:/res/hp/blue1.png","A:/res/arrow-right_3.png")
        blue_view.set_width(lv.pct(100))
        blue_view.add_flag(lv.obj.FLAG.CLICKABLE)
        blue_view.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        blue_view.add_style(Styles.board, lv.PART.MAIN)        
        blue_view.add_event_cb(self.click_blue, lv.EVENT.CLICKED, None)

        qr_view = ItemWithtwoImg(self.content, i18n.Setting.qr_code, "A:/res/hp/ic_erweima1.png","A:/res/arrow-right_3.png")
        qr_view.set_width(lv.pct(100))
        qr_view.add_flag(lv.obj.FLAG.CLICKABLE)
        qr_view.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        qr_view.add_style(Styles.board, lv.PART.MAIN)        
        qr_view.add_event_cb(self.click_qr_code, lv.EVENT.CLICKED, None)

        # blue_view = ItemImgLeft(self.content,i18n.Title.blue_connect,"A:/res/hp/blue.png")
        # blue_view.add_event_cb(self.click_blue, lv.EVENT.CLICKED, None)

        # qr_view = ItemImgLeft(self.content,i18n.Setting.qr_code,"A:/res/hp/ic_erweima.png")
        # qr_view.add_event_cb(self.click_qr_code, lv.EVENT.CLICKED, None)

    def click_blue(self,_):
        from .blue import BlueConnect
        workflow.spawn(BlueConnect().show())

    def click_qr_code(self,_):
        from .qr_code import QRConnect
        workflow.spawn(QRConnect().show())

class WalletItem(HStack):
    def __init__(self, parent, icon: str, label: str, supports: str):
        super().__init__(parent)
        self.add_style(Styles.board, lv.PART.MAIN)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.set_height(lv.SIZE.CONTENT)

        self.add(lv.img).set_src(icon)

        container = self.add(VStack)
        container.set_height(lv.SIZE.CONTENT)
        obj = container.add(lv.label)
        obj.set_text(label)
        obj.add_style(
            Style()
            .text_font(font.medium)
            .text_color(colors.USER.WHITE),
            lv.PART.MAIN,
        )

        obj = container.add(lv.label)
        obj.set_long_mode(lv.label.LONG.DOT)
        obj.set_text(supports)
        obj.add_style(
            Style()
            .text_font(font.small)
            .text_color(colors.USER.LIGHT_GRAY),
            lv.PART.MAIN,
        )
