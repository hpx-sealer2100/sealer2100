import lvgl as lv
import ujson as json
from storage import device
from trezor.ui import i18n, Style, theme, colors, font, events
from trezor.ui.component import VStack, HStack, LabeledText
from trezor.ui.screen.confirm import SimpleConfirm
from trezor.ui.screen import Navigation
from trezor import io, loop, uart, utils, workflow, log
from trezor.ui.theme import Styles

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Generator, List, Literal
    PathType = Literal['FS', 'LV']
    pass

NFT_DIR = "/user/nfts"

###
# the id is unique in `token`
# NFT dir structure:
# ├── {token}
#   ├── desc/
#   │   ├── {id}.json
#   ├── thumbnail
#   │   ├── {id}.png
#   ├── image/
#   │   ├── {id}.png
#   ├── wallpaper/
#   │   ├── {id}.png



_DESC_KEYS = ['id', 'name', 'network', 'owner', 'token', 'ext']

class NftDesc:
    valided: bool
    def __init__(self, file: str):
        self.valided = False
        self.file = file

        with io.fs.open(file, "r") as f:
            try:
                content = bytearray()
                block = bytearray(256)
                while len(content) < 1024:
                    n = f.read(block)
                    if not n:
                        break
                    content.extend(block)
                meta = json.loads(content)
                if not isinstance(meta, dict):
                    log.debug(__name__, f"invalid metadata struct in {file}")
                    return
                if not all(k in meta.keys() for k in _DESC_KEYS):
                    log.debug(__name__, f"not contain all desc keys in {file}")
                    return

                self.valided = True
                self.meta = meta
            except Exception as e:
                log.exception(__name__, f'{e}')
                return

    def id(self) -> str:
        return self.meta.get('id')

    def name(self) -> str:
        return self.meta.get('name')

    def network(self) -> str:
        return self.meta.get('network')

    def owner(self) -> str:
        return self.meta.get('owner')

    def token(self) -> str:
        return self.meta.get('token')

    def ext(self) -> str:
        return self.meta.get('ext')

class NftSource:
    def __init__(self, desc: NftDesc):
        self.desc = desc

    def thumbnail(self, pt: PathType) -> str:
        path = f"{NFT_DIR}/{self.desc.token()}/thumbnail/{self.desc.id()}.{self.desc.ext()}"
        if pt == 'LV':
            return f"A:{path}"
        return path

    def source(self, pt: PathType) -> str:
        path = f"{NFT_DIR}/{self.desc.token()}/image/{self.desc.id()}.{self.desc.ext()}"
        if pt == 'LV':
            return f"A:{path}"
        return path

    def wallpaper(self, pt: PathType) -> str:
        path = f"{NFT_DIR}/{self.desc.token()}/wallpaper/{self.desc.id()}.{self.desc.ext()}"
        if pt == 'LV':
            return f"A:{path}"
        return path

    def valid(self) -> bool:
        if not self.desc.valided:
            return False
        # check thumbnail, image, and wallpaper exist
        try:
            io.fs.stat(self.thumbnail('FS'))
            io.fs.stat(self.source('FS'))
            io.fs.stat(self.wallpaper('FS'))
        except Exception as e:
            log.exception(__name__, f'{e}')
            return False
        return True

def nfts_sources() -> Generator[NftSource]:
    try:
        dir = io.fs.dir_open(NFT_DIR)
    except Exception as e:
        log.exception(__name__, f'{e}')
        return []

    for _, attrs, token in dir:
        # filter only token dir
        if 'd' not in attrs or token == '.' or token == '..' or token == '.DS_Store':
            continue
        log.debug(__name__, f"nfts for token: {token}")
        try:
            desc_dir = io.fs.dir_open(f"{NFT_DIR}/{token}/desc")
        except:
            continue
        for _, _, name in desc_dir:
            # filter only desc file
            if not name.endswith(".json"):
                continue
            desc = NftDesc(f"{NFT_DIR}/{token}/desc/{name}")
            if not desc.valided:
                continue
            source = NftSource(desc)
            if not source.valid():
                continue
            yield source

def delete_nft(nft: NftSource):
    desc = nft.desc
    try:
        io.fs.remove(nft.thumbnail('FS'))
        io.fs.remove(nft.source('FS'))
        io.fs.remove(nft.wallpaper('FS'))
        io.fs.remove(desc.file)
        home_screen = device.get_homescreen()
        if home_screen == nft.wallpaper('LV'):
            device.set_homescreen('')
    except Exception as e:
        log.exception(__name__, f'{e}')

class NftApp(Navigation):
    def __init__(self):
        super().__init__()
        self.set_scroll(True)
        self.title.set_text(i18n.App.nft)
        self.create_content(VStack)
        self.content: VStack
        self.load_nft_views()

        self.add_event_cb(self.on_delete_nft, events.NFT_CHANGED, None)

    def load_nft_views(self):
        nfts = list(nfts_sources())
        # name first, then id
        nfts.sort(key=lambda x: (x.desc.name(), x.desc.id()))

        self.subtitle.set_text(i18n.Subtitle.nft_count.format(len(nfts)))

        # get nft groups aka. name, `nfts` have sorted
        groups: List[str] = []
        for nft in nfts:
            name = nft.desc.name()
            if name not in groups:
                groups.append(name)

        for name in groups:
            self.create_nft_group(name, [nft for nft in nfts if nft.desc.name() == name])

    def on_delete_nft(self, _):
        # clean then load again
        self.content.clean()
        self.load_nft_views()

    def create_nft_group(self, name: str, nfts: List[NftSource]):
        container = self.add(VStack)
        container.add_style(Styles.container, lv.PART.MAIN)

        label = container.add(lv.label)
        label.set_width(lv.pct(100))
        label.set_height(lv.SIZE.CONTENT)
        label.set_long_mode(lv.label.LONG.WRAP)
        label.set_text(name)

        group = container.add(HStack)
        group.add_style(Styles.container, lv.PART.MAIN)
        group.add_style(Styles.board, lv.PART.MAIN)
        group.set_height(lv.SIZE.CONTENT)

        group.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
        group.set_style_flex_main_place(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.PART.MAIN)
        for nft in nfts:
            item = group.add(NftItem)
            item.set_nft(nft)
        if len(nfts)%3:
            # append a placeholder
            obj = group.add(lv.obj)
            obj.add_style(
                Style()
                .width(108)
                .height(108)
                .bg_opa(lv.OPA.TRANSP)
                .border_width(0),
                lv.PART.MAIN
            )

class NftItem(lv.img):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.set_style_clip_corner(True, lv.PART.MAIN)
        self.set_style_radius(8, lv.PART.MAIN)
        self.nft = None

        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def set_nft(self, nft: NftSource):
        self.nft = nft
        self.set_src(nft.thumbnail('LV'))
    def on_click(self, _):
        workflow.spawn(NftDetail(self.nft).show())


class NftDetail(Navigation):
    def __init__(self, nft: NftSource):
        super().__init__()

        del_btn = lv.img(self)
        del_btn.set_src('A:/res/hp/remove.png')
        del_btn.add_flag(lv.obj.FLAG.CLICKABLE)
        del_btn.add_event_cb(self.on_click_del, lv.EVENT.CLICKED, None)
        del_btn.align(lv.ALIGN.TOP_RIGHT, -24, 60)
        del_btn.add_style(Styles.pressed, lv.PART.MAIN|lv.STATE.PRESSED)

        self.nft = nft
        self.create_content(VStack)
        self.content: VStack

        # img
        img = self.add(lv.img)
        img.set_src(nft.source('LV'))
        img.set_style_radius(32, lv.PART.MAIN)
        img.set_style_clip_corner(True, lv.PART.MAIN)

        desc = nft.desc
        # id
        id = self.add(lv.label)
        id.set_width(lv.pct(100))
        id.set_text(f"#{desc.id()}")
        id.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        # name
        name = self.add(lv.label)
        name.set_width(lv.pct(100))
        name.set_text(desc.name())
        name.set_style_text_font(font.small, lv.PART.MAIN)


        self.btn.set_text(i18n.Button.set_home_screen)
        self.btn.add_event_cb(self.on_click_btn, lv.EVENT.CLICKED, None)

    def on_click_btn(self, _):
        workflow.spawn(self.do_set_home_screen())

    def on_click_del(self, _):
        workflow.spawn(self.do_delete_nft(self.nft))

    async def do_set_home_screen(self):
        screen = SimpleConfirm(i18n.Text.set_as_homescreen_desc)
        screen.mode('result')
        screen.set_title(i18n.Title.set_as_homescreen)
        screen.set_icon('A:/res/hp/set_home.png')
        await screen.show()

        if not await screen:
            return

        device.set_homescreen(self.nft.wallpaper('LV'))

    async def do_delete_nft(self, nft: NftSource):
        screen = SimpleConfirm(i18n.Text.delete_nft_desc)
        screen.mode('result')
        screen.set_title(i18n.Title.delete_nft)
        screen.set_icon('A:/res/hp/delete.png')
        await screen.show()

        if not await screen:
            return

        delete_nft(nft)

        from trezor.ui.screen import manager
        manager.publish(events.NFT_CHANGED)
        manager.try_switch_to(NftApp)
