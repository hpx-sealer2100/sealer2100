# generated from tokens.py.mako
# do not edit manually!
# flake8: noqa
# fmt: off
from typing import Iterator

class SPLToken:
    def __init__(self, symbol: str, mint: str, decimals: int):
        self.symbol = symbol
        self.mint = mint
        self.decimals = decimals


def get_spl_token(mint: str) -> SPLToken | None:
    for symbol, _mint, decimals in _spl_tokens_iterator():
        if mint == _mint:
            return SPLToken(symbol, _mint, decimals)
    return UNKNOWN_TOKEN


UNKNOWN_TOKEN = SPLToken('UNKN', '', 0)


def _spl_tokens_iterator() -> Iterator[SPLToken]:
% for token in supported_on("trezor2", solana):
    yield (${black_repr(token.symbol)}, ${black_repr(token.address)}, ${token.decimals})
% endfor
