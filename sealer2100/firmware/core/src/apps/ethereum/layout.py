from typing import TYPE_CHECKING
from ubinascii import hexlify

from storage import device
from trezor import ui
from trezor import wire, log
from trezor.ui import i18n
from trezor.enums import (
    ButtonRequestType,
    EthereumDataType,
)
from trezor.messages import (
    EthereumFieldType,
    EthereumStructMember,
)
from trezor.strings import format_amount, format_amount_ceil
from trezor.ui.layouts import  (
    hold_confirm_action,
    confirm_blob,
    confirm_text,
    show_defi_lock,
)
from trezor.ui.layouts.ethereum import (
    confirm_address,
    confirm_output,
    confirm_sign_typed_hash,
    should_show_more,
    show_transaction_overview,
    confirm_transaction_detail,
    confirm_transaction_detail_eip1559,
    show_approve_overview,
    show_approve_detail,
    confirm_sign_eip712_message,
    show_contract_call,
)

from . import networks, tokens, erc20
from .helpers import (
    address_from_bytes,
    decode_typed_data,
    get_type_name,
)

if TYPE_CHECKING:
    from typing import Awaitable, Iterable
    from trezor.wire import Context

    pass


def require_confirm_tx(
    ctx: Context,
    to_bytes: bytes,
    value: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    is_nft: bool = False,
) -> Awaitable[None]:
    if to_bytes:
        to_str = address_from_bytes(to_bytes, networks.by_chain_id(chain_id))
    else:
        to_str = i18n.Text.new_contract
    return confirm_output(
        ctx,
        address=to_str,
        amount=format_ethereum_amount(value, token, chain_id, is_nft),
    )


def require_show_overview(
    ctx: Context,
    network: str,
    to_bytes: bytes,
    value: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    is_nft: bool = False,
) -> Awaitable[bool]:
    if to_bytes:
        to_str = address_from_bytes(to_bytes, networks.by_chain_id(chain_id))
    else:
        to_str = i18n.Text.new_contract
    return show_transaction_overview(
        ctx,
        amount=format_ethereum_amount(value, token, chain_id, is_nft),
        to=to_str,
        network=network,
    )

def require_show_contract_call(ctx: Context, myself: str, contract: str, chain_id: int, fee_price: int, gas_limit: int):
    fee_max = fee_price * gas_limit
    fee_max = format_ethereum_fee(fee_max, None, chain_id)
    return show_contract_call(ctx, myself, contract, chain_id, fee_max)


def require_confirm_fee(
    ctx: Context,
    spending: int,
    gas_price: int,
    gas_limit: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    from_address: str | None = None,
    to_address: str | None = None,
    contract_addr: str | None = None,
    token_id: int | None = None,
    evm_chain_id: int | None = None,
    raw_data: bytes | None = None,
) -> Awaitable[None]:
    fee_max = gas_price * gas_limit
    from trezor import log

    log.debug(__name__, "enter require_confirm_fee")
    return confirm_transaction_detail(
        ctx,
        format_ethereum_amount(
            spending, token, chain_id, is_nft=True if token_id else False
        ),
        format_ethereum_fee(gas_price, None, chain_id),
        format_ethereum_fee(fee_max, None, chain_id),
        from_address,
        to_address,
        (
            format_ethereum_amount(spending + fee_max, None, chain_id)
            if (token is None and contract_addr is None)
            else None
        ),
        contract_addr,
        token_id,
        evm_chain_id=evm_chain_id,
        raw_data=raw_data,
    )

async def require_confirm_eip1559_fee(
    ctx: Context,
    spending: int,
    max_priority_fee: int,
    max_gas_fee: int,
    gas_limit: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    from_address: str | None = None,
    to_address: str | None = None,
    contract_addr: str | None = None,
    token_id: int | None = None,
    evm_chain_id: int | None = None,
    raw_data: bytes | None = None,
) -> None:

    fee_max = max_gas_fee * gas_limit
    await confirm_transaction_detail_eip1559(
        ctx,
        format_ethereum_amount(
            spending, token, chain_id, is_nft=True if token_id else False
        ),
        format_ethereum_fee(max_priority_fee, None, chain_id),
        format_ethereum_fee(max_gas_fee, None, chain_id),
        format_ethereum_fee(fee_max, None, chain_id),
        from_address,
        to_address,
        (
            format_ethereum_amount(spending + fee_max, None, chain_id)
            if (token is None and contract_addr is None)
            else None
        ),
        contract_addr,
        token_id,
        evm_chain_id=evm_chain_id,
        raw_data=raw_data,
    )


def require_confirm_unknown_token(
    ctx: Context, address_bytes: bytes
) -> Awaitable[None]:
    contract_address_hex = "0x" + hexlify(address_bytes).decode()
    return confirm_address(
        ctx,
        i18n.Title.unknown_token,
        contract_address_hex,
        description=i18n.Text.contract,
    )


def require_confirm_data(ctx: Context, data: bytes, data_total: int) -> Awaitable[None]:
    from trezor.ui.layouts import confirm_data

    return confirm_data(
        ctx,
        title=i18n.Title.view_data,
        description=i18n.Text.bytes_.format(data_total),
        data=data,
        br_code=ButtonRequestType.SignTx,
    )


async def confirm_typed_data_final(ctx: Context) -> None:
    from trezor.ui.layouts import confirm_action
    await confirm_action(
        ctx,
        title=i18n.Title.typed_data.format(ctx.name),
        msg=i18n.Text.do_sign_712_typed_data,
        br_code=ButtonRequestType.SignTx,
    )


async def confirm_typed_hash_final(ctx: Context) -> None:
    from trezor.ui.layouts import confirm_action
    await confirm_action(
        ctx,
        title=i18n.Title.typed_hash.format(ctx.name),
        msg=i18n.Text.do_sign_typed_hash,
        br_code=ButtonRequestType.SignTx,
    )


async def confirm_typed_hash(ctx: Context, domain_hash, message_hash) -> None:
    await confirm_sign_typed_hash(ctx, domain_hash, message_hash)


def confirm_empty_typed_message(ctx: Context) -> Awaitable[None]:
    return confirm_text(
        ctx, title=i18n.Title.confirm_message, data="", description=i18n.Text.no_message
    )


async def confirm_domain(ctx: Context, domain: dict[str, bytes]) -> None:
    domain_name = (
        decode_typed_data(domain["name"], "string") if domain.get("name") else None
    )
    domain_version = (
        decode_typed_data(domain["version"], "string")
        if domain.get("version")
        else None
    )
    chain_id = (
        decode_typed_data(domain["chainId"], "uint256")
        if domain.get("chainId")
        else None
    )
    verifying_contract = (
        decode_typed_data(domain["verifyingContract"], "address")
        if domain.get("verifyingContract")
        else None
    )
    salt = decode_typed_data(domain["salt"], "bytes32") if domain.get("salt") else None
    from trezor.ui.layouts.ethereum import confirm_domain

    await confirm_domain(
        ctx,
        **{
            "name": domain_name,
            "version": domain_version,
            "chainId": chain_id,
            "verifyingContract": verifying_contract,
            "salt": salt,
        },
    )


async def should_show_domain(ctx: Context, name: bytes, version: bytes) -> bool:
    domain_name = decode_typed_data(name, "string")
    domain_version = decode_typed_data(version, "string")

    para = (
        "Name and version",
        domain_name,
        domain_version,
    )
    return await should_show_more(
        ctx,
        title="Confirm domain",
        param=para,
        button_str="Show full domain",
    )


async def should_show_struct(
    ctx: Context,
    description: str,
    data_members: list[EthereumStructMember],
    title: str = "Confirm struct",
    button_text: str | None = None,
) -> bool:
    para = (
        description,
        i18n.Text.contains_x_key.format(len(data_members)),
        ", ".join(field.name for field in data_members),
    )
    return await should_show_more(
        ctx,
        title=title,
        param=para,
        button_str=button_text,
    )


async def should_show_array(
    ctx: Context,
    parent_objects: Iterable[str],
    data_type: str,
    size: int,
) -> bool:
    para = (
         i18n.Text.array_of_x_type.format(size, data_type),
    )
    return await should_show_more(
        ctx,
        title=limit_str(".".join(parent_objects)),
        param=para,
        button_str= i18n.Button.view_full_array,
    )


async def confirm_typed_value(
    ctx: Context,
    name: str,
    value: bytes,
    parent_objects: list[str],
    field: EthereumFieldType,
    array_index: int | None = None,
) -> None:
    type_name = get_type_name(field)

    if array_index is not None:
        title = limit_str(".".join(parent_objects + [name]))
        description = f"[{array_index}] ({type_name}):"
    else:
        title = limit_str(".".join(parent_objects))
        description = f"{name} ({type_name}):"

    data = decode_typed_data(value, type_name)

    if field.data_type in (EthereumDataType.ADDRESS, EthereumDataType.BYTES):
        await confirm_blob(
            ctx,
            title,
            "",
            blob = data,
            description=description,
        )
    else:
        await confirm_text(
            ctx,
            title,
            data,
            description=description,
        )

def require_show_approve(ctx: Context, spender: str, value: int, token: tokens.EthereumTokenInfo | None) -> bool:
    if erc20.approve_is_unlimited(value, token):
        value = i18n.Text.unlimited
    else :
        value = format_ethereum_amount(value, token, token.chain_id)
    
    contract_addr = address_from_bytes(token.address)

    return show_approve_overview(ctx, spender, value, contract_addr)

def require_confirm_approve_detail(ctx: Context, spender: str, myself: str, gas_price: int, gas_limit: int, token: tokens.EthereumTokenInfo | None):
    max_fee = format_ethereum_fee(gas_price * gas_limit, None, token.chain_id)
    contract_addr = address_from_bytes(token.address)
    return show_approve_detail(ctx, spender, myself, max_fee, contract_addr)

def format_ethereum_amount(
    value: int,
    token: tokens.EthereumTokenInfo | None,
    chain_id: int,
    is_nft: bool = False,
) -> str:
    if is_nft:
        return f"{value} NFT"
    elif token:
        suffix = token.symbol
        decimals = token.decimals
    else:
        suffix = networks.shortcut_by_chain_id(chain_id)
        decimals = 18

    # Don't want to display wei values for tokens with small decimal numbers
    # if decimals > 9 and value < 10 ** (decimals - 9):
    #     suffix = "Wei " + suffix
    #     decimals = 0

    return f"{format_amount(value, decimals)} {suffix}"

def format_ethereum_fee(
    value: int,
    token: tokens.EthereumTokenInfo | None,
    chain_id: int,
):
    if token:
        suffix = token.symbol
        decimals = token.decimals
    else:
        suffix = networks.shortcut_by_chain_id(chain_id)
        decimals = 18

    return f"{format_amount_ceil(value, decimals)} {suffix}"

def limit_str(s: str, limit: int = 16) -> str:
    """Shortens string to show the last <limit> characters."""
    if len(s) <= limit + 2:
        return s

    return ".." + s[-limit:]

async def check_defi_lock(ctx):
    if device.get_defi_lock():
        await show_defi_lock(ctx)
        raise wire.DataError("Forbidden transaction type")
