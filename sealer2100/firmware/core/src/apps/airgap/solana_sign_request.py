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
    pass

async def solana_sign_request(ctx: "GenericContext", rust_ur: RustDecodedUR):
    log.debug(__name__, "handling solana sign request")
    try:
        from apps.base import handle_Initialize
        from trezor.messages import Initialize
        #1.Initialize session
        init = Initialize(session_id=b"\x00")
        await handle_Initialize(ctx, init)
        #2.Get path from ur, and convert to address_n
        path = ur_parser.solana_get_path(rust_ur.data())
        log.debug(__name__, f"solana_get_path: {path}")
        address_n = parser_path(path)
        #3.Get master key fingerprint
        mfp = await get_mfp(ctx)
        #5.Solana check
        check = ur_parser.solana_check(rust_ur.data(), mfp)
        if check != 0:
            raise MFPNotMatch()

        #6.Solana parse raw
        parsed_raw = ur_parser.solana_parse_tx_raw(rust_ur.data())
        if len(parsed_raw) == 0:
            raise errors.DataError("Invalid solana sign request data")

        #7 sign transaction
        from trezor.messages import SolanaSignTx
        from apps.solana.sign_tx import sign_tx
        raw_tx_bytes = utils.hex_to_bytes(parsed_raw)
        req = SolanaSignTx(raw_tx=raw_tx_bytes,address_n=address_n)
        resp = await sign_tx(ctx, req)
        signature = resp.signature
        log.debug(__name__, f"signature: {utils.bytes_to_hex(signature)}")
        #8. Show signature qr code
        ur_result = ur_parser.solana_ur_encode_signature(rust_ur.data(), signature)
        rust_encoded_ur = RustEncodedUR()
        rust_encoded_ur.encoding(ur_result)
        await show_airgap_qrcode(rust_encoded_ur)       

    finally:
        from trezor.messages import EndSession
        from apps.base import handle_EndSession

        await handle_EndSession(ctx, EndSession())