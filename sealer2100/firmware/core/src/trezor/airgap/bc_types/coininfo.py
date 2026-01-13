"""
; Metadata for the type and use of a cryptocurrency
crypto-coininfo = {
    ? type: uint31 .default cointype-btc, ; values from [SLIP44] with high bit turned off
    ? network: int .default mainnet ; coin-specific identifier for testnet
}

type = 1
network = 2

cointype-btc = 0
cointype-eth = 0x3c

mainnet = 0;
testnet-btc = 1;

; from [ETH-TEST-NETWORKS]
testnet-eth-ropsten = 1;
testnet-eth-kovan = 2;
testnet-eth-rinkeby = 3;
testnet-eth-gorli = 4;
"""

from .__typing import *

COIN_TYPE_BTC = 0
COIN_TYPE_ETH = 0x3C

NETWORK_MAINNET = 0
NETWORK_TESTNET = 1

ETH_TESTNET_ROPSTEN = 1
ETH_TESTNET_KOVAN = 2
ETH_TESTNET_RINKEBY = 3
ETH_TESTNET_GORLI = 4

from apps.common.cbor import encode, decode, OrderedMap

class CoinInfo(OrderedMap):
    version: Literal[1, 2]

    def __init__(
        self,
        type: Optional[int] = None,
        network: Optional[int] = None,
        version: Literal[1, 2] = 1,
    ):
        super().__init__()
        self.type = type or COIN_TYPE_BTC
        self.network = network or NETWORK_MAINNET
        self.version = version

    # URType
    def tag(self) -> int:
        # 305 for version 1
        # 40305 for version 2
        tag = 305 if self.version == 1 else 40305
        return tag

    def type(self) -> str:
        # crypto-coininfo for version 1
        # coininfo for version 2
        type = "crypto-coininfo" if self.version == 1 else "coininfo"
        return type

    def build(self) -> None:
        # rebuild
        self._internal_list.clear()

        # 1: type
        self[1] = self.type

        # 2: network
        self[2] = self.network
    def cbor(self) -> bytes:
        self.build()
        return encode(self)

    @classmethod
    def from_cbor(cls, cbor: bytes):
        obj = decode(cbor)
        return cls.from_object(obj)

    @classmethod
    def from_object(cls, obj: dict):
        if not isinstance(obj, dict):
            raise TypeError('Unexpected type, expected dict')

        # 1: type
        type = obj.get(1)
        if not isinstance(type, int):
            raise TypeError('type must be int')

        # 2: network
        network = obj.get(2)
        if not isinstance(network, int):
            raise TypeError('network must be int')

        return cls(type, network)
