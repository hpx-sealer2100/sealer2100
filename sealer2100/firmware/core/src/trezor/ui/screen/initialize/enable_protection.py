import lvgl as lv

from typing import TYPE_CHECKING

from storage.device import (
    DEVICE_PROTECT_TYPE_PIN,
    DEVICE_PROTECT_TYPE_IRIS,
    DEVICE_PROTECT_TYPE_PIN_AND_IRIS,
)
from trezor.ui import i18n, Style, font, colors, log, Cancel
from trezor.ui.constants import *
from trezor.ui.theme import Styles
from trezor.ui.screen import Modal, Navigation
from trezor.ui.component.container import VStack, HStack
from trezor import log, workflow, motor
from trezor.ui.component.item import ItemWithtwoImg

if TYPE_CHECKING:
    from storage.device import DeviceProtectType

    pass


class EnableProtectionScreen(Navigation):
    def __init__(self):
        super().__init__()
        # create title
        self.set_icon("A:/res/hp/ic_baohu.png")
        self.title.set_text(i18n.Title.enable_protection)
        self.subtitle.set_text(i18n.Subtitle.enable_protection)

        # create content
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)
        self.content.spacing(8)

        # enable iris and pin
        # container = self.add(HStack)
        # container.add_style(Styles.board_1, lv.PART.MAIN)
        # container.add_style(
        #     Style()
        #     .flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN)
        #     .flex_cross_place(lv.FLEX_ALIGN.CENTER)
        #     .height(lv.SIZE.CONTENT),
        #     lv.PART.MAIN,
        # )
        # label = container.add(lv.label)
        # label.set_text(i18n.Button.enable_pin_iris)
        # label.set_style_text_line_space(8, lv.PART.MAIN)
        # label.set_style_text_font(font.medium, lv.PART.MAIN)
        # label.set_size(256, lv.SIZE.CONTENT)
        # label.set_long_mode(lv.label.LONG.WRAP)

        # recommend = container.add(lv.label)
        # recommend.set_text(i18n.Text.recommend)
        # recommend.add_style(
        #     Style()
        #     .radius(6)
        #     .text_color(colors.USER.WHITE)
        #     .bg_color(colors.USER.GREEN)
        #     .bg_opa(lv.OPA.COVER)
        #     .pad_all(6),
        #     lv.PART.MAIN,
        # )

        # container.add_event_cb(lambda _: self.on_click_enable_x(DEVICE_PROTECT_TYPE_PIN_AND_IRIS), lv.EVENT.CLICKED, None)
        item = ItemWithtwoImg(self.content, i18n.Button.enable_pin_iris, "A:/res/hp/ic_enable_pin_iris.png","A:/res/arrow-right_3.png")
        item.set_width(lv.pct(100))
        item.add_flag(lv.obj.FLAG.CLICKABLE)
        item.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        item.add_style(Styles.board, lv.PART.MAIN)        
        item.add_event_cb(lambda _: self.on_click_enable_x(DEVICE_PROTECT_TYPE_PIN_AND_IRIS), lv.EVENT.CLICKED, None)

        # enable iris
        # app = self.add(lv.label)
        # app.set_style_text_font(font.medium, lv.PART.MAIN)
        # app.set_size(lv.pct(100), lv.SIZE.CONTENT)
        # app.add_style(Styles.board_1, lv.PART.MAIN)
        # app.set_text(i18n.Button.enable_iris)
        # app.add_flag(lv.obj.FLAG.CLICKABLE)
        # app.add_event_cb(lambda _: self.on_click_enable_x(DEVICE_PROTECT_TYPE_IRIS), lv.EVENT.CLICKED, None)
        item = ItemWithtwoImg(self.content, i18n.Button.enable_iris, "A:/res/hp/ic_enable_iris.png","A:/res/arrow-right_3.png")
        item.set_width(lv.pct(100))
        item.add_flag(lv.obj.FLAG.CLICKABLE)
        item.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        item.add_style(Styles.board, lv.PART.MAIN)        
        item.add_event_cb(lambda _: self.on_click_enable_x(DEVICE_PROTECT_TYPE_IRIS), lv.EVENT.CLICKED, None)

        # enable pin
        
        # app = self.add(lv.label)
        # app.set_style_text_font(font.medium, lv.PART.MAIN)
        # app.set_size(lv.pct(100), lv.SIZE.CONTENT)
        # app.add_style(Styles.board_1, lv.PART.MAIN)
        # app.set_text(i18n.Button.enable_pin)
        # app.add_flag(lv.obj.FLAG.CLICKABLE)
        # app.add_event_cb(lambda _: self.on_click_enable_x(DEVICE_PROTECT_TYPE_PIN), lv.EVENT.CLICKED, None)
        item = ItemWithtwoImg(self.content, i18n.Button.enable_pin, "A:/res/hp/ic_enable_pin.png","A:/res/arrow-right_3.png")
        item.set_width(lv.pct(100))
        item.add_flag(lv.obj.FLAG.CLICKABLE)
        item.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        item.add_style(Styles.board, lv.PART.MAIN)        
        item.add_event_cb(lambda _: self.on_click_enable_x(DEVICE_PROTECT_TYPE_PIN), lv.EVENT.CLICKED, None)


    def on_click_enable_x(self, protect: DeviceProtectType):
        log.debug(__name__, f"user choose device protect type: {protect}")
        self.close(protect)

    def on_nav_back(self, event):
        self.close(Cancel())

    def dismiss(self):
        super().dismiss()
        from trezor.ui.screen import manager
        manager.mark_dismissing(self)