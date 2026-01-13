import lvgl as lv
import ujson as json
from storage import device
from trezor.ui import i18n, Style, theme, colors, font
from trezor.ui.component import VStack, HStack, LabeledText
from trezor.ui.screen.confirm import SimpleConfirm
from trezor.ui.screen import Navigation
from trezor import io, loop, uart, utils, workflow, log
from trezor.ui.theme import Styles

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Generator, List
    pass

NFT_DIR = "1:/res/nfts"
class NftSource:
    def __init__(self, name: str):
        self.name = name

    def thumbnail(self) -> str:
        return f"A:{NFT_DIR}/zooms/{self.name}"

    def source(self) -> str:
        return f"A:{NFT_DIR}/imgs/{self.name}"

    def desc(self) -> 'NftDesc':
        return NftDesc(f"{NFT_DIR}/desc/-{self.key()}.json")

    def key(self) -> int:
        # 'xxx-d.png'
        items = self.name.split(".")
        items = items[0].split('-')
        return int(items[-1])

_DESC_KEYS = ['header', 'subheader', 'network', 'owner']

class NftDesc:
    valided: bool
    def __init__(self, file: str):
        self.valided = False

        with io.fatfs.open(file, "r") as f:
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

    def header(self) -> str:
        return self.meta.get('header')

    def subheader(self) -> str:
        return self.meta.get('subheader')

def NftSources() -> List[NftSource]:

    nfts: List[NftSource] = []

    for size, _, name in io.fatfs.listdir(f"{NFT_DIR}/zooms"):
        log.debug(__name__, f"nft: {name}")
        if name == '.DS_Store':
            continue
        if not size:
            continue

        nft = NftSource(name)
        try:
            pth = f"{NFT_DIR}/imgs/{name}"
            size, _, _ = io.fatfs.stat(pth)
            if not size:
                continue
            nfts.append(nft)
        except:
            continue

    nfts.sort(key=lambda x: x.key())
    return nfts

class NftApp(Navigation):
    def __init__(self):
        super().__init__()
        self.set_scroll(True)
        self.title.set_text(i18n.App.nft)
        nfts = NftSources()
        self.subtitle.set_text(i18n.Subtitle.nft_count.format(len(nfts)))

        grid = self.add(lv.obj)
        grid.align(lv.ALIGN.TOP_MID, 0, 0)
        grid.add_style(Styles.container, lv.PART.MAIN)

        rows = (len(nfts)+1) // 2
        row_dsc = [208] * rows
        row_dsc.append(lv.GRID_TEMPLATE.LAST)
        # 2 columns
        col_dsc = [
            208,
            208,
            lv.GRID_TEMPLATE.LAST,
        ]
        grid.set_layout(lv.LAYOUT_GRID.value)
        grid.add_style(
            Style()
            .pad_gap(0)
            .grid_column_dsc_array(col_dsc)
            .grid_row_dsc_array(row_dsc),
            0,
        )
        grid.set_grid_align(lv.GRID_ALIGN.SPACE_AROUND, lv.GRID_ALIGN.START)

        for i, nft in enumerate(nfts):
            col = i % 2
            row = i // 2
            img = NftImgItem(grid)
            img.set_nft(nft)
            img.set_grid_cell( lv.GRID_ALIGN.STRETCH, col, 1, lv.GRID_ALIGN.STRETCH, row, 1)

class NftImgItem(lv.img):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.set_style_radius(16, lv.PART.MAIN)
        self.set_style_clip_corner(True, lv.PART.MAIN)
        self.nft = None

        self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def set_nft(self, nft: NftSource):
        self.set_src(nft.thumbnail())
        self.nft = nft
    def on_click(self, _):
        workflow.spawn(NftDetail(self.nft).show())


class NftDetail(Navigation):
    def __init__(self, nft: NftSource):
        super().__init__()
        self.nft = nft
        self.create_content(VStack)
        self.content: VStack

        # img
        img = self.add(lv.img)
        img.set_src(nft.source())
        self.set_style_radius(32, lv.PART.MAIN)
        self.set_style_clip_corner(True, lv.PART.MAIN)

        desc = nft.desc()
        # header
        header = self.add(lv.label)
        header.set_width(lv.pct(100))
        header.set_text(f"#{desc.header()}")
        header.set_style_text_font(font.Medium.PF44, lv.PART.MAIN)

        # subheader
        subheader = self.add(lv.label)
        subheader.set_width(lv.pct(100))
        subheader.set_text(desc.subheader())
        subheader.set_style_text_font(font.small, lv.PART.MAIN)


        self.btn.set_text(i18n.Button.set_home_screen)
        self.btn.add_event_cb(self.on_click_btn, lv.EVENT.CLICKED, None)

    def on_click_btn(self, _):
        workflow.spawn(self.do_set_home_screen())

    async def do_set_home_screen(self):
        screen = SimpleConfirm(i18n.Text.set_as_homescreen_desc)
        screen.mode('result')
        screen.set_title(i18n.Title.set_as_homescreen, 'A:/res/hp/set_home.png')
        await screen.show()

        if not await screen:
            return

        device.set_homescreen(self.nft.source())
