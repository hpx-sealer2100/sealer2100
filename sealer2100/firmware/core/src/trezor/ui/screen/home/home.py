import lvgl as lv
from typing import Callable

from storage import device
from trezor import log, workflow, motor, utils
from trezor.ui import i18n, events, font, Style
from trezor.ui.theme import Styles, colors
from trezor.ui.screen import Screen
from trezor.ui.component import HStack, VStack, TabIndicator


class HomeScreen(Screen):
    def __init__(self):
        super().__init__()
        self.set_style_bg_color(colors.USER.BLACK, lv.PART.MAIN)
        # Create down_img
        down_img = lv.img(self)
        down_img.set_src("A:/res/hp/ic_jiesuo.png")
        down_img.align(lv.ALIGN.TOP_MID, 0, 80)

        self.create_content(HStack)
        self.content: HStack
        self.content.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
        self.content.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN)
        self.content.align(lv.ALIGN.TOP_MID, 0, 0)
        self.content.set_style_pad_row(32, lv.PART.MAIN)

        # tab indicator
        # indicator = TabIndicator(self)
        # indicator.align(lv.ALIGN.BOTTOM_MID, 0, -34)
        # indicator.set_size(60, 6)
        # indicator.radius(0)
        # indicator.colors(lv.color_hex(0x666666), colors.USER.WHITE)
        # indicator.set_total(2)
        # indicator.set_current(0)

        apps = [
            # account
            {
                "label": i18n.App.account,
                "icon": "A:/res/hp/ic_zhanghu1.png",
                "action": self.click_account,
            },
            # Connect App Wallet
            {
                "label": i18n.App.connect_app,
                "icon": "A:/res/hp/ic_qianbao1.png",
                "action": self.click_connect_app,
            },
            # scan
            {
                "label": i18n.App.scan,
                "icon": "A:/res/hp/ic_sao1.png",
                "action": self.click_scan,
            },
            # setting
            {
                "label": i18n.App.setting,
                "icon": "A:/res/hp/ic_shezhi1.png",
                "action": self.click_setting,
            },
            # guide
            {
                "label": i18n.App.guide,
                "icon": "A:/res/hp/ic_book1.png",
                "action": self.click_guide,
            },
            #nft
            {
                "label": i18n.App.nft,
                "icon": "A:/res/hp/ic_nft1.png",
                "action": self.click_nft,
            },
        ]
        self.apps: list[Item] = []
        for a in apps:
            label, icon, action = a["label"], a["icon"], a["action"]
            app = Item(self.content, label, icon)
            app.on_click = action
            self.apps.append(app)

        # a placeholder
        obj = lv.obj(self.content)
        obj.add_style(Styles.container, lv.PART.MAIN)
        obj.add_style(Styles.home_app, lv.PART.MAIN)
        obj.set_size(206, 206)

        self.add_event_cb(self.event_cb, lv.EVENT.GESTURE, None)
        self.add_event_cb(self.on_change_wallpaper, events.WALLPAPER_CHANGED, None)
        self.add_event_cb(self.on_change_language, events.LANGUAGE_CHANGED, None)

    def event_cb(self, e):
        gesture = lv.indev_get_act().get_gesture_dir()
        indev = lv.indev_get_act()
        target = e.get_target()
        if target.get_parent() == self.content:
            return
        else:
            if gesture == lv.DIR.BOTTOM:
                from ..lockscreen import ProtectScreen
                indev.wait_release()
                workflow.spawn(ProtectScreen().show())
            # elif gesture == lv.DIR.LEFT:
            #     indev.wait_release()
            #     from .home_right import HomeRightScreen

            #     workflow.spawn(HomeRightScreen().show())

    def on_change_wallpaper(self, event):
        wallpaper = device.get_homescreen()
        self.set_style_bg_img_src(wallpaper, 0)

    def on_change_language(self, event):
        labels = [
            i18n.App.account,
            i18n.App.connect_app,
            i18n.App.scan,
            i18n.App.setting,
            i18n.App.guide,
            i18n.App.nft,                       

        ]
        for label, item in zip(labels, self.apps):
            log.debug(__name__, f"update lable: {label}")
            item.label.set_text(label)

    def click_account(self, app: "Item"):
        log.debug(__name__, "click account")
        from .account import AccountApp

        workflow.spawn(AccountApp().show())

    def click_scan(self, app: "Item"):
        log.debug(__name__, "click scan")
        from .scan import ScanApp

        workflow.spawn(ScanApp().show())

    def click_connect_app(self, app: "Item"):
        log.debug(__name__, "click connect app")
        from .connectapp import ConnectApp

        workflow.spawn(ConnectApp().show())

    def click_setting(self, app: "Item"):
        from .setting import SettingApp

        workflow.spawn(SettingApp().show())

    def click_guide(self, app: 'Item'):
        log.debug(__name__, "click guide")
        motor.vibrate()#触摸振动
        from .guide import GuideApp
        workflow.spawn(GuideApp().show())
        
    def click_nft(self, app: 'Item'):
        log.debug(__name__, "click nft")
        motor.vibrate()#触摸振动
        from .nft import NftApp
        workflow.spawn(NftApp().show())

    async def show(self):
        from trezor.ui.screen import manager

        await manager.switch_scene(self)
    def on_loaded(self):
        super().on_loaded()
        import trezor.ui.screen
        trezor.ui.screen.TITLE_LOCALTION = None


class Item(VStack):
    def __init__(self, parent, label: str, icon: str):
        super().__init__(parent)
        self.add_style(Styles.container, lv.PART.MAIN)
        self.add_style(Styles.home_app, lv.PART.MAIN)
        self.add_style(Styles.pressed, lv.STATE.PRESSED)
        self.items_center()

        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.clear_flag(lv.obj.FLAG.EVENT_BUBBLE)

        board = self.add(lv.obj)
        board.add_style(Styles.home_app_icon_board, lv.PART.MAIN)
        board.add_flag(lv.obj.FLAG.CLICKABLE)
        board.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        # Create img
        img = lv.img(board)
        img.set_src(icon)
        img.center()

        # Create text
        self.label = lv.label(self)
        self.label.set_text(label)
        self.label.set_long_mode(lv.label.LONG.WRAP)
        self.label.add_style(
            Style()
            .width(lv.pct(100))
            .height(lv.SIZE.CONTENT)
            .text_align(lv.TEXT_ALIGN.CENTER)
            .text_color(colors.USER.WHITE),
            lv.PART.MAIN,
        )
        self.on_click: Callable[["Item"], None] = None
        self.add_event_cb(self.__on_clicked, lv.EVENT.CLICKED, None)

    def __on_clicked(self, event: lv.event_t):
        motor.vibrate()  # 触摸振动
        if self.on_click:
            self.on_click(self)
