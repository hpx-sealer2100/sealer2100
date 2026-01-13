
import lvgl as lv

from trezor import io, log, loop, workflow
from trezor.ui import i18n,font, Style, theme, colors
from trezor.ui.screen import Navigation
class ScanApp1(Navigation):
    def __init__(self):
        super().__init__()
        # Create bg_img
        bg_img = lv.img(self)
        bg_img.set_src("A:/res/hp/subtract.png")
        bg_img.set_size(480, 800)
        bg_img.set_style_img_recolor_opa(79, lv.PART.MAIN|lv.STATE.DEFAULT)
        bg_img.set_style_img_recolor(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
        bg_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

        # Create lab
        lab = lv.label(self)
        lab.set_text(i18n.Text.scan_app)
        lab.set_long_mode(lv.label.LONG.WRAP)
        lab.set_pos(0, 611)
        lab.set_size(480, lv.SIZE.CONTENT)
        lab.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
        lab.set_style_text_font(font.Regular.PF28, lv.PART.MAIN|lv.STATE.DEFAULT)
        lab.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

        # Create left_bottom_img
        left_bottom_img = lv.img(self)
        left_bottom_img.set_src("A:/res/hp/left_bottom.png")
        left_bottom_img.set_pos(67, 538)
        left_bottom_img.set_size(24, 24)
        left_bottom_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

        # Create left_top_img
        left_top_img = lv.img(self)
        left_top_img.set_src("A:/res/hp/left_top.png")
        left_top_img.set_pos(67, 217)
        left_top_img.set_size(24, 24)
        left_top_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

        # Create right_top_img
        right_top_img = lv.img(self)
        right_top_img.set_src("A:/res/hp/right_top.png")
        right_top_img.set_pos(388, 217)
        right_top_img.set_size(24, 24)
        right_top_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

        # Create right_bottom_img
        right_bottom_img = lv.img(self)
        right_bottom_img.set_src("A:/res/hp/right_bottom.png")
        right_bottom_img.set_pos(388, 538)
        right_bottom_img.set_size(24, 24)
        right_bottom_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

        self.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
