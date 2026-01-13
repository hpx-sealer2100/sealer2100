import lvgl as lv

from trezor import log
from trezor.ui import Style, colors, events

class ArcHolder(lv.obj):
    def __init__(self, parent):
        super().__init__(parent)
        self.add_style(
            Style()
            .pad_all(0)
            .width(256)
            .height(256)
            .radius(lv.RADIUS.CIRCLE)
            .bg_opa(lv.OPA._40)
            .border_width(0),
            0
        )
        self.set_style_bg_color(colors.USER.GRAY, lv.PART.MAIN|lv.STATE.DISABLED)
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)

        # a label for showing text
        self.text = lv.label(self)
        self.text.set_text("")
        self.text.set_long_mode(lv.label.LONG.WRAP)
        self.text.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
        self.text.center()

        # a arc
        self.arc = lv.arc(self)
        self.arc.add_style(
            Style()
            .width(lv.pct(100))
            .height(lv.pct(100))
            .bg_opa(lv.OPA.TRANSP),
            0
        )
        self.arc.remove_style(None, lv.PART.KNOB)
        self.arc.set_bg_angles(0, 360)
        self.arc.set_angles(0, 0)
        self.arc.set_rotation(270)
        self.arc.center()

        # animation for fill `Holder`, aka make circle bigger
        self.anim = lv.anim_t()
        self.anim.init()
        self.anim.set_var(self.arc)
        self.anim.set_custom_exec_cb(lambda _, v: self.arc.set_end_angle(v))

        # animation time line for anim
        self.timeline = None

        self.arc.add_event_cb(self.__on_pressed, lv.EVENT.PRESSED, None)
        self.arc.add_event_cb(self.__on_released, lv.EVENT.RELEASED, None)

        # hold time for animation
        self.hold_time = 1500
        # release time for animation
        self.release_time = 100

        self.done = False

    def __on_pressed(self, event):
        if self.done:
            return
        self.__hold_anim(self.hold_time, lambda _: self.__on_ready())

    def __on_released(self, event):
        if self.done:
            return
        self.__hold_anim(self.release_time, released=True)

    def __on_ready(self):
        lv.event_send(self, events.HOLDER_DONE, None)
        self.done = True
        self.arc.clear_flag(lv.obj.FLAG.CLICKABLE)

    def __hold_anim(self, duration, on_ready = None, released = False):
        if self.timeline:
            lv.anim_timeline_stop(self.timeline)
            lv.anim_timeline_del(self.timeline)

        if released:
            start = 0
            end = self.arc.get_angle_end()
        else:
            start = self.arc.get_angle_end()
            end = 360

        self.anim.set_values(start, end)
        self.anim.set_ready_cb(on_ready)
        self.anim.set_time(duration)

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, self.anim)

        lv.anim_timeline_set_reverse(self.timeline, released)
        lv.anim_timeline_start(self.timeline)
    
    def reset(self):
        self.done = False
        self.arc.add_flag(lv.obj.FLAG.CLICKABLE)
        self.arc.set_end_angle(0)

    @property
    def disabled(self):
        return self.has_state(lv.STATE.DISABLED)

    @disabled.setter
    def disabled(self, value):
        if value:
            self.add_state(lv.STATE.DISABLED)
            self.arc.add_state(lv.STATE.DISABLED)
        else:
            self.clear_state(lv.STATE.DISABLED)
            self.arc.clear_state(lv.STATE.DISABLED)

    def set_text(self, text):
        self.text.set_text(text)


class FillHolder(lv.obj):
    """
    A circle object, user pressed on it, it will fill with color
    """

    def __init__(self, parent, text: str | None = None):
        super().__init__(parent)
        self.add_style(
            Style()
            .pad_all(0)
            .width(200)
            .height(200)
            .radius(lv.RADIUS.CIRCLE)
            .bg_opa(lv.OPA._40)
            .border_width(2),
            0,
        )
        self.set_style_bg_color(colors.USER.GRAY, lv.PART.MAIN|lv.STATE.DISABLED)
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)

        # a circle fo animation
        self.circle = lv.obj(self)
        self.circle.add_style(
            Style()
            .width(0)
            .height(0)
            .radius(lv.RADIUS.CIRCLE)
            .bg_opa(lv.OPA._80)
            .bg_color(colors.USER.DARK_GREEN),
            0,
        )
        self.circle.center()

        # animation for fill `Holder`, aka make circle bigger
        self.anim = lv.anim_t()
        self.anim.init()
        self.anim.set_var(self.circle)
        self.anim.set_custom_exec_cb(lambda _, v: self.circle.set_size(v, v))

        # animation time line for anim
        self.timeline = None

        # a label for showing text
        if text:
            self.text = lv.label(self)
            self.text.set_text(text)
            self.text.center()

        # a object on the top of Holder for click event
        self.click = lv.obj(self)
        self.click.add_style(
            Style()
            .width(lv.pct(100))
            .height(lv.pct(100))
            .radius(lv.RADIUS.CIRCLE)
            .bg_opa(lv.OPA.TRANSP)
            .border_width(0),
            0
        )
        self.click.center()

        self.click.add_event_cb(self.__on_pressed, lv.EVENT.PRESSED, None)
        self.click.add_event_cb(self.__on_released, lv.EVENT.RELEASED, None)

        # hold time for animation
        self.hold_time = 1500
        # release time for animation
        self.release_time = 100

        self.done = False

    def __on_pressed(self, event):
        if self.done:
            return
        self.__hold_anim(self.hold_time, lambda _: self.__on_ready())

    def __on_released(self, event):
        if self.done:
            return
        self.__hold_anim(self.release_time, released=True)

    def __on_ready(self):
        lv.event_send(self, events.HOLDER_DONE, None)
        self.done = True

    def __hold_anim(self, duration, on_ready = None, released = False):
        if self.timeline:
            lv.anim_timeline_stop(self.timeline)
            lv.anim_timeline_del(self.timeline)

        if released:
            start = 0
            end = self.circle.get_width()
        else:
            start = self.circle.get_width()
            end = self.get_width()

        self.anim.set_values(start, end)
        self.anim.set_ready_cb(on_ready)
        self.anim.set_time(duration)

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, self.anim)

        lv.anim_timeline_set_reverse(self.timeline, released)
        lv.anim_timeline_start(self.timeline)

    @property
    def disabled(self):
        return self.has_state(lv.STATE.DISABLED)

    @disabled.setter
    def disabled(self, value):
        if value:
            self.add_state(lv.STATE.DISABLED)
            self.click.add_state(lv.STATE.DISABLED)
        else:
            self.clear_state(lv.STATE.DISABLED)
            self.click.clear_state(lv.STATE.DISABLED)

    def set_text(self, text):
        if not self.text:
            return
        self.text.set_text(text)
