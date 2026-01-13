import lvgl as lv
from typing import TYPE_CHECKING

from . import *
from storage import device, io
from trezor import utils
from trezor.ui.screen import Navigation
from trezor.ui.component import HStack
from trezor.ui.screen.message import Success
from trezor.ui import i18n, Cancel
from trezor.ui.screen.confirm import SimpleConfirm
from trezor import workflow

if TYPE_CHECKING:
    from typing import Generator, List
    pass

if utils.EMULATOR:
    def wallpaper_filter(name: str):
        if name == ".DS_Store":
            return False
        return True

class WallpaperImage(lv.img):
    def __init__(self, parent):
        super().__init__(parent)
        self.add_flag(lv.obj.FLAG.CLICKABLE) 
        self.set_style_radius(16, lv.PART.MAIN)
        self.set_style_clip_corner(True, lv.PART.MAIN)

        self._focused_img = lv.img(self)
        self._focused_img.set_src("A:/res/hp/ic_done.png")
        self._focused_img.align(lv.ALIGN.TOP_RIGHT, -16, 16)
        self._focused_img.add_flag(lv.obj.FLAG.HIDDEN)

        self.add_event_cb(lambda _: self._focused_img.clear_flag(lv.obj.FLAG.HIDDEN), lv.EVENT.FOCUSED, None)
        self.add_event_cb(lambda _: self._focused_img.add_flag(lv.obj.FLAG.HIDDEN), lv.EVENT.DEFOCUSED, None)

    def set_src(self, src : 'ImageSource'):
        super().set_src(src.thumbnail())
        self.src = src

class ImageSource:
    def __init__(self, path, name):
        self.path = path
        self.name = name

    def thumbnail(self) -> str:
        return f"A:{self.path}/thumbnail/{self.name}"

    def source(self) -> str:
        return f"A:{self.path}/{self.name}"
    
class HomeScreen(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Setting.home_screen)

        # hack and padding
        body = self.header.get_parent()
        body.set_style_pad_bottom(0, lv.PART.MAIN)

        self.create_content(HStack)
        self.content: HStack
        self.content.spacing(0)
        self.content.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
        self.content.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN)
        self.content.set_style_pad_row(16, 0)
        self.images: List[WallpaperImage] = []
        for w in self.wallpapers():
            log.debug(__name__, f"wallpaper: {w.source()}")
            log.debug(__name__, f"thumbnail wallpaper: {w.thumbnail()}")
            img = WallpaperImage(self.content)
            img.set_src(w)
            self.reverting = False
            # 添加点击事件回调
            img.add_event_cb(self.on_image_clicked, lv.EVENT.CLICKED, img)
            self.images.append(img)
        if len(self.images)%2:
            # append a placeholder
            obj = lv.obj(self.content)
            obj.add_style(
                Style()
                .width(280)
                .height(300)
                .bg_opa(lv.OPA.TRANSP)
                .border_width(0),
                lv.PART.MAIN
            )


        self.group = lv.group_create()
        for img in self.images:
            self.group.add_obj(img)

        # find the current and set focus
        current = self.current()
        item = utils.first(self.images, lambda item: item.src.source() == current)
        lv.group_focus_obj(item)
        if not item:
            self.images[0]._focused_img.add_flag(lv.obj.FLAG.HIDDEN)
        
        # self.group.set_focus_cb(self.on_group_focus_changed)
    def on_image_clicked(self, event):
        # 获取被点击的图片对象
        img = event.get_target()
        item = utils.first(self.images, lambda item: item == img)
        log.debug(__name__, f"user clicked: {item.src.thumbnail()}")
        self.save_option(item.src.source())

    def on_deleting(self):
        super().on_deleting()
        # 如果一个对象在group，在screen删除的时候，会发送DEFOCUSED消息
        # 这会导致已经删除的对象去响应消息，出现crash问题
        # 所以在页面消失的时候，删除掉group，规避掉问题
        self.group._del()

    def on_group_focus_changed(self, group: lv._group_t):
        obj = group.get_focused()
        item = utils.first(self.images, lambda item: item == obj)
        log.debug(__name__, f"user clicked: {item.src.thumbnail()}")
        self.save_option(item.src.source())

    def current(self) -> str:
        return device.get_homescreen()

    def save_option(self, option: str):
        device.set_homescreen(option)

        # from trezor.ui.screen import manager
        # from trezor.ui import events
        # manager.publish(events.WALLPAPER_CHANGED)

        # workflow.spawn(self.do_confirm_save_option(option))
        # # 显示成功提示框
        # success_popup = Success(i18n.Title.operate_success, i18n.Title.theme_success)
        # workflow.spawn(success_popup.show())  #异步显示弹框

    async def do_confirm_save_option(self, option: str):
        screen = SimpleConfirm(i18n.Text.confirm_replace_wallpaper)
        screen.btn_confirm.set_text(i18n.Button.confirm)
        # screen.btn_confirm.color(colors.DS.DANGER)
        await screen.show()
        r = await screen
        if isinstance(r, Cancel):
            self.reverting = True
            return

        # `Continue`
        device.set_homescreen(option)
        from trezor.ui.screen import manager
        from trezor.ui import events
        manager.publish(events.WALLPAPER_CHANGED)
 
    @staticmethod
    def wallpapers() -> Generator[ImageSource]:

        SYS_WALLPAPER_PATH = "0:/res/wallpapers"
        papers: List[ImageSource] = []
        # internal wallpapers
        for _, _, name in io.fatfs.listdir(SYS_WALLPAPER_PATH):
            if utils.EMULATOR:
               if not wallpaper_filter(name):
                   continue
            source = ImageSource("0:/res/wallpapers", name)
            try :
                path = f"{SYS_WALLPAPER_PATH}/thumbnail/{name}"
                size, _, _ = io.fatfs.stat(path)
                log.debug(__name__, f"{source.thumbnail()} : {size}")
                if size :
                    papers.append(source)
            except:
                continue

        USER_WALLPAPER_PATH = "1:/res/wallpapers"
        # user wallpapers
        for _, _, name in io.fatfs.listdir(USER_WALLPAPER_PATH):
            if utils.EMULATOR:
               if not wallpaper_filter(name):
                   continue
            source = ImageSource("1:/res/wallpapers", name)
            try :
                path = f"{USER_WALLPAPER_PATH}/thumbnail/{name}"
                size, _, _ = io.fatfs.stat(path)
                log.debug(__name__, f"{source.thumbnail()} : {size}")
                if size :
                    papers.append(source)
            except:
                continue

        papers.sort(key = lambda x: x.source())
        for p in papers:
            yield p


