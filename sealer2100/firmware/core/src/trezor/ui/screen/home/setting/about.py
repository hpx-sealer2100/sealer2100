import lvgl as lv

from . import *
from trezor import log, workflow, motor, utils
from trezor.ui import i18n,colors
from trezor.ui.screen import Navigation,VStack
from storage import device
from trezor.ui.component import LabeledText, ItemToggle, Button

class About(Navigation):
    def __init__(self):        
        super().__init__()
        self.title.set_text(i18n.Setting.about_device)
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF) 
        self.content.add_style(Styles.container, lv.PART.MAIN)
        self.content.spacing(8)

        def labeld(label: str, text: str) -> LabeledText:
            obj = self.add(LabeledText)
            obj.set_label(label)
            obj.set_text(text)
            obj.desc.set_style_text_color(colors.USER.SUBTITLE, lv.PART.MAIN)
            obj.add_style(Styles.board, lv.PART.MAIN)
            obj.set_height(lv.SIZE.CONTENT)
            obj.spacing(8)
            return obj

        # mode
        labeld(i18n.Text.model, 'Sealer2100')

        # bluetooth name
        labeld(i18n.Text.bluetooth_name, utils.BLE_NAME or "")

        # system version
        labeld(i18n.Text.system_version, device.get_firmware_version())

        # bluetooth version
        labeld(i18n.Text.bluetooth_version, utils.BLE_VERSION or "")

        # bootloader version
        labeld(i18n.Text.bootloader, utils.boot_version())

        cnt = self.add(HStack)
        cnt.add_style(Styles.container, lv.PART.MAIN)
        cnt.add_style(Styles.board, lv.PART.MAIN)

        obj = cnt.add(LabeledText)
        obj.set_width(200)
        obj.set_label(i18n.Text.fcc)
        obj.set_text("288990KHJ")
        obj.spacing(8)

        # 填充
        obj = cnt.add(lv.obj)
        obj.remove_style_all()
        obj.set_height(1)
        obj.set_flex_grow(1)

        img = lv.img(cnt)
        img.set_src("A:/res/hp/ic_fc.png")
        
        CompatibleTrezor(self.content,i18n.Text.compatible)
        
        self.btn.set_text(i18n.Text.system_update)
        self.btn.add_event_cb(self.on_click_system_udpate, lv.EVENT.CLICKED, None)
        
    def on_click_system_udpate(self, e):
        from .system_update import SystemUpdate
        workflow.spawn(SystemUpdate().show())
    
        
class CompatibleTrezor(ItemToggle):
    def __init__(self, parent,text):
        super().__init__(parent,text)
        self.add_style(Styles.board, lv.PART.MAIN)
        self.switch.set_size(40, 24)
    
    def toggle(self):
        motor.vibrate()
        enabled = self.checked
        log.debug(__name__, f"Compatible Trezor {'enabled' if enabled else 'disabled'}")
        device.enable_trezor_compatible(enabled)
        