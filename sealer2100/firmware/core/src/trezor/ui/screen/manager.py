import gc
import lvgl as lv

from trezor import log, loop, utils
from storage import device
from typing import List, TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Union, Literal, Type, TypeVar
    # break recursion importing
    from trezor.ui.screen import Screen, Navigation, Modal
    S = TypeVar('S', bound=Screen)
    ScreenType = Union[Navigation, Modal]
    SceneType = Literal['load', 'unload']
    ScreenState = Literal['loaded', 'dismissing']

__screens: List['Screen'] = []
__signal__: loop.chan = loop.chan()

class Lock:
    def __init__(self):
        self.locked = False

    async def acquire(self):
        # wait until lock is free.
        # log.debug(__name__, "acquiring lock ...")
        while self.locked:
            await loop.sleep(13)

        # lock the locker
        self.locked = True
        # log.debug(__name__, "locked")
    async def release(self):
        # unlock the locker
        # log.debug(__name__, "release lock")
        self.locked = False

    async def __aenter__(self):
        # log.debug(__name__, "aenter locker")
        await self.acquire()
        __signal__.publish(None)

    async def __aexit__(self, exc_type, exc, tb):
        # log.debug(__name__, "aexit locker")
        await self.release()

__locker__ = Lock()

def __src_anim_type(screen: ScreenType, scene: SceneType):
    # break recursion importing
    from trezor.ui.screen import Navigation, Modal
    if isinstance(screen, Navigation):
        if scene == 'load':
            log.debug(__name__, f"load Navigation from left to right")
            return lv.SCR_LOAD_ANIM.MOVE_LEFT
        elif scene == 'unload':
            log.debug(__name__, f"unload Navigation from right to left")
            return lv.SCR_LOAD_ANIM.MOVE_RIGHT
    elif isinstance(screen, Modal):
        if scene == 'load':
            log.debug(__name__, f"load Model from bottom to top")
            return lv.SCR_LOAD_ANIM.OVER_TOP
        elif scene == 'unload':
            log.debug(__name__, f"unload Model from top to bottom")
            return lv.SCR_LOAD_ANIM.OUT_BOTTOM
    else:
        log.debug(__name__, f"unknown screen type: {screen.__class__.__name__}")
        return lv.SCR_LOAD_ANIM.NONE

async def switch_scene(screen: 'Screen'):
    """
    Switch to new scene, push `screen` on stack, release previous screens

    e.g. switch from `Lock` to `Main` screen
    """
    async with __locker__:
        global __screens

        if __debug__:
            old_scene = __screens[0].__class__.__name__ if __screens else "None"
            new_scene = screen.__class__.__name__
            log.debug(__name__, f"switch scene: {old_scene} -> {new_scene}")

        for s in __screens:
            s.state = "dismissing"

        # add state to screen
        screen.state = 'loaded'

        __screens.append(screen)

        animation = device.is_animation_enabled()
        if animation:
            # animation load screen
            lv.scr_load_anim(screen, lv.SCR_LOAD_ANIM.FADE_IN, 100, 0, False)
        else:
            screen.set_pos(0, 0)
            lv.scr_load(screen)

        await screen.wait_loaded()

def try_switch_to(cls: Type[S]) -> S:
    """There maybe a `S` type instance on manager stack, we switch to it
    """
    log.debug(__name__, f"try switch to: {cls.__name__}")
    it = filter(lambda s: s.state == "loaded", __screens)
    dst = None
    for s in it:
        # have found destination screen, mark all screen after it dismissing
        if dst:
            mark_dismissing(s)
        # cache found instance
        # if isinstance(s, cls):
        if s.__class__.__name__ == cls.__name__:
            dst = s

    return dst

async def push(screen: 'Screen'):
    """
    Push a new screen on stack and show it
    """
    async with __locker__:
        global __screens
        if __debug__:
            old_scene = __screens[-1].__class__.__name__ if __screens else "None"
            new_scene = screen.__class__.__name__
            log.debug(__name__, f"push scene: {old_scene} -> {new_scene}")

        screen.state = 'loaded'
        __screens.append(screen)

        animation = device.is_animation_enabled()
        if animation:
            # animation load screen
            lv.scr_load_anim(screen, __src_anim_type(screen, 'load'), 200, 0, False)
            # utime.sleep_ms(300)
        else:
            screen.set_pos(0, 0)
            lv.scr_load(screen)

        await screen.wait_loaded()

async def pop(screen: Screen):
    """
    Pop the current screen, show the previous
    """
    # `Modal` instance just need to be marked as `dismissing`
    # don't move this import top of file, it will cause circular import
    async with __locker__:
        from trezor.ui.screen import Modal
        if isinstance(screen, Modal):
            screen.state = 'dismissing'
            log.debug(__name__, f"dismissing modal: {screen.__class__.__name__}")
            log.debug(__name__, "only mark it dismissing, delete it later")
            return

        if len(__screens) < 2:
            log.debug(__name__, "not enough screens on stack, can't pop")
            log.debug(__name__, "does your logic fit this sense?")
            return

        await _do_pop_last(screen)

async def _do_pop_last(screen):
    new_scr = first_alive_screen(__screens[:-1])
    if __debug__:
        log.debug(__name__, f"popping screen: {screen.__class__.__name__} -> {new_scr.__class__.__name__}")

    animation = device.is_animation_enabled()
    if animation:
        lv.scr_load_anim(new_scr, __src_anim_type(screen, 'unload'), 200, 0, False)
    else:
        new_scr.set_pos(0, 0)
        lv.scr_load(new_scr)

    log.debug(__name__, f"waiting {new_scr.__class__.__name__} loaded")
    await new_scr.wait_loaded()
    log.debug(__name__, f"waited  {new_scr.__class__.__name__} loaded")
    screen.delete()
    __screens.remove(screen)
    del screen
    gc.collect()


async def replace(screen: 'Screen'):
    """
    Replace the current screen, show the new
    """
    async with __locker__:
        global __screens
        cur_scr = __screens.pop()
        if __debug__:
            log.debug(__name__, f"replacing screen: {cur_scr.__class__.__name__} -> {screen.__class__.__name__}")
        screen.state = 'loaded'
        __screens.append(screen)

        animation = device.is_animation_enabled()
        if animation:
            lv.scr_load_anim(screen, lv.SCR_LOAD_ANIM.FADE_OUT, 100, 0, False)
        else:
            screen.set_pos(0, 0)
            lv.scr_load(screen)

        await screen.wait_loaded()
        cur_scr.delete()
        del cur_scr
        gc.collect()

def mark_dismissing(screen: Screen):
    """
    Mark a screen as `dismissing`, it will be deleted later
    """
    screen.state = 'dismissing'
    log.debug(__name__, f"mark dismissing: {screen.__class__.__name__}")

def publish(event):
    """
    Publish an event to all screens
    """
    for screen in __screens:
        lv.event_send(screen, event, None)

def first_alive_screen(screens: List['Screen']|None = None) -> Union[Screen, None]:
    # reverse find first loaded screen
    screens = screens or __screens
    return utils.first(screens[::-1], lambda s: s.state == 'loaded')

async def monitor_task():
    # reset the locker state
    # maybe locked when main loop restart
    __locker__.locked = False

    async def delay():
        await loop.sleep(100)

    async def event():
        await __signal__.take()

    while True:
        delay_task = delay()
        event_task = event()

        racer = loop.race(delay_task, event_task)
        await racer

        # `manager` doing operation, reset timer
        if event_task in racer.finished:
            continue
        # find all dismissing screen
        dismissing = [s for s in __screens if s.state == 'dismissing']

        # no dismissing screen
        if not dismissing:
            continue
        log.debug(__name__, f"found dismissing screens, count: {len(dismissing)}")

        async with __locker__:
            # if the screen is last of the __screens, we should do a `complete` destroy
            if dismissing[-1] == __screens[-1]:
                dismissing.pop()
                await _do_pop_last(__screens[-1])

            # check again, if have any dismissing screen, destroy them
            if not dismissing:
                continue

            # destroy all dismissing screen
            log.debug(__name__, "remove dismissing screens")
            for s in dismissing:
                log.debug(__name__, f"removing screen: {s.__class__.__name__}")
                s.delete()
                __screens.remove(s)
                del s

            gc.collect()

def monitor_task_exit():
    # find all dismissing screen
    log.debug(__name__, "manager task exit")
    if __debug__:
        log.debug(__name__, "screens: ")
        ss = ((s.__class__.__name__, s.state) for s in __screens)
        for (name, state) in ss:
            log.debug(__name__, f"\t{name}: {state}")

    dismissing = [s for s in __screens if s.state == 'dismissing']
    screen = first_alive_screen()
    if screen and screen != __screens[-1]:
        screen.set_pos(0, 0)
        lv.scr_load(screen)
    for s in dismissing:
        log.debug(__name__, f"removing screen: {s.__class__.__name__}")
        s.delete()
        __screens.remove(s)
        del s
