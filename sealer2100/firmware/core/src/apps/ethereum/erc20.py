from micropython import const

from typing import TYPE_CHECKING

from . import tokens

if TYPE_CHECKING:
    from typing import Literal
    Erc20Call = Literal['transfer', 'approve']


ERC20_METHOD_ID_TRANSFER = const(b"\xa9\x05\x9c\xbb\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
ERC20_METHOD_ID_APPROVE = const(b"\x09\x5e\xa7\xb3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")

def approve_is_unlimited(value: int, token: tokens.EthereumTokenInfo | None) -> bool:
    decimals = token.decimals if token else 18
    all = 0xFFFFFFFFFFFFFFFF * (10 ** decimals) # HP defined
    all_ff = (1 << 256) - 1 # max uint256
    return value in (all, all_ff)
