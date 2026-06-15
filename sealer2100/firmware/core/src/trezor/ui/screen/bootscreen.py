import lvgl as lv

from . import Screen
from trezor.ui import font, Style, colors
from trezor.ui.theme import Styles
from trezor.ui.component.container import HStack
from storage import device


class BootScreen(Screen):
    def __init__(self):
        super().__init__()
        wallpaper = device.get_homescreen()
        self.set_style_bg_img_src(wallpaper, lv.PART.MAIN)
        bar = self.add(HStack)
        bar.add_style(Styles.container, lv.PART.MAIN)
        bar.set_width(46)
        bar.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN)

        bar.align(lv.ALIGN.TOP_MID, 0, 0)

        style = Style().bg_color(colors.USER.WHITE).bg_opa()

        obj = bar.add(lv.obj)
        obj.remove_style_all()
        obj.add_style(style, lv.PART.MAIN)
        obj.set_size(4, 8)

        obj = bar.add(lv.obj)
        obj.remove_style_all()
        obj.add_style(style, lv.PART.MAIN)
        obj.set_size(8, 8)

        obj = bar.add(lv.obj)
        obj.remove_style_all()
        obj.add_style(style, lv.PART.MAIN)
        obj.set_size(4, 8)

        # logo
        img = lv.img(self)
        img.set_src("A:/res/logo.png")
        img.center()

        # 动画
        anim = lv.anim_t()
        anim.init()
        anim.set_var(bar)
        anim.set_time(400)  # 1.5秒完成
        anim.set_playback_time(400)
        anim.set_values(46, 128)
        anim.set_repeat_count(2)
        anim.set_custom_exec_cb(lambda _, v: self.update_animation(bar, v))  # 动态更新
        anim.set_ready_cb(lambda _: self.channel.publish(lv.EVENT.READY))
        anim.start()

        self.click_count = 0
        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

        # self.timeline = lv.anim_timeline_create()
        # lv.anim_timeline_add(self.timeline, 0, anim)
        # lv.anim_timeline_set_reverse(self.timeline, True)
        # lv.anim_timeline_start(self.timeline)

    def update_animation(self, bar: HStack, v):
        bar.set_width(v)

    def on_click(self, event: lv.event_t):
        from trezor import utils, log
        self.click_count += 1
        log.info(__name__, f"click_count: {self.click_count}")
        if self.click_count < 5:
            return

        utils.reboot_to_bootloader()


    async def show(self):
        from . import manager

        await manager.switch_scene(self)
