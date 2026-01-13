import lvgl as lv

from trezor import log, workflow, utils
from trezor.ui import i18n, Style, font, colors
from trezor.ui.component import VStack
from trezor.ui.screen import Navigation, Modal
from trezor.ui.theme import Styles
from storage import device
from trezor.wire import DUMMY_CONTEXT
from .entry import ActionSettingEntry


class PinManage(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.pin_management)
        options = [
            {
                "label": i18n.Setting.enable_pin,
                "action": self.enable_pin,
                "need_show": lambda x: x == device.DEVICE_PROTECT_TYPE_IRIS,
            },
            {
                "label": i18n.Setting.change_pin,
                "action": self.change_pin,
                "need_show": lambda x: x & device.DEVICE_PROTECT_TYPE_PIN,
            },
            {
                "label": i18n.Setting.remove_pin,
                "action": self.remove_pin,
                "need_show": lambda x: x == device.DEVICE_PROTECT_TYPE_PIN_AND_IRIS,
            },
        ]
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Styles.container, lv.PART.MAIN)
        self.content.spacing(8)
        self.apps: list[ActionSettingEntry] = []
        for a in options:
            label, action = a["label"], a["action"]
            app = ActionSettingEntry(self.content, label, action)
            app.add_style(Styles.board, lv.PART.MAIN)
            app.need_show = a["need_show"]
            self.apps.append(app)

    def on_load_start(self):
        super().on_load_start()
        protect_type = device.get_device_protect_type()
        for app in self.apps:
            if app.need_show(protect_type):
                app.clear_flag(lv.obj.FLAG.HIDDEN)
            else:
                app.add_flag(lv.obj.FLAG.HIDDEN)

    def enable_pin(self):
        from apps.management.enable_pin import enable_pin

        workflow.spawn(enable_pin(DUMMY_CONTEXT))

    def change_pin(self):
        from apps.management.change_pin import change_pin
        from trezor.messages import ChangePin

        workflow.spawn(change_pin(DUMMY_CONTEXT, ChangePin(remove=False)))

    def remove_pin(self):
        from apps.management.change_pin import change_pin
        from trezor.messages import ChangePin

        workflow.spawn(change_pin(DUMMY_CONTEXT, ChangePin(remove=True)))
