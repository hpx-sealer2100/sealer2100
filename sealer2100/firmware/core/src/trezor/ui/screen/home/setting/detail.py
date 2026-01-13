"""
General setting detail base stting
"""

import lvgl as lv

from trezor import utils, log
from trezor.ui import Style, colors, i18n, font
from trezor.ui.screen import Navigation
from trezor.ui.component import VStack, HStack
from trezor.ui.theme import Styles
from trezor.ui.constants import CONTENT_PAD, SCREEN_WIDTH

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import TypeVar, Sequence, List

    T = TypeVar["T"]

    from typing import Protocol

    class OptionValue(Protocol):
        def __str__(self) -> str: ...

        def __eq__(self, value) -> bool: ...


### Options setting detail, e.g. `language`
class OptionsDetail(Navigation):
    def __init__(self, title, options: Sequence[OptionValue]):
        super().__init__()
        self.set_title(title)

        # the property of SettingDetailProtocol
        self.subscriber: lv.obj = None

        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Styles.container, lv.PART.MAIN)
        self.content.spacing(8)

        self.options = [OptionsDetailItem(self.content, o) for o in options]

        # a description
        self.desc = self.add(lv.label)
        self.desc.set_recolor(True)
        self.desc.set_width(lv.pct(100))
        self.desc.set_style_text_line_space(8, lv.PART.MAIN)
        self.desc.set_style_text_font(font.small, lv.PART.MAIN)
        self.desc.set_style_text_color(colors.USER.TAUPE,lv.PART.MAIN)
        self.desc.set_long_mode(lv.label.LONG.WRAP)
        self.desc.set_text("")

        self.group = lv.group_create()
        for item in self.options:
            self.group.add_obj(item)

        # find the current and set focus
        current = self.current()

        item = utils.first(self.options, lambda item: item.option == current)
        lv.group_focus_obj(item)

        self.group.set_focus_cb(self.on_group_focus_changed)

    @classmethod
    def current(cls) -> OptionValue:
        """the default one"""
        raise NotImplementedError

    def save_option(self, option: T):
        raise NotImplementedError

    def on_deleting(self):
        super().on_deleting()
        self.group._del()

    def on_group_focus_changed(self, group: lv._group_t):
        obj = group.get_focused()
        item = utils.first(self.options, lambda item: obj == item)
        log.debug(__name__, f"user clicked: {item.option}")
        self.save_option(item.option)

        from . import _SETTING_VALUE_CHANGED

        if self.subscriber:
            lv.event_send(self.subscriber, _SETTING_VALUE_CHANGED, None)


class OptionsDetailItem(HStack):
    """Options detail item with options and a icon"""

    def __init__(self, parent, option: OptionValue):
        super().__init__(parent)
        self.add_style(Styles.container, lv.PART.MAIN)
        self.add_style(Styles.board, lv.PART.MAIN)
        # the `option` value
        self.option = option

        # option
        self.label = self.add(lv.label)
        self.label.set_text(f"{option}")
        self.label.set_style_text_color(colors.USER.WHITE,lv.PART.MAIN)

        # state
        self.state = self.add(lv.img)
        self.state.set_src("A:/res/hp/ic_done.png")
        self.state.add_flag(lv.obj.FLAG.HIDDEN)

        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.add_event_cb(self.on_event, lv.EVENT.ALL, None)

    def on_event(self, event):
        code = event.code
        if code == lv.EVENT.FOCUSED:
            self.state.clear_flag(lv.obj.FLAG.HIDDEN)
            self.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN)
        elif code == lv.EVENT.DEFOCUSED:
            self.state.add_flag(lv.obj.FLAG.HIDDEN)
            self.set_style_flex_main_place(lv.FLEX_ALIGN.START, lv.PART.MAIN)


class TimeOption:
    def __init__(self, v: int):
        self.v = v

    def __eq__(self, value: "TimeOption"):
        return self.v == value.v

    def __str__(self):
        if self.v < 0:
            return i18n.Text.never
        elif self.v < 60:
            return f"{self.v} {i18n.Text.seconds}"
        elif self.v == 60:
            return f"{self.v // 60} {i18n.Text.minute}"
        else:
            return f"{self.v // 60} {i18n.Text.minutes}"

class TimeOptionColored(TimeOption):
    def __init__(self, v):
        super().__init__(v)

    def __str__(self):
        fmt = "#00FE33 {}# {}"
        if self.v < 0:
            return i18n.Text.never
        elif self.v < 60:
            return fmt.format(self.v, i18n.Text.seconds)
        elif self.v == 60:
            return fmt.format(self.v // 60, i18n.Text.minute)
        else:
            return fmt.format(self.v // 60, i18n.Text.minutes)


### Toggle detail
class ToggleOptionValue:
    def __init__(self, v: bool):
        self.v = v

    def __str__(self):
        if self.v:
            return i18n.Text.on
        else:
            return i18n.Text.off

    def __eq__(self, o):
        self.v == o.v


class ToggleDetail(Navigation):
    def __init__(self, title: str, label: str):
        super().__init__()
        self.set_title(title)

        # the property of SettingDetailProtocol
        self.subscriber: lv.obj = None

        self.create_content(VStack)
        self.content: VStack

        container = self.add(HStack)
        container.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.board, lv.PART.MAIN)

        # label
        obj = container.add(lv.label)
        obj.set_text(label)
        obj.set_flex_grow(1)
        obj.set_long_mode(lv.label.LONG.SCROLL)
        obj.set_style_text_color(colors.USER.WHITE,lv.PART.MAIN)

        # a switcher
        self.switch = container.add(lv.switch)
        self.switch.set_size(40, 24)
        self.switch.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)
        option = self.current()
        if option.v:
            self.switch.add_state(lv.STATE.CHECKED)
        else:
            self.switch.clear_state(lv.STATE.CHECKED)

        # a description
        self.desc = self.add(lv.label)
        self.desc.set_width(lv.pct(100))
        self.desc.set_style_text_line_space(8, lv.PART.MAIN)
        self.desc.set_style_text_font(font.small, lv.PART.MAIN)
        self.desc.set_style_text_color(colors.USER.TAUPE,lv.PART.MAIN)
        self.desc.set_long_mode(lv.label.LONG.WRAP)
        self.desc.set_text("")

    @classmethod
    def current(cls) -> ToggleOptionValue:
        """the default one"""
        return ToggleOptionValue(cls.get_current_value())

    @classmethod
    def get_current_value(cls) -> bool:
        raise NotImplementedError

    def on_value_changed(self, e):
        from . import _SETTING_VALUE_CHANGED

        # tell subscriber change value
        if self.subscriber:
            lv.event_send(self.subscriber, _SETTING_VALUE_CHANGED, None)
