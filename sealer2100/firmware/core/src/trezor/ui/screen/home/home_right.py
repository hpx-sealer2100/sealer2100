import lvgl as lv
from typing import Callable

from storage import device
from trezor import log, workflow, motor
from trezor.ui import i18n, events,font,Style
from trezor.ui.theme import Styles,colors
from trezor.ui.screen import Screen, Navigation
from trezor.ui.component import HStack, VStack
from trezor.ui.component import Swipedown, TabIndicator
from .home import Item
class HomeRightScreen(Screen):
    def __init__(self):
        super().__init__()
        self.set_style_bg_color(colors.USER.BLACK, lv.PART.MAIN)

        # Create down_img
        down_img = lv.img(self)
        down_img.set_src("A:/res/hp/ic_jiesuo.png")
        down_img.align(lv.ALIGN.TOP_MID, 0, 80)
        # apps
        self.create_content(HStack)
        self.content : HStack
        self.content.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
        self.content.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN)
        self.content.align(lv.ALIGN.TOP_MID, 0, 0)
        self.content.set_style_pad_row(32, lv.PART.MAIN)

        indicator = TabIndicator(self)
        indicator.align(lv.ALIGN.BOTTOM_MID, 0, -34)
        indicator.set_size(60, 6)
        indicator.radius(0)
        indicator.colors(lv.color_hex(0x666666), colors.USER.WHITE)
        indicator.set_total(2)
        indicator.set_current(1)

        apps = [
            # # guide
            # {
            #     "label": i18n.App.guide,
            #     "icon": "A:/res/hp/ic_book1.png",
            #     "action": self.click_guide,
            # },
            # # nft
            # {
            #     "label": i18n.App.nft,
            #     "icon": "A:/res/hp/ic_nft1.png",
            #     "action": self.click_nft,
            # },
            #backup
            {
                "label": i18n.App.backup,
                "icon": "A:/res/hp/ic_t1.png",
                "action": self.click_backup,
            }

        ]
        for a in apps:
            label, icon, action = a["label"], a["icon"], a["action"]
            app = Item(self.content, label, icon)
            app.on_click = action

        # a placeholder
        obj = lv.obj(self.content)
        obj.add_style(Styles.container, lv.PART.MAIN)
        obj.add_style(Styles.home_app, lv.PART.MAIN)
        obj.set_size(206, 206)

        self.add_event_cb(self.event_cb, lv.EVENT.GESTURE, None)
        self.add_event_cb(self.on_change_wallpaper, events.WALLPAPER_CHANGED, None)

    def event_cb(self,e):
        gesture = lv.indev_get_act().get_gesture_dir()
        indev = lv.indev_get_act()
        if gesture == lv.DIR.BOTTOM:
            from ..lockscreen import ProtectScreen
            indev.wait_release()
            workflow.spawn(ProtectScreen().show())
        elif gesture == lv.DIR.RIGHT:
            from .home import HomeScreen
            indev.wait_release()
            workflow.spawn(HomeScreen().show())

    def on_change_wallpaper(self, event):
        # self.set_style_bg_color(lv.color_hex(0x0D0D17), lv.PART.MAIN)# 设置背景颜色
        wallpaper = device.get_homescreen()
        self.set_style_bg_img_src(wallpaper, 0)

    def click_backup(self, app: 'Item'):
        log.debug(__name__, "click backup")
        motor.vibrate()#触摸振动
        from .backup import BackupApp
        workflow.spawn(BackupApp().show())

    def click_nft(self, app: 'Item'):
        log.debug(__name__, "click nft")
        motor.vibrate()#触摸振动
        from .nft import NftApp
        workflow.spawn(NftApp().show())

    def click_guide(self, app: 'Item'):
        log.debug(__name__, "click guide")
        motor.vibrate()#触摸振动
        from .guide import GuideApp
        workflow.spawn(GuideApp().show())

    async def show(self):
        from trezor.ui.screen import manager
        await manager.switch_scene(self)

    def on_loaded(self):
        super().on_loaded()
        import trezor.ui.screen
        trezor.ui.screen.TITLE_LOCALTION = None
