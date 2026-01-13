import lvgl as lv

from trezor.ui import colors

class Url(lv.spangroup):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.set_mode(lv.SPAN_MODE.BREAK)

    def set_lable(self, txt: str):
        self.label = self.new_span()
        self.label.set_text(txt)

    def set_url(self, txt: str):
        self.url = self.new_span()
        self.url.style.set_text_color(colors.USER.GREEN)
        self.url.set_text(txt)