import lvgl as lv

from . import Modal
from trezor import log
from trezor.ui import i18n, Cancel, events, colors, Style, theme, font
from trezor.ui.component import ArcHolder, VStack, HStack

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Literal
    ConfirmMode = Literal['normal', 'result']


class Confirm(Modal):
    """
    Confirm screen have 2 buttons: `cancel` and `ok`

    User can wait this screen to get result
    """

    def __init__(self):
        super().__init__()

        # `cancel` and `confirm` buttons
        self.btn_cancel = self.btn_left
        self.btn_confirm = self.btn_right
        self.btn_cancel.set_text(i18n.Button.cancel)
        self.btn_confirm.set_text(i18n.Button.confirm)

        self.btn_cancel.mode('cancel') # default cancel mode
        self.btn_cancel.add_event_cb(
            lambda _: self.on_click_cancel(), lv.EVENT.CLICKED, None
        )

        self.btn_confirm.add_event_cb(
            lambda _: self.on_click_confirm(), lv.EVENT.CLICKED, None
        )

    def on_click_confirm(self):
        log.debug(__name__, "Confirm click ok")
        from trezor.ui import Confirm
        self.close(Confirm())

    def on_click_cancel(self):
        log.debug(__name__, "Confirm click cancel")
        self.close(Cancel())

class SimpleConfirm(Confirm):
    """
    Confirm with a message
    """

    def __init__(self, message: str):
        super().__init__()
        self.text = self.add(lv.label)
        self.text.add_style(theme.Styles.subtitle, lv.PART.MAIN)
        self.text.set_text(message)

    def mode(self, mode: ConfirmMode):
        if mode == 'result':
            #self.title.items_center()
            #self.title.text.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
            self.text.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)

    def text_color(self, color):
        self.text.set_style_text_color(color, 0)


class HolderConfirm(Confirm):
    """
    Confirm with a message with a holder button, when user press `holder` button enough time,
    `ok` button will be enabled
    """

    def __init__(self):
        super().__init__()

        # disable `ok` button, user need hold sometime on to enable `ok` button
        self.btn_confirm.add_state(lv.STATE.DISABLED)
        self.set_style_pad_left(16, lv.PART.MAIN)
        self.set_style_pad_right(16, lv.PART.MAIN)
        self.create_content(VStack)
        self.content : VStack
        self.content.items_center()
        self.content.add_style(theme.Styles.board, lv.PART.MAIN)
        self.content.reverse()
        self.content.set_style_pad_bottom(32, lv.PART.MAIN)

        # a holder button, user need hold sometime on to enable `ok` button
        self.holder = self.add(ArcHolder)
        self.holder.set_text(i18n.Button.hold)
        self.holder.add_event_cb(
            lambda _: self.btn_confirm.clear_state(lv.STATE.DISABLED),
            events.HOLDER_DONE,
            None,
        )

        # `content` is remained, for draw all other ui components
        self.create_content(lv.obj)
        self.content.set_flex_grow(1)
