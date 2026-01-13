import lvgl as lv

from trezor.ui.theme import Styles

class Title(lv.label):
    def __init__(self, parent):
        super().__init__(parent)
        self.add_style(Styles.title, lv.PART.MAIN)
        self.set_long_mode(lv.label.LONG.WRAP)
        self.set_text("")

class Subtitle(lv.label):
    def __init__(self, parent):
        super().__init__(parent)
        self.add_style(Styles.subtitle,lv.PART.MAIN|lv.STATE.DEFAULT)
        self.set_long_mode(lv.label.LONG.WRAP) 
        self.set_recolor(True)
        self.set_text("")
