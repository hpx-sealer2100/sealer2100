import lvgl as lv
# Medium
class Medium:
    PF20 = lv.font_ping_fang_medium_20
    PF32 = lv.font_ping_fang_medium_32
    PF44 = lv.font_ping_fang_medium_44

# Regular
class Regular:
    PF20 = lv.font_ping_fang_regular_20
    PF24 = lv.font_ping_fang_regular_24

# 等宽
class Mono:
    JB28 = lv.font_mono_reg_28

default = Regular.PF20
small = Regular.PF20
bold = Medium.PF20
medium = Medium.PF20
mono = Mono.JB28
status_bar = lv.font_status_bar
