import lvgl as lv

from .template import TransactionBase

from trezor.ui import i18n, log, font, colors, Style
from trezor.ui.theme import Styles
from trezor.ui import Confirm, Reject
from trezor.ui.component import VStack, LabeledText, DividerLine
from trezor.ui.screen import Modal
from trezor.ui.constants import CONTENT_PAD

class PublicKey(Modal):
    def __init__(self, pubkey: str, path: str, network: str):
        super().__init__()
        self.set_title(i18n.Title.public_key.format(network))
        self.btn.set_text(i18n.Button.confirm)

        self.btn_confirm = self.btn
        self.btn_confirm.add_event_cb(self.on_click_confirm, lv.EVENT.CLICKED, None)

        self.create_content(VStack)
        self.content: VStack

        # `path`
        item = self.add(LabeledText)
        item.set_label(i18n.Text.path)
        item.set_text(path)
        item.add_style(Styles.board, lv.PART.MAIN)

        # `public key`
        item = self.add(LabeledText)
        item.set_label(i18n.Text.public_key)
        item.set_text(pubkey)
        item.add_style(Styles.board, lv.PART.MAIN)

    def on_click_confirm(self, e):
        log.debug(__name__, "user click confirm")
        self.close(Confirm())


# move `xPub` to bitcoin
class XPub(Modal):
    def __init__(self, xpub: str, path: str, network: str):
        super().__init__()
        self.set_title(i18n.Title.xpub.format(network))
        self.btn.set_text(i18n.Button.confirm)

        self.btn_confirm = self.btn
        self.btn_confirm.add_event_cb(self.on_click_confirm, lv.EVENT.CLICKED, None)

        self.create_content(VStack)
        self.content: VStack

        # `path`
        item = self.add(LabeledText)
        item.set_label(i18n.Text.path)
        item.set_text(path)
        item.add_style(Styles.board, lv.PART.MAIN)
        # `public key`
        item = self.add(LabeledText)
        item.set_label(i18n.Text.xpub)
        item.set_text(xpub)
        item.add_style(Styles.board, lv.PART.MAIN)
    def on_click_confirm(self, e):
        log.debug(__name__, "user click confirm")
        self.close(Confirm())

class PaymentRequest(TransactionBase):
    def __init__(self, amount: str, to: str, *, message: str|None = None, icon: str|None = None):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Text.send)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(amount)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        if message:
            item = container.add(lv.label)
            item.set_text(message)
            container.add(DividerLine)

        # to
        self.labeled(container, i18n.Text.receiver, to)


class Output(TransactionBase):
    def __init__(self, amount: str, to: str, icon = None):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Text.send)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(amount)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        # to
        item = self.labeled(self.content, i18n.Text.receiver, to)
        item.add_style(Styles.group, lv.PART.MAIN)

class Total(TransactionBase):
    def __init__(self, amount: str, fee: str, total: str, coin: str, icon: str|None = None):
        super().__init__()

        self.set_icon(icon)
        self.set_title(i18n.Text.send)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(amount)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)

        # fee
        labeled(i18n.Text.fee, fee)
        container.add(DividerLine)
        # total
        labeled(i18n.Text.total, total)

class JointAmount(TransactionBase):
    def __init__(self, amount: str, total: str, coin: str, icon: str|None = None):
        super().__init__()
        self.set_icon(icon)
        self.title.set_text(i18n.Title.x_joint_transaction.format(coin))

        self.content.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(self.content, label, text)

        # your spend
        labeled(i18n.Text.your_spend, amount)
        self.add(DividerLine)

        # total
        labeled(i18n.Text.total, total)


class OutputChange(TransactionBase):
    def __init__(self, amount: str, to: str, amount_changed: str, amount_changed_label: str, icon: str|None = None):
        super().__init__()
        self.set_icon(icon)
        self.set_title(i18n.Text.send)

        obj = self.add(lv.label)
        obj.set_width(lv.pct(100))
        obj.set_text(amount)
        obj.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)
        obj.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)
        container.add_style(Styles.group, lv.PART.MAIN)

        labeled = lambda label, text: self.labeled(container, label, text)

        labeled(amount_changed_label, amount_changed)
        container.add(DividerLine)

        labeled(i18n.Text.receiver, to)

class FeeChange(TransactionBase):
    def __init__(self, fee: str, fee_changed: str, fee_changed_label: str, icon: str|None = None):
        super().__init__()
        self.set_icon(icon)

        self.content.add_style(Styles.group, lv.PART.MAIN)
        
        labeled = lambda label, text: self.labeled(self.content, label, text)

        # changed
        labeled(fee_changed_label, fee_changed)
        self.add(DividerLine)

        # fee
        item = labeled(i18n.Text.fee, fee)
        item.desc.set_style_text_color(colors.USER.DANGER, lv.PART.MAIN)
        item.desc.set_style_text_font(font.bold, lv.PART.MAIN)

class Processing(Modal):
    def __init__(self, icon: str):
        super().__init__()
        container = self.add(VStack)
        container.align(lv.ALIGN.TOP_MID, 0, 0)
        container.items_center()
        container.set_height(lv.SIZE.CONTENT)

        # icon
        self.img = container.add(lv.img)
        self.img.set_src(icon)
        self.img.center()

        self.label = container.add(lv.label)
        self.label.set_text('')
        self.label.set_style_text_color(colors.USER.WHITE, lv.PART.MAIN)
        self.label.set_style_text_font(font.Medium.PF32, lv.PART.MAIN)

        self.bar = self.add(lv.bar)
        self.bar.set_mode(lv.bar.MODE.NORMAL)
        self.bar.set_range(0, 100)
        self.bar.set_value(0, lv.ANIM.OFF)
        self.bar.add_style(
            Style()
            .width(213)
            .height(6)
            .radius(lv.RADIUS.CIRCLE)
            .bg_color(lv.color_hex(0))
            .pad_all(0),
            lv.PART.MAIN,
        )
        self.bar.add_style(
            Style().radius(lv.RADIUS.CIRCLE).bg_color(colors.USER.GREEN),
            lv.PART.INDICATOR,
        )
        self.bar.align(lv.ALIGN.BOTTOM_MID, 0, 0)

        # a animation
        a = lv.anim_t()
        a.init()
        a.set_var(self.img)
        a.set_time(1000)
        a.set_values(0, 3600)
        a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
        a.set_custom_exec_cb(lambda a, v: self.img.set_angle(v))

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, a)
        lv.anim_timeline_start(self.timeline)

    def on_deleting(self):
        super().on_deleting()
        lv.anim_timeline_stop(self.timeline)
        lv.anim_timeline_del(self.timeline)

    def set_label(self, txt):
        if txt != self.label.get_text():
            self.label.set_text(txt)
            
    def set_icon(self, icon: str):
        self.img.set_src(icon)

    def set_progess(self, v: int):
        self.bar.set_value(v, lv.ANIM.OFF)
