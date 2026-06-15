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
    # buildin network
    for n in _networks_iterator():
        n_chain_id = n[0]
        if n_chain_id == chain_id:
            return EthereumNetworkInfo(
                chain_id=n[0],
                slip44=n[1],
                symbol=n[2],
                name=n[3],
                icon=n[4],
            )

    # try to load from fs
    from .helpers import get_network_from_fs
    network = get_network_from_fs(chain_id)
    if network is not UNKNOWN_NETWORK:
        return network

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
            )
    return UNKNOWN_NETWORK

def is_buildin(chain_id: int) -> bool:
    return chain_id in (x[0] for x in _networks_iterator())

# fmt: off
def _networks_iterator() -> Iterator[NetworkInfoTuple]:
    yield (
        1,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Ethereum",  # name
        "evm-eth.png",  # name
    )
    yield (
        2,  # chain_id
        60,  # slip44
        "EXP",  # symbol
        "Expanse Network",  # name
        "evm-exp.png",  # name
    )
    yield (
        7,  # chain_id
        60,  # slip44
        "TCH",  # symbol
        "ThaiChain",  # name
        "evm-tch.png",  # name
    )
    yield (
        8,  # chain_id
        60,  # slip44
        "UBQ",  # symbol
        "Ubiq",  # name
        "evm-ubq.png",  # name
    )
    yield (
        10,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Optimism",  # name
        "evm-oeth.png",  # name
    )
    yield (
        11,  # chain_id
        60,  # slip44
        "META",  # symbol
        "Metadium",  # name
        "evm-meta.png",  # name
    )
    yield (
        14,  # chain_id
        60,  # slip44
        "FLR",  # symbol
        "Flare",  # name
        "evm-flr.png",  # name
    )
    yield (
        15,  # chain_id
        60,  # slip44
        "DIODE",  # symbol
        "Diode Prenet",  # name
        "evm-diode.png",  # name
    )
    yield (
        17,  # chain_id
        60,  # slip44
        "TFI",  # symbol
        "ThaiChain 2.0 ThaiFi",  # name
        "evm-tfi.png",  # name
    )
    yield (
        19,  # chain_id
        60,  # slip44
        "SGB",  # symbol
        "Songbird Canary-Network",  # name
        "evm-sgb.png",  # name
    )
    yield (
        20,  # chain_id
        60,  # slip44
        "ELA",  # symbol
        "Elastos Smart Chain",  # name
        "evm-ela.png",  # name
    )
    yield (
        22,  # chain_id
        60,  # slip44
        "ELA",  # symbol
        "ELA-DID-Sidechain",  # name
        "evm-ela.png",  # name
    )
    yield (
        25,  # chain_id
        60,  # slip44
        "CRO",  # symbol
        "Cronos",  # name
        "evm-cro.png",  # name
    )
    yield (
        30,  # chain_id
        60,  # slip44
        "RBTC",  # symbol
        "Rootstock",  # name
        "evm-rbtc.png",  # name
    )
    yield (
        40,  # chain_id
        60,  # slip44
        "TLOS",  # symbol
        "Telos EVM",  # name
        "evm-tlos.png",  # name
    )
    yield (
        42,  # chain_id
        60,  # slip44
        "LYX",  # symbol
        "LUKSO",  # name
        "evm-lyx.png",  # name
    )
    yield (
        44,  # chain_id
        60,  # slip44
        "CRAB",  # symbol
        "Crab Network",  # name
        "evm-crab.png",  # name
    )
    yield (
        46,  # chain_id
        60,  # slip44
        "RING",  # symbol
        "Darwinia Network",  # name
        "evm-ring.png",  # name
    )
    yield (
        50,  # chain_id
        60,  # slip44
        "XDC",  # symbol
        "XDC Network",  # name
        "evm-xdc.png",  # name
    )
    yield (
        52,  # chain_id
        60,  # slip44
        "cet",  # symbol
        "CoinEx Smart Chain",  # name
        "evm-cet.png",  # name
    )
    yield (
        56,  # chain_id
        60,  # slip44
        "BNB",  # symbol
        "BNB Chain",  # name
        "evm-bnb.png",  # name
    )
    yield (
        58,  # chain_id
        60,  # slip44
        "ONG",  # symbol
        "Ontology",  # name
        "evm-ong.png",  # name
    )
    yield (
        60,  # chain_id
        60,  # slip44
        "GO",  # symbol
        "GoChain",  # name
        "evm-go.png",  # name
    )
    yield (
        61,  # chain_id
        60,  # slip44
        "ETC",  # symbol
        "Ethereum Classic",  # name
        "evm-etc.png",  # name
    )
    yield (
        66,  # chain_id
        60,  # slip44
        "OKT",  # symbol
        "OKTC",  # name
        "evm-okt.png",  # name
    )
    yield (
        81,  # chain_id
        60,  # slip44
        "JOC",  # symbol
        "Japan Open Chain",  # name
        "evm-joc.png",  # name
    )
    yield (
        82,  # chain_id
        60,  # slip44
        "MTR",  # symbol
        "Meter",  # name
        "evm-mtr.png",  # name
    )
    yield (
        88,  # chain_id
        60,  # slip44
        "VIC",  # symbol
        "Viction",  # name
        "evm-vic.png",  # name
    )
    yield (
        96,  # chain_id
        60,  # slip44
        "KUB",  # symbol
        "KUB",  # name
        "evm-kub.png",  # name
    )
    yield (
        100,  # chain_id
        60,  # slip44
        "XDAI",  # symbol
        "Gnosis",  # name
        "evm-xdai.png",  # name
    )
    yield (
        106,  # chain_id
        60,  # slip44
        "VLX",  # symbol
        "Velas EVM",  # name
        "evm-vlx.png",  # name
    )
    yield (
        108,  # chain_id
        60,  # slip44
        "TT",  # symbol
        "ThunderCore",  # name
        "evm-tt.png",  # name
    )
    yield (
        119,  # chain_id
        60,  # slip44
        "NULS",  # symbol
        "ENULS",  # name
        "evm-nuls.png",  # name
    )
    yield (
        122,  # chain_id
        60,  # slip44
        "FUSE",  # symbol
        "Fuse",  # name
        "evm-fuse.png",  # name
    )
    yield (
        128,  # chain_id
        60,  # slip44
        "HT",  # symbol
        "Huobi ECO Chain",  # name
        "evm-ht.png",  # name
    )
    yield (
        130,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Unichain",  # name
        "evm-eth.png",  # name
    )
    yield (
        137,  # chain_id
        60,  # slip44
        "POL",  # symbol
        "Polygon",  # name
        "evm-pol.png",  # name
    )
    yield (
        143,  # chain_id
        60,  # slip44
        "MON",  # symbol
        "Monad",  # name
        "evm-mon.png",  # name
    )
    yield (
        146,  # chain_id
        60,  # slip44
        "S",  # symbol
        "Sonic",  # name
        "evm-s.png",  # name
    )
    yield (
        169,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Manta Pacific",  # name
        "evm-eth.png",  # name
    )
    yield (
        173,  # chain_id
        60,  # slip44
        "EGAS",  # symbol
        "ENI",  # name
        "evm-egas.png",  # name
    )
    yield (
        185,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Mint",  # name
        "evm-eth.png",  # name
    )
    yield (
        196,  # chain_id
        60,  # slip44
        "OKB",  # symbol
        "X Layer",  # name
        "evm-okb.png",  # name
    )
    yield (
        199,  # chain_id
        60,  # slip44
        "BTT",  # symbol
        "BitTorrent Chain",  # name
        "evm-btt.png",  # name
    )
    yield (
        204,  # chain_id
        60,  # slip44
        "BNB",  # symbol
        "opBNB",  # name
        "evm-bnb.png",  # name
    )
    yield (
        207,  # chain_id
        60,  # slip44
        "VC",  # symbol
        "VinuChain Network",  # name
        "evm-vc.png",  # name
    )
    yield (
        225,  # chain_id
        60,  # slip44
        "LA",  # symbol
        "LACHAIN",  # name
        "evm-la.png",  # name
    )
    yield (
        232,  # chain_id
        60,  # slip44
        "GHO",  # symbol
        "Lens",  # name
        "evm-gho.png",  # name
    )
    yield (
        239,  # chain_id
        60,  # slip44
        "TAC",  # symbol
        "TAC",  # name
        "evm-tac.png",  # name
    )
    yield (
        246,  # chain_id
        60,  # slip44
        "EWT",  # symbol
        "Energy Web Chain",  # name
        "evm-ewt.png",  # name
    )
    yield (
        248,  # chain_id
        60,  # slip44
        "OAS",  # symbol
        "Oasys",  # name
        "evm-oas.png",  # name
    )
    yield (
        250,  # chain_id
        60,  # slip44
        "FTM",  # symbol
        "Fantom",  # name
        "evm-ftm.png",  # name
    )
    yield (
        252,  # chain_id
        60,  # slip44
        "FRAX",  # symbol
        "Fraxtal",  # name
        "evm-frax.png",  # name
    )
    yield (
        254,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Swan Chain",  # name
        "evm-eth.png",  # name
    )
    yield (
        288,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Boba Network",  # name
        "evm-boba.png",  # name
    )
    yield (
        295,  # chain_id
        60,  # slip44
        "HBAR",  # symbol
        "Hedera",  # name
        "evm-hbar.png",  # name
    )
    yield (
        311,  # chain_id
        60,  # slip44
        "OMAX",  # symbol
        "Omax",  # name
        "evm-omax.png",  # name
    )
    yield (
        314,  # chain_id
        60,  # slip44
        "FIL",  # symbol
        "Filecoin",  # name
        "evm-filecoin.png",  # name
    )
    yield (
        321,  # chain_id
        60,  # slip44
        "KCS",  # symbol
        "KCC",  # name
        "evm-kcs.png",  # name
    )
    yield (
        324,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "zkSync Era",  # name
        "evm-zksync-era.png",  # name
    )
    yield (
        336,  # chain_id
        60,  # slip44
        "SDN",  # symbol
        "Shiden",  # name
        "evm-sdn.png",  # name
    )
    yield (
        360,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Shape",  # name
        "evm-eth.png",  # name
    )
    yield (
        361,  # chain_id
        60,  # slip44
        "TFUEL",  # symbol
        "Theta",  # name
        "evm-tfuel.png",  # name
    )
    yield (
        369,  # chain_id
        60,  # slip44
        "PLS",  # symbol
        "PulseChain",  # name
        "evm-pls.png",  # name
    )
    yield (
        388,  # chain_id
        60,  # slip44
        "zkCRO",  # symbol
        "Cronos zkEVM",  # name
        "evm-zkcro.png",  # name
    )
    yield (
        478,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Form Network",  # name
        "evm-eth.png",  # name
    )
    yield (
        570,  # chain_id
        60,  # slip44
        "SYS",  # symbol
        "Rollux",  # name
        "evm-sys.png",  # name
    )
    yield (
        592,  # chain_id
        60,  # slip44
        "ASTR",  # symbol
        "Astar",  # name
        "evm-astr.png",  # name
    )
    yield (
        648,  # chain_id
        60,  # slip44
        "ACE",  # symbol
        "Endurance Smart Chain",  # name
        "evm-ace.png",  # name
    )
    yield (
        690,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Redstone",  # name
        "evm-eth.png",  # name
    )
    yield (
        698,  # chain_id
        60,  # slip44
        "BNB",  # symbol
        "Matchain",  # name
        "evm-bnb.png",  # name
    )
    yield (
        747,  # chain_id
        60,  # slip44
        "FLOW",  # symbol
        "Flow EVM",  # name
        "evm-flow.png",  # name
    )
    yield (
        820,  # chain_id
        60,  # slip44
        "CLO",  # symbol
        "Callisto",  # name
        "evm-clo.png",  # name
    )
    yield (
        888,  # chain_id
        60,  # slip44
        "WAN",  # symbol
        "Wanchain",  # name
        "evm-wan.png",  # name
    )
    yield (
        996,  # chain_id
        60,  # slip44
        "WETH",  # symbol
        "Bifrost Polkadot",  # name
        "evm-weth.png",  # name
    )
    yield (
        1030,  # chain_id
        60,  # slip44
        "CFX",  # symbol
        "Conflux eSpace",  # name
        "evm-cfx.png",  # name
    )
    yield (
        1088,  # chain_id
        60,  # slip44
        "METIS",  # symbol
        "Metis Andromeda",  # name
        "evm-metis.png",  # name
    )
    yield (
        1100,  # chain_id
        60,  # slip44
        "DYM",  # symbol
        "Dymension",  # name
        "evm-dym.png",  # name
    )
    yield (
        1101,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Polygon zkEVM",  # name
        "evm-eth.png",  # name
    )
    yield (
        1116,  # chain_id
        60,  # slip44
        "CORE",  # symbol
        "Core Blockchain",  # name
        "evm-core.png",  # name
    )
    yield (
        1135,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Lisk",  # name
        "evm-eth.png",  # name
    )
    yield (
        1231,  # chain_id
        60,  # slip44
        "ULX",  # symbol
        "Ultron",  # name
        "evm-ulx.png",  # name
    )
    yield (
        1234,  # chain_id
        60,  # slip44
        "FITFI",  # symbol
        "Step Network",  # name
        "evm-fitfi.png",  # name
    )
    yield (
        1284,  # chain_id
        60,  # slip44
        "GLMR",  # symbol
        "Moonbeam",  # name
        "evm-glmr.png",  # name
    )
    yield (
        1285,  # chain_id
        60,  # slip44
        "MOVR",  # symbol
        "Moonriver",  # name
        "evm-movr.png",  # name
    )
    yield (
        1329,  # chain_id
        60,  # slip44
        "SEI",  # symbol
        "Sei Network",  # name
        "evm-sei.png",  # name
    )
    yield (
        1625,  # chain_id
        60,  # slip44
        "G",  # symbol
        "Gravity Alpha",  # name
        "evm-g.png",  # name
    )
    yield (
        1729,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Reya Network",  # name
        "evm-eth.png",  # name
    )
    yield (
        1868,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Soneium",  # name
        "evm-eth.png",  # name
    )
    yield (
        1890,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Lightlink Phoenix",  # name
        "evm-eth.png",  # name
    )
    yield (
        1907,  # chain_id
        60,  # slip44
        "BITCI",  # symbol
        "Bitcichain",  # name
        "evm-bitci.png",  # name
    )
    yield (
        1923,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Swellchain",  # name
        "evm-eth.png",  # name
    )
    yield (
        1975,  # chain_id
        60,  # slip44
        "ONUS",  # symbol
        "ONUS Chain",  # name
        "evm-onus.png",  # name
    )
    yield (
        1996,  # chain_id
        60,  # slip44
        "DMT",  # symbol
        "Sanko",  # name
        "evm-dmt.png",  # name
    )
    yield (
        2000,  # chain_id
        60,  # slip44
        "DOGE",  # symbol
        "Dogechain",  # name
        "evm-doge.png",  # name
    )
    yield (
        2001,  # chain_id
        60,  # slip44
        "mADA",  # symbol
        "Milkomeda C1",  # name
        "evm-mada.png",  # name
    )
    yield (
        2020,  # chain_id
        60,  # slip44
        "RON",  # symbol
        "Ronin",  # name
        "evm-ron.png",  # name
    )
    yield (
        2152,  # chain_id
        60,  # slip44
        "FRA",  # symbol
        "Findora",  # name
        "evm-fra.png",  # name
    )
    yield (
        2222,  # chain_id
        60,  # slip44
        "KAVA",  # symbol
        "Kava",  # name
        "evm-kava.png",  # name
    )
    yield (
        2345,  # chain_id
        60,  # slip44
        "BTC",  # symbol
        "GOAT Network",  # name
        "evm-btc.png",  # name
    )
    yield (
        2355,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Silicon zkEVM",  # name
        "evm-eth.png",  # name
    )
    yield (
        2525,  # chain_id
        60,  # slip44
        "INJ",  # symbol
        "inEVM",  # name
        "evm-inj.png",  # name
    )
    yield (
        2649,  # chain_id
        60,  # slip44
        "BTC",  # symbol
        "AILayer",  # name
        "evm-btc.png",  # name
    )
    yield (
        2741,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Abstract",  # name
        "evm-eth.png",  # name
    )
    yield (
        2818,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Morph",  # name
        "evm-eth.png",  # name
    )
    yield (
        3338,  # chain_id
        60,  # slip44
        "PEAQ",  # symbol
        "peaq",  # name
        "evm-peaq.png",  # name
    )
    yield (
        3637,  # chain_id
        60,  # slip44
        "BTC",  # symbol
        "Botanix",  # name
        "evm-btc.png",  # name
    )
    yield (
        3721,  # chain_id
        60,  # slip44
        "XOC",  # symbol
        "Xone",  # name
        "evm-xoc.png",  # name
    )
    yield (
        3776,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Astar zkEVM",  # name
        "evm-eth.png",  # name
    )
    yield (
        4158,  # chain_id
        60,  # slip44
        "XFI",  # symbol
        "CrossFi",  # name
        "evm-xfi.png",  # name
    )
    yield (
        4337,  # chain_id
        60,  # slip44
        "BEAM",  # symbol
        "Beam",  # name
        "evm-beam.png",  # name
    )
    yield (
        4488,  # chain_id
        60,  # slip44
        "HYDRA",  # symbol
        "Hydra Chain",  # name
        "evm-hydra.png",  # name
    )
    yield (
        4689,  # chain_id
        60,  # slip44
        "IOTX",  # symbol
        "IoTeX Network",  # name
        "evm-iotx.png",  # name
    )
    yield (
        5000,  # chain_id
        60,  # slip44
        "MNT",  # symbol
        "Mantle",  # name
        "evm-mnt.png",  # name
    )
    yield (
        5112,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Ham",  # name
        "evm-eth.png",  # name
    )
    yield (
        5330,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Superseed",  # name
        "evm-eth.png",  # name
    )
    yield (
        5464,  # chain_id
        60,  # slip44
        "GAS",  # symbol
        "Saga",  # name
        "evm-gas.png",  # name
    )
    yield (
        5551,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Nahmii 2",  # name
        "evm-eth.png",  # name
    )
    yield (
        6001,  # chain_id
        60,  # slip44
        "BB",  # symbol
        "BounceBit",  # name
        "evm-bb.png",  # name
    )
    yield (
        6880,  # chain_id
        60,  # slip44
        "MTT",  # symbol
        "MTT Network",  # name
        "evm-mtt.png",  # name
    )
    yield (
        6900,  # chain_id
        60,  # slip44
        "NIBI",  # symbol
        "Nibiru cataclysm-1",  # name
        "evm-nibi.png",  # name
    )
    yield (
        6969,  # chain_id
        60,  # slip44
        "TOMB",  # symbol
        "Tomb Chain",  # name
        "evm-tomb.png",  # name
    )
    yield (
        7000,  # chain_id
        60,  # slip44
        "ZETA",  # symbol
        "ZetaChain",  # name
        "evm-zeta.png",  # name
    )
    yield (
        7070,  # chain_id
        60,  # slip44
        "PLQ",  # symbol
        "Planq",  # name
        "evm-plq.png",  # name
    )
    yield (
        7700,  # chain_id
        60,  # slip44
        "CANTO",  # symbol
        "Canto",  # name
        "evm-canto.png",  # name
    )
    yield (
        8008,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Polynomial",  # name
        "evm-eth.png",  # name
    )
    yield (
        8217,  # chain_id
        60,  # slip44
        "KAIA",  # symbol
        "Kaia",  # name
        "evm-kaia.png",  # name
    )
    yield (
        8453,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Base",  # name
        "evm-base.png",  # name
    )
    yield (
        8668,  # chain_id
        60,  # slip44
        "HLUSD",  # symbol
        "Hela Official Runtime",  # name
        "evm-hlusd.png",  # name
    )
    yield (
        8811,  # chain_id
        60,  # slip44
        "H1",  # symbol
        "Haven1",  # name
        "evm-h1.png",  # name
    )
    yield (
        8899,  # chain_id
        60,  # slip44
        "JBC",  # symbol
        "JIBCHAIN L1",  # name
        "evm-jbc.png",  # name
    )
    yield (
        9001,  # chain_id
        60,  # slip44
        "EVMOS",  # symbol
        "Evmos",  # name
        "evm-evmos.png",  # name
    )
    yield (
        9745,  # chain_id
        60,  # slip44
        "XPL",  # symbol
        "Plasma",  # name
        "evm-xpl.png",  # name
    )
    yield (
        9790,  # chain_id
        60,  # slip44
        "SWTH",  # symbol
        "Carbon EVM",  # name
        "evm-swth.png",  # name
    )
    yield (
        10000,  # chain_id
        60,  # slip44
        "BCH",  # symbol
        "Smart Bitcoin Cash",  # name
        "evm-bch.png",  # name
    )
    yield (
        13371,  # chain_id
        60,  # slip44
        "IMX",  # symbol
        "Immutable zkEVM",  # name
        "evm-imx.png",  # name
    )
    yield (
        15551,  # chain_id
        60,  # slip44
        "LOOP",  # symbol
        "LoopNetwork",  # name
        "evm-loop.png",  # name
    )
    yield (
        16507,  # chain_id
        60,  # slip44
        "GSYS",  # symbol
        "Genesys",  # name
        "evm-gsys.png",  # name
    )
    yield (
        17777,  # chain_id
        60,  # slip44
        "EOS",  # symbol
        "EOS EVM Network",  # name
        "evm-eos.png",  # name
    )
    yield (
        18686,  # chain_id
        60,  # slip44
        "MXC",  # symbol
        "MXC zkEVM Moonchain",  # name
        "evm-mxc.png",  # name
    )
    yield (
        18888,  # chain_id
        60,  # slip44
        "TKX",  # symbol
        "Titan (TKX)",  # name
        "evm-tkx.png",  # name
    )
    yield (
        23294,  # chain_id
        60,  # slip44
        "ROSE",  # symbol
        "Oasis Sapphire",  # name
        "evm-rose.png",  # name
    )
    yield (
        31612,  # chain_id
        60,  # slip44
        "BTC",  # symbol
        "Mezo",  # name
        "evm-btc.png",  # name
    )
    yield (
        32520,  # chain_id
        60,  # slip44
        "Brise",  # symbol
        "Bitgert",  # name
        "evm-brise.png",  # name
    )
    yield (
        32769,  # chain_id
        60,  # slip44
        "ZIL",  # symbol
        "Zilliqa EVM",  # name
        "evm-zil.png",  # name
    )
    yield (
        33139,  # chain_id
        60,  # slip44
        "APE",  # symbol
        "ApeChain",  # name
        "evm-ape.png",  # name
    )
    yield (
        34443,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Mode",  # name
        "evm-eth.png",  # name
    )
    yield (
        39797,  # chain_id
        60,  # slip44
        "NRG",  # symbol
        "Energi",  # name
        "evm-nrg.png",  # name
    )
    yield (
        41923,  # chain_id
        60,  # slip44
        "EDU",  # symbol
        "EDU Chain",  # name
        "evm-edu.png",  # name
    )
    yield (
        42161,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Arbitrum",  # name
        "evm-arb1.png",  # name
    )
    yield (
        42170,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Arbitrum Nova",  # name
        "evm-eth.png",  # name
    )
    yield (
        42220,  # chain_id
        60,  # slip44
        "CELO",  # symbol
        "Celo",  # name
        "evm-celo.png",  # name
    )
    yield (
        42262,  # chain_id
        60,  # slip44
        "ROSE",  # symbol
        "Oasis Emerald",  # name
        "evm-rose.png",  # name
    )
    yield (
        42766,  # chain_id
        60,  # slip44
        "USDC",  # symbol
        "ZKFair",  # name
        "evm-usdc.png",  # name
    )
    yield (
        42793,  # chain_id
        60,  # slip44
        "XTZ",  # symbol
        "Etherlink",  # name
        "evm-xtz.png",  # name
    )
    yield (
        43111,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Hemi",  # name
        "evm-eth.png",  # name
    )
    yield (
        43114,  # chain_id
        60,  # slip44
        "AVAX",  # symbol
        "AVAX-C",  # name
        "evm-avax.png",  # name
    )
    yield (
        47805,  # chain_id
        60,  # slip44
        "REI",  # symbol
        "REI Network",  # name
        "evm-rei.png",  # name
    )
    yield (
        48900,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Zircuit",  # name
        "evm-eth.png",  # name
    )
    yield (
        50104,  # chain_id
        60,  # slip44
        "SOPH",  # symbol
        "Sophon",  # name
        "evm-soph.png",  # name
    )
    yield (
        53935,  # chain_id
        60,  # slip44
        "JEWEL",  # symbol
        "DFK Chain",  # name
        "evm-jewel.png",  # name
    )
    yield (
        55244,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Superposition",  # name
        "evm-eth.png",  # name
    )
    yield (
        55555,  # chain_id
        60,  # slip44
        "REI",  # symbol
        "REI Chain",  # name
        "evm-rei.png",  # name
    )
    yield (
        57073,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Ink",  # name
        "evm-eth.png",  # name
    )
    yield (
        59144,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Linea",  # name
        "evm-linea.png",  # name
    )
    yield (
        60808,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "BOB",  # name
        "evm-eth.png",  # name
    )
    yield (
        71402,  # chain_id
        60,  # slip44
        "pCKB",  # symbol
        "Godwoken",  # name
        "evm-pckb.png",  # name
    )
    yield (
        80094,  # chain_id
        60,  # slip44
        "BERA",  # symbol
        "Berachain",  # name
        "evm-bera.png",  # name
    )
    yield (
        81457,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Blast",  # name
        "evm-eth.png",  # name
    )
    yield (
        88888,  # chain_id
        60,  # slip44
        "CHZ",  # symbol
        "Chiliz Chain",  # name
        "evm-chz.png",  # name
    )
    yield (
        105105,  # chain_id
        60,  # slip44
        "STRAX",  # symbol
        "Stratis",  # name
        "evm-strax.png",  # name
    )
    yield (
        153153,  # chain_id
        60,  # slip44
        "DIONE",  # symbol
        "Odyssey Chain",  # name
        "evm-dione.png",  # name
    )
    yield (
        167000,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Taiko Alethia",  # name
        "evm-eth.png",  # name
    )
    yield (
        200901,  # chain_id
        60,  # slip44
        "BTC",  # symbol
        "Bitlayer",  # name
        "evm-btc.png",  # name
    )
    yield (
        222222,  # chain_id
        60,  # slip44
        "WETH",  # symbol
        "Hydration",  # name
        "evm-weth.png",  # name
    )
    yield (
        322202,  # chain_id
        60,  # slip44
        "PRX",  # symbol
        "Parex",  # name
        "evm-prx.png",  # name
    )
    yield (
        432204,  # chain_id
        60,  # slip44
        "ALOT",  # symbol
        "Dexalot Subnet",  # name
        "evm-alot.png",  # name
    )
    yield (
        534352,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Scroll",  # name
        "evm-eth.png",  # name
    )
    yield (
        747474,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "katana",  # name
        "evm-eth.png",  # name
    )
    yield (
        777777,  # chain_id
        60,  # slip44
        "WINR",  # symbol
        "Winr Protocol",  # name
        "evm-winr.png",  # name
    )
    yield (
        888888,  # chain_id
        60,  # slip44
        "VS",  # symbol
        "Vision",  # name
        "evm-vs.png",  # name
    )
    yield (
        7225878,  # chain_id
        60,  # slip44
        "OAS",  # symbol
        "Saakuru",  # name
        "evm-oas.png",  # name
    )
    yield (
        7777777,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Zora",  # name
        "evm-eth.png",  # name
    )
    yield (
        9999999,  # chain_id
        60,  # slip44
        "FLT",  # symbol
        "Fluence",  # name
        "evm-flt.png",  # name
    )
    yield (
        20250217,  # chain_id
        60,  # slip44
        "XP",  # symbol
        "Xphere",  # name
        "evm-xp.png",  # name
    )
    yield (
        21000000,  # chain_id
        60,  # slip44
        "BTCN",  # symbol
        "Corn",  # name
        "evm-btcn.png",  # name
    )
    yield (
        245022934,  # chain_id
        60,  # slip44
        "NEON",  # symbol
        "Neon EVM",  # name
        "evm-neon.png",  # name
    )
    yield (
        420420419,  # chain_id
        60,  # slip44
        "DOT",  # symbol
        "Asset Hub",  # name
        "evm-dot.png",  # name
    )
    yield (
        666666666,  # chain_id
        60,  # slip44
        "DEGEN",  # symbol
        "Degen Chain",  # name
        "evm-degen.png",  # name
    )
    yield (
        888888888,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Ancient8",  # name
        "evm-eth.png",  # name
    )
    yield (
        1313161554,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Aurora",  # name
        "evm-aurora.png",  # name
    )
    yield (
        1380012617,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "RARI Chain",  # name
        "evm-eth.png",  # name
    )
    yield (
        1666600000,  # chain_id
        60,  # slip44
        "ONE",  # symbol
        "Harmony",  # name
        "evm-one.png",  # name
    )
    yield (
        11297108109,  # chain_id
        60,  # slip44
        "PALM",  # symbol
        "Palm",  # name
        "evm-palm.png",  # name
    )
    yield (
        2716446429837000,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "DCHAIN",  # name
        "evm-eth.png",  # name
    )
