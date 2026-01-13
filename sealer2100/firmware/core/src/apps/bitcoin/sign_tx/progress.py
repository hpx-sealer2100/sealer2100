from typing import TYPE_CHECKING

from trezor import ui

if TYPE_CHECKING:
    from trezor.messages import SignTx
    from apps.common.coininfo import CoinInfo
    from .tx_info import OriginalTxInfo

# Checking previous transactions typically requires the following pieces of
# information to be fetched for each input:
# the input, prevtx metadata, prevtx input, prevtx output, prevtx change-output
_PREV_TX_MULTIPLIER = 0


class Progress:
    def __init__(self):
        self.progress = 0
        self.steps = 0
        self.signing = False
        self.icon = None

        # We don't know how long it will take to fetch the previous transactions,
        # so for each one we reserve _PREV_TX_MULTIPLIER steps in the signing
        # progress. Once we fetch a prev_tx's metadata, we subdivide the reserved
        # space and then prev_tx_step represents the progress of fetching one
        # prev_tx input or output in the overall signing progress.
        self.prev_tx_step = 0

    def init(self, tx: SignTx) -> None:
        self.progress = 0
        self.signing = False

        # Step 1 and 2 - load inputs and outputs
        self.steps = tx.inputs_count + tx.outputs_count

        self.report_init()
        self.report()

    def init_signing(
        self,
        external: int,
        segwit: int,
        presigned: int,
        taproot_only: bool,
        serialize: bool,
        coin: CoinInfo,
        tx: SignTx,
        orig_txs: list[OriginalTxInfo],
    ) -> None:
        if __debug__:
            self.assert_finished()

        self.progress = 0
        self.steps = 0
        self.signing = True

        # Step 3 - verify inputs
        if taproot_only or (coin.overwintered and tx.version == 5):
            self.steps += presigned
        else:
            self.steps = tx.inputs_count * _PREV_TX_MULTIPLIER

        for orig in orig_txs:
            self.steps += orig.tx.inputs_count

        # Steps 3 and 4 - get_legacy_tx_digest() for each legacy input.
        if not (coin.force_bip143 or coin.overwintered or coin.decred):
            self.steps += (tx.inputs_count - segwit) * (
                tx.inputs_count + tx.outputs_count
            )

            if segwit != tx.inputs_count:
                # The transaction has a legacy input.

                # Simplification: We assume that all original transaction inputs
                # are legacy, since mixed script types are not supported in Suite.
                for orig in orig_txs:
                    self.steps += orig.tx.inputs_count * (
                        orig.tx.inputs_count + orig.tx.outputs_count
                    )

        # Steps 4 and 6 - serialize and sign inputs
        if serialize:
            # Step 4 - serialize all inputs.
            self.steps += tx.inputs_count

            # Step 6 - serialize witnesses for all segwit inputs except for the
            # external ones that are not presigned.
            self.steps += segwit - (external - presigned)
        else:
            # Add the number of inputs to be signed.
            self.steps += tx.inputs_count - external

        # Step 5 - serialize outputs
        if serialize and not coin.decred:
            self.steps += tx.outputs_count

        self.report_init()
        self.report()

    def init_prev_tx(self, inputs: int, outputs: int) -> None:
        self.prev_tx_step = _PREV_TX_MULTIPLIER / (inputs + outputs)

    def advance(self) -> None:
        self.progress += 1
        self.report()

    def advance_prev_tx(self) -> None:
        self.progress += self.prev_tx_step
        self.report()

    def report_init(self) -> None:
        from trezor import workflow

        workflow.close_others()

    def report(self) -> None:
        from trezor import workflow, log
        from trezor.ui import i18n
        from trezor.ui.screen import manager
        from trezor.ui.screen.apps.bitcoin import Processing
        operation = i18n.Text.signing if self.signing else i18n.Text.loading
        pct = self.progress*100//self.steps
        log.debug(__name__, f"{operation}: {pct}")
        screen = manager.try_switch_to(Processing)
        if screen:
            if self.signing:
                screen.set_icon("A:/res/hp/ic_construct_trans.png")
            else:
                screen.set_icon("A:/res/hp/ic_loading_trans.png")
            screen.set_label(operation)
            screen.set_progess(pct)
            return

        screen = Processing(self.icon)
        if self.signing:
            screen.set_icon("A:/res/hp/ic_construct_trans.png")
        else:
            screen.set_icon("A:/res/hp/ic_loading_trans.png")
        screen.set_label(operation)
        screen.set_progess(pct)
        workflow.spawn(screen.show())

    if __debug__:

        def assert_finished(self) -> None:
            if abs(self.progress - self.steps) > 0.5:
                from trezor import wire

                operation = "signing" if self.signing else "loading"
                raise wire.FirmwareError(
                    f"Transaction {operation} progress finished at {self.progress}/{self.steps}."
                )


progress = Progress()
