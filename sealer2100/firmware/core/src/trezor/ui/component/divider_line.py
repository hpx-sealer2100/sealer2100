import lvgl as lv
from trezor.ui.theme import Styles


class DividerLine(lv.obj):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.remove_style_all()
        self.add_style(Styles.divider_line, lv.PART.MAIN)