import lvgl as lv

from storage import device
from . import manager
from trezor.ui import Style, i18n
from trezor import loop, log
from trezor.ui import Style, events, theme, colors
from trezor.ui.constants import *
from trezor.ui.component import Subtitle, Title, Button, VStack, HStack

# for type annotations
from trezor.ui.types import *

if TYPE_CHECKING:
    from typing import Literal

    ButtonsMode = Literal["horizontal", "vertical", "signle"]
    TitleLocation = Literal['normal', 'center']


class Screen(lv.obj):
    """
    Screen 是呈现在屏幕上的 UI 组件。 一页界面，就是一个 Screen

    Screen 包含三部分区域: header content footer, 分别表示标题、内容和底部按钮区域。
    `Scree`封装了 titles buttons区域的操作函数，方便用户使用; content区域留给具体类去填充。
    """

    def __init__(self):
        super().__init__()

        # the default bg color
        self.set_style_bg_color(colors.USER.BLACK, lv.PART.MAIN)

        self.set_width(SCREEN_WIDTH)
        self.set_height(SCREEN_HEIGHT)
        self.set_scroll(False)

        # use wallpaper
        # wallpaper = device.get_homescreen()
        # if wallpaper:  # 判断 `wallpaper` 是否存在
        #     self.set_style_bg_img_src(wallpaper, 0)

        # an empty content view, this is the root of `all` user UI components
        # almost all

        # default is left right buttons
        self._buttons_mode: ButtonsMode = "horizontal"

        # create a VStack for layout `header` `content` and `footer`
        self._content = VStack(self)
        self._content.add_style(theme.Styles.container, lv.PART.MAIN)
        self._content.add_style(theme.Styles.screen, lv.PART.MAIN)
        self._content.spacing(CONTENT_SPACE)

        # header, icon and titles container
        self.header = self._content.add(VStack)
        self.header.add_style(theme.Styles.container, lv.PART.MAIN)
        self.header.add_style(theme.Styles.header, lv.PART.MAIN)
        self.header.spacing(HEADER_SPACE)
        self.header.items_center()
        # self.header.add_style(theme.Styles.debug, lv.PART.MAIN)
        
        # titles container, lazy initialize
        self._titles: VStack|None = None

        # content, replace `self._content` later
        obj = self._content.add(lv.obj)
        obj.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        obj.add_style(theme.Styles.container, lv.PART.MAIN)
        obj.set_flex_grow(1)
        obj.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        # obj.add_style(theme.Styles.debug, lv.PART.MAIN)

        # footer, buttons container
        self.footer = self._content.add(HStack)
        self.footer.reverse()
        self.footer.add_style(theme.Styles.container, lv.PART.MAIN)
        self.footer.spacing(12)
        # self.footer.add_style(theme.Styles.debug, lv.PART.MAIN)

        # replace the content
        self._content = obj

        # lazy initialize components
        self._xicon: lv.img | None = None # `icon` is a common name, use `xicon` to avoid this
        self._title: Title | None = None
        self._subtitle: Subtitle | None = None
        self._btn_1: Button | None = None
        self._btn_2: Button | None = None

        # a channel for events
        self.channel = loop.chan()

        # a channel for screen lifecycle
        self.__life_chan = loop.chan()
        self.add_event_cb(
            lambda _: self.on_load_start(), lv.EVENT.SCREEN_LOAD_START, None
        )
        self.add_event_cb(lambda _: self.on_loaded(), lv.EVENT.SCREEN_LOADED, None)
        self.add_event_cb(
            lambda _: self.on_unload_start(), lv.EVENT.SCREEN_UNLOAD_START, None
        )
        self.add_event_cb(lambda _: self.on_unloaded(), lv.EVENT.SCREEN_UNLOADED, None)
        self.add_event_cb(lambda _: self.on_deleting(), lv.EVENT.DELETE, None)

    def create_content(self, cls: Type[Widget]):
        """
        Create a `Widget` as the `content`

        The `content` is part of current `content` view
        """
        content = cls(self.content)
        content.add_style(theme.Styles.container, lv.PART.MAIN)
        self._content = content

    @property
    def content(self) -> Widget:
        return self._content
    @property 
    def xicon(self) -> lv.img:
        """ the icon of screen"""
        if not self._xicon:
            self._xicon = self.header.add(lv.img)
            if self._titles:
                self._titles.spacing(TITLE_SPACE)
                # maybe create title first, move icon to first
                self._xicon.move_to_index(0)
        return self._xicon

    @property
    def _titles_container(self) -> VStack:
        if not self._titles:
            self._titles = self.header.add(VStack)
            self._titles.add_style(theme.Styles.container, lv.PART.MAIN)
        return self._titles

    @property
    def title(self) -> Title:
        """ the title of screen"""
        if not self._title:
            # add title
            self._title = self._titles_container.add(Title)

            # if user create subtitle first, makesure title first
            if self._subtitle:
                self._title.move_to_index(0)
        return self._title

    @property
    def subtitle(self) -> Subtitle:
        if not self._subtitle:
            self._subtitle = self._titles_container.add(Subtitle)
        return self._subtitle

    @property
    def btn_1(self) -> Button:
        """Depending on the button_mode, it represents the buttons on the right side or the top side or the signle button."""
        if not self._btn_1:
            self._btn_1 = self.footer.add(Button)
            self._btn_1.mode('primary')
            # if user create btn2 first, makesure btn_1 first
            if self._btn_2:
                self._btn_1.move_to_index(0)

        return self._btn_1

    @property
    def btn_2(self) -> Button:
        """Depending on the button_mode, it represents the buttons on the left side or the top side."""
        if not self._btn_2:
            self._btn_2 = self.footer.add(Button)
            self._btn_2.mode('second')

        return self._btn_2

    @property
    def btn_right(self) -> Button:
        self.buttons_mode("horizontal")
        return self.btn_1

    @property
    def btn_left(self) -> Button:
        self.buttons_mode("horizontal")
        return self.btn_2

    @property
    def btn_upper(self) -> Button:
        self.buttons_mode("vertical")
        return self.btn_1

    @property
    def btn_down(self) -> Button:
        self.buttons_mode("vertical")
        return self.btn_2

    @property
    def btn(self) -> Button:
        self.buttons_mode("signle")
        return self.btn_1

    # component manager
    def add(self, cls: Type[Widget]) -> Widget:
        """
        Add a `Widget` to the `content`
        """
        return cls(self.content)

    def set_icon(self, icon: str):
        self.xicon.set_src(icon)

    def set_title(self, title: str):
        self.title.set_text(title)

    def set_subtitle(self, subtitle: str):
        self.subtitle.set_text(subtitle)

    def buttons_mode(self, mode: ButtonsMode):
        if self._buttons_mode == mode:
            return
        self._buttons_mode = mode
        if mode == "horizontal":
            self.footer.set_direction("HStack")
            self.footer.reverse()
        elif mode == "vertical":
            self.footer.set_direction("VStack")
            self.btn_1.set_width(lv.pct(100))
            self.btn_2.set_width(lv.pct(100))
        elif mode == "signle":
            # only one button in container
            self.btn_1.set_width(lv.pct(100))
            pass

        self.footer.spacing(8)

    def set_scroll(self, scroll: bool):
        if scroll:
            self.set_scroll_dir(lv.DIR.VER)
            self.add_flag(lv.obj.FLAG.SCROLLABLE)
        else:
            self.set_scroll_dir(lv.DIR.NONE)
            self.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)

    # screen life cycle
    def on_load_start(self):
        log.debug(__name__, f"{self.__class__.__name__} load start")
        self.__life_chan.publish(lv.EVENT.SCREEN_LOAD_START)

    def on_loaded(self):
        log.debug(__name__, f"{self.__class__.__name__} loaded")
        self.__life_chan.publish(lv.EVENT.SCREEN_LOADED)

    def on_unload_start(self):
        log.debug(__name__, f"{self.__class__.__name__} unload start")
        self.__life_chan.publish(lv.EVENT.SCREEN_UNLOAD_START)

    def on_unloaded(self):
        log.debug(__name__, f"{self.__class__.__name__} unloaded")
        self.__life_chan.publish(lv.EVENT.SCREEN_UNLOADED)

    def on_deleting(self):
        log.debug(__name__, f"{self.__class__.__name__} deleting")
        self.__life_chan.publish(lv.EVENT.DELETE)

    async def __waiting(self, event: int):
        while True:
            e = await self.__life_chan.take()
            if e == event:
                return event

    async def wait_loaded(self):
        await self.__waiting(lv.EVENT.SCREEN_LOADED)

    async def wait_unloaded(self):
        await self.__waiting(lv.EVENT.SCREEN_UNLOADED)

    async def show(self):
        """
        Show the screen.

        User decide how to show it.
        """
        log.debug(__name__, f"{self.__class__.__name__} show")
        pass

    def dismiss(self):
        """
        Dismiss the screen.

        User decide how to dismiss it.
        """
        log.debug(__name__, f"{self.__class__.__name__} dismiss")
        pass

    def close(self, value: R):
        """
        Close the screen, and return `value`
        """
        self.channel.publish(value)
        self.dismiss()

    # make `Screen` awaitable
    # see https://github.com/peterhinch/micropython-async/blob/master/v3/docs/TUTORIAL.md#4-designing-classes-for-asyncio
    # https://peps.python.org/pep-0492/#await-expression
    def __await__(self) -> Generator[R]:
        return (yield from self.channel.take())

    __iter__ = __await__


TITLE_LOCALTION: TitleLocation|None = None

# Two type of interactions:
# 1. Navigation
# 2. Modal
class Navigation(Screen):

    def __init__(self):
        super().__init__()

        # navigation icon container
        # global ui component, not belong to `content`
        # when screen need scorll, the navigation icon can keep position
        obj = lv.obj(self)
        obj.set_size(112, 112)
        obj.add_style(
            Style()
            .pad_top(56)
            .pad_bottom(0)
            .pad_left(20)
            .bg_opa(lv.OPA.TRANSP)
            .border_width(0),
            lv.PART.MAIN,
        )
        obj.add_flag(lv.obj.FLAG.CLICKABLE)
        obj.set_pos(0, 0)

        nav = lv.img(obj)
        nav.set_src("A:/res/hp/nav-back-1.png")
        nav.align(lv.ALIGN.LEFT_MID, 0, 0)
        nav.add_flag(lv.obj.FLAG.CLICKABLE)
        nav.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

        obj.add_event_cb(self.on_nav_back, lv.EVENT.CLICKED, None)
        self.add_event_cb(self.on_nav_back, events.NAVIGATION_BACK, None)

        if TITLE_LOCALTION == 'center':
            self.center_title()

    def on_nav_back(self, event):
        from trezor.ui import NavigationBack

        # should notify caller
        self.channel.publish(NavigationBack())
        from . import manager
        loop.spawn(manager.pop(self))


    def on_loaded(self):
        super().on_loaded()
        # a small `gesture pad` for navigation back by swipe right from left screen edge
        # create when load screen done to make it one top of all other widgets
        # there is another way: create a `navigator` on `top_layer` like `StatusBar`
        navigator = lv.obj(self)
        # it not active on all left edge
        navigator.add_style(
            Style().width(CONTENT_PAD).height(lv.pct(50)).bg_opa(lv.OPA.TRANSP).border_width(0),
            lv.PART.MAIN,
        )
        navigator.set_pos(0, 112)
        navigator.add_flag(lv.obj.FLAG.CLICKABLE)
        navigator.clear_flag(lv.obj.FLAG.SCROLLABLE)
        navigator.clear_flag(lv.obj.FLAG.GESTURE_BUBBLE)
        navigator.add_event_cb(self.__on_swipe, lv.EVENT.GESTURE, None)

    def center_title(self):
        """
        Move up `title`
        """

        body = self.header.get_parent()
        body.set_style_pad_top(56+4, lv.PART.MAIN)
        #self.title.items_center()
        #self.title.text.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)

    def __on_swipe(self, event):
        dir = lv.indev_get_act().get_gesture_dir()
        if dir == lv.DIR.RIGHT:
            lv.indev_get_act().wait_release()
            # send events.NAVIGATION_BACK to active screen
            lv.event_send(self, events.NAVIGATION_BACK, None)
            log.debug(__name__, "gesture navigation back")

    async def show(self):
        await super().show()
        await manager.push(self)

    # as navigation styled screen, it should provide `dismiss` function?
    # does user need close it by code?
    # def dismiss(self):
    #     from . import manager
    #     manager.pop()


class Modal(Screen):
    """
    Model Screen

    User should call `dismiss` when done something
    """

    def __init__(self):
        super().__init__()
        self._auto_close_timeout = None
        self._auto_close_task = None

    @property
    def auto_close_timeout(self):
        """
        Auto close timeout in milliseconds.
        """
        return self._auto_close_timeout

    @auto_close_timeout.setter
    def auto_close_timeout(self, value):
        self._auto_close_timeout = value
        if self._auto_close_task:
            # user update `auto_close_timeout`, need to close old task
            log.debug(
                __name__, "user update auto close timeout, close old auto close task"
            )
            self._auto_close_task.close()
        self._auto_close_task = loop.spawn(self._auto_close())

    async def _auto_close(self):
        from trezor import loop
        from trezor.ui import AutoClose

        await loop.sleep(self.auto_close_timeout)
        self.close(AutoClose())

    async def show(self):
        await super().show()
        await manager.push(self)

    def dismiss(self):
        super().dismiss()
        loop.spawn(manager.pop(self))
