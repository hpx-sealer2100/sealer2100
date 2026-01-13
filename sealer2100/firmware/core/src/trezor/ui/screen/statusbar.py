import lvgl as lv

from storage import device
from trezor import log
from trezor.ui import font, Style, colors
from trezor.ui.component.container import HStack

class StatusBar(HStack):
    """
    StatusBar is a special screen, which show top on all screens(aka lvgl.top_layer).
    Contains `usb` `bluetooth` `battery` `charging` status
    """
    _instance: 'StatusBar' = None

    BLE_STATE_CONNECTED = 0
    BLE_STATE_DISABLED = 1
    BLE_STATE_ENABLED = 2

    @classmethod
    def instance(cls) -> "StatusBar":
        if not cls._instance:
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        super().__init__(lv.layer_top())
        self.remove_style_all()
        self.set_flex_align(lv.FLEX_ALIGN.END, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)
        self.add_style(
            Style()
            .width(lv.pct(100))
            .height(lv.SIZE.CONTENT)
            .border_width(0)
            .pad_row(8)
            .pad_right(8),
            0
        )
        self.align(lv.ALIGN.TOP_RIGHT, 0, 0)

        # airgap state
        self.airgap_only = self.add(lv.label)
        self.airgap_only.set_flex_grow(1)
        self.airgap_only.set_style_text_color(colors.USER.GRAY, lv.PART.MAIN)
        self.airgap_only.set_style_text_font(font.status_bar, lv.PART.MAIN)
        self.airgap_only.set_text("  Air Gap Only")

        self.airgap_only.add_flag(lv.obj.FLAG.HIDDEN)

        # usb
        self.usb = lv.img(self)
        self.usb.set_src("A:/res/usb.png")
        self.usb.set_style_img_recolor_opa(lv.OPA.COVER, lv.PART.MAIN)
        self.usb.set_style_img_recolor(colors.USER.GRAY, lv.PART.MAIN)
        self.usb.add_flag(lv.obj.FLAG.HIDDEN)

        # ble
        self.ble = lv.img(self)
        self.ble.set_src("A:/res/ble-enabled.png" if device.ble_enabled() else "A:/res/ble-disabled.png")
        self.ble.set_style_img_recolor_opa(lv.OPA.COVER, lv.PART.MAIN)
        self.ble.set_style_img_recolor(colors.USER.GRAY, lv.PART.MAIN)
        self.ble.add_flag(lv.obj.FLAG.HIDDEN)

        # battery capacity icon
        self.battery_icon = lv.img(self)
        self.battery_icon.set_style_img_recolor_opa(lv.OPA._50, lv.PART.MAIN)
        self.battery_icon.add_flag(lv.obj.FLAG.HIDDEN)

        # battery charging icon
        self.battery_charging = lv.img(self)
        self.battery_charging.set_src("A:/res/charging.png")
        self.battery_charging.add_flag(lv.obj.FLAG.HIDDEN)

        # battery capacity percent
        self.battery_percent = lv.label(self)
        self.battery_percent.set_style_text_font(font.status_bar, 0)
        self.battery_percent.add_flag(lv.obj.FLAG.HIDDEN)

    def show_ble(self, status: int):
        log.debug(__name__, f"show ble {status}")
        if status == StatusBar.BLE_STATE_CONNECTED:
            icon = "A:/res/ble-connected.png"
        elif status == StatusBar.BLE_STATE_ENABLED:
            icon = "A:/res/ble-enabled.png"
        elif status == StatusBar.BLE_STATE_DISABLED:
            icon = "A:/res/ble-disabled.png"
        else:
            # this should never happen
            assert f"unknown ble status: {status}"
        self.ble.set_src(icon)

        if status == StatusBar.BLE_STATE_DISABLED:
            self.ble.add_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.ble.clear_flag(lv.obj.FLAG.HIDDEN)

    def show_usb(self, show: bool = False):
        log.debug(__name__, f"show usb {show}")
        if show:
            self.usb.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.usb.add_flag(lv.obj.FLAG.HIDDEN)

    def show_battery_img(self, value: int, charging: bool):
        # log.debug(__name__, f"show battery: {value}, charging {charging}")
        percent = min(value, 100)
        if charging:
            self.battery_percent.clear_flag(lv.obj.FLAG.HIDDEN)
            self.battery_percent.set_text(f"{percent}%")
            self.battery_percent.set_style_text_color(colors.USER.GRAY, lv.PART.MAIN)  # 设置文本颜色为白色
        else:
            # self.battery_percent.add_flag(lv.obj.FLAG.HIDDEN)
            self.battery_percent.clear_flag(lv.obj.FLAG.HIDDEN)
            self.battery_percent.set_text(f"{percent}%")
            self.battery_percent.set_style_text_color(colors.USER.GRAY, lv.PART.MAIN)  # 设置文本颜色为白色

        percent = ((value+5)//5)*5
        percent = min(percent, 100)

        state = "normal" if not charging else "charging"
        icon = f"A:/res/battery-{percent}-{state}.png"
        self.battery_icon.clear_flag(lv.obj.FLAG.HIDDEN)
        self.battery_icon.set_src(icon)

        if charging:
            self.battery_icon.set_style_img_recolor(colors.USER.GREEN, lv.PART.MAIN)
        else:
            self.battery_icon.set_style_img_recolor(colors.USER.GRAY, lv.PART.MAIN)

    def show_battery_none(self):
        self.battery_percent.add_flag(lv.obj.FLAG.HIDDEN)
        self.battery_icon.clear_flag(lv.obj.FLAG.HIDDEN)
        self.battery_icon.set_src("A:/res/battery-none.png")

    def show_charging(self, show: bool = False):
        log.debug(__name__, f"show charging {show}")
        if show:
            self.battery_charging.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.battery_charging.add_flag(lv.obj.FLAG.HIDDEN)

    def show_airgap_only(self, show: bool):
        if show:
            self.airgap_only.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.airgap_only.add_flag(lv.obj.FLAG.HIDDEN)
