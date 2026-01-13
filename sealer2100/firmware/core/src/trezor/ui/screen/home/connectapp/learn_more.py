import lvgl as lv
from . import *
from trezor.ui import i18n, events
from trezor.ui.screen import Navigation


class LearnMore(Navigation):
    def __init__(self, icon):
        super().__init__()
        self.title.set_text(i18n.Title.learn_more)
        self.subtitle.set_text(i18n.Subtitle.learn_more)
        self.btn.set_text(i18n.Button.close)
        self.btn.mode("close")


        app_img = self.add(lv.img)
        app_img.align(lv.ALIGN.TOP_MID, 0, 32)
        app_img.add_style(Styles.group, lv.PART.MAIN)
        app_img.set_src(icon)

        self.btn.add_event_cb(self.on_close, lv.EVENT.CLICKED, None)

    def on_close(self, event: lv.event_t):
        lv.event_send(self, events.NAVIGATION_BACK, None)
