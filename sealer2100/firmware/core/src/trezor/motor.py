import utime

import storage.device
from trezor import io, utils

if not utils.EMULATOR:
    MOTOR_CTL = io.MOTOR()

    def vibrate():
        if not storage.device.keyboard_haptic_enabled():
            return

        # vibrate
        MOTOR_CTL.ctrl(True)
        # sleep
        utime.sleep_ms(30)
        # stop
        MOTOR_CTL.ctrl(False)

else:

    def vibrate():
        pass
