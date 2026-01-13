import lvgl as lv

from typing import TYPE_CHECKING

from trezor import loop, workflow, log
from trezor.ui import i18n, Style, theme, colors, font
from trezor.ui.component import VStack, HStack, LabeledText
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor.ui.constants import *

# register a id for setting value changed
_SETTING_VALUE_CHANGED = lv.event_register_id()

# Category
from .general import General
from .air_gap import AirGap
from .home_screen import HomeScreen
from .security import Security
from .wallet import Wallet
from .about import About
# power off
from trezor.ui.screen.power import PowerOff

if TYPE_CHECKING:
    from typing import Type
    pass

class SettingApp(Navigation):
    def __init__(self):
        # import trezor.ui.screen
        # trezor.ui.screen.TITLE_LOCALTION = 'center'
        super().__init__()
        self.title.set_text(i18n.App.setting)

        self.create_content(VStack)
        self.content: VStack
        self.content.spacing(8)

        categories = [
            {
                "label": i18n.Setting.general,
                "icon": "A:/res/hp/ic_tongyong1.png",
                "cls": General,
            },
            {
                "label": i18n.Setting.airgap_mode,
                "icon": "A:/res/hp/ic_air1.png",
                "cls": AirGap,
            },
            {
                "label": i18n.Setting.home_screen,
                "icon": "A:/res/hp/ic_pingmu1.png",
                "cls": HomeScreen,
            },
            {
                "label": i18n.Setting.security,
                "icon": "A:/res/hp/ic_anquan1.png",
                "cls": Security,
            },
            {
                "label": i18n.Setting.wallet,
                "icon": "A:/res/hp/ic_wallet1.png",
                "cls": Wallet,
            },
            {
                "label": i18n.Setting.about_device,
                "icon": "A:/res/hp/ic_guanyu1.png",
                "cls": About,
            },
            # {
            #     "label": i18n.Setting.power_off,
            #     "icon": "A:/res/hp/ic_guanji.png",
            #     "cls": PowerOff,
            # },
        ]

        group = self.add(VStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.spacing(8)

        group.set_height(lv.SIZE.CONTENT)
        for category in categories:
            group.add(lambda parent: Category(parent=parent, **category))

        self.btn.set_text(i18n.Setting.power_off)
        self.btn.mode('second')
        self.btn.text_color(colors.USER.DANGER)
        self.btn.add_event_cb(self.on_click_power_off, lv.EVENT.CLICKED, None)

        self.add_event_cb(self.gesture_event_cb, lv.EVENT.GESTURE, None)

    def on_click_power_off(self, e:lv.event_t):
        screen = PowerOff()
        workflow.spawn(screen.show())

    def gesture_event_cb(self, e: lv.event_t):
        gesture = lv.indev_get_act().get_gesture_dir()
        indev = lv.indev_get_act()
        target = e.current_target
        indev.wait_release()
        # disable

class Category(HStack):
    """Setting object, with a icon and label"""

    def __init__(self, parent, icon: str, label: str, cls: Type[Navigation]):
        super().__init__(parent)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.add_style(Styles.board, lv.PART.MAIN)
        self.set_height(lv.SIZE.CONTENT)
        self.set_style_pad_top(22, lv.PART.MAIN)
        self.set_style_pad_bottom(22, lv.PART.MAIN)
        self.spacing(CONTENT_SPACE)
        # the detail class
        self.detail_cls = cls

        # icon
        obj = self.add(lv.img)
        obj.set_src(icon)

        # label
        obj = self.add(lv.label)
        obj.set_text(label)
        # obj.set_style_text_font(font.small, lv.PART.MAIN)
        # obj.set_style_text_color(colors.USER.WHITE, lv.PART.MAIN)
        obj.set_flex_grow(1)

        # right-arrow
        obj = self.add(lv.img)
        obj.set_src("A:/res/arrow-right_3.png")

        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, e):
        screen = self.detail_cls()
        workflow.spawn(screen.show())
