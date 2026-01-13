import lvgl as lv
from micropython import const

from . import *
from trezor import motor, io
from trezor.ui.screen.statusbar import StatusBar
from storage import device

BLE_CMD_CTRL = const(0x81)

BLE_CMD_CTRL_OP_OPEN = const(1)
BLE_CMD_CTRL_OP_CLOSE = const(2)
BLE_CMD_CTRL_OP_DISCONNECT = const(3)

class Bluetooth(ToggleItem):

    def __init__(self, parent):
        super().__init__(parent, i18n.Setting.bluetooth, "A:/res/ble-connected-two.png")
        self.checked = device.ble_enabled()

    def toggle(self):
        motor.vibrate()
        enabled = self.checked
        log.debug(__name__, f"bluetooth {'enabled' if enabled else 'disabled'}")
        device.set_ble_status(enabled)
        ble = io.BLE()
        if enabled:
            ble.power_on()
            StatusBar.instance().show_ble(StatusBar.BLE_STATE_ENABLED)#打开蓝牙图标
            from trezor import workflow, loop
            from trezor.uart import fetch_all
            async def get_ble_info():
                await loop.sleep(7)
                fetch_all()
            workflow.spawn(get_ble_info())
            # async def get_ble_info():
            #     count = 0
            #     while count < 20:
            #         await loop.sleep(1000)
            #         log.debug(__name__, f"count: {count}")
            #         count += 1
            #         fetch_all()
            # workflow.spawn(get_ble_info())

        else:
            # power off reduce battery use
            ble.ctrl(BLE_CMD_CTRL, BLE_CMD_CTRL_OP_DISCONNECT)
            ble.ctrl(BLE_CMD_CTRL, BLE_CMD_CTRL_OP_CLOSE)
            ble.power_off()
