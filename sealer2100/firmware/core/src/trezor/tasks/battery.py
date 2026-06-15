"""
background task for updating battery
"""
from trezor import io, utils, loop, log
from trezor.ui.screen.statusbar import StatusBar
from apps import base

battery = io.Battery()

LOW_STATE_OF_CHARGE = 5


async def updating_battery_state():

    if not battery.exist():
        log.debug(__name__, "battery not exist")
        StatusBar.instance().show_battery_img(100, True)
        return

    cached_charging = battery.charging()
    cached_state_of_charge = battery.state_of_charge()

    StatusBar.instance().show_battery_img(cached_state_of_charge, cached_charging)
    StatusBar.instance().show_charging(cached_charging)

    alert = LowPowerAlert()
    while True:
        await loop.sleep(1000)
        state_of_charge = battery.state_of_charge()
        if state_of_charge is None:
            continue
        charging = battery.charging()

        state_of_charge_changed = cached_state_of_charge != state_of_charge
        charging_changed = cached_charging != charging

        # need refresh if charge state changed or charging changed
        refresh = charging_changed or state_of_charge_changed

        if not refresh:
            continue

        if state_of_charge_changed:
            cached_state_of_charge = state_of_charge
            StatusBar.instance().show_battery_img(state_of_charge, charging)

        # cache charging state
        if charging_changed:
            cached_charging = charging
            StatusBar.instance().show_battery_img(state_of_charge, charging)
            StatusBar.instance().show_charging(charging)
            utils.lcd_resume()
            base.reload_settings_from_storage()
            if charging:
                # reset alert state
                alert.destroy()
                del alert
                alert = LowPowerAlert()

        if charging_changed or state_of_charge_changed:
            # less than 20%, show a message
            if not charging and state_of_charge <= LOW_STATE_OF_CHARGE:
                await alert(state_of_charge)

        # not connect usb, and battery is empty, shut down device
        if not state_of_charge and not charging:
            from trezor.ui import i18n
            from trezor import workflow, uart
            from storage import device
            from trezor.ui.layouts import wait_doing
            # workflow.spawn(wait_doing(i18n.Text.shutting_down, (lambda: (yield utils.power_off()))()))
            async def do_poweroff():
                await loop.sleep(1500)
                if device.ble_enabled():
                    uart.ctrl_ble_disconnect()
                    await loop.sleep(10)
                    io.BLE().power_off()
                utils.power_off()
            workflow.spawn(wait_doing(i18n.Text.shutting_down, do_poweroff()))

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Literal
    AlertState = Literal[
        # Just create
        'None',
        # Showing `LowPower` screen
        'Showing',
        # user have click confirm, screen has disappeared
        'Confirmed'
        ]
from trezor.ui.screen.power import LowPower

class LowPowerAlert():
    state: AlertState
    screen: LowPower | None
    def __init__(self):
        self.state = 'None'
        self.screen = None

    def destroy(self):
        from trezor.ui import Cancel
        if self.screen:
            self.screen.close(Cancel())

    async def __call__(self, charge: int):
        if self.state == 'Confirmed':
            return
        elif self.state == 'Showing':
            self.screen.update_charge(charge)
        elif self.state == 'None':
            self.state = 'Showing'
            self.screen = LowPower(charge)
            self.screen.on_confirm = self.on_confirm
            await self.screen.show()

    def on_confirm(self):
        self.state = 'Confirmed'
        self.screen = None
