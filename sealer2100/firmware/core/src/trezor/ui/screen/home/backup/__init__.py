from trezor.ui.component.item import ItemImg
import lvgl as lv

from trezor.ui import i18n
from trezor.ui.component import VStack
from trezor.ui.screen import Navigation
from trezor import workflow, motor,log
from trezor.ui.theme import Styles
class BackupApp(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.App.backup)
        self.subtitle.set_text(i18n.Subtitle.backup)
        # create self.content
        self.create_content(VStack)
        self.content: VStack
        self.content.add_style(Styles.board, lv.PART.MAIN)
        
        blue_view = ItemImg(self.content,i18n.Text.MetaCard_pro,"A:/res/hp/ic_MateCard.png", 'left')
        blue_view.add_event_cb(self.backup_card, lv.EVENT.CLICKED, None)
        
    def backup_card(self,_):
        workflow.spawn(self.do_backup_mnemonic())


    async def do_backup_mnemonic(self):

        from trezor import workflow
        from trezor.ui.screen import manager
        from apps.management.recovery_device import recovery_device
        from trezor.messages import RecoveryDevice,Success
        from trezor.wire import DUMMY_CONTEXT, ProcessError
        async def check_mnemonic():
            try:
                return await recovery_device(DUMMY_CONTEXT, RecoveryDevice(dry_run=True))
            except Exception:
                return None
            finally:
                manager.try_switch_to(BackupApp)

        # check mnemonic first
        r = await check_mnemonic()
        if not isinstance(r, Success):
            return

        screen = StartBackup()
        await screen.show()
        await screen

class StartBackup(Navigation):
    def __init__(self):
        super().__init__()
        self.title.set_text(i18n.Title.start_backup)
        self.subtitle.set_text(i18n.Subtitle.start_backup)

        img = self.add(lv.img)
        img.set_src("A:/res/hp/nfc_card.png")
        img.align(lv.ALIGN.TOP_MID, 0, 16)

        self.btn.set_text(i18n.Button.continue_)
        self.btn.add_state(lv.STATE.DISABLED)