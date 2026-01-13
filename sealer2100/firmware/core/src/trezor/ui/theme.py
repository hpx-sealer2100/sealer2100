import lvgl as lv

from trezor import log

from . import colors
from . import font
from . import Style

from .constants import *


class UserTheme(lv.theme_t):
    def __init__(self, parent: lv.theme_t = None):
        super().__init__()
        self.set_parent(parent or lv.theme_default_get())
        self.set_apply_cb(self.theme_apply_cb)

    def theme_apply_cb(self, theme, obj: lv.obj):
        typ = obj.get_class()
        if typ == lv.btn_class:
            obj.add_style(Styles.button, lv.PART.MAIN)
            obj.add_style(Styles.pressed, lv.STATE.PRESSED)
            obj.add_style(Styles.button_disabled, lv.STATE.DISABLED)
        elif typ == lv.switch_class:
            # indicator
            obj.add_style(Styles.switch_indicator, lv.PART.INDICATOR)
            obj.add_style(Styles.switch_indicator_checked, lv.PART.INDICATOR|lv.STATE.CHECKED)
            # knob
            obj.add_style(Styles.switch_knob, lv.PART.KNOB)
            obj.add_style(Styles.switch_knob_checked, lv.PART.KNOB|lv.STATE.CHECKED)


        # 回调函数在调用时，obj对象会转换成lvgl内部对象，无法保留子类信息
        # 例如 class Title(lv.label), 只能获取到都应的lvgl对象
        # 故而无法在此处统一对子类进行设置style
        # from .component.title import Title, Subtitle
        # if isinstance(obj, Title):
        #     log.debug(__name__, "Got a `Title` object")
        #     obj.add_style(Styles.title, lv.PART.MAIN)
        # elif isinstance(obj, Subtitle):
        #     log.debug(__name__, "Got a `Subtitle` object")
        #     obj.add_style(Styles.subtitle, lv.PART.MAIN)


def user_theme():
    # default theme
    theme = lv.theme_default_init(
        None,
        colors.USER.PRIMARY,  # primary color
        colors.USER.SECONDARY,  # secondary color
        True,  # dark mode? No
        lv.font_default(),
    )
    disp = lv.disp_get_default()
    disp.set_theme(UserTheme(theme))


pressed_dsc = lv.color_filter_dsc_t()
pressed_dsc.init(lambda _, c, o: colors.USER.PRESSED_FILTER_COLOR.color_mix(c, o))

checked_dsc = lv.color_filter_dsc_t()
checked_dsc.init(lambda _, c, o: colors.USER.CHECKED_FILTER_COLOR.color_mix(c, o))

disabled_dsc = lv.color_filter_dsc_t()
disabled_dsc.init(lambda _, c, o: colors.USER.DISABLED_FILTER_COLOR.color_mix(c, o))


class Styles:
    primary = Style().bg_color(colors.USER.PRIMARY)

    label_disabled = Style().text_color(colors.USER.TEXT_DISABLED)

    switch_indicator = Style().bg_color(colors.USER.SWITCH).bg_opa(lv.OPA._20)
    switch_indicator_checked = Style().bg_color(colors.USER.GREEN).bg_opa(lv.OPA._20)
    switch_knob = Style().bg_color(colors.USER.WHITE)
    switch_knob_checked = Style().bg_color(colors.USER.GREEN)

    ta_label = Style().text_color(colors.STD.WHITE)
    ta_text = Style().text_color(colors.STD.BLACK)

    button = (
        Style()
        .radius(12)
        .shadow_ofs_x(0)
        .shadow_ofs_y(0)
        .text_font(font.Medium.PF20)
        .text_color(colors.USER.BLACK)
    )

    # button_disabled = (
    #     Style().bg_color(lv.color_hex(0xC6E9CD)).text_color(lv.color_hex(0x93C29D))
    # )
    button_disabled = (
        Style()
        .color_filter_opa(colors.USER.DISABLED_FILTER_OPA)
        .color_filter_dsc(disabled_dsc)
    )

    keyboard = (
        Style()
        .pad_gap(2)
        .border_width(0)
        .radius(0)
        .pad_all(0)
        .shadow_width(0)
        .bg_color(colors.USER.BLACK)
        .bg_opa()
    )
    keyboard_btn = (
        Style()
        .radius(8)
        .shadow_ofs_x(0)
        .shadow_ofs_y(0)
        .bg_color(colors.USER.DARK_GRAY)
        .bg_opa(lv.OPA.COVER)
    )

    keyboard_pressed = (
        Style().bg_color(colors.USER.DARK_GREEN).text_color(colors.USER.GREEN)
    )

    keyboard_disabled = Style().text_opa(lv.OPA._30)

    pressed = (
        Style()
        .color_filter_opa(colors.USER.PRESSED_FILTER_OPA)
        .color_filter_dsc(pressed_dsc)
    )

    checked = (
        Style()
        .color_filter_opa(colors.USER.CHECKED_FILTER_OPA)
        .color_filter_dsc(checked_dsc)
    )

    focused = Style().border_color(colors.USER.PRIMARY).border_width(1)
    disabled = (
        Style().bg_color(colors.USER.LIGHT_GRAY).text_color(colors.USER.DARK_GRAY)
    )

    # 组件的style无法在callback里统一设置，但是尽量在此处配置style，不要分散在组件里去设置
    # 此处设置style然后在组件代码里进行添加

    # a filled transparent container
    container = (
        Style()
        .pad_all(0)
        .radius(0)
        .width(lv.pct(100))
        .height(lv.SIZE.CONTENT)
        .bg_opa(lv.OPA.TRANSP)
        .border_width(0)
        .pad_gap(16)
    )

    # the screen main style
    screen = (
        Style()
        .width(lv.pct(100))
        .height(lv.pct(100))
        .pad_top(56 + 60)
        .pad_left(24)
        .pad_right(24)
        .pad_bottom(24)
        .pad_gap(16)
    )
    # 480 - 24*2 - 12*2 = 408
    header = (
        Style().width(lv.pct(100)).height(lv.SIZE.CONTENT).pad_left(12).pad_right(12)
    )

    content = Style().pad_all(24)

    group = (
        Style()
        .pad_all(20)
        .border_width(1)
        .border_color(colors.USER.DEEP_TAUPE)
        .radius(16)
    )

    qrcode = (
        Style()
        .bg_opa()
        .pad_all(16)
        .border_width(0)
        .radius(16)
    )

    divider_line = (
        Style()
        .width(lv.pct(100))
        .height(1)
        .border_width(0)
        .bg_opa()
        .bg_color(colors.USER.DEEP_TAUPE)
    )

    board = (
        Style()
        .pad_all(24)
        .bg_opa(lv.OPA.COVER)
        .bg_color(colors.USER.DARK_GRAY)
        .radius(12)
    )

    title = (
        Style()
        .width(lv.pct(100))
        .height(lv.SIZE.CONTENT)
        .text_align_center()
        .text_font(font.Medium.PF44)
    )

    subtitle = (
        Style()
        .text_font(font.small)
        .text_align_center()
        .text_line_space(8)
        .text_color(colors.USER.SUBTITLE)
        .height(lv.SIZE.CONTENT)
        .width(lv.pct(100))
    )

    popup = (
        Style()
        .width(lv.pct(100))
        .text_align(lv.TEXT_ALIGN.CENTER)
    )

    # apps in home screen: icon and a label
    home_app = (
        Style()
        .pad_gap(CONTENT_SPACE)
        .width(184)
        .height(lv.SIZE.CONTENT)
        .border_width(0)
    )

    home_app_icon_board = (
        Style()
        .width(184)
        .height(140)
        .bg_color(colors.USER.DARK_GRAY)
        .bg_opa()
        .radius(20)
        .border_width(0)
    )

    btn_item = (
        Style()
        .width(432)
        .height(84)
        .radius(16)
        .text_color(colors.USER.BLACK)
        .bg_color(colors.USER.WHITE)
        .text_align(lv.TEXT_ALIGN.LEFT)
        .pad_all(24)
    )

    list_default = (
        Style()
        .width(432)
        .height(lv.SIZE.CONTENT)
        .pad_left(24)
        .pad_right(24)
        .bg_color(colors.USER.WHITE)
        .radius(16)
    )

    bottom_border = (
        Style()
        .border_width(1)
        .border_side(lv.BORDER_SIDE.BOTTOM)
        .border_opa(lv.OPA.COVER)
        .border_color(colors.USER.DEEP_TAUPE)
    )

    debug = (
        Style()
        .border_side(lv.BORDER_SIDE.FULL)
        .border_color(colors.USER.RED)
        .border_opa(lv.OPA.COVER)
        .border_width(1)
    )


user_theme()
