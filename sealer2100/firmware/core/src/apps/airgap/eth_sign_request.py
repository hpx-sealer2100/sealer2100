from . import _keypath_to_address_n, MFPNotMatch

from trezor import log
from trezor.airgap.ur.ur import UR
from trezor.airgap.ur.ur_encoder import UREncoder
from trezor.wire import errors
from trezor.airgap.bc_types.eth_sign_request import (
    EthSignRequest,
    ETH_TRANSACTION_DATA,
    ETH_TYPED_DATA,
    ETH_RAW_BYTES,
    ETH_TYPED_TRANSACTION,
)
from trezor.ui.layouts import show_airgap_signature

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from trezor.wire import GenericContext
    pass


async def eth_sign_request(ctx: "GenericContext", cbor: bytes):
    req = EthSignRequest.from_cbor(cbor)

    try:
        # initialize session
        from apps.base import handle_Initialize
        from trezor.messages import Initialize
        from trezor import utils

        init = Initialize(session_id=b"\x00")
        await handle_Initialize(ctx, init)

        # get public key
        # verify keypath, keypath.source_fingerprint is matching
        from trezor.messages import EthereumGetPublicKey
        from apps.ethereum.get_public_key import get_public_key

        address_n = _keypath_to_address_n(req.derivation_path)
        # get master key fingerprint
        msg = EthereumGetPublicKey(address_n=address_n[:1])
        pubkey = await get_public_key(ctx, msg)
        log.debug(
            __name__, f"master key fingerprint in device: {hex(pubkey.node.fingerprint)}"
        )
        log.debug(
            __name__,
            f"master key fingerprint in request: {hex(req.derivation_path.source_fingerprint)}",
        )
        if pubkey.node.fingerprint != req.derivation_path.source_fingerprint:
            raise MFPNotMatch()
        # verify address
        if req.address:
            from trezor.messages import EthereumGetAddress
            from apps.ethereum.get_address import get_address
            from apps.ethereum.helpers import bytes_from_address

            msg = EthereumGetAddress(address_n=address_n)
            resp = await get_address(ctx, msg)
            addres_bytes = bytes_from_address(resp.address)
            if addres_bytes != req.address:
                raise MFPNotMatch()

        if req.data_type == ETH_TRANSACTION_DATA:
            # legacy transaction
            resp = await eth_sign_tx(ctx, req)
            v_bytes_length = utils.byte_length(resp.signature_v)
            signature = (
                resp.signature_r
                + resp.signature_s
                + resp.signature_v.to_bytes(v_bytes_length, "big")
            )
        elif req.data_type == ETH_TYPED_DATA:
            # eip-712 message
            resp = await eth_sign_typed_data(ctx, req)
            signature = resp.signature
        elif req.data_type == ETH_RAW_BYTES:
            # raw bytes
            resp = await eth_sign_message(ctx, req)
            signature = resp.signature
        elif req.data_type == ETH_TYPED_TRANSACTION:
            # typed transaction
            resp = await eth_sign_tx_eip1559(ctx, req)
            v_bytes_length = utils.byte_length(resp.signature_v)
            signature = (
                resp.signature_r
                + resp.signature_s
                + resp.signature_v.to_bytes(v_bytes_length, "big")
            )
        else:
            raise errors.DataError(f"Unknown data type: {req.data_type}")

        from trezor.airgap.bc_types.eth_signature import EthSignature

        sig = EthSignature(
            signature=signature, request_id=req.request_id, origin="HyperMate"
        )
        ur =UR(sig.type(), sig.cbor())

        # the result is `eth-signature`, is not too long, no need multi part
        qrcode = UREncoder.encode(ur)
        # make upper for compatibility
        qrcode = qrcode.upper()
        log.debug(__name__, f"qrcode: {qrcode}")

        await show_airgap_signature(qrcode)
    finally:
        from trezor.messages import EndSession
        from apps.base import handle_EndSession

        await handle_EndSession(ctx, EndSession())


# === eth sign request ===
async def eth_sign_tx(ctx: "GenericContext", req: EthSignRequest):
    from trezor.messages import EthereumSignTx
    from trezor.crypto import rlp
    from apps.ethereum.sign_tx import sign_tx
    from apps.ethereum.helpers import address_from_bytes

    # legacy
    # rlp([nonce, gasPrice, gasLimit, to, value, data, v, r, s])
    data = req.sign_data
    items = rlp.decode(data)
    log.debug(__name__, f"length of items: {len(items)}")
    if len(items) < 6:
        # we not care (v, r s)
        raise errors.DataError("Invalid sign data")
    address_n = _keypath_to_address_n(req.derivation_path)
    chain_id = req.chain_id
    nonce = items[0]
    gas_price = items[1]
    gas_limit = items[2]
    to = items[3]
    to = address_from_bytes(to)
    value = items[4]
    data = items[5]
    # _ = items[6]
    # _ = items[7]
    # _ = items[8]

    msg = EthereumSignTx(
        address_n=address_n,
        chain_id=chain_id,
        gas_price=gas_price,
        gas_limit=gas_limit,
        nonce=nonce,
        to=to,
        value=value,
        data_initial_chunk=data,
        data_length=len(data),
    )

    return await sign_tx(ctx, msg)


async def eth_sign_typed_data(ctx: "GenericContext", req: EthSignRequest):
    from trezor.messages import EthereumSignTypedData
    from apps.ethereum.sign_typed_data import sign_typed_data

    address_n = _keypath_to_address_n(req.derivation_path)
    primary_type = req.sign_data[0]
    data = req.sign_data[1:]

    msg = EthereumSignTypedData(
        address_n=address_n, primary_type=primary_type, data=data
    )

    return await sign_typed_data(ctx, msg)
    # eip-712
    pass


async def eth_sign_message(ctx: "GenericContext", req: EthSignRequest):
    from trezor.messages import EthereumSignMessage
    from apps.ethereum.sign_message import sign_message

    address_n = _keypath_to_address_n(req.derivation_path)
    data = req.sign_data

    msg = EthereumSignMessage(address_n=address_n, data=data)
    return await sign_message(ctx, msg)
    # eip-191
    pass


async def eth_sign_tx_eip1559(ctx: "GenericContext", req: EthSignRequest):
    from trezor.messages import EthereumSignTxEIP1559
    from trezor.crypto import rlp
    from apps.ethereum.sign_tx_eip1559 import sign_tx_eip1559
    from apps.ethereum.helpers import address_from_bytes

    # eip-2930
    # 0x01 || rlp([chainId, nonce, gasPrice, gasLimit, to, value, data, accessList, signatureYParity, signatureR, signatureS])

    # eip-1559
    # 0x02 || rlp([chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, destination, amount, data, access_list, signature_y_parity, signature_r, signature_s])
    data = req.sign_data
    if data[0] != 0x02:
        raise errors.DataError("Invalid sign data, unexpected data type")

    items = rlp.decode(data, 1)
    if len(items) < 9:
        raise errors.DataError("Invalid sign data")

    address_n = _keypath_to_address_n(req.derivation_path)
    chain_id = req.chain_id
    nonce = items[1]
    max_priority_fee_per_gas = items[2]
    max_fee_per_gas = items[3]
    gas_limit = items[4]
    to = items[5]
    to = address_from_bytes(to)
    value = items[6]
    data = items[7]
    access_list = items[8]
    # _ = items[9]
    # _ = items[10]
    # _ = items[11]

    msg = EthereumSignTxEIP1559(
        address_n=address_n,
        chain_id=chain_id,
        nonce=nonce,
        max_priority_fee=max_priority_fee_per_gas,
        max_gas_fee=max_fee_per_gas,
        gas_limit=gas_limit,
        data_length=len(data),
        data_initial_chunk=data,
        to=to,
        value=value,
        access_list=access_list,
    )
    return await sign_tx_eip1559(ctx, msg)
