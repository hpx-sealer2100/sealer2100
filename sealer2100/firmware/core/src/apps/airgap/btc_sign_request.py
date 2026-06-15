from . import parser_path, MFPNotMatch, get_mfp
from apps.common.seed import get_seed

from trezor import log
from trezor.wire import errors
from trezor.ui.layouts import show_airgap_qrcode
import  ur_parser
from trezor.airgap.rust_ur.rust_ur import *
from trezor.airgap.rust_ur import utils


from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from trezor.wire import GenericContext
    from typing import List
    pass

def log_psbt(detail: ur_parser.DisplayTxDetail):
    log.debug(__name__, f"network: {detail.network()}")
    log.debug(__name__, f"total input amount: {detail.total_input_amount()}")
    log.debug(__name__, f"total output amount: {detail.total_output_amount()}")
    log.debug(__name__, f"fee: {detail.fee_amount()}")

    log.debug(__name__, f"total input (Satoshi): {detail.total_input_amount()}")
    log.debug(__name__, f"total output (Satoshi): {detail.total_output_amount()}")
    log.debug(__name__, f"fee (Satoshi): {detail.fee_sat()}")

    inputs: List[ur_parser.DisplayTxDetailInput] = detail.inputs()
    for (i, input) in enumerate(inputs):
        log.debug(__name__, f"input[{i}].address: {input.address()}")
        log.debug(__name__, f"input[{i}].amount: {input.amount()}")
        log.debug(__name__, f"input[{i}].is_mine: {input.is_mine()}")
        log.debug(__name__, f"input[{i}].is_external: {input.is_external()}")
    outputs: List[ur_parser.DisplayTxDetailOutput] = detail.outputs()
    for (i, output) in enumerate(outputs):
        log.debug(__name__, f"output[{i}].address: {output.address()}")
        log.debug(__name__, f"output[{i}].amount: {output.amount()}")
        log.debug(__name__, f"output[{i}].is_mine: {output.is_mine()}")
        log.debug(__name__, f"output[{i}].is_external: {output.is_external()}")

async def psbt_sign_request(ctx: "GenericContext", rust_ur: RustDecodedUR):
    log.debug(__name__, "handling psbt sign request")
    try:
        from apps.base import handle_Initialize
        from trezor.messages import Initialize
        #1.Initialize session
        init = Initialize(session_id=b"\x00")
        await handle_Initialize(ctx, init)
        #2.get master key fingerprint
        mfp = await get_mfp(ctx)
        #3.get btc keys
        from trezor.ui.screen.home.connectapp.wallets import BtcAirgapWallet
        wallet = BtcAirgapWallet()
        keys = await wallet.get_keys(have_initialize=True)
        log.debug(__name__, f"keys: {keys}")
        #4.PSBT check
        check = ur_parser.btc_check_psbt(rust_ur.data(), mfp, keys)
        if check != 0:
            raise MFPNotMatch()
        #5.PSBT parse
        parsed_psbt = ur_parser.btc_parse_psbt(rust_ur.data(), mfp, keys)
        if parsed_psbt.error_code() != 0:
            raise errors.DataError("Invalid PSBT data")

        log_psbt(parsed_psbt)

        #6.confirm psbt
        await psbt_confirm_request(ctx, parsed_psbt)

        #7.get seed
        seed = await get_seed(ctx)
        #8.sign psbt
        ur_result = ur_parser.btc_sign_psbt(rust_ur.data(), seed,mfp)
        if ur_result.error_code() != 0:
            raise errors.DataError("Failed to sign PSBT")
        #9.show signed psbt
        rust_encoded_ur = RustEncodedUR()
        rust_encoded_ur.encoding(ur_result)
        await show_airgap_qrcode(rust_encoded_ur)
    finally:
        from trezor.messages import EndSession
        from apps.base import handle_EndSession

        await handle_EndSession(ctx, EndSession())

async def psbt_confirm_request(ctx: "GenericContext", psbt: ur_parser.DisplayTxDetail):
    from trezor.ui.layouts.bitcoin import (
        confirm_output,
        confirm_total,
    )
    from trezor.ui.layouts import confirm_final

    from apps.bitcoin.keychain import get_coin_by_name
    name = psbt.network()
    if name == "Bitcoin Mainnet":
        name = "Bitcoin"
    coin = get_coin_by_name(name)
    ctx.icon_path = f"A:/res/{coin.icon}"

    # ask user to confirm each output
    outputs = psbt.outputs()
    for output in outputs:
        # skip mine output
        if output.is_mine():
            continue
        await confirm_output(ctx, output.address(), output.amount())

    # ask user to confirm total amount
    total = psbt.total_input_amount()
    amount = psbt.total_output_amount()
    fee = psbt.fee_amount()
    await confirm_total(ctx, total, fee, amount)

    # ask user to confirm final
    await confirm_final(ctx, coin.coin_name)
