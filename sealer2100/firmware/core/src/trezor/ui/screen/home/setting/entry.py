import lvgl as lv

from typing import TYPE_CHECKING

from trezor import loop, workflow, log
from trezor.ui import i18n, Style, colors, font
from trezor.ui.component import VStack, HStack
from trezor.ui.screen import Screen, Navigation
from trezor.ui.theme import Styles
from trezor.ui.constants import CONTENT_PAD

from . import _SETTING_VALUE_CHANGED

if TYPE_CHECKING:
    from typing import Protocol, Type, Callable

    class OptionValue(Protocol):
        def __str__(self) -> str: ...

        def __eq__(self, value) -> bool: ...

    class SettingDetailProtocol(Navigation, Protocol):
        # object subscibe this setting which will call `current` when setting changed
        subscriber: lv.obj

        @classmethod
        def current(cls) -> OptionValue:
            """The current value of this setting"""
            ...

    Action = Callable[[None], None]

class SettingEntry(HStack):
    def __init__(self, parent):
        super().__init__(parent)
        self.add_style(Styles.board, lv.PART.MAIN)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.set_height(lv.SIZE.CONTENT)

class SampleSettingEntry(SettingEntry):
    """Sample setting entry with a label

    Click this entry will native to a detail view
    """
    def __init__(self, parent, label: str, cls: Type[Screen]):
        super().__init__(parent)

        self.detail_cls = cls

        # label
        obj = self.add(lv.label)
        obj.set_text(label)
        obj.set_style_text_color(colors.USER.WHITE,lv.PART.MAIN)
        obj.set_flex_grow(1)

        # add clicked event callback
        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

        # arrow-right
        obj = self.add(lv.img)
        obj.set_src("A:/res/arrow-right_3.png")

    def on_click(self, e):
        screen = self.detail_cls()
        workflow.spawn(screen.show())

class ActionSettingEntry(SettingEntry):
    def __init__(self, parent, label: str, action: Action):
        super().__init__(parent)

        # label
        obj = self.add(lv.label)
        obj.set_text(label)
        obj.set_flex_grow(1)

        obj = self.add(lv.img)
        obj.set_src("A:/res/arrow-right_3.png")

        # add clicked event callback
        self.add_event_cb(lambda _: action(), lv.EVENT.CLICKED, None)

class ObserveSettingEntry(SettingEntry):
    """General setting entry with label and current value"""

    def __init__(self, parent, label: str, cls: Type[SettingDetailProtocol]):
        super().__init__(parent)

        self.detail_cls = cls

        # label
        obj = self.add(lv.label)
        obj.set_text(label)
        obj.set_style_text_color(colors.USER.WHITE,lv.PART.MAIN)
        obj.set_flex_grow(1)
        obj.set_long_mode(lv.label.LONG.SCROLL)

        # value
        self.value = self.add(lv.label)
        self.value.set_recolor(True)
        self.value.set_text(f"{cls.current()}")
        self.value.set_style_text_color(colors.USER.SUBTITLE, lv.PART.MAIN)
        self.value.set_style_text_font(font.small, lv.PART.MAIN)

        # arrow-right
        obj = self.add(lv.img)
        obj.set_src("A:/res/arrow-right_3.png")

        # add setting changed event callback
        self.add_event_cb(
            lambda _: self.value.set_text(f"{cls.current()}"),
            _SETTING_VALUE_CHANGED,
            None,
        )

        # add clicked event callback
        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, e):
        screen = self.detail_cls()
        # assign `self` to detail class. detail class will send a event to `subscriber`
        screen.subscriber = self
        workflow.spawn(screen.show())
