import lvgl as lv

from . import *

from trezor import log, utils, workflow
from trezor.ui import i18n, Style, colors, font
from trezor.ui.screen import Modal
from trezor.ui.theme import Styles
from trezor.ui.component.item import Item
from trezor.ui.component.container import VStack
from trezor.ui.constants import *
# LanguageScreen is first screen when device is not initialized
# It not allow to go back
class LanguageScreen(Modal):
    def __init__(self):
        super().__init__()
        self.set_icon("A:/res/hp/ic_yuyan.png")
        self.set_title(i18n.Title.select_language)

        self.create_content(VStack)
        self.content.spacing(8)

        self.languages = []
        for lang in i18n.languages:
            item = Item(self.content, lang.name, "A:/res/arrow-right_3.png")
            item.set_width(lv.pct(100))
            item.add_flag(lv.obj.FLAG.CLICKABLE)
            item.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
            item.add_style(Styles.board, lv.PART.MAIN)
            self.languages.append(item)


        self.add_event_cb(self.on_click_languages, lv.EVENT.CLICKED, None)

    def on_click_languages(self, e: lv.event_t):
        target = e.get_target()

        obj = utils.first(self.languages, lambda x: x == target)
        if not obj:
            return
        index: int = self.languages.index(obj)
        log.debug(__name__, f"Clicked language : {i18n.languages[index].name}")
        language = i18n.languages[index]
        i18n.change_language(language)
        self.update_ui()

    def update_ui(self):
        from .activate_device import ActivateDevice        
        workflow.spawn(ActivateDevice().show())        
        

    async def show(self):
        from trezor.ui.screen import manager
        await manager.switch_scene(self)

