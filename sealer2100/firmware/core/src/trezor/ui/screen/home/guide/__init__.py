import lvgl as lv

from trezor.ui import i18n, Style, theme, colors, font
from trezor.ui.component import VStack, Item, IndexedItem, Url
from trezor.ui.screen import Navigation, Modal
from trezor import log, workflow, utils
from trezor.ui.theme import Styles
from trezor.ui.constants import urls

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import List

    pass


class GuideApp(Navigation):
    def __init__(self):
        # import trezor.ui.screen
        # trezor.ui.screen.TITLE_LOCALTION = 'center'
        super().__init__()
        self.title.set_text(i18n.App.guide)
        self.create_content(VStack)
        self.content: VStack
        self.content.spacing(8)

        guides = [
            {
                "label": i18n.Guide.quickstart,
                "action": self.click_quickstart,
            },
            {
                "label": i18n.Guide.device_on_and_off,
                "action": self.click_device_power_on_off,
            },
            {
                "label": i18n.Guide.what_mnemonic,
                "action": self.click_what_mnemonic,
            },
            {
                "label": i18n.Guide.enable_pin_protection,
                "action": self.click_enable_pin_protection,
            },
            {
                "label": i18n.Guide.enable_iris_protection,
                "action": self.click_iris,
            },
            {
                "label": i18n.Guide.how_work,
                "action": self.click_how_work,
            },
            {
                "label": i18n.Guide.passphrase,
                "action": self.click_passphrase,
            },
            {
                "label": i18n.Guide.defi_lock,
                "action": self.click_defi_lock,
            },
        ]

        for g in guides:
            label, action = g["label"], g["action"]
            obj = Item(self.content, label, "A:/res/arrow-right_3.png")
            obj.set_width(lv.pct(100))
            obj.add_flag(lv.obj.FLAG.CLICKABLE)
            obj.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
            obj.add_style(Styles.board, lv.PART.MAIN)
            obj.action = action

        help = Item(self.content, i18n.Guide.need_help, "A:/res/arrow-right_3.png")
        help.set_width(lv.pct(100))
        help.add_flag(lv.obj.FLAG.CLICKABLE)
        help.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        help.label.set_style_text_font(font.medium, lv.PART.MAIN)
        help.label.set_style_text_color(colors.USER.GREEN, lv.PART.MAIN)
        help.add_style(Styles.board, lv.PART.MAIN)

        help.add_event_cb(self.click_need_help, lv.EVENT.CLICKED, None)

    def click_quickstart(self):
        screen = QuicStart()
        workflow.spawn(screen.show())
        # from apps.tron.serialize import serialize,deserialize
        # import trezor.airgap.rust_ur.utils as utils
        # from trezor.crypto import base58
        # str_tx = "0a0229722208212e9c020f76a13a4080c4e89ff52f5a53080d124f0a34747970652e676f6f676c65617069732e636f6d2f70726f746f636f6c2e576974686472617742616c616e6365436f6e747261637412170a15419f154c741a98251c16e4fa1f3acaf5b6cdb158b37083f7e49ff52f"
        # tx = deserialize(bytearray(utils.hex_to_bytes(str_tx)))
        # owner_address =base58.encode_check(utils.hex_to_bytes("419f154c741a98251c16e4fa1f3acaf5b6cdb158b3"))
        # str_tx2 = utils.bytes_to_hex(serialize(tx,owner_address))
        # if str_tx != str_tx2:
        #     raise ValueError(f"Transaction mismatch: {str_tx} != {str_tx2}")
        # else:
        #     log.debug(__name__, "Transaction serialization and deserialization successful")

    def click_what_mnemonic(self):
        icon = "A:/res/hp/guide-mnemonic.png"
        title = i18n.Guide.what_mnemonic
        desc = i18n.Guide.what_mnemonic_desc
        screen = SimplteGuide(icon, title, desc)
        workflow.spawn(screen.show())

    def click_device_power_on_off(self):
        icon = "A:/res/hp/guide-power_on_off.png"
        title = i18n.Guide.device_on_and_off
        desc = i18n.Guide.device_on_and_off_desc
        screen = SimplteGuide(icon, title, desc)
        workflow.spawn(screen.show())

    def click_enable_pin_protection(self):
        icon = "A:/res/hp/guide-pin.png"
        title = i18n.Guide.enable_pin_protection
        desc = i18n.Guide.enable_pin_protection_desc
        screen = SimplteGuide(icon, title, desc)
        workflow.spawn(screen.show())

    def click_iris(self):
        icon = "A:/res/hp/guide-iris.png"
        title = i18n.Guide.enable_iris_protection
        desc = i18n.Guide.enable_iris_protection_desc
        screen = SimplteGuide(icon, title, desc)
        workflow.spawn(screen.show())

    def click_how_work(self):
        icon = "A:/res/hp/guide-wallet.png"
        title = i18n.Guide.how_work
        desc = i18n.Guide.how_work_desc
        screen = SimplteGuide(icon, title, desc)
        workflow.spawn(screen.show())

    def click_passphrase(self):
        icon = "A:/res/hp/guide-passphrase.png"
        title = i18n.Guide.passphrase
        desc = i18n.Guide.passphrase_desc
        screen = SimplteGuide(icon, title, desc)
        workflow.spawn(screen.show())

    def click_defi_lock(self):
        icon = "A:/res/hp/guide-defi_lock.png"
        title = i18n.Guide.defi_lock
        desc = i18n.Guide.defi_lock_desc
        screen = SimplteGuide(icon, title, desc)
        workflow.spawn(screen.show())

    def click_need_help(self, e: lv.event_t):
        log.debug(__name__, "click need help")
        from .help import Help

        workflow.spawn(Help().show())


class SimplteGuide(Modal):
    def __init__(self, icon: str, title: str, desc: str):
        super().__init__()

        self.btn.mode("close")
        self.btn.set_text(i18n.Button.close)

        self.create_content(VStack)
        self.content: VStack
        self.content.items_center()

        # image
        img = self.add(lv.img)
        img.set_src(icon)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.board, lv.PART.MAIN)

        obj = container.add(lv.label)
        obj.set_text(title)
        obj.add_style(Style().width(lv.pct(100)).text_font(font.medium), lv.PART.MAIN)

        obj = container.add(lv.label)
        obj.set_text(desc)
        obj.add_style(
            Style()
            .width(lv.pct(100))
            .text_font(font.small)
            .text_color(colors.USER.SUBTITLE),
            lv.PART.MAIN,
        )

        self.btn.add_event_cb(lambda _: self.close(None), lv.EVENT.CLICKED, None)


class QuicStart(Navigation):
    def __init__(self):
        super().__init__()

        self.content: lv.obj

        style = Style().bg_img_recolor(colors.USER.DISABLED_FILTER_COLOR).bg_img_recolor_opa(colors.USER.DISABLED_FILTER_OPA)

        self.btn_left.mode("switch")
        self.btn_left.set_style_bg_img_src("A:/res/hp/ic_arrow_left.png", lv.PART.MAIN)
        self.btn_left.add_style(style, lv.PART.MAIN|lv.STATE.DISABLED)
        self.btn_left.add_state(lv.STATE.DISABLED)

        self.btn_right.mode("switch")
        self.btn_right.set_style_bg_img_src("A:/res/hp/ic_arrow_right.png", lv.PART.MAIN)
        self.btn_right.add_style(style, lv.PART.MAIN|lv.STATE.DISABLED)

        # click events
        self.btn_left.add_event_cb(self.on_click_left_btn, lv.EVENT.CLICKED, None)
        self.btn_right.add_event_cb(self.on_click_right_btn, lv.EVENT.CLICKED, None)

        self.download_app_view()

    def download_app_view(self):
        self.title.set_text(i18n.Title.download_app)
        self.content.clean()

        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.spacing(32)
        group.items_center()

        url = group.add(Url)
        url.set_width(lv.pct(100))
        url.set_lable(i18n.Text.download_app)
        url.set_url(urls.URL_DOWNLOAD_APP)

        self.app_img = group.add(lv.img)
        self.app_img.add_style(Styles.group, lv.PART.MAIN)
        self.app_img.set_src("A:/res/hp/down_app.png")

    def connect_view(self):
        self.title.set_text(i18n.Title.link_wallet)
        self.content.clean()

        texts = [
            i18n.Text.connect_wallet_1,
            i18n.Text.connect_wallet_2.format(utils.BLE_NAME),
            i18n.Text.connect_wallet_3,
        ]
        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)

        for index, connect_text in enumerate(texts):
            item = IndexedItem(group, index + 1, connect_text)
            item.add_style(Styles.board, lv.PART.MAIN)
            item.label.set_style_text_font(font.medium, lv.PART.MAIN)

    def on_click_left_btn(self, e: lv.event_t):
        self.btn_left.add_state(lv.STATE.DISABLED)
        self.btn_right.clear_state(lv.STATE.DISABLED)

        self.download_app_view()

    def on_click_right_btn(self, event):
        self.btn_left.clear_state(lv.STATE.DISABLED)
        self.btn_right.add_state(lv.STATE.DISABLED)
        self.connect_view()

