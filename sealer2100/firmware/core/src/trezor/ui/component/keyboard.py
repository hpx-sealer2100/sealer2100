import lvgl as lv
from micropython import const

from trezor.ui.constants import MAX_PIN_LENGTH, MIN_PIN_LENGTH
from trezor.ui import colors, Style, font
from trezor.ui.theme import Styles
from trezor.ui.component.container import HStack
from trezor import motor, loop, log, utils
from storage import device
from trezor.crypto import random, bip39
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import List, Dict, Literal, Tuple, Any
    KeyboardInputEvent = Literal['none', 'ready', 'cancel', 'delete', 'style', 'char']
    pass

__BTN_CTRL = (
    lv.btnmatrix.CTRL.CLICK_TRIG
    | lv.btnmatrix.CTRL.NO_REPEAT
)

_KBD_EVENT_CHANGE_MODE = lv.event_register_id()
_KBD_MODE_LOWER = "abc"
_KBD_MODE_UPPER = "ABC"
_KBD_MODE_NUMBER = "123"
_KBD_MODE_SYMBOL = "#*<"

class Keyboard(lv.btnmatrix):
    """
    DO NOT use this class directly, use it's subclasses
    """

    def __init__(self, parent) -> None:
        super().__init__(parent)
        # a channel for notify caller
        self.channel = loop.chan()
        self.remove_style_all()
        self.add_style(Styles.keyboard, lv.PART.MAIN)
        self.add_style(Styles.keyboard_btn, lv.PART.ITEMS)
        self.add_style(Styles.keyboard_pressed, lv.PART.ITEMS|lv.STATE.PRESSED)
        self.add_style(Styles.keyboard_disabled, lv.PART.ITEMS|lv.STATE.DISABLED)

        self.add_event_cb(self.on_event, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(lambda e: self.channel.publish(e.code), lv.EVENT.CANCEL, None)
        self.add_event_cb(lambda e: self.channel.publish(e.code), lv.EVENT.READY, None)
        self._ta = None

    @property
    def textarea(self) -> lv.textarea | None:
        return self._ta

    @textarea.setter
    def textarea(self, ta: lv.textarea | None):
        self._ta = ta

    def set_textarea(self, ta: lv.textarea):
        self.textarea = ta

    # trigger when textarea content changed
    def content_changed(self):
        pass

    def input_event(self, txt: str) -> Tuple[KeyboardInputEvent, Any]:
        if not txt:
            return 'none', None
        elif txt in (lv.SYMBOL.CLOSE, lv.SYMBOL.KEYBOARD, lv.SYMBOL.DOWN):
            # click `close` or `keyboard` or `down`
            return 'cancel', None
        elif txt == lv.SYMBOL.OK:
            # click `ok`
            return 'ready', None
        elif txt in (lv.SYMBOL.NEW_LINE, 'Enter') and self.textarea and self.textarea.get_one_line():
            # click `new line` or `enter` when `ta` is one line mode
            return 'ready', None
        elif txt == lv.SYMBOL.BACKSPACE:
            # click `backspace`
            return 'delete', txt
        elif txt in (_KBD_MODE_LOWER, _KBD_MODE_UPPER, _KBD_MODE_NUMBER, _KBD_MODE_SYMBOL):
            return 'style', txt
        else:
            # click character
            return 'char', txt

    def on_event(self, event: lv.event_t):
        target = event.target

        btn_id = target.get_selected_btn()
        # do nothing if not a button
        if btn_id == lv.BTNMATRIX_BTN.NONE:
            return

        txt = target.get_btn_text(btn_id)
        ie, payload = self.input_event(txt)
        if ie == 'none':
            return
        elif ie == 'cancel':
            if self.textarea:
                lv.event_send(self.textarea, lv.EVENT.CANCEL, None)
            lv.event_send(target, lv.EVENT.CANCEL, None)
        elif ie == 'ready':
            if self.textarea:
                lv.event_send(self.textarea, lv.EVENT.READY, None)
            lv.event_send(target, lv.EVENT.READY, None)
        elif ie == 'delete':
            if self.textarea:
                self.textarea.del_char()
            self.content_changed()
        elif ie == 'style':
            log.debug(__name__, f"click change mode: {payload}")
            lv.event_send(self, _KBD_EVENT_CHANGE_MODE, payload)
        elif ie == 'char':
            if self.textarea:
                self.textarea.add_text(str(txt))
            self.content_changed()
        motor.vibrate()

class PinKeyboard(Keyboard):
    """A pin keyboard"""

    def __init__(self, parent) -> None:
        super().__init__(parent)

        self.ACCESS_BTN_ID = const(11)
        # btn id 9: backspace or close
        self.DELETE_BTN_ID = const(9)
        self.CLOSE_BTN_ID = const(9)
        self.set_style_text_font(font.Medium.PF44, lv.PART.ITEMS)

        # show length of user have input
        self.tip_count_min: int = 10
        self.min_pin_length: int = MIN_PIN_LENGTH
        self.max_pin_length: int = MAX_PIN_LENGTH

        # set keyboard
        self.set_size(lv.pct(100), 440)

        """
        1  2  3
        4  5  6
        7  8  9
        x  0  OK
        """
        nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
        if device.is_random_pin_map_enabled():
            random.shuffle(nums)
        nums = [str(i) for i in nums]
        # fmt: off
        self.maps = [
            nums[0], nums[1], nums[2], "\n",
            nums[3], nums[4], nums[5], "\n",
            nums[6], nums[7], nums[8], "\n",
            lv.SYMBOL.BACKSPACE, nums[9], lv.SYMBOL.OK,
            "",
        ]
        # fmt: on
        self.set_map(self.maps)
        self.set_btn_ctrl_all(__BTN_CTRL)

        self.accessable = False
        self.deleteable = False

        self.add_event_cb(self.on_draw, lv.EVENT.DRAW_PART_BEGIN, None)

    # override `content_changed`
    def content_changed(self):
        # no need this test, because only called when `textarea` is set
        # if not self.textarea:
        #     return
        count = len(self.textarea.get_text())
        max_count = self.textarea.get_max_length()
        if self.max_pin_length and max_count:
            max_count = min(self.max_pin_length, max_count)
        elif self.max_pin_length:
            max_count = self.max_pin_length

        # count in [min, max]
        self.accessable = max_count >= count >= (self.min_pin_length or 1)
        self.deleteable = count != 0

        # symbol = lv.SYMBOL.BACKSPACE if count else lv.SYMBOL.CLOSE
        # self.maps[12] = symbol
        # self.set_map(self.maps)


    @property
    def accessable(self):
        return not self.has_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @accessable.setter
    def accessable(self, value):
        if value:
            self.clear_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)
        else:
            self.set_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @property
    def deleteable(self):
        return not self.has_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @deleteable.setter
    def deleteable(self, value):
        if value:
            self.clear_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)
        else:
            self.set_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    def on_draw(self, event):
        dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
        if dsc.id == self.ACCESS_BTN_ID and self.accessable:
            dsc.rect_dsc.bg_color = colors.USER.DARK_GREEN
            dsc.label_dsc.color = colors.USER.GREEN


class Candidate(lv.obj):
    """Candiate componet for input mnemonic"""

    def __init__(self, parent):
        super().__init__(parent)
        self.label = lv.label(self)
        self.label.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.label.align(lv.ALIGN.LEFT_MID, 4, 0)
        self.label.add_style(
            Style()
            .height(lv.SIZE.CONTENT)
            .width(lv.SIZE.CONTENT)
            .text_font(font.Medium.PF32)
            .radius(8)
            .pad_all(16)
            .bg_opa(lv.OPA.COVER)
            .bg_color(colors.USER.DARK_GRAY),
            lv.PART.MAIN
        )

    @property
    def text(self):
        return self.label.get_text()

    @text.setter
    def text(self, txt):
        self.label.set_text(txt)


class MnemonicKeyboard(Keyboard):

    def __init__(self, parent) -> None:
        super().__init__(parent)
        self.ACCESS_BTN_ID = const(29)
        self.DELETE_BTN_ID = const(21)
        self.CLOSE_BTN_ID = const(21)
        self.X_BTN_INDEX = const(23)


        self.set_style_text_font(font.Medium.PF32, lv.PART.ITEMS)

        self.set_size(lv.pct(100), 286)
        self.set_style_pad_top(72, lv.PART.MAIN)

        container = HStack(self)
        container.align(lv.ALIGN.TOP_MID, 0, -72)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.spacing(6)
        container.clear_flag(lv.obj.FLAG.EVENT_BUBBLE)

        # add 3 candidates label
        self.tips: List[Candidate] = []
        for _ in range(3):
            tip = Candidate(container)
            tip.remove_style_all()
            tip.set_size(140, 60)
            tip.add_flag(lv.obj.FLAG.CLICKABLE)

            tip.add_event_cb(self.on_click_candidate, lv.EVENT.CLICKED, None)
            tip.add_flag(lv.obj.FLAG.HIDDEN)
            self.tips.append(tip)

        self.add_event_cb(self.on_draw, lv.EVENT.DRAW_PART_BEGIN, None)
        self.add_event_cb(self.on_ready, lv.EVENT.READY, None)

        self.default_state()

    @property
    def accessable(self):
        return not self.has_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @accessable.setter
    def accessable(self, value):
        if value:
            self.clear_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)
        else:
            self.set_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @property
    def deleteable(self):
        return not self.has_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @deleteable.setter
    def deleteable(self, value):
        if value:
            self.clear_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)
        else:
            self.set_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    def on_draw(self, event):
        dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
        if dsc.id == self.ACCESS_BTN_ID and self.accessable:
            dsc.rect_dsc.bg_color = colors.USER.DARK_GREEN
            dsc.label_dsc.color = colors.USER.GREEN

    def content_changed(self):
        # clear all tips
        for tip in self.tips:
            tip.text = ""
        txt = self.textarea.get_text()

        # update buttons state
        mask = bip39.word_completion_mask(txt)

        # if have world, can delete
        self.update_btn_state(mask)

        self.deleteable = bool(txt)

        # not have words begin with `x`
        if not txt:
            self.set_btn_ctrl(self.X_BTN_INDEX, lv.btnmatrix.CTRL.DISABLED)

        words = bip39.complete_word(txt)
        # log.debug(__name__, f"words: {words}")

        if not words:
            self.accessable = False
            # tow many candidates, not show tips
            for tip in self.tips:
                tip.add_flag(lv.obj.FLAG.HIDDEN)
            return

        candidates = words.rstrip().split()

        # if only one candidate or txt in candidates, enable access button
        self.accessable = txt in candidates or len(candidates) == 1

        count = len(candidates) if candidates else 0
        for i, tip in enumerate(self.tips):
            if i < count:
                tip.clear_flag(lv.obj.FLAG.HIDDEN)
            else:
                tip.add_flag(lv.obj.FLAG.HIDDEN)

        # show tips
        for candidate, tip in zip(candidates, self.tips):
            tip.text = candidate

    def on_ready(self, event):

        # get ready event from user click `Ok` button or click `candidate` label
        # 1. txt not empty
        # 2. candidate not too many
        # 3. txt in candidates or only one candidate
        txt = self.textarea.get_text()

        words = bip39.complete_word(txt)
        candidates = words.rstrip().split()

        # text of textarea already is mnemonic
        if txt in candidates:
            return
        # only one candidate
        self.textarea.set_text(candidates[0])
        self.content_changed()

    def on_click_candidate(self, event):
        target: Candidate = event.target
        obj = utils.first(self.tips, lambda tip: tip == target)
        txt = obj.text
        self.textarea.set_text(txt)
        self.content_changed()
        lv.event_send(self, lv.EVENT.READY, None)

    def update_btn_state(self, mask):
        # mask is a 27 bits mask
        # from right to left one bit represent a character a - z
        def have(c):
            return mask & (1 << (ord(c) - 97))

        for i in utils.count():
            c = self.get_btn_text(i)
            if not c:
                break
            if not "a" <= c <= "z":
                continue
            if have(c):
                self.clear_btn_ctrl(i, lv.btnmatrix.CTRL.DISABLED)
            else:
                self.set_btn_ctrl(i, lv.btnmatrix.CTRL.DISABLED)

    def default_state(self):
        # keyboard lower characters
        # fmt: off
        self.maps = [
            "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
            " ","a", "s", "d", "f", "g", "h", "j", "k", "l", " ", "\n",
            lv.SYMBOL.BACKSPACE, "z", "x", "c", "v", "b", "n", "m", lv.SYMBOL.OK, ""
        ]
        # fmt: on

        # see lvgl/src/extra/widgets/keyboard/lv_keyboard.c
        self.set_map(self.maps)

        # first line 10 characters
        self.ctrls = [__BTN_CTRL] * 10

        # second line 11 characters contains 2 placeholders
        self.ctrls.append(1 | lv.btnmatrix.CTRL.HIDDEN)  # placeholder
        self.ctrls.extend([2 | __BTN_CTRL] * 9)  # characters
        self.ctrls.append(1 | lv.btnmatrix.CTRL.HIDDEN)  # placeholder

        # third line 9 characters contains 2 symbols(backspace + ok)
        self.ctrls.append(3 |__BTN_CTRL)  # backspace
        self.ctrls.extend([2 | __BTN_CTRL] * 7)  # characters
        self.ctrls.append(3 | __BTN_CTRL)  # OK
        # not have word begin with `x`
        self.ctrls[self.X_BTN_INDEX] |= lv.btnmatrix.CTRL.DISABLED
        self.ctrls[self.DELETE_BTN_ID] |= lv.btnmatrix.CTRL.DISABLED

        # fourth line 3 symbol
        self.ctrls.extend([__BTN_CTRL] * 3)

        self.set_ctrl_map(self.ctrls)

        self.accessable = False
        self.deleteable = False

        for tip in self.tips:
            tip.add_flag(lv.obj.FLAG.HIDDEN)
            tip.text = ""

class PassphraseKeyboardStyle:
    maps: list[str]
    ctrls: list[int]

def to_str(obj):
    output = b''
    count = 0
    
    while not output.endswith(b'\x00'):
        count += 1
        output = bytes(obj.__dereference__(count))

    try:
        return output[:-1].decode('utf-8')
    except:
        return output[:-1].decode('latin1')

class PassphraseKeyboard(Keyboard):
    def __init__(self, parent) -> None:
        super().__init__(parent)
        self.ACCESS_BTN_ID = const(33)
        self.DELETE_BTN_ID = const(30)

        self.modes: Dict[str, PassphraseKeyboardStyle] = {}

        self.set_style_text_font(font.Medium.PF32, lv.PART.ITEMS)

        self.set_size(lv.pct(100), 216)
        self.init_modes()
        # default is `lower` case
        self.change_mode(_KBD_MODE_LOWER)
        self.deleteable = False

        self.add_event_cb(self.on_change_mode, _KBD_EVENT_CHANGE_MODE, None)

    @property
    def accessable(self):
        return not self.has_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @accessable.setter
    def accessable(self, value):
        if value:
            self.clear_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)
        else:
            self.set_btn_ctrl(self.ACCESS_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @property
    def deleteable(self):
        return not self.has_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    @deleteable.setter
    def deleteable(self, value):
        if value:
            self.clear_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)
        else:
            self.set_btn_ctrl(self.DELETE_BTN_ID, lv.btnmatrix.CTRL.DISABLED)

    def content_changed(self):
        txt = self.textarea.get_text()
        log.debug(__name__, f"ta: {txt}")
        self.deleteable = bool(txt)

    def init_modes(self):
        self.modes[_KBD_MODE_LOWER] = self.lower_style()
        self.modes[_KBD_MODE_UPPER] = self.upper_style()
        self.modes[_KBD_MODE_NUMBER] = self.number_style()
        self.modes[_KBD_MODE_SYMBOL] = self.symbol_style()
    
    def on_change_mode(self, e: lv.event_t):
        mode = to_str(e.get_param())
        log.debug(__name__, f"change mode: {mode}")
        self.change_mode(mode)

    def change_mode(self, mode: PassphraseKeyboardStyle):
        style = self.modes[mode]
        self.set_map(style.maps)
        self.set_ctrl_map(style.ctrls)

    def lower_style(self) -> PassphraseKeyboardStyle:
        # keyboard lower characters
        # fmt: off
        upper = _KBD_MODE_UPPER
        number = _KBD_MODE_NUMBER
        maps = [
            "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
            " ","a", "s", "d", "f", "g", "h", "j", "k", "l", " ", "\n",
            " ", upper, "z", "x", "c", "v", "b", "n", "m", " ", "\n",
            lv.SYMBOL.BACKSPACE, number, " ", lv.SYMBOL.OK, ""
        ]
        # fmt: on
        # see lvgl/src/extra/widgets/keyboard/lv_keyboard.c
        ctrls =[]
        # 10 characters
        ctrls.extend([__BTN_CTRL] * 10)
        # <placehoder> 9 characters contains <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.extend([2|__BTN_CTRL] * 9)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <placeholder><UPPER> 7 characters <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.append(4|__BTN_CTRL)
        ctrls.extend([2|__BTN_CTRL] * 7)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <backspace><NUMBER><space><OK>
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(8|__BTN_CTRL)
        ctrls.append(4|__BTN_CTRL)  # OK

        style = PassphraseKeyboardStyle()
        style.maps = maps
        style.ctrls = ctrls
        return style
        

    def upper_style(self) -> PassphraseKeyboardStyle:
        # keyboard lower characters
        # fmt: off
        lower = _KBD_MODE_LOWER
        number = _KBD_MODE_NUMBER
        maps = [
            "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "\n",
            " ","A", "S", "D", "F", "G", "H", "J", "K", "L", " ", "\n",
            " ", lower, "Z", "X", "C", "V", "B", "N", "M", " ", "\n",
            lv.SYMBOL.BACKSPACE, number, " ", lv.SYMBOL.OK, ""
        ]
        # fmt: on
        # see lvgl/src/extra/widgets/keyboard/lv_keyboard.c
        ctrls =[]
        # 10 characters
        ctrls.extend([__BTN_CTRL] * 10)
        # <placehoder> 9 characters contains <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.extend([2|__BTN_CTRL] * 9)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <placeholder><UPPER> 7 characters <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.append(4|__BTN_CTRL)
        ctrls.extend([2|__BTN_CTRL] * 7)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <backspace><NUMBER><space><OK>
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(8|__BTN_CTRL)
        ctrls.append(4|__BTN_CTRL)  # OK

        style = PassphraseKeyboardStyle()
        style.maps = maps
        style.ctrls = ctrls
        return style

    def number_style(self) -> PassphraseKeyboardStyle:
        # keyboard lower characters
        # fmt: off
        lower = _KBD_MODE_LOWER
        symbol = _KBD_MODE_SYMBOL
        maps = [
            "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n",
            " ","^", "_", "[", "]", "@", "$", "%", "{", "}", " ", "\n",
            " ", symbol, "`", "-", "/", ",", ".", ":", ";", " ", "\n",
            lv.SYMBOL.BACKSPACE, lower, " ", lv.SYMBOL.OK, ""
        ]
        # fmt: on
        # see lvgl/src/extra/widgets/keyboard/lv_keyboard.c
        ctrls =[]
        # 10 characters
        ctrls.extend([__BTN_CTRL] * 10)
        # <placehoder> 9 characters contains <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.extend([2|__BTN_CTRL] * 9)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <placeholder><UPPER> 7 characters <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.append(4|__BTN_CTRL)
        ctrls.extend([2|__BTN_CTRL] * 7)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <backspace><NUMBER><space><OK>
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(8|__BTN_CTRL)
        ctrls.append(4|__BTN_CTRL)  # OK

        style = PassphraseKeyboardStyle()
        style.maps = maps
        style.ctrls = ctrls
        return style

    def symbol_style(self) -> PassphraseKeyboardStyle:
        # keyboard lower characters
        # fmt: off
        number = _KBD_MODE_NUMBER
        lower = _KBD_MODE_LOWER
        maps = [
            "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n",
            " ","!", "?", "#", "~", "&", '"', "'", "(", ")", " ", "\n",
            " ", number, "+", "=", "<", ">", "\\", "|", "*", " ", "\n",
            lv.SYMBOL.BACKSPACE, lower, " ", lv.SYMBOL.OK, ""
        ]
        # fmt: on
        # see lvgl/src/extra/widgets/keyboard/lv_keyboard.c
        ctrls =[]
        # 10 characters
        ctrls.extend([__BTN_CTRL] * 10)
        # <placehoder> 9 characters contains <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.extend([2|__BTN_CTRL] * 9)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <placeholder><UPPER> 7 characters <placeholder>
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)
        ctrls.append(4|__BTN_CTRL)
        ctrls.extend([2|__BTN_CTRL] * 7)
        ctrls.append(1|lv.btnmatrix.CTRL.HIDDEN)

        # <backspace><NUMBER><space><OK>
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(4|__BTN_CTRL)  # Backspace
        ctrls.append(8|__BTN_CTRL)
        ctrls.append(4|__BTN_CTRL)  # OK

        style = PassphraseKeyboardStyle()
        style.maps = maps
        style.ctrls = ctrls
        return style