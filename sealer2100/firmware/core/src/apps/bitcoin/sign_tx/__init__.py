from typing import TYPE_CHECKING

from trezor import utils, wire, log
from trezor.enums import RequestType
from trezor.messages import TxRequest

from ..common import BITCOIN_NAMES
from ..keychain import with_keychain
from . import approvers, bitcoin, helpers, progress

if not utils.BITCOIN_ONLY:
    from . import bitcoinlike, decred, zcash_v4
    from apps.zcash.signer import Zcash

if TYPE_CHECKING:
    from typing import Protocol, Dict, List, Any

    from trezor.messages import (
        SignTx,
        TxAckInput,
        TxAckOutput,
        TxAckPrevMeta,
        TxAckPrevInput,
        TxAckPrevOutput,
        TxAckPrevExtraData,
    )

    from apps.common.coininfo import CoinInfo
    from apps.common.keychain import Keychain

    from ..authorization import CoinJoinAuthorization

    TxAckType = (
        TxAckInput
        | TxAckOutput
        | TxAckPrevMeta
        | TxAckPrevInput
        | TxAckPrevOutput
        | TxAckPrevExtraData
    )

    class SignerClass(Protocol):
        def __init__(  # pylint: disable=super-init-not-called
            self,
            tx: SignTx,
            keychain: Keychain,
            coin: CoinInfo,
            approver: approvers.Approver | None,
        ) -> None: ...

        async def signer(self) -> None: ...


@with_keychain
async def sign_tx(
    ctx: wire.Context,
    msg: SignTx,
    keychain: Keychain,
    coin: CoinInfo,
    authorization: CoinJoinAuthorization | None = None,
) -> TxRequest:
    approver: approvers.Approver | None = None
    if authorization:
        approver = approvers.CoinJoinApprover(msg, coin, authorization)

    if utils.BITCOIN_ONLY or coin.coin_name in BITCOIN_NAMES:
        signer_class: type[SignerClass] = bitcoin.Bitcoin
    else:
        if coin.decred:
            signer_class = decred.Decred
        elif coin.overwintered:
            if msg.version == 5:
                signer_class = Zcash
            else:
                signer_class = zcash_v4.ZcashV4
        else:
            signer_class = bitcoinlike.Bitcoinlike

    signer = signer_class(msg, keychain, coin, approver).signer()
    utils.disable_show_busy()
    cached: Dict[(RequestType, int), bytes] = {}
    res: TxAckType | bool | None = None
    ctx.icon_path = f"A:/res/{coin.icon}"
    progress.progress.icon = ctx.icon_path
    while True:
        req = signer.send(res)
        if isinstance(req, tuple):
            request_class, req = req
            assert TxRequest.is_type_of(req)
            if req.request_type == RequestType.TXFINISHED:
                from trezor.ui.layouts import confirm_final

                utils.enable_show_busy()
                await confirm_final(ctx, coin.coin_name)
                return req
            res = await get_cached_res(cached, req)
            if res:
                log.debug(__name__, "get cached .........")
                from trezor.wire import _wrap_protobuf_load
                res = _wrap_protobuf_load(res, request_class)
                continue
            if not res:
                log.debug(__name__, "not get cached .........")
                res = await ctx.call(req, request_class)

            # cache it
            cache_req(cached, req, res)
        elif isinstance(req, helpers.UiConfirm):
            res = await req.confirm_dialog(ctx)
            progress.progress.report_init()
        else:
            raise TypeError("Invalid signing instruction")


def cache_req(cached: Dict[RequestType, Dict[int, Any]], req: TxRequest, res: Any):
    if req.request_type is None:
        return

    # 有需要通知给外部的数据
    if req.serialized:
        if req.serialized.serialized_tx or req.serialized.signature:
            return

    if req.request_type not in (
        RequestType.TXINPUT,
        RequestType.TXOUTPUT,
        RequestType.TXORIGINPUT,
        RequestType.TXORIGOUTPUT,
    ):
        return
    key = (req.request_type, req.details.request_index)
    log.debug(__name__, f"cacheing type: {key}")
    from trezor.protobuf import dump_message_buffer
    cached[key] = dump_message_buffer(res)

async def get_cached_res(cached: Dict[RequestType, Dict[int, Any]], req: TxRequest) -> bytes|None:
    if req.request_type is None:
        return None

    # 有需要通知给外部的数据
    if req.serialized:
        if req.serialized.serialized_tx or req.serialized.signature:
            return None

    if req.request_type not in (
        RequestType.TXINPUT,
        RequestType.TXOUTPUT,
        RequestType.TXORIGINPUT,
        RequestType.TXORIGOUTPUT,
    ):
        return None

    key = (req.request_type, req.details.request_index)

    log.debug(__name__, f"getting cache type: {key}")
    from trezor import workflow, loop
    workflow.idle_timer.touch()
    await loop.sleep(1)
    return cached.get(key)
