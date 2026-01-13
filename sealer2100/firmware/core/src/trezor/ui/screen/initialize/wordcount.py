import lvgl as lv
from typing import TYPE_CHECKING

from . import *

from trezor.ui import i18n, Style, colors,font
from trezor.ui.screen import Navigation
from trezor.ui.theme import Styles
from trezor.ui.component.container import VStack
from trezor.ui.component.item import Item

if TYPE_CHECKING:
    from typing import Literal

    WordcountMode = Literal['create', 'recover', 'check']

counts = [12, 18, 24]

class WordcountScreen(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.prepare_create)
        self.subtitle.set_text(i18n.Subtitle.select_mnemonics)
        self.create_content(VStack)
        self.content: VStack
        self.content.spacing(8)
        self.counts = [self.create_count_item(count) for count in counts]

    def create_count_item(self, count):
        # a container
        # obj = lv.obj(self.content)
        # obj.set_size(lv.pct(100), lv.SIZE.CONTENT)
        # obj.add_style(Styles.board, 0)
        # obj.set_style_border_side(lv.BORDER_SIDE.NONE, lv.PART.MAIN)

        # obj.add_event_cb(lambda e: self.on_close(count), lv.EVENT.CLICKED, None)
        #label = lv.label(obj)
        #label.set_style_text_font(font.medium, lv.PART.MAIN)
        #label.set_text(i18n.Text.str_words.format(count))
        #return obj
        item = Item(self.content, i18n.Text.str_words.format(count), "A:/res/arrow-right_3.png")
        item.set_width(lv.pct(100))
        item.add_flag(lv.obj.FLAG.CLICKABLE)
        item.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        item.add_style(Styles.board, lv.PART.MAIN)
        item.add_event_cb(lambda e: self.on_close(count), lv.EVENT.CLICKED, None)
        return item

    def mode(self, m: WordcountMode):
        if m == 'create':
            self.title.set_text(i18n.Title.prepare_create)
        elif m == 'recover':
            self.title.set_text(i18n.Title.prepare_recover)
        elif m == 'check':
            self.title.set_text(i18n.Title.prepare_check)

    def on_close(self, count):
        self.channel.publish(count)
        from trezor.ui.screen import manager
        manager.mark_dismissing(self)
