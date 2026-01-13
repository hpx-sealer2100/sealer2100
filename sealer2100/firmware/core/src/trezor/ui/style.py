import lvgl as lv

class Style(lv.style_t):
    """
    lvgl style wrapper
    """
    def __init__(self):
        super().__init__()
        self.init()

    def reset(self) -> "Style":
        super().reset()
        return self

    def radius(self, radius) -> "Style":
        self.set_radius(radius)
        return self

    def align(self, align) -> "Style":
        self.set_align(align)
        return self

    def text_color(self, text_color) -> "Style":
        self.set_text_color(text_color)
        return self

    def text_opa(self, text_opa) -> "Style":
        self.set_text_opa(text_opa)
        return self

    def text_align(self, text_align) -> "Style":
        self.set_text_align(text_align)
        return self

    def text_align_right(self) -> "Style":
        self.set_text_align(lv.TEXT_ALIGN.RIGHT)
        return self

    def text_align_left(self) -> "Style":
        self.set_text_align(lv.TEXT_ALIGN.LEFT)
        return self

    def text_align_center(self) -> "Style":
        self.set_text_align(lv.TEXT_ALIGN.CENTER)
        return self

    def text_font(self, text_font) -> "Style":
        self.set_text_font(text_font)
        return self

    def bg_color(self, bg_color) -> "Style":
        self.set_bg_color(bg_color)
        return self

    def bg_opa(self, bg_opa=lv.OPA.COVER) -> "Style":
        self.set_bg_opa(bg_opa)
        return self

    def text_line_space(self, line_space) -> "Style":
        self.set_text_line_space(line_space)
        return self

    def text_letter_space(self, letter_space) -> "Style":
        self.set_text_letter_space(letter_space)
        return self

    def pad_all(self, pad) -> "Style":
        self.set_pad_all(pad)
        return self

    def pad_hor(self, pad) -> "Style":
        self.set_pad_hor(pad)
        return self

    def pad_ver(self, pad) -> "Style":
        self.set_pad_ver(pad)
        return self

    def pad_gap(self, gap) -> "Style":
        """pad row + pad column"""
        self.set_pad_gap(gap)
        return self

    def pad_row(self, pad) -> "Style":
        self.set_pad_row(pad)
        return self

    def pad_column(self, pad) -> "Style":
        self.set_pad_column(pad)
        return self

    def pad_top(self, pad) -> "Style":
        self.set_pad_top(pad)
        return self

    def pad_bottom(self, pad) -> "Style":
        self.set_pad_bottom(pad)
        return self

    def pad_left(self, pad) -> "Style":
        self.set_pad_left(pad)
        return self

    def pad_right(self, pad) -> "Style":
        self.set_pad_right(pad)
        return self

    def transform_height(self, height) -> "Style":
        self.set_transform_height(height)
        return self

    def transform_width(self, width) -> "Style":
        self.set_transform_width(width)
        return self

    def transform_zoom(self, zoom) -> "Style":
        self.set_transform_zoom(zoom)
        return self

    def translate_y(self, y) -> "Style":
        self.set_translate_y(y)
        return self

    def translate_x(self, x) -> "Style":
        self.set_translate_x(x)
        return self

    def bg_img_src(self, src) -> "Style":
        self.set_bg_img_src(src)
        return self

    def bg_img_opa(self, opa) -> "Style":
        self.set_bg_img_opa(opa)
        return self

    def bg_img_recolor(self, color) -> "Style":
        self.set_bg_img_recolor(color)
        return self

    def bg_img_recolor_opa(self, opa) -> "Style":
        self.set_bg_img_recolor_opa(opa)
        return self

    def border_width(self, width) -> "Style":
        self.set_border_width(width)
        return self
    def border_side(self, side) -> "Style":
        self.set_border_side(side)
        return self

    def max_height(self, height) -> "Style":
        self.set_max_height(height)
        return self

    def max_width(self, width) -> "Style":
        self.set_max_width(width)
        return self

    def min_width(self, width) -> "Style":
        self.set_min_width(width)
        return self

    def min_height(self, height) -> "Style":
        self.set_min_height(height)
        return self

    def transition(self, transition) -> "Style":
        self.set_transition(transition)
        return self

    def bg_grad_color(self, color) -> "Style":
        self.set_bg_grad_color(color)
        return self

    def bg_grad_dir(self, dir) -> "Style":
        self.set_bg_grad_dir(dir)
        return self

    def bg_main_stop(self, value) -> "Style":
        self.set_bg_main_stop(value)
        return self

    def bg_grad_stop(self, value) -> "Style":
        self.set_bg_grad_stop(value)
        return self

    def border_color(self, color) -> "Style":
        self.set_border_color(color)
        return self

    def border_opa(self, opa=lv.OPA.COVER) -> "Style":
        self.set_border_opa(opa)
        return self

    def width(self, width) -> "Style":
        self.set_width(width)
        return self

    def height(self, height) -> "Style":
        self.set_height(height)
        return self

    def size(self, value) -> "Style":
        self.set_size(value)
        return self

    def grid_column_dsc_array(self, arr) -> "Style":
        self.set_grid_column_dsc_array(arr)
        return self

    def grid_row_dsc_array(self, arr) -> "Style":
        self.set_grid_row_dsc_array(arr)
        return self

    def line_width(self, width) -> "Style":
        self.set_line_width(width)
        return self

    def line_color(self, color) -> "Style":
        self.set_line_color(color)
        return self

    def line_dash_gap(self, gap) -> "Style":
        self.set_line_dash_gap(gap)
        return self

    def line_dash_width(self, value):
        self.set_line_dash_width(value)
        return self

    def line_opa(self, opa: lv.opa_t) -> "Style":
        self.set_line_opa(opa)
        return self

    def color_filter_opa(self, value):
        self.set_color_filter_opa(value)
        return self
    def color_filter_dsc(self, value):
        self.set_color_filter_dsc(value)
        return self
    def shadow_color(self, color) -> "Style":
        self.set_shadow_color(color)
        return self
    def shadow_opa(self, opa) -> "Style":
        self.set_shadow_opa(opa)
        return self
    def shadow_width(self, value):
        self.set_shadow_width(value)
        return self
    def shadow_ofs_x(self, value):
        self.set_shadow_ofs_x(value)
        return self
    def shadow_ofs_y(self, value):
        self.set_shadow_ofs_y(value)
        return self
    def shadow_spread(self, value):
        self.set_shadow_spread(value)
        return self
    def flex_main_place(self, value):
        self.set_flex_main_place(value)
        return self
    def flex_cross_place(self, value):
        self.set_flex_cross_place(value)
        return self
    def flex_track_place(self, value):
        self.set_flex_track_place(value)
        return self
    def flex_flow(self, value):
        self.set_flex_flow(value)
        return self
