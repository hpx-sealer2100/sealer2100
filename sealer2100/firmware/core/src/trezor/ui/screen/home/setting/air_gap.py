import lvgl as lv

from trezor import log, io
from trezor.ui import i18n,colors
from storage import device

from .detail import ToggleDetail

class AirGap(ToggleDetail):
    @classmethod
    def get_current_value(cls):
        return device.airgap_enabled()

    def __init__(self):
        super().__init__(i18n.Setting.airgap_mode, i18n.Text.airgap_mode)
        self.desc.set_text(i18n.Text.airgap_mode_desc)
        self.desc.set_style_text_color(colors.USER.TAUPE,lv.PART.MAIN)

    def on_value_changed(self, e):
        from trezor import workflow
        workflow.spawn(self.togging())

    async def togging(self):
        enable = self.switch.has_state(lv.STATE.CHECKED)
        if enable:
            icon = "A:/res/hp/ic_air_open.png"
            title = i18n.Title.airgap_mode_open
            msg = i18n.Subtitle.airgap_mode_open
            btn = i18n.Button.enable
        else:
            icon = "A:/res/hp/ic_air_close.png"
            title = i18n.Title.airgap_mode_close
            msg = i18n.Subtitle.airgap_mode_close
            btn = i18n.Button.close
        from trezor.ui.screen.confirm import SimpleConfirm
        screen = SimpleConfirm(msg)
        screen.set_title(title)
        screen.set_icon(icon)
        screen.btn_confirm.set_text(btn)
        screen.mode('result')
        await screen.show()
        confirmed = await screen

        if confirmed:
            self.airgap_enabled(enable)
        else:
            if enable:
                self.switch.clear_state(lv.STATE.CHECKED)
            else:
                self.switch.add_state(lv.STATE.CHECKED)


    def airgap_enabled(self, enable: bool):
        ble = io.BLE()
        from trezor import workflow, loop
        from trezor.uart import ctrl_ble_enabled
        from trezor.ui.screen.statusbar import StatusBar
        StatusBar.instance().show_airgap_only(enable)

        if not enable:
            async def ble_power_on():
                ble.power_on()
                while not device.ble_enabled():
                    log.debug(__name__, "power on ble ...")
                    await loop.sleep(500)
                    ctrl_ble_enabled(True)
            workflow.spawn(ble_power_on())
        else:
            async def ble_power_off():
                ctrl_ble_enabled(False)
                await loop.sleep(200)
                ble.power_off()
            workflow.spawn(ble_power_off())
