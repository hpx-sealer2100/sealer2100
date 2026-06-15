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


async def tron_sign_request(ctx: "GenericContext", rust_ur: RustDecodedUR):
    log.debug(__name__, "handling tron sign request")
    try:
        from apps.base import handle_Initialize
        from trezor.messages import Initialize
        #1.Initialize session
        init = Initialize(session_id=b"\x00")
        await handle_Initialize(ctx, init)
        #2.get master key fingerprint
        mfp = await get_mfp(ctx)
        #3.Get path from ur, and convert to address_n and get xpub from device
        path = ur_parser.tron_parse_keystone_path(rust_ur.data())
        log.debug(__name__, f"tron_parse_keystone_path: {path}")
        address_n = parser_path(path)
        from trezor.messages import EthereumGetPublicKey
        from apps.ethereum.get_public_key import get_public_key
        hrd_address_n = []
        for i in address_n:
            if i & 0x80000000:
                hrd_address_n.append(i)
        log.debug(__name__, f"hrd_address_n: {hrd_address_n}")
        msg = EthereumGetPublicKey(address_n=hrd_address_n)
        pubkey = await get_public_key(ctx, msg)
        log.debug(
            __name__, f"xpub in device: {pubkey.xpub}"
        )
        #4.Check ur data with mfp and address_n
        check = ur_parser.tron_check_keystone(rust_ur.data(), mfp, pubkey.xpub)
        if check != 0:
            raise MFPNotMatch()
        #5. Get raw from ur
        hex_raw = ur_parser.tron_parse_keystone_raw(rust_ur.data())
        if hex_raw is None:
            raise ValueError("tron_parse_keystone_raw returned None")
        log.debug(__name__, f"tron_parse_keystone_raw: {hex_raw}")
        #6. deserialize raw
        from apps.tron.serialize import deserialize
        msg = deserialize(bytearray(utils.hex_to_bytes(hex_raw)))
        #7. sign the tx with device
        from apps.tron.sign_tx import sign_tx
        msg.address_n = address_n
        signed_tx = await sign_tx(ctx, msg)
        signature = signed_tx.signature
        #8. encode signature to ur
        log.debug(__name__, f"signature: {utils.bytes_to_hex(signature)}")
        ur_result = ur_parser.tron_ur_encode_signature(rust_ur.data(), signature)
        if ur_result.error_code() != 0:
            raise errors.DataError("Failed to encode tron signature")
        rust_encoded_ur = RustEncodedUR()
        rust_encoded_ur.encoding(ur_result)
        #9. Show the signature qr code
        await show_airgap_qrcode(rust_encoded_ur)

    finally:
        from trezor.messages import EndSession
        from apps.base import handle_EndSession

        await handle_EndSession(ctx, EndSession())
