import lvgl as lv

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Literal

    ButtonMode = Literal[
        "primary", 'continue', 'done', 'retry', # default style, green background color
        "second", "cancel", "reject",  "info", 'close', # second style, white background color
        "danger", "delete", # danger style, red background color
        "switch", # switch style
    ]


class Button(lv.btn):
    def __init__(self, parent):
        super().__init__(parent)
        self.set_size(208, 76)
        self.label: lv.label = None

    def set_text(self, text: str):
        if not self.label:
            self.label = lv.label(self)
            self.label.center()

        self.label.set_text(text)

    def color(self, c):
        self.set_style_bg_color(c, 0)

    def bg_opa(self, opa):
        self.set_style_bg_opa(opa, 0)

    def text_color(self, c):
        self.set_style_text_color(c, 0)

    def width(self, c):
        self.set_width(c)

    def mode(self, mode: ButtonMode):
        from trezor.ui.colors import USER
        if mode in ("primary", 'continue', 'done', 'retry'):
            # the default 
            self.color(USER.DARK_GREEN)
            self.text_color(USER.GREEN)
        #elif mode in ("second", "cancel", "reject", "info", 'close'):
        elif mode in ("second", "cancel", "info", 'close'):
            #self.color(USER.WHITE)
            self.color(USER.DARK_GRAY)
            self.text_color(USER.WHITE)
        elif mode in ("danger", 'delete'):
            self.color(USER.DARK_RED)
            self.text_color(USER.RED)
        elif mode in ("reject"):
            self.color(USER.DARK_GRAY)
            self.text_color(USER.RED)
        elif mode in ("switch"):
            self.text_color(USER.WHITE)
            self.set_style_bg_opa(lv.OPA.TRANSP, lv.PART.MAIN)
            self.set_style_border_width(1, lv.PART.MAIN)
            self.set_style_border_color(USER.SILVER, lv.PART.MAIN)