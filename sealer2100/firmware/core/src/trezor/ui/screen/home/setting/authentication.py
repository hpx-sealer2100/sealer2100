from trezor.ui.component.button import Button
from trezor.ui.component.labeled_item import LabeledText
import lvgl as lv
from . import *
from trezor.ui import i18n, colors
from trezor.ui.screen import Navigation, VStack
from storage import device
from trezor import workflow, utils



class Authentication(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.authentication)
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Styles.container, lv.PART.MAIN)
        self.content.spacing(8)
        infos = [
            {
                "label": i18n.Text.serial_number,
                "value": device.get_serial(),
            },
            {
                "label": i18n.Text.system_version,
                "value": device.get_firmware_version(),
            },
            {
                "label": i18n.Text.bluetooth_version,
                "value": utils.BLE_VERSION,
            },
            {
                "label": i18n.Text.bootloader,
                "value": utils.boot_version(),
            },
        ]

        for info in infos:
            lable, text = info['label'], info['value']
            item = self.add(LabeledText)
            item.set_label(lable)
            item.set_text(text)
            item.desc.set_style_text_font(font.small, lv.PART.MAIN)
            item.desc.set_style_text_color(colors.USER.SUBTITLE, lv.PART.MAIN)
            item.add_style(Styles.board, lv.PART.MAIN)


        self.btn.set_text(i18n.Button.verify_now)
        self.btn.add_event_cb(self.on_verify_device, lv.EVENT.CLICKED, None)

    def on_verify_device(self, event: lv.event_t):
        from .verify_device import VerifyDevice
        screen = VerifyDevice()
        workflow.spawn(screen.show())
