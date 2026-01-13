import lvgl as lv
from typing import Callable

from storage import device
from trezor import log, workflow
from trezor.ui import i18n, events,font
from trezor.ui.theme import Styles
from trezor.ui.screen import Screen, Navigation
from trezor.ui.component import HStack, VStack
from trezor.ui.component import Swipedown


class FrontApp(Screen):
    def __init__(self):
        super().__init__()
        self.set_style_pad_top(64, lv.PART.MAIN)# 设置顶部填充
        self.set_style_bg_color(lv.color_hex(0x0D0D17), lv.PART.MAIN)# 设置背景颜色

        self.add_event_cb(self.on_swipe_back, lv.EVENT.GESTURE, None)# 注册手势事件

        tip = lv.label(self)# 创建一个label
        tip.set_text(i18n.Tip.swipe_down_to_close)# 设置文本
        tip.align(lv.ALIGN.TOP_MID, 0, -32)# 设置位置
        tip.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN)

        # apps
        self.create_content(HStack)# 创建内容
        self.content : HStack# 设置内容为VStack
        self.content.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)# 设置排列方式
        self.content.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_EVENLY, lv.PART.MAIN)# 设置主轴排列方式
        self.content.set_style_pad_top(40, lv.PART.MAIN)  # 设置顶部填充
        self.content.set_style_pad_row(20, lv.PART.MAIN)  # 设置行间距
        self.content.align(lv.ALIGN.TOP_MID, 0, 0)# 设置位置


        # btn1 = lv.btn(self)
        # btn1.set_size(50, 10)
        # btn1.align(lv.ALIGN.BOTTOM_MID, 0, -32)
        # btn1.set_style_bg_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN)

        # btn2 = lv.btn(self)
        # btn2.set_size(50, 10)
        # btn2.align(lv.ALIGN.BOTTOM_MID, 0, -32)
        # btn2.set_style_bg_color(lv.color_hex(0x2E4772), lv.PART.MAIN)

        btn_container = lv.obj(self)
        btn_container.set_size(160, 50)  # 设置容器大小
        btn_container.align(lv.ALIGN.BOTTOM_MID, 0, -32)  # 对齐到底部
        btn_container.set_flex_flow(lv.FLEX_FLOW.ROW)  # 让子元素水平排列
        btn_container.set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)  # 主轴、交叉轴、子元素居中
        btn_container.set_style_bg_opa(lv.OPA.TRANSP, lv.PART.MAIN)  # 透明背景
        btn_container.set_style_border_width(0, lv.PART.MAIN)

        # 按钮 1
        btn1 = lv.btn(btn_container)
        btn1.set_size(50, 10)
        btn1.set_style_bg_color(lv.color_hex(0x2E4772), lv.PART.MAIN)

        # 按钮 2
        btn2 = lv.btn(btn_container)
        btn2.set_size(50, 10)
        btn2.set_style_bg_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN)

        apps = [
            # guide
            {
                "label": i18n.App.guide,
                "icon": "A:/res/app_guide_six.png",
                "action": self.click_guide,
            },
            # setting
            {
                "label": i18n.App.setting,
                "icon": "A:/res/app_settings_six.png",
                "action": self.click_setting,
            }
        ]

        for a in apps:
            label, icon, action = a["label"], a["icon"], a["action"]
            app = Item(self.content, label, icon)
            app.on_click = action


    def on_swipe_back(self, event: lv.event_t):# 下滑事件
        print("手势事件触发")
        dir = lv.indev_get_act().get_gesture_dir()
        if dir == lv.DIR.RIGHT:
            lv.indev_get_act().wait_release()
            from .home import HomeScreen
            workflow.spawn(HomeScreen().show())

        elif dir == lv.DIR.BOTTOM:
            from apps.base import lock_device
            lock_device()


    def click_guide(self, app: 'Item'):
        log.debug(__name__, "click guide")
        from .guide import GuideApp
        workflow.spawn(GuideApp().show())

    def click_setting(self, app: 'Item'):
        from .setting import SettingApp
        workflow.spawn(SettingApp().show())

    async def show(self):
        from trezor.ui.screen import manager
        await manager.switch_scene(self)

class Item(VStack):

    def __init__(self, parent, label: str, icon: str):
        super().__init__(parent)
        self.add_style(Styles.home_app, lv.PART.MAIN)
        self.add_style(Styles.pressed, lv.STATE.PRESSED)
        self.items_center()
        self.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_EVENLY, lv.PART.MAIN)
        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)  # 禁止拖拽

         # 取消 `HStack` 内部默认的 padding
        self.set_style_pad_column(0, lv.PART.MAIN)
        self.set_style_pad_row(0, lv.PART.MAIN)

        img = lv.img(self)
        img.set_src(icon)
        img.clear_flag(lv.obj.FLAG.SCROLLABLE)  # 禁止拖拽
        title = lv.label(self)
        title.set_text(label)

        title.set_style_text_font(font.Medium.PF28, lv.PART.MAIN)
        title.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN)

        # a callback function when `App` is clicked
        # firs argument is self and return None
        self.on_click: Callable[["Item"], None] = None

        self.add_event_cb(self.__on_clicked, lv.EVENT.CLICKED, None)

    def __on_clicked(self, event: lv.event_t):
        if self.on_click:
            self.on_click(self)

