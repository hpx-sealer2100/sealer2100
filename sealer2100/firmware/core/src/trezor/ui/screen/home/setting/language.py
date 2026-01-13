import lvgl as lv

from typing import TYPE_CHECKING

from trezor import log, utils, workflow
from trezor.ui.screen import Modal
from trezor.ui.component.container import VStack
from trezor.ui import i18n
from trezor.ui.screen.confirm import SimpleConfirm
from trezor.ui.result import Cancel
from storage import device

from .detail import OptionsDetail

class Language(OptionsDetail):
    # the method for `SettingDetailProtocl`
    @classmethod
    def current(cls):
        log.debug(__name__, f"current --language : {i18n.using}")
        return i18n.using

    def __init__(self):
        super().__init__(i18n.Setting.language, i18n.languages)
        self.reverting = False

    def on_loaded(self):
        super().on_loaded()

        # focus on current again
        # maybe user click cancel on confirm
        # revert to current
        current = self.current()
        item = utils.first(self.options, lambda item: item.option == current)
        obj = self.group.get_focused()
        if obj != item:
            # this function will trigger save_option
            lv.group_focus_obj(item)

    def save_option(self, option: i18n.Language):
        if self.reverting:
            self.reverting = False
            return
        workflow.spawn(self.do_confirm_save_option(option))

    async def do_confirm_save_option(self, option: i18n.Language):
        screen = SimpleConfirm(i18n.Text.changing_language)
        screen.btn_confirm.set_text(i18n.Button.continue_)
        await screen.show()
        r = await screen
        if isinstance(r, Cancel):
            self.reverting = True
            return

        # `Continue`
        i18n.change_language(option)
        device.set_language(option.code)

        from trezor.ui.screen import manager
        from trezor.ui import events
        manager.publish(events.LANGUAGE_CHANGED)

        await RestartApp().show()

class RestartApp(Modal):
    def __init__(self):
        super().__init__()
        self.content.set_style_pad_all(16, lv.PART.MAIN)

        self.create_content(VStack)
        self.content: VStack

        self.content.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.content.items_center()
        self.content.center()

        label = self.add(lv.label)
        label.set_text(i18n.Text.restarting)

        async def restart_delay():
            from trezor import loop
            await loop.sleep(1500)
            utils.restart_app()

        from trezor import workflow
        workflow.spawn(restart_delay())

    async def show(self):
        from trezor.ui.screen import manager
        await manager.replace(self)

