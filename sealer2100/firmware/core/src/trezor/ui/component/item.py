import lvgl as lv

from . import HStack, VStack
from trezor.ui.types import *
from trezor.ui import i18n, Style,colors,font
from trezor.ui.theme import Styles
from trezor import log
from trezor.ui.constants import CONTENT_PAD

if TYPE_CHECKING:
    ImageLocation = Literal['left', 'right']

class Item(HStack):
    """
    Item with text or icon
    """
    def __init__(self, parent, text, icon):
        super().__init__(parent)
        self.label = lv.label(self)
        self.label.set_flex_grow(1)
        self.label.set_style_text_line_space(8, lv.PART.MAIN)
        self.label.set_text(text)
        self._icon = None
        if icon:
            self._icon = lv.img(self)
            self._icon.set_src(icon)
        self.add_event_cb(lambda _: self.action(), lv.EVENT.CLICKED, None)

    def set_text(self, text):
        """
        Set the text of the item label

        """
        self.label.set_text(text)
    def get_text(self):
        """
        Get the current text of the item label
        """
        return self.label.get_text()
    def action(self):
        log.debug(__name__, "click action ")
        pass

class ItemWithtwoImg(HStack):
    """
    Item with text or left_icon or right_icon
    """
    def __init__(self, parent, text, left_icon=None, right_icon=None):
        super().__init__(parent)
        self.add_style(
            Style()
            .flex_cross_place(lv.FLEX_ALIGN.CENTER)
            .height(lv.SIZE.CONTENT)
            .text_font(font.default),
            lv.PART.MAIN,
        )
        # 左侧图标
        self._left_icon = None
        if left_icon:
            self._left_icon = lv.img(self)
            self._left_icon.set_src(left_icon)
            # 设置左侧图标与文本的间距为12
            self._left_icon.set_style_pad_right(12, lv.PART.MAIN)
        
        self.label = lv.label(self)
        self.label.set_flex_grow(1)
        self.label.set_text(text)

        # 右侧图标
        self._right_icon = None
        if right_icon:
            self._right_icon = lv.img(self)
            self._right_icon.set_src(right_icon)
        self.add_event_cb(lambda _: self.action(), lv.EVENT.CLICKED, None)
    def action(self):
        log.debug(__name__, "click action ")
        pass

class ItemToggle(Item):
    """
    Item with text and switch
    """
    def __init__(self, parent, text):
        super().__init__(parent, text,None)
        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.switch = lv.switch(self)
        self.switch.set_height(40)
        self.switch.set_width(60)
        self.switch.add_flag(lv.obj.FLAG.CLICKABLE)
        self.switch.add_event_cb(lambda _: self.toggle(), lv.EVENT.VALUE_CHANGED, None)
        self.switch.set_style_bg_color(lv.color_hex(0x00D01C), lv.PART.INDICATOR | lv.STATE.CHECKED)
        # self.switch.set_style_bg_opa(lv.OPA.COVER, lv.PART.INDICATOR | lv.STATE.CHECKED)
        self.switch.align(lv.ALIGN.RIGHT_MID, 0, 0)

    @property
    def checked(self):
        return self.switch.has_state(lv.STATE.CHECKED)

    @checked.setter
    def checked(self, checked: bool):
        if checked:
            self.switch.add_state(lv.STATE.CHECKED)
        else:
            self.switch.clear_state(lv.STATE.CHECKED)

    def toggle(self):
        log.debug(__name__, "click toggle {self.checked}")
        pass

class ItemImg(Item):
    """
    Item with text and icon
    """
    def __init__(self, parent, text,icon, img_location: ImageLocation = 'right'):
        super().__init__(parent,text,icon)
        if img_location == "left":
            self.reverse()

class ItemDescribe:
    """
    Item with describe and number
    """
    def __init__(self,parent,text,desc,icon):
        # Create self.cont
        self.cont = lv.obj(parent)
        self.cont.add_style(
            Style()
            .bg_color(colors.USER.WHITE)
            .bg_opa(255)
            .radius(16)
            .width(432)
            .height(lv.SIZE.CONTENT)
            .pad_all(24),
            0
        )
        # Create self.img
        self.img = lv.img(self.cont)
        self.img.set_src(icon)
        self.img.set_size(30, 30)
        self.img.align(lv.ALIGN.TOP_LEFT, 0, 0)
        # Create self.text
        self.text = lv.label(self.cont)
        self.text.set_text(text)
        self.text.set_long_mode(lv.label.LONG.WRAP)
        self.text.add_style(
            Style()
            .width(312)
            .height(lv.SIZE.CONTENT)
            .pad_left(24)
            .pad_bottom(4)
            .text_color(colors.USER.BLACK)
            .text_align(lv.TEXT_ALIGN.LEFT)
            .text_font(font.Regular.PF24),
            0
        )
        self.text.align_to(self.img,lv.ALIGN.TOP_LEFT, 24, 0)
        # Create self.describe
        self.describe = lv.label(self.cont)
        self.describe.set_text(desc)
        self.describe.set_long_mode(lv.label.LONG.WRAP)
        self.describe.add_style(
            Style()
            .height(lv.SIZE.CONTENT)
            .width(404)
            .pad_left(24)
            .pad_bottom(4)
            .text_color(lv.color_hex(0x666666))
            .text_align(lv.TEXT_ALIGN.LEFT)
            .text_font(font.Regular.PF24),
            0
        )
        self.describe.set_recolor(True)
        self.describe.align(lv.ALIGN.TOP_LEFT, -24, 48)
class ItemImgLeft(HStack):
    """
    Item with text or left icon
    """
    def __init__(self, parent, text, icon):
        super().__init__(parent)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.add_style(Styles.board, lv.PART.MAIN)
        self.set_height(lv.SIZE.CONTENT)
        if icon != None:
            self.icon = lv.img(self)
            self.icon.set_src(icon)
        self.label = lv.label(self)
        self.label.set_flex_grow(1)
        self.label.set_long_mode(lv.label.LONG.WRAP)
        self.label.set_text(text)
        self.add_event_cb(lambda _: self.action(), lv.EVENT.CLICKED, None)

    def action(self):
        log.debug(__name__, "click action ")
        pass

class CheckboxItem(VStack):
    def __init__(self, parent):
        super().__init__(parent)
        self.add_style(Styles.container, lv.PART.MAIN)
        self.add_flag(lv.obj.FLAG.CLICKABLE)

        # `icon` `title` `checkbox`
        self.group = self.add(HStack)
        self.group.add_style(Styles.container, lv.PART.MAIN)
        self.group.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)

        self._icon = None

        self.title = self.group.add(lv.label)
        self.title.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.title.set_style_text_line_space(8, lv.PART.MAIN)
        self.title.set_flex_grow(1)

        self.cb = self.group.add(lv.checkbox)
        self.cb.set_text("")
        
        # desc
        self._desc = None

        self.cb.add_style(
            Style()
            .size(24)
            .radius(6)
            .border_color(colors.USER.GREEN),
            lv.PART.INDICATOR
        )
        self.cb.add_style(
            Style()
            .border_width(0)
            .bg_color(colors.USER.DARK_GREEN)
            .text_color(colors.USER.GREEN) ,
            lv.PART.INDICATOR | lv.STATE.CHECKED
        )
        self.cb.add_event_cb(self.on_click_cb, lv.EVENT.CLICKED, None)
        self.clicking = False
        
        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    @property
    def icon(self) -> lv.img:
        if self._icon:
            return self._icon
        
        self._icon = self.group.add(lv.img)
        self._icon.move_to_index(0)
        return self._icon

    @property
    def desc(self) -> lv.label:
        if self._desc:
            return self.desc
        self._desc = self.add(lv.label)
        self._desc.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN)
        self._desc.set_style_text_color(colors.USER.LIGHT_GRAY, lv.PART.MAIN)
        self._desc.set_width(lv.pct(100))
        self._desc.set_style_text_line_space(8, lv.PART.MAIN)
        self._desc.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self._desc.set_text("")
        return self._desc

    def has_state(self, state):
        # forward to checkbox
        return self.cb.has_state(state)

    def add_state(self, state):
        # forward to checkbox
        return self.cb.add_state(state)
    
    def clear_state(self, state):
        # forward to checkbox
        return self.cb.clear_state(state)

    def set_icon(self, icon):
        self.icon.set_src(icon)
    
    def set_text(self, text):
        # rewrite super `set_text` methd
        self.title.set_text(text)

    def set_title(self,text):
        self.title.set_text(text)   

    def set_desc(self, text):
        self.desc.set_text(text)
    
    def on_click_cb(self, e: lv.event_t):
        self.clicking = True 
        lv.event_send(self, lv.EVENT.CLICKED, self.cb)

    def on_click(self, e: lv.event_t):
        if self.clicking:
            self.clicking = False
            return
        if self.cb.has_state(lv.STATE.CHECKED):
            self.cb.clear_state(lv.STATE.CHECKED)
        else:
            self.cb.add_state(lv.STATE.CHECKED) 


class IndexedItem(HStack):
    def __init__(self, parent, index: int, text: str):
        super().__init__(parent)
        self.set_height(lv.SIZE.CONTENT)
        self.index = self.add(lv.img)
        self.index.set_src(f"A:/res/{index}.png")

        self.label = self.add(lv.label)
        self.label.set_flex_grow(1)
        self.label.set_style_text_line_space(8, lv.PART.MAIN)
        self.label.set_text(text)