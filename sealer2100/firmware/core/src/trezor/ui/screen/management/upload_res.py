import lvgl as lv

from trezor.ui import Style, colors, font
from trezor.ui.component import VStack
from trezor.ui.screen import Modal

class LoadingResource(Modal):
    def __init__(self, res: str, total: int):
        super().__init__()

        self.total = total
        self.size = 0

        container = self.add(VStack)
        container.align(lv.ALIGN.TOP_MID, 0, 0)
        container.items_center()
        container.set_height(lv.SIZE.CONTENT)

        # icon
        self.img = container.add(lv.img)
        self.img.set_src("A:/res/hp/ic_loading_trans.png")

        self.label = container.add(lv.label)
        self.label.set_text(res)
        self.label.set_style_text_color(colors.USER.WHITE, lv.PART.MAIN)
        self.label.set_style_text_font(font.Medium.PF32, lv.PART.MAIN)
        self.label.set_width(lv.pct(100))
        self.label.set_height(lv.SIZE.CONTENT)
        self.label.set_long_mode(lv.label.LONG.WRAP)

        self.bar = self.add(lv.bar)
        self.bar.set_mode(lv.bar.MODE.NORMAL)
        self.bar.set_range(0, 100)
        self.bar.set_value(0, lv.ANIM.OFF)
        self.bar.add_style(
            Style()
            .width(213)
            .height(6)
            .radius(lv.RADIUS.CIRCLE)
            .bg_color(lv.color_hex(0))
            .pad_all(0),
            lv.PART.MAIN,
        )
        self.bar.add_style(
            Style().radius(lv.RADIUS.CIRCLE).bg_color(colors.USER.GREEN),
            lv.PART.INDICATOR,
        )
        self.bar.align(lv.ALIGN.BOTTOM_MID, 0, 0)

        # a animation
        a = lv.anim_t()
        a.init()
        a.set_var(self.img)
        a.set_time(1000)
        a.set_values(0, 3600)
        a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
        a.set_custom_exec_cb(lambda a, v: self.img.set_angle(v))

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, a)
        lv.anim_timeline_start(self.timeline)

    def on_deleting(self):
        super().on_deleting()
        lv.anim_timeline_stop(self.timeline)
        lv.anim_timeline_del(self.timeline)

    def update(self, size: int):
        self.size += size
        value = (self.size * 100) // self.total
        if value > 100:
            value = 100
        self.bar.set_value(value, lv.ANIM.OFF)

    def close(self, value):
        super().close(value)
