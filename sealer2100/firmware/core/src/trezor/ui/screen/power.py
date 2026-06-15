import lvgl as lv

from . import Modal
from trezor import utils, workflow, loop
from trezor.ui import i18n, colors, Style
from trezor.ui.component import VStack
from trezor.ui.screen.confirm import Confirm
from trezor.ui.screen.message import Message

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Callable
    pass

class PowerOff(Confirm):
    def __init__(self):
        super().__init__()
        self.set_title(i18n.Title.power_off)
        self.set_icon( "A:/res/hp/ic_guanji2.png")
        self.btn_cancel.set_text(i18n.Button.cancel)
        self.btn_confirm.set_text(i18n.Button.confirm)

    def on_click_confirm(self):
        # no need close self, so we not call super
        from trezor.ui.layouts import wait_doing
        # workflow.spawn(wait_doing(i18n.Text.shutting_down, (lambda: (yield utils.power_off()))()))
        async def do_poweroff():
            from storage import device
            from trezor import io, uart
            await loop.sleep(1500)
            if device.ble_enabled():
                uart.ctrl_ble_disconnect()
                await loop.sleep(10)
                io.BLE().power_off()
            utils.power_off()
        workflow.spawn(wait_doing(i18n.Text.shutting_down, do_poweroff()))

class Restart(Confirm):
    def __init__(self):
        super().__init__()
        self.set_title(i18n.Title.restart)
        self.btn_confirm.set_text(i18n.Button.restart)
        self.btn_confirm.color(colors.USER.DANGER)

    def on_click_confirm(self):
        # no need close self, so we not call super
        from trezor.ui.layouts import wait_doing
        # workflow.spawn(wait_doing(i18n.Text.shutting_down, (lambda: (yield utils.power_off()))()))
        async def do_restart():
            await loop.sleep(1500)
            utils.reset()
        workflow.spawn(wait_doing(i18n.Text.restarting, do_restart()))

class LowPower(Message):
    on_confirm: Callable[[], None]|None

    def __init__(self, charge):
        msg = i18n.Text.low_power_message.format(charge)
        super().__init__(i18n.Title.low_power, msg)
        self.on_confirm = None

    def update_charge(self, charge: int):
        msg = i18n.Text.low_power_message.format(charge)
        self.text.set_text(msg)

    def on_click_ok(self):
        super().on_click_ok()
        if self.on_confirm:
            self.on_confirm()
