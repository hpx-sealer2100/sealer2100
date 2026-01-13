import lvgl as lv
from storage import device
from trezor.ui import i18n, font, Done
from trezor.ui.screen import Modal
from trezor.ui.component import VStack
from trezor.ui.theme import Styles

class BluetoothPairing(Modal):
    def __init__(self, code: str):
        super().__init__()
        self.set_icon("A:/res/bluetooth-pairing.png")
        self.set_title(i18n.Title.bluetooth_pairing)
        self.btn.set_text(i18n.Button.close)
        self.btn.mode('close')
        self.btn.add_event_cb(lambda _: self.close(Done()), lv.EVENT.CLICKED, None)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.items_center()
        container.align(lv.ALIGN.TOP_MID, 0, 64)

        # tips
        label = container.add(lv.label)
        label.set_width(lv.pct(100))
        label.set_text(i18n.Text.bluetooth_pair)
        label.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
        label.set_long_mode(lv.label.LONG.WRAP)

        # code
        label = container.add(lv.label)
        label.set_text(code)
        label.set_style_text_font(font.Medium.PF44, 0)

