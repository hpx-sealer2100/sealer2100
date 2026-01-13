import lvgl as lv

class TabIndicator(lv.bar):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.set_mode(lv.bar.MODE.RANGE)
        self.set_style_radius(lv.RADIUS.CIRCLE, lv.PART.MAIN)
        self.set_style_radius(lv.RADIUS.CIRCLE, lv.PART.INDICATOR)
        self.total = 1

    def radius(self, value: int):
        self.set_style_radius(value, lv.PART.MAIN)
        self.set_style_radius(value, lv.PART.INDICATOR)

    def colors(self, bg: lv.color16_t, indicator: lv.color16_t):
        self.set_style_bg_color(bg, lv.PART.MAIN)
        self.set_style_bg_color(indicator, lv.PART.INDICATOR)

    def set_total(self, total: int):
        self.total = total
        self.set_range(0, total)

    def set_current(self, cur: int):
        if cur < 0:
            cur = 0

        if cur >= self.total:
            cur = self.total - 1

        self.set_value(cur + 1, lv.ANIM.OFF)
        # must set start value after
        self.set_start_value(cur, lv.ANIM.OFF)