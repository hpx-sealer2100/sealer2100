import lvgl as lv

from trezor import log, workflow, utils
from trezor.wire import DUMMY_CONTEXT
from trezor.ui import i18n, Style, font, colors
from trezor.ui.component import VStack
from trezor.ui.screen import Navigation, Modal
from trezor.ui.theme import Styles
from storage import device
from .entry import ActionSettingEntry

class IrisManage(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.iris_management)

        options = [
            {
                "label": i18n.Setting.regist_iris,
                "action": self.regist_iris,
                "need_show": lambda x: x == device.DEVICE_PROTECT_TYPE_PIN
            },
            {
                "label": i18n.Setting.change_iris,
                "action": self.change_iris,
                "need_show": lambda x: x & device.DEVICE_PROTECT_TYPE_IRIS
            },
            {
                "label": i18n.Setting.remove_iris,
                "action": self.remove_iris,
                "need_show": lambda x: x == device.DEVICE_PROTECT_TYPE_PIN_AND_IRIS
            },
        ]
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Styles.container, lv.PART.MAIN)
        self.content.spacing(8)

        self.apps: list[ActionSettingEntry] = []
        for o in options:
            label, action = o["label"], o["action"]
            app = ActionSettingEntry(self.content, label, action)
            app.need_show = o["need_show"]
            app.add_style(Styles.board, lv.PART.MAIN)
            self.apps.append(app)

    def on_load_start(self):
        super().on_load_start()
        protect_type = device.get_device_protect_type()
        for app in self.apps:
            if app.need_show(protect_type):
                app.clear_flag(lv.obj.FLAG.HIDDEN)
            else:
                app.add_flag(lv.obj.FLAG.HIDDEN)
            
    def regist_iris(self):
        from apps.management.regist_iris import regist_iris

        workflow.spawn(regist_iris(DUMMY_CONTEXT))

    def change_iris(self):
        from apps.management.change_iris import change_iris

        workflow.spawn(change_iris(DUMMY_CONTEXT))

    def remove_iris(self):
        from apps.management.remove_iris import remove_iris

        workflow.spawn(remove_iris(DUMMY_CONTEXT))
