import lvgl as lv

from typing import TYPE_CHECKING
from trezor.ui.component.item import Item as LItem

from . import *
from trezor import utils, log, loop
from trezor.ui import i18n, Style, colors, Redo, font, Done
from trezor.ui.screen import Modal, Navigation
from trezor.ui.theme import Styles
from trezor.ui.component import VStack, HStack
from trezor.ui.component import MnemonicKeyboard
from trezor.ui import i18n, Cancel, Confirm
from trezor.ui.screen.confirm import SimpleConfirm
from trezor import workflow
from trezor.crypto import random
from trezor.crypto import bip39

if TYPE_CHECKING:
    from typing import Iterator, Tuple, Literal

    pass

from trezor.ui.types import *


class CheckMnemonic(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.prepare_check)
        self.subtitle.set_text(i18n.Subtitle.select_mnemonics)
        self._options = [
            {
                "label": i18n.Text.str_words.format(12),
                "value": 12,
                "action": self.select_count_12,
            },
            {
                "label": i18n.Text.str_words.format(18),
                "value": 18,
                "action": self.select_count_18,
            },
            {
                "label": i18n.Text.str_words.format(24),
                "value": 24,
                "action": self.select_count_24,
            },
        ]
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)
        self.content.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.content.set_style_pad_left(24, lv.PART.MAIN)

        for a in self._options:
            label, action = a["label"], a["action"]
            app = LItem(self.content, label, None)
            app.add_style(Styles.board, lv.PART.MAIN)
            app.action = action

    def select_count_12(self):
        self.select_count(12)

    def select_count_18(self):
        self.select_count(18)

    def select_count_24(self):
        self.select_count(24)

    def select_count(self, value):
        log.debug(__name__, f"current --: {value}")
        pass


class MnemonicDisplay(Modal):
    def __init__(self, index: int | None):
        super().__init__()
        if index is None:
            index = 12

        self.btn.set_text(i18n.Button.continue_)
        self.btn.mode("done")

        self.title.set_text(i18n.Title.backup_mnemonic)
        self.subtitle.set_text(i18n.Subtitle.backup_mnemonic.format(str(index)))
        # create self.content
        self.create_content(HStack)
        self.content: HStack
        self.content.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
        self.btn.add_event_cb(
            lambda e: self.channel.publish(Done()), lv.EVENT.CLICKED, None
        )
        self.items: List[Item] = []

    def update_mnemonics(self, mnemonics: Sequence[str]):
        log.debug(__name__, f"mnemonics: {mnemonics}")

        mnemonics = list(enumerate(mnemonics))

        if self.items:
            for item, (index, word) in zip(self.items, mnemonics):
                item.word = word
                item.index = index
            return

        for index, word in mnemonics:
            item = Item(self.content, word, index)
            self.items.append(item)

class MnemonicCheck(Modal):
    def __init__(self, words: list[str]):
        super().__init__()

        self.words = words
        self.iter = enumerate(self.words_iter())

        index, checked_words = next(self.iter)
        self.index = index
        self.title.set_text(i18n.Title.word_check.format(str(index + 1)))
        self.subtitle.set_text(i18n.Subtitle.word_check)

        self.create_content(VStack)
        self.content: VStack
        self.content.spacing(8)

        self.word_labels: List[LItem] = []
        for w in checked_words:
            item = LItem(self.content, w, "A:/res/arrow-right_3.png")
            item.set_width(lv.pct(100))
            item.add_style(Styles.pressed, lv.PART.MAIN|lv.STATE.PRESSED)
            item.add_flag(lv.obj.FLAG.CLICKABLE)
            item.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
            item.add_style(Styles.board, lv.PART.MAIN)
            item.add_event_cb(self.on_click_word_label, lv.EVENT.CLICKED, None)
            self.word_labels.append(item)

        self.spacer = self.add(lv.obj)
        self.spacer.set_height(64)  
        self.spacer.set_width(lv.pct(100)) 
        self.spacer.add_style(
            Style()
            .bg_opa(0)  
            .border_opa(0)  
            .shadow_opa(0), 
            lv.PART.MAIN
        )

        self.result = self.add(HStack)
        self.result.set_height(lv.SIZE.CONTENT)
        self.result.items_center()
        self.result_img = self.result.add(lv.img)
        self.result_label = self.result.add(lv.label)
        self.result_label.set_style_text_font(font.medium, lv.PART.MAIN)
        self.result.add_flag(lv.obj.FLAG.HIDDEN)


    def update_checked_words(self):
        self.result.add_flag(lv.obj.FLAG.HIDDEN)
        index, checked_words = next(self.iter)
        self.index = index
        self.title.set_text(i18n.Title.word_check.format(str(index + 1)))
        for label, w in zip(self.word_labels, checked_words):
            label.add_flag(lv.obj.FLAG.CLICKABLE)
            label.set_text(w)

    def words_iter(self) -> Iterator[Tuple[str, str, str]]:
        for word in self.words:
            candidate = [word]
            while len(candidate) < 3:
                rnd = random.uniform(1024)
                w = bip39.get_word(rnd)
                if w not in candidate:
                    candidate.append(w)

            random.shuffle(candidate)
            yield tuple(candidate)

    def on_click_word_label(self, e):
        label = utils.first(self.word_labels, lambda x: x == e.target)

        if label.get_text() == self.words[self.index]:
            self.check_success()
        else:
            self.check_failed()


    def check_failed(self):
        log.debug(__name__, f"the {self.index+1} checked failed")
        for label in self.word_labels:
            label.clear_flag(lv.obj.FLAG.CLICKABLE)

        workflow.spawn(self.delay_failed())

    def check_success(self):
        log.debug(__name__, f"the {self.index+1} checked pass")
        for label in self.word_labels:
            label.clear_flag(lv.obj.FLAG.CLICKABLE)

        workflow.spawn(self.delay_show_next())

    async def delay_show_result(self, success: bool):
        if success:
            self.result_img.set_src("A:/res/hp/icon_done_2.png")
            self.result_label.set_text(i18n.Text.correct)
        else:
            self.result_img.set_src("A:/res/hp/icon_error_2.png")
            self.result_label.set_text(i18n.Text.incorrect)
        self.result.clear_flag(lv.obj.FLAG.HIDDEN)
        await loop.sleep(800)

    async def delay_show_next(self):
        await self.delay_show_result(True)
        if self.index == len(self.words) - 1:
            self.close(True)
        self.update_checked_words()

    async def delay_failed(self):
        await self.delay_show_result(False)
        self.close(False)



class MnemonicInput(Navigation):
    def __init__(self, count):
        super().__init__()
        self.mode('recover')

        self.count = count
        self.index = 0
        self.mnemonics: list[str] = []

        self.set_title(self.title_str.format(self.index + 1))
        self.create_content(VStack)
        self.content: VStack
        self.content.set_height(lv.SIZE.CONTENT)

        # a text area
        self.ta = self.add(lv.textarea)
        self.ta.set_password_mode(False)
        self.ta.set_one_line(True)
        self.ta.set_size(lv.pct(100), 220)
        self.ta.add_style(
            Style()
            .text_font(font.Medium.PF44)
            .text_color(colors.USER.WHITE)
            .radius(16)
            .pad_all(24)
            .bg_color(colors.USER.DARK_GRAY)
            .text_align(lv.TEXT_ALIGN.LEFT),
            lv.PART.MAIN
        )

        self.result = self.add(HStack)
        self.result.items_center()
        self.result.set_height(lv.SIZE.CONTENT)
        img = self.result.add(lv.img)
        img.set_src("A:/res/hp/icon_done_2.png")
        label = self.result.add(lv.label)
        label.set_text(i18n.Text.already_input)
        self.result.add_flag(lv.obj.FLAG.HIDDEN)

        self.kbd = MnemonicKeyboard(self)
        self.kbd.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.kbd.set_size(lv.pct(100), 286)
        self.kbd.align(lv.ALIGN.BOTTOM_MID, 0, 0)
        self.kbd.textarea = self.ta

        self.kbd.add_event_cb(self.on_input_ready, lv.EVENT.READY, None)

    def on_nav_back(self, event):
        if self.index == 0:
            import storage.recovery

            if not storage.recovery.is_dry_run():
                self.close(Cancel())
                return

            workflow.spawn(self.confirmed_abort())
            return
        self.show_prev()

    async def confirmed_abort(self):
            screen = SimpleConfirm(i18n.Subtitle.terminate_mnemonic)
            screen.mode('result')
            screen.set_icon('A:/res/hp/ic_zhongzhihedui.png')
            screen.set_title(i18n.Title.terminate_mnemonic)
            await screen.show()
            if await screen:
                self.close(Cancel())

    def dismiss(self):
        super().dismiss()
        from trezor.ui.screen import manager
        manager.mark_dismissing(self)

    def mode(self, m: Literal['recover', 'check']):
        if m == 'recover':
            self.title_str = i18n.Title.word_input
        else:
            self.title_str = i18n.Title.word_check



    def on_input_ready(self, event):
        log.debug(__name__, "input ready")
        word = self.ta.get_text()
        self.kbd.default_state()
        self.kbd.add_state(lv.STATE.DISABLED)
        log.debug(__name__, f"input the word: {word}")
        self.mnemonics.append(word)
        workflow.spawn(self.delay_show_result())

        if len(self.mnemonics) == self.count:
            # have input done
            workflow.spawn(self.delay_input_done())
            return

        # input next
        workflow.spawn(self.delay_input_next())

    async def delay_input_next(self):
        await loop.sleep(800)
        self.kbd.textarea.set_text("")
        self.index += 1
        self.set_title(self.title_str.format(self.index + 1))
        self.kbd.clear_state(lv.STATE.DISABLED)

    def show_prev(self):
        self.index -= 1
        self.set_title(self.title_str.format(self.index + 1))
        mnemonic = self.mnemonics.pop()
        self.kbd.textarea.set_text(mnemonic)
        self.kbd.content_changed()

    async def delay_input_done(self):
        await loop.sleep(800)
        self.close(Done())

    async def delay_show_result(self):
        self.result.clear_flag(lv.obj.FLAG.HIDDEN)
        await loop.sleep(500)
        self.result.add_flag(lv.obj.FLAG.HIDDEN)


class Item(HStack):
    #    <index>
    #    <word>

    def __init__(self, parent, word: str | None = None, index: int | None = None):
        super().__init__(parent)
        self._word = word
        self._index = index
        self.add_style(Styles.group, lv.PART.MAIN)
    
        self.add_style(
            Style()
            .pad_top(12)
            .pad_bottom(12)
            .border_width(1)
            .radius(16)
            .width(208)
            .height(60)
            .pad_gap(8)
            .flex_track_place(lv.FLEX_ALIGN.CENTER),
            lv.PART.MAIN,
        )

        self.add_style(Styles.checked, lv.PART.MAIN | lv.STATE.CHECKED)
        self.add_style(Styles.focused, lv.PART.MAIN | lv.STATE.USER_1)
        self.index_label = lv.label(self)
        self.index_label.set_text("" if index is None else f"{index + 1}.")

        self.word_label = lv.label(self)
        self.word_label.set_text(word or "")

    @property
    def word(self):
        return self._word

    @word.setter
    def word(self, value):
        self._word = value
        self.word_label.set_text(value)

    @property
    def index(self):
        return self._index

    @index.setter
    def index(self, value):
        self._index = value
        self.index_label.set_text("" if value is None else f"{value + 1}.")

    @property
    def clickable(self):
        return self.has_flag(lv.obj.FLAG.CLICKABLE)

    @clickable.setter
    def clickable(self, value):
        if value:
            self.add_flag(lv.obj.FLAG.CLICKABLE)
            self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        else:
            self.clear_flag(lv.obj.FLAG.CLICKABLE)
            self.clear_flag(lv.obj.FLAG.EVENT_BUBBLE)

    @property
    def checked(self):
        return self.has_state(lv.STATE.CHECKED)

    @checked.setter
    def checked(self, value):
        if value:
            self.add_state(lv.STATE.CHECKED)
        else:
            self.clear_state(lv.STATE.CHECKED)

    @property
    def inputting(self):
        return self.has_state(lv.STATE.USER_1)

    @inputting.setter
    def inputting(self, value):
        if value:
            self.add_state(lv.STATE.USER_1)
        else:
            self.clear_state(lv.STATE.USER_1)

    def toggle(self):
        self.checked = not self.checked


class Input(lv.obj):
    def on_click_blank(self, e: lv.event_t):
        if e.get_target() != self:
            return

        lv.event_send(self, lv.EVENT.CANCEL, None)

    def __init__(self, parent, index):
        super().__init__(parent)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

        self.add_style(
            Style()
            .pad_left(0)
            .pad_right(0)
            .pad_bottom(0)
            .bg_color(colors.USER.GRAY)
            .bg_opa(lv.OPA._20)
            .border_width(0)
            .width(lv.pct(100))
            .height(lv.pct(100)),
            0,
        )
        self.set_pos(0, 0)
        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.add_event_cb(self.on_click_blank, lv.EVENT.CLICKED, None)
        self.set_style_text_color(colors.USER.BLACK, 0)

        self.content = VStack(self)
        self.content.add_style(
            Style()
            .pad_left(0)
            .pad_right(0)
            .pad_bottom(0)
            .pad_top(0)
            .width(lv.pct(100))
            .height(lv.SIZE.CONTENT)
            .text_color(colors.USER.BLACK)  # 设置文本颜色为黑色
            .bg_color(colors.USER.WHITE)
            .bg_opa(),
            lv.PART.MAIN,
        )
        self.content.items_center()
        self.content.align(lv.ALIGN.BOTTOM_MID, 0, 0)

        # <index> and `textarea`
        container = HStack(self.content)
        container.add_style(
            Style()
            .width(lv.pct(100))
            .height(80)
            .pad_left(16)
            .pad_right(16)
            .text_color(colors.USER.BLACK)
            .pad_column(16),
            lv.PART.MAIN,
        )
        container.set_style_flex_cross_place(lv.FLEX_ALIGN.END, lv.PART.MAIN)

        # a label for index
        # self.index = lv.label(container)
        # self.index.add_style(Styles.title_text, lv.PART.MAIN)
        # self.index.set_text(f"#{index + 1}")
        # self.index.set_style_text_color(colors.DS.BLACK, 0)

        self.ta = lv.textarea(container)
        self.ta.set_one_line(True)
        self.ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        # self.ta.set_style_text_color(colors.DS.BLACK , lv.PART.MAIN)
        self.ta.add_style(
            Style()
            .bg_opa(lv.OPA.TRANSP)
            .width(lv.pct(100))  # 使用百分比宽度
            .height(lv.SIZE.CONTENT)  # 高度自适应内容
            .text_font(font.Medium.PF44)
            .border_width(0)
            .text_align_center(),
            # .border_width(3)
            # .border_color(colors.DS.BLACK)
            # .border_side(lv.BORDER_SIDE.BOTTOM)
            lv.PART.MAIN,
        )
        self.ta.set_flex_grow(1)
        self.kbd = MnemonicKeyboard(self.content)
        self.kbd.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.kbd.set_width(lv.pct(100))  # 设置键盘宽度为父容器的 100%
        self.kbd.set_height(286)  # 设置键盘高度为 300 像素
        self.kbd.add_style(
            Style()
            .text_font(font.Medium.PF44)  # 增大键盘字体
            .text_color(colors.USER.BLACK),  # 设置字体颜色为黑色
            lv.PART.MAIN,
        )
        # self.kbd.set_style_text_color(colors.DS.BLACK , lv.PART.MAIN)
        self.kbd.textarea = self.ta

    def reset(self):
        self.ta.set_text("")
        self.kbd.default_state()

    def set_index(self, index):
        self.index.set_text(f"#{index + 1}")
