import lvgl as lv

from micropython import const
from . import Modal

from trezor import log, loop, iris, io, utils
from trezor.ui import Style, font, colors, i18n
from trezor.ui.theme import Styles
from trezor.ui.component import VStack, HStack, Title, Subtitle, Button
from trezor.ui.component.item import ItemImg
from trezor.ui.constants import *

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Literal, Dict
    IrisOperationState = Literal['none', 'start', 'regist', 'match', 'done']
    pass

CAMERA_WIDTH = 400
CAMERA_HEIGHT = 280

MATCH_TIMEOUT = 0
MATCH_TIMEOUT_MAX_COUNT = const(10)

# 近，远，过远
DISTANCE_RANG = (20, 30, 819)

def state_from_distance(distance: int) -> int:
        close, far, too_far = DISTANCE_RANG
        if distance < close:
            return iris.IRIS_DISTANCE_STATE_CLOSE
        elif close <= distance <= far:
            return iris.IRIS_DISTANCE_STATE_NORMAL
        elif far < distance < too_far:
            return iris.IRIS_DISTANCE_STATE_FAR
        elif too_far == distance:
            return iris.IRIS_DISTANCE_STATE_NONE
        else:
            return iris.IRIS_DISTANCE_STATE_NONE
def state_of_iris_status(status: iris.IrisStatus):
    # 如果模组已经明确返回了没有找到虹膜，则返回没有找到虹膜
    if status.status == iris.IRIS_DISTANCE_STATE_NONE:
        return iris.IRIS_DISTANCE_STATE_NONE

    # 此时不使用模组返回的距离状态了,使用距离判断状态
    return state_from_distance(status.distance)

CAMERA = io.Camera(CAMERA_WIDTH, CAMERA_HEIGHT)

class IrisRegistTip(Modal):
    def __init__(self):
        super().__init__()
        self.btn.set_text(i18n.Button.iris_entry)
        self.set_icon("A:/res/hp/ic_hongmo_1.png")
        self.set_title(i18n.Text.register_iris)
        self.set_subtitle(i18n.Subtitle.iris_desc)

        self.create_content(VStack)
        self.content: VStack

        item = ItemImg(self.content, i18n.Text.iris_desc, "A:/res/hp/ic_infotip.png","left")
        item.spacing(8)
        item.label.set_style_text_color(colors.USER.LIGHT_GRAY, lv.PART.MAIN)
        item.add_style(Styles.group, lv.PART.MAIN)

        self.btn.add_event_cb(lambda _: self.close(True), lv.EVENT.CLICKED, None)

class IrisRegistTipAgain(Modal):
    def __init__(self):
        super().__init__()
        self.set_icon("A:/res/hp/ic_cuowu23.png")
        self.set_title(i18n.Text.note)
        self.btn.set_text(i18n.Button.confirm)

        self.create_content(VStack)
        self.content: VStack

        item = ItemImg(self.content, i18n.Text.iris_note, "A:/res/hp/ic_infotip1.png","left")
        item.spacing(8)
        item.add_style(Styles.group, lv.PART.MAIN)
        item.set_style_border_color(lv.color_hex(0xFED500), lv.PART.MAIN)
        item.label.set_style_text_color(lv.color_hex(0xFED500), lv.PART.MAIN)

        label = self.add(lv.label)
        label.set_style_text_line_space(8, lv.PART.MAIN)
        label.set_text(i18n.Text.iris_entry_desc)
        label.set_width(lv.pct(100))
        label.set_style_text_color(colors.USER.LIGHT_GRAY, lv.PART.MAIN)

        self.btn.add_event_cb(lambda _: self.close(True), lv.EVENT.CLICKED, None)

class IrisScreen(Modal):

    def __init__(self):
        super().__init__()
        self.iris_power_down_chan = loop.chan()
        self.clean()
        self._content = lv.obj(self)
        self._content.add_style(Styles.container, lv.PART.MAIN)
        self._content.add_style(Styles.screen, lv.PART.MAIN)
        self._content.set_style_pad_top(56, lv.PART.MAIN)

        ## header: a close button, a suggestion
        header = self.add(lv.obj)
        header.add_style(Styles.container, lv.PART.MAIN)
        header.set_height(64)

        # a close button
        close = lv.img(header)
        close.set_src("A:/res/hp/ic_closed.png")
        close.add_flag(lv.obj.FLAG.CLICKABLE)
        close.set_ext_click_area(64)
        close.add_event_cb(lambda e: self.close(None), lv.EVENT.CLICKED, None)
        close.align(lv.ALIGN.LEFT_MID, 0, 0)

        # suggestion tip, hidden when startup
        self.suggestion = IrisSuggestion(header)
        self.suggestion.align(lv.ALIGN.CENTER, 12, 0)
        self.suggestion.add_flag(lv.obj.FLAG.HIDDEN)

        ## preview and focust
        # preview
        self.preview = self.add(lv.canvas)
        self.preview.set_buffer(CAMERA.buffer(), CAMERA_WIDTH, CAMERA_HEIGHT, lv.img.CF.TRUE_COLOR)
        if utils.EMULATOR:
            self.preview.fill_bg(colors.USER.DARK_GRAY, lv.OPA.COVER)
        else:
            self.preview.fill_bg(colors.USER.BLACK, lv.OPA.COVER)
        self.preview.set_style_radius(8, lv.PART.MAIN)
        self.preview.set_style_clip_corner(True, lv.PART.MAIN)
        self.preview.align(lv.ALIGN.TOP_MID, 0, 128)

        # a focus icon, center in preview
        self.focus = lv.img(self.preview)
        self.focus.set_src("A:/res/hp/ic_hongmo.png")
        self.focus.center()
        self.focus.add_flag(lv.obj.FLAG.HIDDEN)

        ## states

        # a caputre state
        self.caputre = self.add(IrisCapture)
        self.caputre.add_flag(lv.obj.FLAG.HIDDEN)
        self.caputre.set_height(160)
        self.caputre.align(lv.ALIGN.BOTTOM_MID, 0, 0)

        # a distance state
        self.distance = self.add(IrisDistance)
        self.distance.add_flag(lv.obj.FLAG.HIDDEN)
        self.distance.set_height(160)
        self.distance.align(lv.ALIGN.BOTTOM_MID, 0, 0)

        # a operation state
        self.operation = self.add(IrisOperation)
        self.operation.add_flag(lv.obj.FLAG.HIDDEN)
        self.operation.align(lv.ALIGN.BOTTOM_MID, 0, -128)

        # a chan for update msg
        self.msg_chan = loop.chan()

        self.tasks: list[loop.spawn] = []
        task = loop.spawn(self.states_update())
        self.tasks.append(task)

        # preview task
        task = loop.spawn(self.preview_refresh())
        self.tasks.append(task)

    def on_loaded(self):
        import lvgldrv as lvdrv
        lvdrv.set_directly_copy(True)
        super().on_loaded()
        state = CAMERA.state()
        log.debug(__name__, f"camera state: {state}")
        if state == io.Camera.NONE:
            task = loop.spawn(self.start_camera())
            self.tasks.append(task)
        elif CAMERA.state() == io.Camera.SUSPENDED:
            log.debug(__name__, "show camera")
            CAMERA.resume()

    def on_unload_start(self):
        import lvgldrv as lvdrv
        lvdrv.set_directly_copy(False)
        super().on_unload_start()
        CAMERA.suspend()

    def on_deleting(self):
        super().on_deleting()
        self.operation.destory()
        CAMERA.stop()
        CAMERA.deinit()

    def close(self, value: 'R'):
        async def close_iris(done: bool):
            if not done:
                # consume the last `registe` or `match` note, sync secure channel state
                await iris.wait_response_with_timeout()
                await iris.cancel()
                # iris will send `registe`/`match` operation result(failed), consume it
                await iris.wait_response_with_timeout()
            await iris.close()
            self.iris_power_down_chan.publish(True)

        for task in self.tasks:
            try:
                task.close()
            except:
                pass

        loop.spawn(close_iris(bool(value)))
        super().close(value)

    async def wait_iris_power_down_and_result(self):
        await self.iris_power_down_chan.take()
        log.debug(__name__, "iris have power down")
        return await self.channel.take()

    def __await__(self) -> Generator['R']:
        return (yield from self.wait_iris_power_down_and_result())

    __iter__ = __await__

    def state_reset(self):
        self.suggestion.add_flag(lv.obj.FLAG.HIDDEN)
        self.caputre.add_flag(lv.obj.FLAG.HIDDEN)
        self.distance.add_flag(lv.obj.FLAG.HIDDEN)
        self.operation.add_flag(lv.obj.FLAG.HIDDEN)

    async def start_camera(self):
        if utils.EMULATOR:
            self.focus.clear_flag(lv.obj.FLAG.HIDDEN)
            return
        await loop.sleep(100)
        CAMERA.init()
        CAMERA.switch("front")
        CAMERA.start()
        self.focus.clear_flag(lv.obj.FLAG.HIDDEN)

    async def preview_refresh(self):
        while True:
            await loop.wait(io.CAMERA_STATE)
            self.preview.invalidate()
            CAMERA.resume()

    def update_detail(self, note: iris.Note):
        # percent when regist
        pass

    async def states_update(self):
        while True:
            note: iris.Note = await self.msg_chan.take()
            self.update_detail(note)

            self.suggestion.update(note)
            self.caputre.update(note)
            self.distance.update(note)
            self.operation.update(note)

class IrisSuggestion(HStack):
    """
    icon and successsion

    mode: close, normal, far

    border is `red` when close or far
    """
    def __init__(self, parent):
        super().__init__(parent)
        self.spacing(8)
        self.items_center()
        self.add_style(Styles.container, lv.PART.MAIN)
        self.add_style(Styles.group, lv.PART.MAIN)
        self.set_width(lv.SIZE.CONTENT)
        self.set_style_pad_all(8, lv.PART.MAIN)
        self.set_style_bg_color(colors.STD.BLACK, lv.PART.MAIN)
        self.set_style_bg_opa(lv.OPA._50, lv.PART.MAIN)

        self.icon = self.add(lv.img)

        self.desc = self.add(lv.label)
        self.desc.set_text('')

        self.msg_map: Dict[int, str] = {}

    def set_distance_msg(self, msg_map: Dict[int, str]):
        self.msg_map = msg_map

    def update(self, note: iris.Note):
        if not isinstance(note, iris.IrisStatus):
            return

        state = state_of_iris_status(note)
        if state == iris.IRIS_DISTANCE_STATE_NONE:
            # 没有捕捉到虹膜，不显示 
            self.add_flag(lv.obj.FLAG.HIDDEN)
            return

        # 捕捉到了虹膜
        self.clear_flag(lv.obj.FLAG.HIDDEN)

        # 根据不同的状态改变boarder颜色、icon
        self.set_distance_state(state)

        # 更新 提示信息
        desc = self.msg_map.get(state)
        self.desc.set_text(desc)

    def set_distance_state(self, state: int):
        if state == iris.IRIS_DISTANCE_STATE_NORMAL:
            color = colors.USER.GREEN
            img = 'A:/res/hp/icon_done_4.png'
        else:
            color = colors.USER.RED
            img = 'A:/res/hp/ic_infotip2.png'

        self.set_style_border_color(color, lv.PART.MAIN)
        self.icon.set_src(img)

class IrisCapture(VStack):
    def __init__(self, parent):
        super().__init__(parent)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)

        self.title = self.add(lv.label)
        self.title.set_style_text_font(font.Medium.PF32, lv.PART.MAIN)
        self.title.set_text("")

        tip = self.add(lv.label)
        tip.add_style(Styles.subtitle, lv.PART.MAIN)
        tip.set_text(i18n.Text.front_of_screen)

    def set_title(self, text: str):
        self.title.set_text(text)

    def update(self, note: iris.Note):
        if not isinstance(note, iris.IrisStatus):
            return
        state = state_of_iris_status(note)
        if state == iris.IRIS_DISTANCE_STATE_NONE:
            # 当没有捕捉到虹膜时, 显示
            self.clear_flag(lv.obj.FLAG.HIDDEN)
            return

        self.add_flag(lv.obj.FLAG.HIDDEN)

class IrisDistance(VStack):
    def __init__(self, parent):
        super().__init__(parent)
        self.set_style_flex_track_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.set_style_flex_cross_place(lv.FLEX_ALIGN.CENTER, lv.PART.MAIN)
        self.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN)

        self.spans = self.add(lv.spangroup)
        span = self.spans.new_span()
        span.set_text(i18n.Text.iris_current_distance)

        self.distance = self.spans.new_span()
        self.distance.set_text('')
        self.distance.style.set_text_font(font.Medium.PF32)
        self.distance.style.set_text_color(colors.USER.RED)

        dummy = self.add(lv.obj)
        dummy.add_style(Styles.container, lv.PART.MAIN)
        dummy.set_height(1)

        self.img = self.add(lv.img)

    def update(self, note: iris.Note):
        if not isinstance(note, iris.IrisStatus):
            return

        state = state_of_iris_status(note)
        if state == iris.IRIS_DISTANCE_STATE_NONE:
            # 没有捕捉到虹膜，不显示 
            self.add_flag(lv.obj.FLAG.HIDDEN)
            return

        self.clear_flag(lv.obj.FLAG.HIDDEN)
        if state == iris.IRIS_DISTANCE_STATE_NORMAL:
            # 距离合适
            self.img.set_src("A:/res/hp/iris0.png")
        elif state == iris.IRIS_DISTANCE_STATE_CLOSE:
            # 距离太近
            self.img.set_src("A:/res/hp/iris1.png")
        elif state == iris.IRIS_DISTANCE_STATE_FAR:
            # 距离太远
            self.img.set_src("A:/res/hp/iris2.png")

        if state == iris.IRIS_DISTANCE_STATE_NORMAL:
            # 正常距离时，不显示距离
            self.spans.add_flag(lv.obj.FLAG.HIDDEN)
            return

        self.spans.clear_flag(lv.obj.FLAG.HIDDEN)
        self.distance.set_text(f"{note.distance}cm")

class IrisOperation(VStack):
    def __init__(self, parent):
        super().__init__(parent)
        self.items_center()
        self.add_style(Styles.container, lv.PART.MAIN)
        self.add_style(Styles.board, lv.PART.MAIN)
        self.set_width(lv.SIZE.CONTENT)
        self.set_style_bg_opa(lv.OPA._50, lv.PART.MAIN)
        self.set_style_bg_color(colors.STD.BLACK, lv.PART.MAIN)

        self.icon = self.add(lv.img)

        self.desc = self.add(lv.label)
        self.desc.set_style_text_font(font.Regular.PF24, lv.PART.MAIN)
        self.desc.set_style_text_color(colors.USER.GREEN, lv.PART.MAIN)

        self.mode: IrisOperationState = 'none'

        # different when regist and match
        self.done = ''

        a = lv.anim_t()
        a.init()
        a.set_var(self.icon)
        a.set_time(1000)
        a.set_values(0, 3600)
        a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
        a.set_custom_exec_cb(lambda a, v: self.icon.set_angle(v))

        self.timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(self.timeline, 0, a)
        lv.anim_timeline_start(self.timeline)

    def update(self, note: iris.Note):
        if isinstance(note, iris.ModuleStatus):
            if note.status == iris.MODULE_STATUS_IRIS_STARTING:
                self.mode = 'start'
            elif note.status == iris.MODULE_STATUS_ENROLL:
                self.mode = 'regist'
            elif note.status == iris.MODULE_STATUS_MATCH:
                self.mode = 'match'
            elif note.status == iris.MODULE_STATUS_READY:
                self.mode = 'none'
            elif note.status == iris.MODULE_STATUS_DONE:
                self.mode = 'done'

        if self.mode in ('regist', 'none'):
            # 注册时，没有识别到虹膜时，不显示
            self.add_flag(lv.obj.FLAG.HIDDEN)
            return

        if isinstance(note, iris.IrisStatus):
            state = state_of_iris_status(note)
            # 虹膜识别过程中，只有正常识别的时候显示, 其他时候隐藏
            if state != iris.IRIS_DISTANCE_STATE_NORMAL:
                self.add_flag(lv.obj.FLAG.HIDDEN)
                return

        self.clear_flag(lv.obj.FLAG.HIDDEN)
        if self.mode == 'start':
            img = 'A:/res/hp/loading-1.png'
            desc = i18n.Text.iris_operation_starting
        elif self.mode == 'match':
            img = 'A:/res/hp/loading-1.png'
            desc = i18n.Text.iris_operation_matching
        elif self.mode == 'regist':
            img = 'A:/res/hp/loading-1.png'
            desc = i18n.Text.iris_operation_registing
        elif self.mode == 'done':
            img = 'A:/res/hp/icon_done_3.png'
            desc = self.done

        self.icon.set_src(img)
        self.desc.set_text(desc)

        if self.mode == 'done':
            lv.anim_timeline_stop(self.timeline)
            self.icon.set_angle(0)

    def destory(self):
        lv.anim_timeline_stop(self.timeline)
        lv.anim_timeline_del(self.timeline)

class IrisBar(HStack):
    def __init__(self, parent):
        super().__init__(parent)
        self.add_style(Styles.container, lv.PART.MAIN)
        self.items_center()

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

        self.percent = self.add(lv.label)
        self.percent.add_style(
            Style()
            .text_font(font.small)
            .text_color(colors.USER.GREEN),
            lv.PART.MAIN
        )
        self.percent.set_text("0%")

    def set_precent(self, v: int):
        self.bar.set_value(v, lv.ANIM.OFF)
        self.percent.set_text(f"{v}%")

    def update(self, note: iris.Note):
        if isinstance(note, iris.IrisStatus) :
            state = state_of_iris_status(note)
            # 只有正常距离时显示
            if state == iris.IRIS_DISTANCE_STATE_NORMAL:
                self.clear_flag(lv.obj.FLAG.HIDDEN)
            else:
                self.add_flag(lv.obj.FLAG.HIDDEN)
        if not isinstance(note, iris.RegistProgress):
            return

        self.clear_flag(lv.obj.FLAG.HIDDEN)
        pct = (note.left + note.right) // 2
        if pct == 100:
            self.add_flag(lv.obj.FLAG.HIDDEN)
        self.set_precent(pct)

class IrisRegist(IrisScreen):

    def __init__(self):
        super().__init__()
        msg_map = {
            iris.IRIS_DISTANCE_STATE_CLOSE: i18n.Text.iris_regist_suggestion_distance_close,
            iris.IRIS_DISTANCE_STATE_NORMAL: i18n.Text.iris_suggestion_registing,
            iris.IRIS_DISTANCE_STATE_FAR: i18n.Text.iris_regist_suggestion_distance_far,
        }
        self.suggestion.set_distance_msg(msg_map)
        self.caputre.set_title(i18n.Text.iris_regist)
        self.operation.done = i18n.Text.iris_operation_regist_done

        # a bar
        self.bar = self.add(IrisBar)
        self.bar.add_flag(lv.obj.FLAG.HIDDEN)
        self.bar.align(lv.ALIGN.BOTTOM_MID, 0, -128)

        self.do_operation()

    def do_operation(self):
        self.state_reset()
        if utils.EMULATOR:
            task = loop.spawn(emulator_mock_regist(self))
        else:
            task = loop.spawn(self.start_registing())
        self.tasks.append(task)

    def update_detail(self, status: iris.Note):
        self.bar.update(status)

    async def iris_error(self, reply: iris.Reply):
        # show timeout screen
        from trezor.ui.screen.message import Error
        msg = i18n.Text.register_iris_timed_out if reply.is_timeout() else reply.error_msg()
        screen = Error(i18n.Text.register_iris_error, msg)
        screen.btn.mode('retry')
        screen.btn.set_text(i18n.Button.try_again)
        await screen.show()
        await screen
        self.do_operation()

    async def start_registing(self):
        status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_IRIS_STARTING,])
        self.msg_chan.publish(status)
        resp = await iris.open()
        # 从上电开始到启动注册完成，都算启动
        # self.msg_chan.publish(resp)
        await loop.sleep(10)
        resp = await iris.regist()
        self.msg_chan.publish(resp)

        while True:
            status = await iris.wait_response()
            log.debug(__name__, f"{status}")
            if isinstance(status, iris.IrisStatus) or isinstance(status, iris.RegistProgress):
                # update ui
                self.msg_chan.publish(status)
            elif isinstance(status, iris.Reply):
                if status.is_success() or status.code == 4:
                    self.msg_chan.publish(status)
                    # success or already regist
                    self.msg_chan.publish(iris.RegistProgress(iris.MSG_ID_NOTE_REGIST_PROGRESS, [100, 100]))
                    self.msg_chan.publish(iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_DONE,]))
                    log.debug(__name__, "regist success")
                    await loop.sleep(500)
                    self.close(status.data)
                    break
                else:
                    loop.spawn(self.iris_error(status))
                    break

class IrisMatch(IrisScreen):

    def __init__(self):
        super().__init__()
        global MATCH_TIMEOUT
        MATCH_TIMEOUT = 0

        msg_map = {
            iris.IRIS_DISTANCE_STATE_CLOSE: i18n.Text.iris_match_suggestion_distance_close,
            iris.IRIS_DISTANCE_STATE_NORMAL: i18n.Text.iris_suggestion_matching,
            iris.IRIS_DISTANCE_STATE_FAR: i18n.Text.iris_match_suggestion_distance_far,
        }
        self.suggestion.set_distance_msg(msg_map)
        self.operation.done = i18n.Text.iris_operation_match_done
        self.caputre.set_title(i18n.Text.iris_match)
        self.do_operation()

    def do_operation(self):
        self.state_reset()

        if utils.EMULATOR:
            task = loop.spawn(emulator_mock_match(self))
        else:
            task = loop.spawn(self.start_matching())
        self.tasks.append(task)

    async def iris_error(self, reply: iris.Reply):
        # show timeout screen
        from trezor.ui.screen.confirm import SimpleConfirm

        global MATCH_TIMEOUT
        if reply.is_timeout():
            MATCH_TIMEOUT += 1

        if MATCH_TIMEOUT == MATCH_TIMEOUT_MAX_COUNT:
            # too may timeout
            loop.spawn(wipe_when_too_many_timeout())
            self.close(None)
            return
        msg = i18n.Text.verify_iris_error_desc if reply.is_timeout() else reply.error_msg()
        screen = SimpleConfirm(msg)
        screen.set_icon("A:/res/hp/ic_cuowu555.png")
        screen.set_title(i18n.Text.verify_iris_error)
        screen.mode('result')
        screen.btn_right.set_text(i18n.Button.try_again)
        await screen.show()
        if not await screen:
            self.close(None)

        self.do_operation()

    async def start_matching(self):
        status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, bytes([iris.MODULE_STATUS_IRIS_STARTING]))
        self.msg_chan.publish(status)
        resp = await iris.open()
        # 从上电开始到启动识别完成，都算启动
        # self.msg_chan.publish(resp)
        await loop.sleep(10)
        resp = await iris.match()
        self.msg_chan.publish(resp)

        while True:
            status = await iris.wait_response()
            log.debug(__name__, f"{status}")
            if isinstance(status, iris.IrisStatus):
                # update ui
                self.msg_chan.publish(status)
            elif isinstance(status, iris.Reply):
                if status.is_success():
                    global MATCH_TIMEOUT
                    MATCH_TIMEOUT = 0
                    log.debug(__name__, "match success")
                    self.msg_chan.publish(iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_DONE,]))
                    await loop.sleep(500)
                    self.close(status.data)
                    break
                else:
                    loop.spawn(self.iris_error(status))
                    break

async def wipe_when_too_many_timeout():
    from trezor.ui.screen.confirm import SimpleConfirm
    from trezor.wire import DummyContext
    from trezor.messages import WipeDevice
    from apps.management.wipe_device import wipe_device

    screen = SimpleConfirm(i18n.Text.too_many_iris_match_count)
    screen.text.set_style_text_font(font.default, lv.PART.MAIN)
    screen.mode('result')
    await screen.show()
    if not await screen:
        return

    ctx = DummyContext()
    ctx.force_wipe = True
    await wipe_device(ctx, WipeDevice())


if utils.EMULATOR:
    async def emulator_mock_regist(obj: IrisRegist):
        await loop.sleep(1000)

        # mock starting
        status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_IRIS_STARTING,])
        obj.msg_chan.publish(status)
        await loop.sleep(1000)

        # mock start done
        # status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_READY,])
        # obj.msg_chan.publish(status)
        await loop.sleep(10)

        # mock matching
        status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_ENROLL,])
        obj.msg_chan.publish(status)

        # mock status
        for d in range(-30, 80, 8):
            if d < 0:
                status = 3 # 未找到虹膜
            elif d < 20:
                status = 1 # 距离太近
            elif d < 30:
                status = 0 # 距离正常
            else:
                status = 2 # 距离太远
            if d < 0:
                d = 0

            d *= 10 # mm
            payload = bytearray([status])
            payload.extend(d.to_bytes(2, 'little'))
            obj.msg_chan.publish(iris.IrisStatus(iris.MSG_ID_NOTE_IRIS_STATUS, payload))
            await loop.sleep(800)

        # mock progress
        for p in range(0, 100, 16):
            payload = bytes([0, 4, 1])
            obj.msg_chan.publish(iris.IrisStatus(iris.MSG_ID_NOTE_IRIS_STATUS, payload))
            await loop.sleep(100)
            payload = bytes([p, p])
            obj.msg_chan.publish(iris.RegistProgress(iris.MSG_ID_NOTE_REGIST_PROGRESS, payload))
            await loop.sleep(600)

        # done
        payload = bytes([100, 100])
        obj.msg_chan.publish(iris.RegistProgress(iris.MSG_ID_NOTE_REGIST_PROGRESS, payload))
        log.debug(__name__, "regist success")
        obj.msg_chan.publish(iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_DONE,]))
        await loop.sleep(800)
        obj.close(b'e'*32)

    async def emulator_mock_match(obj: IrisMatch):
        await loop.sleep(1000)
        # mock starting
        status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_IRIS_STARTING,])
        obj.msg_chan.publish(status)
        await loop.sleep(1000)

        # mock start done
        # status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_READY,])
        # obj.msg_chan.publish(status)
        await loop.sleep(10)

        # mock matching
        status = iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_MATCH,])
        obj.msg_chan.publish(status)

        # mock status
        for d in range(-30, 80, 8):
            if d < 0:
                status = 3 # 未找到虹膜
            elif d < 15:
                status = 1 # 距离太近
            elif d < 45:
                status = 0 # 距离正常
            else:
                status = 2 # 距离太远
            if d < 0:
                d = 0

            d *= 10 # mm
            payload = bytearray([status])
            payload.extend(d.to_bytes(2, 'little'))
            obj.msg_chan.publish(iris.IrisStatus(iris.MSG_ID_NOTE_IRIS_STATUS, payload))
            await loop.sleep(800)

        # mock progress
        for p in range(0, 100, 16):
            payload = bytes([0, 4, 1])
            obj.msg_chan.publish(iris.IrisStatus(iris.MSG_ID_NOTE_IRIS_STATUS, payload))
            await loop.sleep(100)
            payload = bytes([p, p])
            obj.msg_chan.publish(iris.RegistProgress(iris.MSG_ID_NOTE_REGIST_PROGRESS, payload))
            await loop.sleep(600)

        # done
        payload = bytes([100, 100])
        obj.msg_chan.publish(iris.RegistProgress(iris.MSG_ID_NOTE_REGIST_PROGRESS, payload))

        log.debug(__name__, "match success")
        obj.msg_chan.publish(iris.ModuleStatus(iris.MSG_ID_NOTE_MODULE_STATUS, [iris.MODULE_STATUS_DONE,]))
        await loop.sleep(800)
        obj.close(b'e'*32)
