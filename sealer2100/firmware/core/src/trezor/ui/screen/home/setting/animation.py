from . import *

from storage import device

class Animation(ToggleItem):
    def __init__(self, parent):
        super().__init__(parent, i18n.Setting.animation, "A:/res/animation-two.png")
        self.checked = device.is_animation_enabled()

    def toggle(self):
        enabled = self.checked
        log.debug(__name__, f"animation {'enabled' if enabled else 'disabled'}")
        device.set_animation_enable(enabled)
