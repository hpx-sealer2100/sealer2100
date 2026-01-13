# generated from networks.py.mako
# (by running `make templates` in `core`)
# do not edit manually!

# NOTE: using positional arguments saves 4400 bytes in flash size,
# returning tuples instead of classes saved 800 bytes

from typing import TYPE_CHECKING

from trezor.messages import EthereumNetworkInfo

from apps.common.paths import HARDENED

if TYPE_CHECKING:
    from typing import Iterator

    # Removing the necessity to construct object to save space
    # fmt: off
    NetworkInfoTuple = tuple[
        int,  # chain_id
        int,  # slip44
        str,  # symbol
        str,  # name
        str,  # icon
        int,  # primary_color
    ]
    # fmt: on

UNKNOWN_NETWORK = EthereumNetworkInfo(
    chain_id=0,
    slip44=0,
    symbol="UNKN",
    name="Unknown network",
)


def shortcut_by_chain_id(chain_id: int) -> str:
    n = by_chain_id(chain_id)
    return n.symbol if n is not None else "UNKN"


def all_slip44_ids_hardened() -> Iterator[int]:
    for n in _networks_iterator():
        yield n[1] | HARDENED


def by_chain_id(chain_id: int) -> EthereumNetworkInfo:
    for n in _networks_iterator():
        n_chain_id = n[0]
        if n_chain_id == chain_id:
            return EthereumNetworkInfo(
                chain_id=n[0],
                slip44=n[1],
                symbol=n[2],
                name=n[3],
                icon=n[4],
                primary_color=n[5],
            )
    return UNKNOWN_NETWORK


def by_slip44(slip44: int) -> EthereumNetworkInfo:
    for n in _networks_iterator():
        n_slip44 = n[1]
        if n_slip44 == slip44:
            return EthereumNetworkInfo(
                chain_id=n[0],
                slip44=n[1],
                symbol=n[2],
                name=n[3],
                icon=n[4],
                primary_color=n[5],
            )
    return UNKNOWN_NETWORK


# fmt: off
def _networks_iterator() -> Iterator[NetworkInfoTuple]:
    yield (
        1,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Ethereum",  # name
        "evm-eth.png",  # name
        0x637FFF,  # primary_color
    )
    yield (
        10,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Optimism",  # name
        "evm-oeth.png",  # name
        0xFF0420,  # primary_color
    )
    yield (
        56,  # chain_id
        60,  # slip44
        "BNB",  # symbol
        "BNB Chain",  # name
        "evm-bnb.png",  # name
        0xF0B90B,  # primary_color
    )
    yield (
        66,  # chain_id
        60,  # slip44
        "OKT",  # symbol
        "OKTC",  # name
        "evm-okt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        137,  # chain_id
        60,  # slip44
        "POL",  # symbol
        "Polygon",  # name
        "evm-pol.png",  # name
        0x8247E5,  # primary_color
    )
    yield (
        250,  # chain_id
        60,  # slip44
        "FTM",  # symbol
        "Fantom",  # name
        "evm-ftm.png",  # name
        0x1969FF,  # primary_color
    )
    yield (
        324,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "zkSync Era",  # name
        "evm-zksync-era.png",  # name
        0xFFFFFF,  # primary_color
    )
    yield (
        8453,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Base",  # name
        "evm-base.png",  # name
        0x0052FF,  # primary_color
    )
    yield (
        42161,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Arbitrum",  # name
        "evm-arb1.png",  # name
        0x28A0F0,  # primary_color
    )
    yield (
        43114,  # chain_id
        60,  # slip44
        "AVAX",  # symbol
        "AVAX-C",  # name
        "evm-avax.png",  # name
        0xE84142,  # primary_color
    )
