import lvgl as lv

from .container import VStack
from trezor.ui import Style, colors
from trezor.ui import Style, font, colors
from trezor.ui.types import *

# can't use typing.Generic here, so T can't be infer
# class LabeledItem(HStack, Generic[T]) or class LabeledItem[T](HStack) after Python 3.12
class LabeledItem(VStack):
    def __init__(self, parent):
        super().__init__(parent)
        self.set_height(lv.SIZE.CONTENT)

        self.label = self.add(lv.label)
        self.label.set_width(lv.pct(100))
        self.label.set_text("")
        self.label.set_long_mode(lv.label.LONG.WRAP)

    def set_label(self, text: str):
        self.label.set_text(text)
        from trezor.ui import i18n
        #获取当前语言,如果是阿拉伯语则从右到左显示
        cur_language = i18n.using.code if i18n.using is not None else None
        if cur_language == "al":
            self.label.set_style_base_dir(lv.BASE_DIR.RTL, 0)  # 设置标题文本方向为从右到左

    def set_device_label(self, text: str):
        self.label.set_text(text)
        self.label.set_style_text_font(font.Regular.PF24, lv.PART.MAIN)

    def add_item(self, cls: Type[Widget]) -> Widget:
        self.desc = cls(self)
        return self.desc

    def set_bg_style(self,color,opa) :
        self.set_style_bg_color(color, lv.PART.MAIN)
        self.set_style_bg_opa(opa, lv.PART.MAIN)

class LabeledText(LabeledItem):
    def __init__(self, parent):
        super().__init__(parent)
        self.label.set_style_text_color(colors.USER.WHITE,lv.PART.MAIN)
        self.add_item(lv.label)
        self.desc.set_width(lv.pct(100))
        # manually annotate self.item's type
        self.desc: lv.label
        self.desc.set_long_mode(lv.label.LONG.WRAP)
        self.desc.set_style_text_color(colors.USER.LIGHT_GRAY,lv.PART.MAIN)

    def set_text(self, text: str):
        self.desc.set_text(text)
