import lvgl as lv

from trezor import log
from trezor.ui import Style, events

class Swipedown(lv.obj):
    """
    Swipedown is a special component, which is a transparent object on top layer.

    It wait a swipe from left to right gesture, and send `events.SWIPEDOWN`.
    """
    _instance :"Swipedown"= None

    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = Swipedown()
        return cls._instance

    def __init__(self):
        layer = lv.layer_top()
        super().__init__(layer)
        self.add_style(
            Style()
            .width(lv.pct(80))
            .height(32)
            .bg_opa(lv.OPA.TRANSP)
            .border_width(0),
            lv.PART.MAIN
        )
        self.set_pos(96, 0)
        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self.clear_flag(lv.obj.FLAG.GESTURE_BUBBLE)
        self.add_event_cb(self.__on_swipe, lv.EVENT.GESTURE, None)

    def disable(self):
        self.add_flag(lv.obj.FLAG.HIDDEN)

    def enable(self):
        self.clear_flag(lv.obj.FLAG.HIDDEN)

    def __on_swipe(self, event):
        dir = lv.indev_get_act().get_gesture_dir()
        if dir == lv.DIR.BOTTOM:
            lv.indev_get_act().wait_release()
            # send events.SWIPEDOWN to active screen
            lv.event_send(lv.scr_act(), events.SWIPEDOWN, None)

