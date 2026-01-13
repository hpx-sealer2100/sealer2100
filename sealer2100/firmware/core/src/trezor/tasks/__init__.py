"""
background tasks for update device state

Maybe it should be called service?
"""

# we not export anything
__all__ =[]

from trezor import loop

from .battery import updating_battery_state
loop.schedule(updating_battery_state())

from .button import handle_power
loop.schedule(handle_power())

from .usb_state import handle_usb_state
loop.schedule(handle_usb_state())
