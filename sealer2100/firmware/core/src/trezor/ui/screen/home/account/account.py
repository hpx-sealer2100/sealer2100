import lvgl as lv

from typing import TYPE_CHECKING

from trezor import loop, workflow ,log
from trezor.ui import i18n, Style, theme, colors,font
from trezor.ui.component import VStack, HStack, LabeledText
from trezor.ui.screen import Navigation

# __USE_BACKGROUND_IMAGE__ = False
class AccountApp(Navigation):
    def __init__(self):
        super().__init__()
        self.set_scroll(True)
        self.title.set_text(i18n.App.account)
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.align_to(self.title, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)    
        coins = [
            {
                "label": "Bitcoin",
                "icon": "A:/res/hp/btc-btc.png",
            },
            {
                "label": "Ethereum",
                "icon": "A:/res/hp/evm-eth.png",
            },
            {
                "label": "Arbitrum",
                "icon": "A:/res/hp/evm-arb.png",
            },
            {
                "label": "Optimism",
                "icon": "A:/res/hp/evm-op.png",
            },
            {
                "label": "ZKSync Era",
                "icon": "A:/res/hp/evm-zksync.png",
            },
            {

                "label": "MATIC",
                "icon": "A:/res/hp/evm-matic.png",
            },
            {

                "label": "OKX",
                "icon": "A:/res/hp/evm-okx.png",
            },
        ]
        # Create accounts
        accounts = lv.list(self)
        accounts.set_pos(24, 140)
        list_style = (Style()
                      .width(432)
                      .height(lv.SIZE.CONTENT)
                      .pad_top(0)
                      .pad_left(24)
                      .pad_right(24)
                      .pad_bottom(0)
                      .bg_color(colors.STD.WHITE)
                      .radius(16)
                    )
        accounts.add_style(list_style, lv.PART.MAIN|lv.STATE.DEFAULT)
        accounts.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        accounts.set_style_radius(3, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
        accounts.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
        accounts.set_style_bg_color(lv.color_hex(0xb0c7cf), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
        accounts.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
        item_style = (Style()
            .pad_top(24)
            .pad_bottom(24)
            .border_width(1)
            .border_opa(255)
            .border_color(lv.color_hex(0xe1e6ee))
            .border_side(lv.BORDER_SIDE.BOTTOM)
            .text_color(colors.STD.BLACK)
            .text_font(font.Regular.PF28)
        )
        self.icons = {}
        for i, a in enumerate(coins):
            label, icon = a["label"], a["icon"]
            item = accounts.add_btn(icon, label)
            self.icons[id(item)] = i
            item.add_style(item_style, lv.PART.MAIN|lv.STATE.DEFAULT)
            item.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, e: lv.event_t):
        target = e.get_target()
        index = self.icons.get(id(target), -1)
        from trezor import workflow
        from .accountdetail import AccountDetail
        if index == 0:
            from .btcfork import BTCFork, BTC_COIN_NAME
            btc =  BTCFork(BTC_COIN_NAME)
            workflow.spawn(AccountDetail(btc.get_name(),btc.get_path(),btc.get_icon()).show())
        elif index == 1:
            workflow.spawn(AccountDetail("Ethereum","m/44'/60'/0'/0/0","").show())
