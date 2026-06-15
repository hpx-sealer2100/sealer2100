# generated from tokens.py.mako
# (by running `make templates` in `core`)
# do not edit manually!
# fmt: off

# NOTE: returning a tuple instead of `TokenInfo` from the "data" function
# saves 5600 bytes of flash size. Implementing the `_token_iterator`
# instead of if-tree approach saves another 5600 bytes.

# NOTE: interestingly, it did not save much flash size to use smaller
# parts of the address, for example address length of 10 bytes saves
# 1 byte per entry, so 1887 bytes overall (and further decrease does not help).
# (The idea was not having to store the whole address, even a smaller part
# of it has enough collision-resistance.)
# (In the if-tree approach the address length did not have any effect whatsoever.)

from typing import Iterator

from trezor.messages import EthereumTokenInfo

UNKNOWN_TOKEN = EthereumTokenInfo(
    symbol="Wei UNKN",
    decimals=0,
    address=b"",
    chain_id=0,
    name="Unknown token",
)


def token_by_chain_address(chain_id: int, address: bytes) -> EthereumTokenInfo | None:
    # buildin token
    for addr, symbol, decimal, name in _token_iterator(chain_id):
        if address == addr:
            return EthereumTokenInfo(
                symbol=symbol,
                decimals=decimal,
                address=address,
                chain_id=chain_id,
                name=name,
            )

    # try to load from fs
    from .helpers import get_token_from_fs
    token = get_token_from_fs(chain_id, address)
    if token is not UNKNOWN_TOKEN:
        return token

    return UNKNOWN_TOKEN

def is_buildin(chain_id: int, address: bytes) -> bool:
    for addr, _, _, _ in _token_iterator(chain_id):
        if address == addr:
            return True
    return False

def _token_iterator(chain_id: int) -> Iterator[tuple[bytes, str, int, str]]:
    if chain_id == 1:  # eth
        yield (  # address, symbol, decimals, name
            b"\x5a\x3e\x6a\x77\xba\x2f\x98\x3e\xc0\xd3\x71\xea\x3b\x47\x5f\x8b\xc0\x81\x1a\xd5",
            "0x0",
            9,
            "0x0.ai: AI Smart Contract Audito",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\x11\x11\x11\x11\x17\xdc\x0a\xa7\x8b\x77\x0f\xa6\xa7\x38\x03\x41\x20\xc3\x02",
            "1INCH",
            18,
            "1INCH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\xc6\x65\x00\xc8\x4a\x76\xad\x7e\x9c\x93\x43\x7b\xfc\x5a\xc3\x3e\x2d\xda\xe9",
            "AAVE",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\x8d\x4c\x97\x42\x5d\x99\x08\xe6\x6e\x53\xa6\xfd\xf6\x73\xac\xca\x0b\xe9\x86",
            "ABT",
            18,
            "ABT",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x04\x91\x5c\x23\xf0\x0a\x31\x3a\x54\x49\x55\x52\x4e\xb7\xdb\xd8\x23\x14\x3d",
            "ACH",
            8,
            "Alchemy",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\x10\x8f\x02\x23\xa3\xc3\x02\x8f\x5f\xe7\xae\xc7\xf9\xbb\x2e\x66\xbe\xf8\x2f",
            "ACX",
            18,
            "Across Protocol Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x91\xaf\x0f\xbb\x28\xab\xa7\xe3\x14\x03\xcb\x45\x71\x06\xce\x79\x39\x7f\xd4\xe6",
            "AERGO",
            18,
            "Aergo",
        )
        yield (  # address, symbol, decimals, name
            b"\xa4\x06\x40\x45\x8f\xbc\x27\xb6\xee\xfe\xde\xa1\xe9\xc9\xe1\x7d\x4c\xee\x7a\x21",
            "AEUR",
            18,
            "Anchored Coins EUR",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\x28\xed\xbe\xf0\x13\xaf\xf8\x55\xac\x3c\x50\xb3\x81\xf2\x53\xaf\x13\xb9\x97",
            "AEVO",
            18,
            "Aevo",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\xb4\xd3\x4e\x87\x54\x60\x09\x62\xcd\x94\x4b\x53\x51\x80\xbd\x75\x8e\x6c\x2e",
            "agETH",
            18,
            "Kelp Gain",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\xa2\x64\x10\x00\xcb\xb4\x07\xc3\x29\x31\x0c\x46\x1b\x2c\xb9\xc7\x0c\x30\x46",
            "AGI",
            18,
            "AGI Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5b\x75\x33\x81\x27\x59\xb4\x5c\x2b\x44\xc1\x9e\x32\x0b\xa2\xcd\x26\x81\xb5\x42",
            "AGIX",
            8,
            "SingularityNET Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x62\x6e\x80\x36\xde\xb3\x33\xb4\x08\xbe\x46\x8f\x95\x1b\xdb\x42\x43\x3c\xbf\x18",
            "AIOZ",
            18,
            "AIOZ Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x37\xaf\x9d\x8a\xe7\x4f\x35\xf3\xa3\x8b\xd2\xa0\x8f\xcb\x29\x89\x0c\xa6\xd2",
            "AIXBT",
            18,
            "AIXBT",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x6b\xad\x07\x65\x8f\x3b\x6c\xad\x9a\x39\x6c\xfc\xbc\x12\x43\xaf\x45\x2e\xc1",
            "AL",
            18,
            "Archloot",
        )
        yield (  # address, symbol, decimals, name
            b"\x84\x57\xca\x50\x40\xad\x67\xfd\xeb\xbc\xc8\xed\xce\x88\x9a\x33\x5b\xc0\xfb\xfb",
            "ALT",
            18,
            "AltLayer Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\x6d\xa0\xfe\x9a\xd5\xf3\xb0\xd5\x81\x60\x28\x89\x17\xaa\x56\x65\x36\x60\xe9",
            "alUSD",
            18,
            "Alchemix USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\x20\x81\x77\x65\xcb\x7f\x73\xd4\xbd\xe2\xe6\x6e\x06\x7e\x58\xd1\x10\x95\xc2",
            "AMP",
            18,
            "Amp",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\xc2\x6f\xc5\x85\x4e\x76\x48\xa0\x64\xa4\xab\xd5\x90\xbb\xe7\x17\x24\xc2\x77",
            "ANIME",
            18,
            "Animecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\x90\x33\x3c\xef\x9e\x6d\x52\x8d\xd5\x61\x8f\xb9\x7a\x76\xf2\x68\xf3\xed\xd4",
            "ANKR",
            18,
            "Ankr Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xbb\xf4\x50\x8b\x13\x91\xaf\x3a\x0f\x4b\x30\xbb\x5f\xc4\xaa\x9a\xb0\xe0\x7c",
            "ANON",
            18,
            "HeyAnon",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\x17\x00\x00\x00\xf2\x79\xd8\x1a\x1d\x3c\xc7\x54\x30\xfa\xa0\x17\xfa\x5a\x2e",
            "ANT",
            18,
            "Aragon Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\x22\x44\x52\x80\x1a\xce\xd8\xb2\xf0\xae\xbe\x15\x53\x79\xbb\x5d\x59\x43\x81",
            "APE",
            18,
            "ApeCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x38\x21\x0e\xa1\x14\x11\x55\x7c\x13\x45\x7d\x4d\xa7\xdc\x6e\xa7\x31\xb8\x8a",
            "API3",
            18,
            "API3",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\x07\x21\xbc\xf8\xd6\x64\xc3\x04\x12\xcf\xbc\x6c\xf7\xa1\x51\x45\x23\x4a\xd1",
            "ARB",
            18,
            "Arbitrum",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\x2a\x43\xbe\x0b\x1d\x33\xb7\x26\xf0\xca\x3b\x8d\xe6\x0b\x34\x82\xb8\xb0\x50",
            "ARKM",
            18,
            "Arkham",
        )
        yield (  # address, symbol, decimals, name
            b"\xbe\x0e\xd4\x13\x81\x21\xec\xfc\x5c\x0e\x56\xb4\x05\x17\xda\x27\xe6\xc5\x22\x6b",
            "ATH",
            18,
            "Aethir Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x98\x3c\xb9\x38\x8e\xac\x77\xaf\x04\x74\xfa\x44\x1c\x48\x15\x50\x0c\xb7\xbb",
            "ATOM",
            6,
            "Cosmos",
        )
        yield (  # address, symbol, decimals, name
            b"\xa9\xb1\xeb\x59\x08\xcf\xc3\xcd\xf9\x1f\x9b\x8b\x3a\x74\x10\x85\x98\x00\x90\x96",
            "Auction",
            18,
            "Bounce Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xaa\xa7\x11\x57\x05\xe8\xbe\x94\xbf\xfe\xbd\xe5\x7a\xf9\xbf\xc2\x65\xb9\x98",
            "AUDIO",
            18,
            "Audius",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x00\x00\x00\xef\xe3\x02\xbe\xaa\x2b\x3e\x6e\x1b\x18\xd0\x8d\x69\xa9\x01\x2a",
            "AUSD",
            6,
            "test_AUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\xb4\xd8\x40\x0a\xee\xfa\xfc\x1b\x29\x53\xe0\x09\x41\x34\xa8\x87\xc7\x6b\xd8",
            "AVAIL",
            18,
            "Avail",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x77\x19\xad\x09\x02\x5f\xcc\x6c\xf6\xf8\x31\x17\x55\x80\x9d\x45\xa5\xe5\xf3",
            "AXL",
            6,
            "Axelar",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\x0e\x17\xef\x65\xf8\x2a\xb0\x18\xd8\xed\xd7\x76\xe8\xdd\x94\x03\x27\xb2\x8b",
            "AXS",
            18,
            "Axie Infinity Shard",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\x57\xde\x9c\x1a\x09\xfe\xc6\x48\xe9\x3e\xb9\x88\x75\xb2\x12\xdb\x0d\x46\x0b",
            "BabyDoge",
            9,
            "Baby Doge Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x10\x00\x00\x62\x5a\x37\x54\x42\x39\x78\xa6\x0c\x93\x17\xc5\x8a\x42\x4e\x3d",
            "BAL",
            18,
            "Balancer",
        )
        yield (  # address, symbol, decimals, name
            b"\x38\xe6\x8a\x37\xe4\x01\xf7\x27\x15\x68\xce\xca\xac\x63\xc6\xb1\xe1\x91\x30\xb4",
            "BANANA",
            18,
            "Banana",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x11\xd0\x0c\x5f\x74\x25\x5f\x56\xa5\xe3\x66\xf4\xf7\x7f\x5a\x18\x6d\x7f\x55",
            "BAND",
            18,
            "BandToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x87\x75\xf6\x48\x43\x06\x79\xa7\x09\xe9\x8d\x2b\x0c\xb6\x25\x0d\x28\x87\xef",
            "BAT",
            18,
            "BAT",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x59\xec\xed\xda\xfc\xc1\xd8\x76\xa3\xbe\x72\x90\xa2\xe1\x6e\x80\x10\x73\xa3",
            "BB",
            18,
            "BounceBit Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x52\x47\xc0\x73\x79\xd9\x49\x04\xe0\xa8\x7b\x44\x81\xf0\xa1\xdd\xfa\x0c\x64",
            "BCZERO",
            18,
            "Buggyra Coin Zero",
        )
        yield (  # address, symbol, decimals, name
            b"\x62\xd0\xa8\x45\x8e\xd7\x71\x9f\xda\xf9\x78\xfe\x59\x29\xc6\xd3\x42\xb0\xbf\xce",
            "BEAM",
            18,
            "Beam",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\xad\x75\x06\x4b\x20\xfb\x2b\x24\x47\xfe\xd4\xfa\x95\x3b\xf7\xf0\x07\xa7\x06",
            "beraSTONE",
            18,
            "Berachain STONE",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x7d\x5a\xe0\x16\xf8\x06\x60\x3c\xb1\x78\x2b\xea\x29\xac\x69\x47\x1c\xab\x9c",
            "BFC",
            18,
            "Bifrost",
        )
        yield (  # address, symbol, decimals, name
            b"\x54\xd2\x25\x27\x57\xe1\x67\x2e\xea\xd2\x34\xd2\x7b\x12\x70\x72\x8f\xf9\x05\x81",
            "BGB",
            18,
            "BitgetToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xf1\x7e\x65\x82\x2b\x56\x8b\x39\x03\x68\x5a\x7c\x9f\x49\x6c\xf7\x65\x6c\xc6\xc2",
            "BICO",
            18,
            "Biconomy Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xbc\x2c\xa1\xbe\x49\x2b\xe7\x18\x5f\xaa\x2c\x88\x35\xd9\xb8\x24\xc8\xa1\x94",
            "BIGTIME",
            18,
            "Big Time",
        )
        yield (  # address, symbol, decimals, name
            b"\xcb\x15\x92\x59\x19\x96\x76\x5e\xc0\xef\xc1\xf9\x25\x99\xa1\x97\x67\xee\x5f\xfa",
            "BIO",
            18,
            "BIO",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\xe4\xf9\xf8\x08\xc4\x9a\x2a\x61\xde\x9c\x58\x96\x29\x89\x20\xdc\x4e\xee\xa9",
            "BITCOIN",
            8,
            "HarryPotterObamaSonic10Inu",
        )
        yield (  # address, symbol, decimals, name
            b"\x52\x83\xd2\x91\xdb\xcf\x85\x35\x6a\x21\xba\x09\x0e\x6d\xb5\x91\x21\x20\x8b\x44",
            "BLUR",
            18,
            "Blur",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x00\x00\x00\x00\xa3\x9b\xb2\x72\xe7\x90\x75\xad\xe1\x25\xfd\x35\x18\x87\xac",
            "Blur Pool",
            18,
            "Blur Pool",
        )
        yield (  # address, symbol, decimals, name
            b"\x98\x6e\xe2\xb9\x44\xc4\x2d\x01\x7f\x52\xaf\x21\xc4\xc6\x9b\x84\xdb\xea\x35\xd8",
            "BMX",
            18,
            "BMX",
        )
        yield (  # address, symbol, decimals, name
            b"\xb8\xc7\x74\x82\xe4\x5f\x1f\x44\xde\x17\x45\xf5\x2c\x74\x42\x6c\x63\x1b\xdd\x52",
            "BNB",
            18,
            "BNB",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x57\x3d\x6f\xb3\xf1\x3d\x68\x9f\xf8\x44\xb4\xce\x37\x79\x4d\x79\xa7\xff\x1c",
            "BNT",
            18,
            "BNT",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\x51\xcb\x3d\x86\x19\x20\xe0\x7a\x38\xe0\x3e\xea\xd1\x2c\x32\x17\x85\x67\xf6",
            "BONK",
            5,
            "Bonk",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xd0\xf5\x5c\xd8\xc7\x13\x3a\x9d\x71\x02\xb1\x39\x87\x23\x5f\x48\x6f\x22\x24",
            "BORG",
            18,
            "SwissBorg Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x9e\xbf\xab\xe7\x7a\x6e\x34\x31\x1e\xee\xe9\x82\x51\x90\xb9\xec\xe3\x28\x24",
            "BTRST",
            18,
            "BTRST",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\x69\x92\x81\x85\xdb\xce\x49\xd2\x23\x0c\xc9\xb0\x97\x9b\xe6\xdc\x79\x79\x57",
            "BTT",
            18,
            "BitTorrent",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\x12\xc3\x42\x05\x73\x71\x92\x40\x21\x72\x40\x9a\x8f\x7c\xce\xf8\xaa\x2a\xec",
            "BUIDL",
            6,
            "BlackRock USD Institutional Digi",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x12\xc5\x93\x08\x81\xc5\x37\x15\xb3\x69\xce\xc7\x60\x6b\x70\xd8\xeb\x22\x9f",
            "C98",
            18,
            "Coin98",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\x26\x49\xea\x73\xbe\xab\x28\xc5\xb4\x9b\x26\xeb\x48\xf7\xea\xd6\xd4\xc8\x98",
            "Cake",
            18,
            "PancakeSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x84\xea\xac\x1b\x2d\xc3\xf8\x4d\x92\xff\x84\xc3\xec\x20\x5b\x1f\xa7\x46\x71\xfc",
            "CAMP",
            18,
            "Camp",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x8c\xd2\x64\x74\x72\x2c\xe9\x3f\x4d\x0c\x34\xd1\x62\x01\x46\x1c\x10\xaa\x8c",
            "CARV",
            18,
            "CARV",
        )
        yield (  # address, symbol, decimals, name
            b"\xbe\x98\x95\x14\x6f\x7a\xf4\x30\x49\xca\x1c\x1a\xe3\x58\xb0\x54\x1e\xa4\x97\x04",
            "cbETH",
            18,
            "Coinbase Wrapped Staked ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\x7d\x39\xac\x67\x6e\xd1\xc6\x7a\x2b\x26\x8a\xd7\xf1\xe5\x88\x26\xe5\xb0\xaf",
            "CDT",
            18,
            "CoinDash Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x92\x54\xc8\x3e\xb5\x25\xf9\xfc\xf3\x46\x49\x0b\xbb\x3e\xd2\x8a\x81\xc6\x67",
            "CELR",
            18,
            "CelerToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xcc\xcc\xcc\xcc\xcc\x33\xd5\x38\xdb\xc2\xee\x4f\xea\xb0\xa7\xa1\xff\x4e\x8a\x94",
            "CFG",
            18,
            "Centrifuge",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\x93\x18\x94\xa8\x6d\x47\x44\x12\x13\x19\x96\x21\xf1\xf2\x99\x4e\x1c\x39\xaa",
            "CGPT",
            18,
            "ChainGPT",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\xe8\x4f\x6a\x69\x98\x6a\x83\xd9\x2c\x32\x4d\xf1\x0b\xc8\xe6\x47\x71\x03\x0f",
            "CHEX",
            18,
            "Chintai Exchange Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x22\x79\xd4\xa9\x0b\x6f\xe1\xc4\xb3\x0f\xa6\x60\xcc\x9f\x92\x67\x97\xba\xa2",
            "CHR",
            6,
            "Chroma",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x06\x42\x4f\x91\xfd\x33\x08\x44\x66\xf4\x02\xd5\xd9\x7f\x05\xf8\xe3\xb4\xaf",
            "CHZ",
            18,
            "chiliZ",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x0e\x94\xcb\x66\x2c\x35\x20\x28\x2e\x6f\x57\x17\x21\x40\x04\xa7\xf2\x68\x88",
            "COMP",
            18,
            "Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\xdd\xb3\x42\x24\x97\xe6\x1e\x13\x54\x3b\xea\x06\x98\x9c\x07\x89\x11\x75\x55\xc5",
            "COTI",
            18,
            "COTI Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xde\xf1\xca\x1f\xb7\xfb\xcd\xc7\x77\x52\x0a\xa7\xf3\x96\xb4\xe0\x15\xf4\x97\xab",
            "COW",
            18,
            "CoW Protocol Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\x76\x1f\xa4\x13\x77\x00\x36\x22\xae\xe3\xc7\x67\x5f\xc7\xb5\xc1\xc2\xfa\xc5",
            "CPOOL",
            18,
            "Clearpool",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\xb7\x3e\x1f\xf0\xb8\x09\x14\xab\x6f\xe0\x44\x4e\x65\x84\x8c\x4c\x34\x45\x0b",
            "CRO",
            8,
            "CRO",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\x33\xa9\x49\x74\x0b\xb3\x30\x6d\x11\x9c\xc7\x77\xfa\x90\x0b\xa0\x34\xcd\x52",
            "CRV",
            18,
            "Curve DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x5f\x6a\x29\xb1\x00\x12\x3a\xe4\x6a\x3e\x8b\xf0\x54\x1a\xb4\x02\x94\x92\x43",
            "CTB",
            18,
            "Content Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\x16\x04\xc0\xfd\xf0\x83\x47\xdd\x1f\xa4\xee\x06\x2a\x82\x2a\x5d\xd0\x6b\x5d",
            "CTSI",
            18,
            "Cartesi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x81\x7b\xbd\xbc\x3e\x8a\x12\x04\xf3\x69\x1d\x14\xbb\x44\x99\x28\x41\xe3\xdb\x35",
            "CUDOS",
            18,
            "CudosToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\xe5\x56\x00\x54\x82\x4e\xa6\xb0\x73\x2e\x65\x6e\x3a\xd6\x4e\x20\xe9\x4e\x45",
            "CVC",
            8,
            "CVC",
        )
        yield (  # address, symbol, decimals, name
            b"\x4e\x3f\xbd\x56\xcd\x56\xc3\xe7\x2c\x14\x03\xe1\x03\xb4\x5d\xb9\xda\x5b\x9d\x2b",
            "CVX",
            18,
            "Convex Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\x77\x88\x60\xe9\x37\xf5\x09\xe6\x51\x19\x2a\x90\x58\x9d\xe7\x11\xfb\x88\xa9",
            "CYBER",
            18,
            "CyberConnect",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\x17\x54\x74\xe8\x90\x94\xc4\x4d\xa9\x8b\x95\x4e\xed\xea\xc4\x95\x27\x1d\x0f",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xfe\xe2\x93\x84\x0d\x23\xb0\xb2\xde\x8c\x55\xe1\xcf\x7a\x9f\x01\xc1\x57\x76\x7c",
            "DEGEN",
            18,
            "DEGEN",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x97\xbf\xd5\x33\xa9\x9c\x9a\xa0\x83\x58\x7b\x07\x44\x34\xe6\x1e\xb0\xa2\x58",
            "DENT",
            8,
            "DENT",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\x70\x0b\x56\x4c\xa0\x8d\x94\x39\xc5\x8c\xa5\x05\x31\x66\xe8\x31\x7a\xa1\x38",
            "deUSD",
            18,
            "Elixir deUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xde\x4e\xe8\x05\x77\x85\xa7\xe8\xe8\x00\xdb\x58\xf9\x78\x48\x45\xa5\xc2\xcb\xd6",
            "DEXE",
            18,
            "Dexe",
        )
        yield (  # address, symbol, decimals, name
            b"\x32\x46\x2b\xa3\x10\xe4\x47\xef\x34\xff\x0d\x15\xbc\xe8\x61\x3a\xa8\xc4\xa2\x44",
            "DHN",
            18,
            "Dohrnii",
        )
        yield (  # address, symbol, decimals, name
            b"\x5d\xc6\x0c\x4d\x5e\x75\xd2\x25\x88\xfa\x17\xff\xeb\x90\xa6\x3e\x53\x5e\xfc\xe0",
            "DKA",
            18,
            "dKargo",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x01\x0b\xf9\xc2\x68\x81\x78\x8b\x4e\x6b\xf5\xfd\x1b\xdc\x35\x8c\x8f\x90\xb8",
            "DOT",
            18,
            "Polkadot Token (Relay Chain)",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\x4e\x7d\x07\x10\x70\x93\x88\xbc\xe3\x16\x1c\x32\xb4\xee\xa5\x6d\x3f\x91\xcc",
            "DSYNC",
            18,
            "Destra Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x92\xd6\xc1\xe3\x1e\x14\x52\x0e\x67\x6a\x68\x7f\x0a\x93\x78\x8b\x71\x6b\xef\xf5",
            "DYDX",
            18,
            "dYdX",
        )
        yield (  # address, symbol, decimals, name
            b"\x65\x7e\x8c\x86\x7d\x8b\x37\xdc\xc1\x8f\xa4\xca\xea\xd9\xc4\x5e\xb0\x88\xc6\x42",
            "eBTC",
            8,
            "ether.fi BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\x7e\xf9\xb1\x9c\x3e\x29\x31\x7a\x50\xf5\xfb\xe5\x94\xeb\xa3\x61\xda\xda\x4a",
            "EDLC",
            6,
            "Edelcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\x17\x3a\x39\xc5\x6a\x55\x48\x37\xc4\xc7\xf1\x04\x15\x3a\x00\x5d\x28\x4d\x11",
            "EDU",
            18,
            "EDU Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\xfa\x16\x47\x35\x18\x2d\xe5\x08\x11\xe8\xe2\xe8\x24\xcf\xb9\xb6\x11\x8a\xc2",
            "eETH",
            18,
            "ether.fi ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\x53\xbf\x91\x67\xf5\x0c\xde\xb3\xae\x10\x5f\x56\x09\x9a\xaa\xb9\x06\x1f\x83",
            "EIGEN",
            18,
            "Eigen",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\x21\x79\x85\x9f\xc6\xd5\xbe\xe9\xbf\x91\x58\x63\x2d\xc5\x16\x78\xa4\x10\x0e",
            "ELF",
            18,
            "ELF",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\xe1\x14\xb6\x91\xdb\x79\x0c\x35\x20\x7b\x2e\x68\x5d\x4a\x43\x18\x1e\x60\x61",
            "ENA",
            18,
            "ENA",
        )
        yield (  # address, symbol, decimals, name
            b"\xc1\x83\x60\x21\x7d\x8f\x7a\xb5\xe7\xc5\x16\x56\x67\x61\xea\x12\xce\x7f\x9d\x72",
            "ENS",
            18,
            "Ethereum Name Service",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\xad\x0b\xf7\x51\x83\x4f\x2f\xbd\xc6\x2a\x41\x01\x4f\x84\xd6\x7c\xa1\xde\x2a",
            "ERA",
            18,
            "Caldera",
        )
        yield (  # address, symbol, decimals, name
            b"\xfe\x0c\x30\x06\x5b\x38\x4f\x05\x76\x1f\x15\xd0\xcc\x89\x9d\x4f\x9f\x9c\xc0\xeb",
            "ETHFI",
            18,
            "ether.fi governance token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\x5b\x1b\x31\xce\x00\x2f\xbf\x20\x58\xd2\x2f\x30\xf9\x5d\x40\x52\x00\xa1\x5b",
            "ETHx",
            18,
            "ETHx",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\xfc\xd9\x8c\x32\x29\x42\x07\x5a\x5c\x38\x60\x69\x3e\x9f\x4f\x03\xaa\xe0\x7b",
            "EUL",
            18,
            "Euler",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\xba\xea\x1f\x7c\x83\x0b\xd8\x9a\xcc\x67\xec\x4a\xf5\x16\x28\x4b\x1b\xc3\x3c",
            "EURC",
            6,
            "SSS Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\x25\xf2\x11\xab\x05\xb1\xc9\x7d\x59\x55\x16\xf4\x57\x94\x52\x8a\x80\x7a\xd8",
            "EURS",
            2,
            "EURS",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\x54\x95\xef\xe5\xdb\x9c\xe0\x0f\x80\x36\x4c\x8b\x42\x35\x67\xe5\x8d\x21\x10",
            "ezETH",
            18,
            "ezETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xc5\xf0\xf7\xb6\x67\x64\xf6\xec\x8c\x8d\xff\x7b\xa6\x83\x10\x22\x95\xe1\x64\x09",
            "FDUSD",
            18,
            "First Digital USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\xa4\x6a\x60\x36\x8a\x7b\xd0\x60\xee\xc7\xdf\x8c\xba\x43\xb7\xef\x41\xad\x85",
            "FET",
            18,
            "Fetch",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\xf8\xed\xa0\x95\xe3\x7a\x41\xe0\x02\xe2\x66\xda\xad\x7e\xfc\x15\x79\xbc\x0a",
            "FLEX",
            18,
            "FLEX Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xcf\x0c\x12\x2c\x6b\x73\xff\x80\x9c\x69\x3d\xb7\x61\xe7\xba\xeb\xe6\x2b\x6a\x2e",
            "FLOKI",
            9,
            "FLOKI",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\x0c\xd1\x6b\x01\x1b\x98\x7d\xa3\x51\x8f\xbf\x38\xc3\x07\x1d\x4f\x0d\x14\x95",
            "FLUX",
            8,
            "Flux",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\x96\x4e\x35\xa7\x61\x03\xaf\x4c\x7d\x73\x18\xe1\xb1\xa8\x2c\x68\x2a\xe2\x96",
            "FLZ",
            18,
            "Fellaz Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x3d\x95\x5a\xce\xf8\x22\xdb\x05\x8e\xb8\x50\x59\x11\xed\x77\xf1\x75\xb9\x9e",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\x5e\x84\x22\x34\x52\x38\xf3\x42\x75\x88\x80\x49\x02\x18\x21\xe8\xe0\x8c\xaa\x1f",
            "frxETH",
            18,
            "Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\xcd\x6f\xd2\x66\xaf\x91\xb8\xae\xd5\x2a\xcc\xc3\x82\xb4\xe1\x65\x58\x6e\x29",
            "FRXUSD",
            18,
            "Frax USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x4e\x15\x36\x1f\xd6\xb4\xbb\x60\x9f\xa6\x3c\x81\xa2\xbe\x19\xd8\x73\x71\x78\x70",
            "FTM",
            18,
            "Fantom Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\xdf\x38\x6b\x75\x54\x65\x87\x1f\xf8\x74\xe3\xe3\x7a\xf5\x97\x6e\x24\x70\x64",
            "FTN",
            18,
            "Fasttoken",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xd1\xc9\x77\x19\x02\x47\x60\x76\xec\xfc\x8b\x2a\x83\xad\x6b\x93\x55\xa4\xc9",
            "FTX Token",
            18,
            "FTT",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\x9d\x0d\x8b\xdd\x9a\xf5\xe6\x06\xae\x22\x32\xed\x28\x5a\xff\x19\x0e\x71\x1b",
            "FUN",
            8,
            "FUN",
        )
        yield (  # address, symbol, decimals, name
            b"\x34\x32\xb6\xa6\x0d\x23\xca\x0d\xfc\xa7\x76\x1b\x7a\xb5\x64\x59\xd9\xc9\x64\xd0",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\x7b\xeb\xa8\xf6\xef\x66\x43\xab\xd7\x25\xe4\x5a\x4e\x83\x87\xef\x26\x06\x49",
            "G",
            18,
            "Gravity",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\xee\x21\xc3\x06\xa7\x00\xe6\x82\xab\xcd\xfe\x9b\xaa\x6a\x08\xf3\x82\x04\x19",
            "G-CRE",
            18,
            "Gluwa Creditcoin Vesting Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\xaa\x98\x9a\xf9\x6a\xf8\x53\x84\xb8\xa9\x38\xc2\xed\xe4\xa7\x37\x8d\x98\x75",
            "GAL",
            18,
            "Project Galaxy",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\xd2\xeb\x1b\x1e\x90\xb6\x38\x58\x87\x28\xb4\x13\x01\x37\xd2\x62\xc8\x7c\xae",
            "GALA",
            8,
            "Gala",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\xd1\x6f\xc0\x24\x6a\xd3\x16\x0c\xcc\x09\xb8\xd0\xd3\xa2\xcd\x28\xae\x6c\x2f",
            "GHO",
            18,
            "Gho Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\xd9\xc5\xcb\xa0\x5e\x15\x1c\x89\x5f\xde\x1c\xf3\x55\xc9\xa1\xd5\xda\x64\x29",
            "GLM",
            18,
            "Golem Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\xdc\x52\xc4\xde\x30\xe9\x4b\xe3\xa6\xa0\xa2\xb2\x59\xb2\x85\x0f\x42\x19\x89",
            "GMT",
            18,
            "GoMining Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\xc4\x08\xbd\x53\xc3\x1c\x08\x5a\x17\x46\xaf\x40\x1a\x40\x42\x95\x4f\xf7\x40",
            "GMT",
            8,
            "GreenMetaverseToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\x10\xe7\x76\x88\x0c\x02\x93\x3d\x47\xdb\x1b\x9f\xc0\x59\x08\xe5\x38\x6b\x96",
            "GNO",
            18,
            "GNO",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\x45\xee\x0b\x87\xb5\xc7\xf4\xd0\x69\xa1\x4b\x21\xa4\x83\x8b\x70\xaa\x83\xed",
            "GRD",
            18,
            "GRADE",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x44\xe9\x0c\x64\xb2\xc0\x76\x62\xa2\x92\xbe\x62\x44\xbd\xf0\x5c\xda\x44\xa7",
            "GRT",
            18,
            "Graph Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe6\x67\x47\xa1\x01\xbf\xf2\xdb\xa3\x69\x71\x99\xdc\xce\x5b\x74\x3b\x45\x47\x59",
            "GT",
            18,
            "GateChainToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xcf\x51\x04\xd0\x94\xe3\x86\x4c\xfc\xbd\xa4\x3b\x82\xe1\xce\xfd\x26\xa0\x16\xeb",
            "H",
            18,
            "Humanity",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x6e\xe5\xe3\x1d\x82\x8d\xe2\x41\x28\x2b\x96\x06\xc8\xe9\x8e\xa4\x85\x26\xe2",
            "HOT",
            18,
            "HOT",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\xc6\xbf\x46\x9e\x97\xee\xb0\xbf\xb7\x4c\x8d\xbf\xf5\xbd\x47\xd4\xc1\xc9\x8a",
            "HSK",
            18,
            "HashKey Platform Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x93\xa2\xdb\x22\xb7\xc7\x36\xb3\x41\xc3\x2f\xf6\x66\x30\x7f\x4a\x9e\xd9\x10\xf5",
            "HYPER",
            18,
            "Hyperlane",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\xf3\xc4\x28\x33\xc3\x17\x01\x59\xaf\x4e\x92\xdb\xb4\x51\xfb\x3f\x70\x89\x17",
            "ICP",
            8,
            "ICP",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\xff\x88\xa5\x67\x67\x22\x3a\x55\x29\xea\x59\x60\xda\x7a\x3f\x5f\x76\x64\x06",
            "ID",
            18,
            "SPACE ID",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\xc5\x32\x68\xe9\xde\x69\x2a\xe1\xb2\xea\x52\x16\xe2\x4e\x1c\x3a\xd7\xcb\x1e",
            "IDO",
            18,
            "Idexo Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x7f\xe9\xed\xc9\xe0\xdf\x98\xe0\x74\x54\x84\x79\x09\xb5\xe9\x59\xd7\xca\x0e",
            "ILV",
            18,
            "Illuvium",
        )
        yield (  # address, symbol, decimals, name
            b"\xf5\x7e\x7e\x7c\x23\x97\x8c\x3c\xae\xc3\xc3\x54\x8e\x3d\x61\x5c\x34\x6e\x79\xff",
            "IMX",
            18,
            "Immutable X",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\x8b\x3b\x32\xb6\xc3\x45\xa3\x4f\xf6\x46\x74\x60\x61\x24\xdd\x5a\xce\xca\x30",
            "INJ",
            18,
            "Injective Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\x40\xd4\xa6\x23\x7c\x25\x7f\xff\x2d\xb0\x0f\xa0\x51\x0d\xee\xec\xd3\x03\xeb",
            "INST",
            18,
            "Instadapp",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\xb3\xe0\xa2\x17\x40\x7e\xff\xf7\xca\x06\x2d\x46\xc2\x6e\x5d\x60\xa1\x4d\x69",
            "IOTX",
            18,
            "IoTeX Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x2c\x95\x6b\x4a\xb4\x80\x7b\x2f\x94\x22\x35\xc9\x07\x4d\x9b\xd0\x69\xe9\xf0",
            "IPV",
            18,
            "IPVERSE",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\x9c\xea\x18\x89\x99\x1f\x68\xac\xc3\x5f\xf5\xc3\xdd\x06\x21\xff\x29\xb0\xc9",
            "IQ",
            18,
            "Everipedia IQ",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\x20\xb4\xb9\xa0\x11\x0c\xdc\x71\xfb\x72\x09\x08\x34\x0c\x03\xf9\xbc\x03\xec",
            "JASMY",
            18,
            "JasmyCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x35\x6b\x7f\xd3\x6a\x53\x57\xe5\xa0\x17\xef\x11\x88\x7b\xa1\x00\xc9\xab\x76",
            "KAVA",
            6,
            "Kava.io",
        )
        yield (  # address, symbol, decimals, name
            b"\xf3\x49\x60\xd9\xd6\x0b\xe1\x8c\xc1\xd5\xaf\xc1\xa6\xf0\x12\xa7\x23\xa2\x88\x11",
            "KCS",
            6,
            "KuCoin Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\xee\xe3\x0c\x52\xb0\xb3\x79\xb0\x46\xfb\x0f\x85\xf4\xf3\xdc\x30\x09\xaf\xec",
            "KEEP",
            18,
            "KEEP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xde\xfa\x4e\x8a\x7b\xcb\xa3\x45\xf6\x87\xa2\xf1\x45\x6f\x5e\xdd\x9c\xe9\x72\x02",
            "KNC",
            18,
            "Kyber Network Crystal v2",
        )
        yield (  # address, symbol, decimals, name
            b"\x06\x49\xce\xf6\xd1\x1e\xd6\xf8\x85\x35\x46\x2e\x14\x73\x04\xd3\xfe\x5a\xe1\x4d",
            "KUB",
            18,
            "Bitkub Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\xc2\xa5\x5d\x5b\xd1\x30\x33\xf1\xee\x0c\xdd\x11\xf6\x0f\x7e\xfe\x66\xf4\x67",
            "LA",
            18,
            "Lagrange",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\x36\xa8\x70\x84\xf8\xb8\x43\x06\xf7\x20\x07\xf3\x6f\x26\x18\xa5\x63\x44\x94",
            "LBTC",
            8,
            "Lombard Staked Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\x7a\x54\xaa\xb0\x62\x62\x8c\x9b\xba\xe1\xfd\xb1\x58\x3c\x19\x55\x85\xfe\x41",
            "LCX",
            18,
            "LCX",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\x98\xfc\xbe\xa5\x16\xcf\x06\x85\x72\x15\x77\x9f\xd8\x12\xca\x3b\xef\x1b\x32",
            "LDO",
            18,
            "Lido DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\xf5\xd2\xad\x76\x74\x11\x91\xd1\x5d\xfe\x7b\xf6\xac\x92\xd4\xbd\x91\x2c\xa3",
            "LEO",
            18,
            "Bitfinex LEO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x8b\x30\x5c\xac\x06\x99\x0c\x08\x87\x03\x2a\x02\xc0\x3d\x68\x39\xf7\x70\xa8",
            "LGCT",
            18,
            "LEGACY TOKEN",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\x49\x10\x77\x1a\xf9\xca\x65\x6a\xf8\x40\xdf\xf8\x3e\x82\x64\xec\xf9\x86\xca",
            "LINK",
            18,
            "LINK",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x00\x00\x00\x00\x09\x54\x13\xaf\xc2\x95\xd1\x9e\xde\xb1\xad\x7b\x71\xc9\x52",
            "LON",
            18,
            "Tokenlon",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\x24\x41\x5d\x1e\x54\x9e\xbc\x62\x6a\x13\xa4\x82\xbb\x11\x7a\x2b\x43\xe9\xcf",
            "LOVELY",
            8,
            "Lovely Inu Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x58\xb6\xa8\xa3\x30\x23\x69\xda\xec\x38\x33\x34\x67\x24\x04\xee\x73\x3a\xb2\x39",
            "LPT",
            18,
            "Livepeer Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6d\xea\x81\xc8\x17\x1d\x0b\xa5\x74\x75\x4e\xf6\xf8\xb4\x12\xf2\xed\x88\xc5\x4d",
            "LQTY",
            18,
            "LQTY",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\xbb\xca\x6a\x90\x1c\x92\x6f\x24\x0b\x89\xea\xcb\x64\x1d\x8a\xec\x7a\xea\xfd",
            "LRC",
            18,
            "LoopringCoin V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x1b\xed\x5b\x9a\x09\x28\x46\x7c\x9b\x13\x41\xda\x1d\x7b\xd5\xe1\x0b\x65\x49",
            "LsETH",
            18,
            "Liquid Staked ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\x33\xf7\xf8\x83\x32\xb8\xdb\x6a\xd4\x52\xb7\xc6\xd5\xbb\x64\x39\x90\xae\x3f",
            "LSK",
            18,
            "Lisk",
        )
        yield (  # address, symbol, decimals, name
            b"\xbd\x31\xea\x82\x12\x11\x9f\x94\xa6\x11\xfa\x96\x98\x81\xcb\xa3\xea\x06\xfa\x3d",
            "LUNA",
            6,
            "LUNA (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xc7\xa3\xba\x49\xc7\xa6\xea\xba\x6c\xd4\xa9\x6c\x55\xa1\x39\x10\x70\xac\x9a",
            "MAGIC",
            18,
            "MAGIC",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\x5d\x2f\xb2\x9f\xb7\xd3\xcf\xee\x44\x4a\x20\x02\x98\xf4\x68\x90\x8c\xc9\x42",
            "MANA",
            18,
            "MANA",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\xaf\x81\xe7\x3a\x73\xb4\x0a\xdf\x4f\x3d\x42\x23\xcd\x9b\x1e\xce\x62\x30\x74",
            "MASK",
            18,
            "Mask Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\x1a\xfa\x7b\x71\x8f\xb8\x93\xdb\x30\xa3\xab\xc0\xcf\xc6\x08\xaa\xcf\xeb\xb0",
            "MATIC",
            18,
            "MATIC",
        )
        yield (  # address, symbol, decimals, name
            b"\x63\xf7\xb1\xb5\x38\xa7\x8c\xb6\x99\xe5\x39\x96\x21\xb3\xd2\xe0\x47\xc4\x0d\xe4",
            "MAX",
            18,
            "Maxity",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\xe0\x2b\xc2\x87\x5a\x29\x14\xc4\xf5\x85\xbb\xf9\x2a\x6f\x28\xbc\x07\xcb\x70",
            "MBG",
            18,
            "$MBG Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\x31\xf4\xa5\x59\x07\xb1\x0d\x1f\x0a\x50\xd8\xab\x8f\xa0\x9e\xc3\x42\xcd\x74",
            "MEME",
            18,
            "Memecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\x3a\x12\xb7\x8f\xee\x11\xee\x08\x8e\x45\x4a\x05\x47\xbd\xbc\x5a\x25\x3a\x6d",
            "MERL",
            18,
            "MERL",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\xf7\x83\x8f\x5c\x46\x1f\xef\xf7\xfe\x49\xea\x5e\xba\xf7\x72\x8b\xb0\xad\xfa",
            "mETH",
            18,
            "mETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\x32\xb1\x3c\xe7\xf2\xe8\x0a\x01\x93\x2b\x42\x55\x36\x52\xe0\x53\xd6\xed\x8e",
            "Metis",
            18,
            "Metis Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\x8f\x72\xaa\x93\x04\xc8\xb5\x93\xd5\x55\xf1\x2e\xf6\x58\x9c\xc3\xa5\x79\xa2",
            "MKR",
            18,
            "MKR",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\x4d\xa3\xb3\x7b\x6f\x66\xf7\xce\x69\xb4\xbb\xbc\xf9\xa5\x5c\xe6\x16\x87\x07",
            "MMX",
            18,
            "M2 Global Wealth Limited",
        )
        yield (  # address, symbol, decimals, name
            b"\x3c\x3a\x81\xe8\x1d\xc4\x9a\x52\x2a\x59\x2e\x76\x22\xa7\xe7\x11\xc0\x6b\xf3\x54",
            "MNT",
            18,
            "Mantle",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\x44\xe3\x5f\x95\xe8\x19\xe7\x52\xf3\xcc\xb5\xfa\xf4\x09\x57\xd3\x11\xe8\xc5",
            "MOCA",
            18,
            "Moca",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\xee\x1a\x97\x23\xaa\xdb\x7a\xfa\x28\x10\x26\x36\x53\xa3\x4b\xa2\xc2\x1c\x7a",
            "Mog",
            18,
            "Mog Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x58\xd9\x7b\x57\xbb\x95\x32\x0f\x9a\x05\xdc\x91\x8a\xef\x65\x43\x49\x69\xc2\xb2",
            "MORPHO",
            18,
            "Morpho Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\x94\xe3\x5d\xb5\x01\x25\xe0\xdf\x82\xe4\xc2\xdd\xe6\x24\x96\xce\x33\x09\x99",
            "MORPHO",
            18,
            "Morpho Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\x73\xf7\xaa\xa4\xdb\x83\xf9\x5e\x9f\xff\x17\x42\x4f\x71\xd4\x75\x1a\x30\x73",
            "MOVE",
            8,
            "Movement",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\x44\x6a\x08\x74\x19\x78\x39\xd1\x53\x95\xb9\x08\x32\x8a\x74\xcc\xc9\x6b\xc0",
            "mstETH",
            18,
            "mstETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xa8\x49\xea\xae\x99\x4f\xb8\x6a\xfa\x73\x38\x2e\x9b\xd8\x8c\x2b\x6b\x18\xdc\x71",
            "MVL",
            18,
            "Mass Vehicle Ledger Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\xee\xf0\x4c\x88\x4e\x24\xd9\xb7\xb4\x76\x0e\x74\x76\xd0\x6d\xdf\x79\x7f\x36",
            "MX",
            18,
            "MX Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x41\xdd\xf0\x6b\x7f\xfd\x89\xd1\x26\x7b\x5a\x93\xbf\xef\x24\x24\xeb\x20\x03",
            "MYTH",
            18,
            "Mythos",
        )
        yield (  # address, symbol, decimals, name
            b"\x81\x2b\xa4\x1e\x07\x1c\x7b\x7f\xa4\xeb\xcf\xb6\x2d\xf5\xf4\x5f\x6f\xa8\x53\xee",
            "Neiro",
            9,
            "Neiro",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x2a\x03\xaa\x6d\xac\xf5\x1c\x18\x67\x9c\x51\x6a\xd5\x28\x3d\x8e\x7c\x26\x37",
            "NEIRO",
            9,
            "Neiro",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\xec\x02\x8a\x3d\x21\x53\x3f\xdd\x20\x08\x38\xf3\x9c\x85\xb0\x36\x79\x28\x5d",
            "NEWT",
            18,
            "Newton",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\x21\x32\xe3\x5a\x6c\x13\xee\x1e\xe0\xf8\x4d\xc5\xd4\x0b\xad\x8d\x81\x52\x06",
            "NEXO",
            18,
            "NEXO",
        )
        yield (  # address, symbol, decimals, name
            b"\x19\x8d\x14\xf2\xad\x9c\xe6\x9e\x76\xea\x33\x0b\x37\x4d\xe4\x95\x7c\x3f\x85\x0a",
            "NFT",
            6,
            "APENFT",
        )
        yield (  # address, symbol, decimals, name
            b"\xef\x40\xb8\x59\xd2\x1e\x4d\x56\x6a\x3d\x71\x3e\x75\x61\x97\xc0\x21\xbf\xfa\xaa",
            "NFT",
            6,
            "APENFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\x76\xe1\xf2\x6f\x98\xb1\xa5\xdf\x9c\xd3\x47\x95\x3a\x26\xdd\x3c\xb4\x66\x71",
            "NMR",
            18,
            "Numeraire",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\xd9\x7a\x63\x7a\x79\x0b\xe1\xfe\xff\x5e\x88\x8d\x43\x62\x9d\xc0\x54\x08\xf6",
            "NPC",
            18,
            "Non-Playable Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\x5c\x7e\xbe\x1f\x07\xca\xf6\xbf\xf0\x97\xd8\xa5\x89\xfb\x8a\xc4\x9a\xe5\xb3",
            "NPXS",
            18,
            "NPXS",
        )
        yield (  # address, symbol, decimals, name
            b"\x12\x65\x2c\x6d\x93\xfd\xb6\xf4\xf3\x7d\x48\xa8\x68\x77\x83\xc7\x82\xbb\x0d\x10",
            "NTGL",
            18,
            "Entangle",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\xc4\x9c\xee\x7e\x91\x88\xcc\xa6\xad\x8f\xf2\x64\xc1\xda\x2e\x69\xd4\xcf\x3b",
            "NXM",
            18,
            "NXM",
        )
        yield (  # address, symbol, decimals, name
            b"\x96\x7d\xa4\x04\x8c\xd0\x7a\xb3\x78\x55\xc0\x90\xaa\xf3\x66\xe4\xce\x1b\x9f\x48",
            "OCEAN",
            18,
            "Ocean Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x6c\x4e\xfb\x76\xc1\xd1\xae\x02\xe2\x0c\xeb\x03\xa2\xa6\xa0\x8b\x0b\x8d\xc3",
            "OETH",
            0,
            "Origin Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xaa\x33\x64\xf1\x7a\x4d\x01\xc6\xf1\x75\x1f\xd9\x7c\x2b\xd3\xd7\xe7\xf1\xd5",
            "OHM",
            9,
            "Olympus",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\x23\x1f\x58\xb4\x32\x40\xc9\x71\x8d\xd5\x8b\x49\x67\xc5\x11\x43\x42\xa8\x6c",
            "OKB",
            18,
            "OKB",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x93\xd1\x25\xa4\xf7\x84\x9a\x1b\x05\x9e\x64\xf4\x51\x7a\x86\xdd\x60\xc9\x5d",
            "OM",
            18,
            "MANTRA DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x36\xe6\x6f\xbb\xce\x51\xe4\xcd\x5b\xd3\xc6\x2b\x63\x7e\xb4\x11\xb1\x89\x49\xd4",
            "OMNI",
            18,
            "Omni Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\xba\x6f\x8e\x4a\x5e\x8a\xb8\x2f\x62\xfe\x7c\x39\x85\x9f\xa5\x77\x26\x9b\xe3",
            "ONDO",
            18,
            "Ondo",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\x11\x24\x98\x14\xf1\x1b\x93\x46\x80\x81\x79\xcf\x06\xe7\x1a\xc3\x28\xc1\xb5",
            "ORAI",
            18,
            "Oraichain Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x30\xf2\x61\xa9\x62\xd5\xaa\xe9\x4c\x9e\xcd\x17\x05\x44\x60\x0d\x19\x37\x66",
            "ORBR",
            18,
            "Orbler",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\x56\xcc\x6b\x1e\x6d\xed\x34\x7a\xa0\xb7\x67\x6c\x85\xab\x0b\x3d\x08\xb0\xfa",
            "ORBS",
            18,
            "Orbs",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x1a\xf1\x05\x0f\x7b\x26\xe0\xcf\xf4\x5e\xe5\x15\x48\x25\x4c\x41\xed\x6b\x5c",
            "OSAK",
            18,
            "Osaka Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xf1\xc9\xac\xdc\x66\x97\x4d\xfb\x6d\xec\xb1\x2a\xa3\x85\xb9\xcd\x01\x19\x0e\x38",
            "osETH",
            18,
            "Staked ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x93\xa6\x2c\xcf\xcf\x1e\xfc\xb5\xf6\x03\x17\x98\x1f\x71\xed\x6f\xb3\x9f\x4b\xa2",
            "OSMO",
            6,
            "Osmosis",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x19\xc1\x93\x93\xe2\xd0\x34\xd8\xff\x31\xff\x34\xc8\x12\x52\xfc\xbb\xee\x92",
            "OUSG",
            18,
            "Ondo Short-Term U.S. Government",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\xfe\xe6\x80\x69\x09\x00\xba\x0c\xcc\xfc\x76\xad\x70\xfd\x1b\x95\xd1\x0e\x16",
            "PAAL",
            9,
            "PAAL AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\x80\x48\x80\xde\x22\x91\x3d\xaf\xe0\x9f\x49\x80\x84\x8e\xce\x6e\xcb\xaf\x78",
            "PAXG",
            18,
            "Paxos Gold",
        )
        yield (  # address, symbol, decimals, name
            b"\x3c\x2a\x30\x9d\x90\x05\x43\x3c\x1b\xc2\xc9\x2e\xf1\xbe\x06\x48\x9e\x5b\xf2\x58",
            "PCI",
            8,
            "Wrapped PCI",
        )
        yield (  # address, symbol, decimals, name
            b"\x80\x85\x07\x12\x1b\x80\xc0\x23\x88\xfa\xd1\x47\x26\x48\x2e\x06\x1b\x8d\xa8\x27",
            "PENDLE",
            18,
            "Pendle",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x58\xc0\xbe\x72\xbe\x21\x8b\x41\xc6\x08\xb7\xfe\x7c\x5b\xb6\x30\x73\x6c\x71",
            "PEOPLE",
            18,
            "ConstitutionDAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x82\x50\x81\x45\x45\x4c\xe3\x25\xdd\xbe\x47\xa2\x5d\x4e\xc3\xd2\x31\x19\x33",
            "PEPE",
            18,
            "Pepe",
        )
        yield (  # address, symbol, decimals, name
            b"\xa9\xe8\xac\xf0\x69\xc5\x8a\xec\x88\x25\x54\x28\x45\xfd\x75\x4e\x41\xa9\x48\x9a",
            "pepecoin",
            18,
            "pepeCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x5b\xa9\x16\x42\xf1\x02\x82\xb5\x76\xd9\x19\x22\xae\x64\x48\xc9\xd5\x2f\x4e",
            "PHA",
            18,
            "Phala",
        )
        yield (  # address, symbol, decimals, name
            b"\x34\x29\xd0\x3c\x6f\x75\x21\xae\xc7\x37\xa0\xbb\xf2\xe5\xdd\xce\xf2\xc3\xae\x31",
            "PIXEL",
            18,
            "PIXEL",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\x17\x46\xa8\x00\xd2\x24\x39\x3f\xe2\x47\x0c\x70\xa3\x57\x17\xed\x4e\xa5\xf1",
            "PLUME",
            18,
            "Plume",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x4a\x72\x6d\x9c\xed\x04\x33\xa8\xd7\x64\x33\x35\x91\x9d\xeb\x03\xa9\xa9\x35",
            "POKT",
            6,
            "Pocket Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\x5e\x53\xcb\xb8\x60\x18\xac\x2b\x80\x92\xfd\xcd\x39\xd8\x44\x4a\xff\xc3\xf6",
            "POL",
            18,
            "Polygon Ecosystem Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\xb9\x46\x00\x89\x13\xb8\x2e\x4d\xf8\x5f\x50\x1c\xba\xed\x91\x0e\x58\xd2\x6c",
            "POND",
            0,
            "POND",
        )
        yield (  # address, symbol, decimals, name
            b"\xcb\x46\xc5\x50\x53\x9a\xc3\xdb\x72\xdc\x7a\xf7\xc8\x9b\x11\xc3\x06\xc7\x27\xc2",
            "pONT",
            9,
            "Poly Ontology Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x58\x32\xf8\xfc\x6b\xf5\x9c\x85\xc5\x27\xfe\xc3\x74\x0a\x1b\x7a\x36\x12\x69",
            "POWR",
            6,
            "POWR",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\x3d\x80\xf5\xfe\xfc\xdd\xaa\x21\x22\x12\xf0\x28\x02\x1b\x41\xde\xd4\x28\xcf",
            "PRIME",
            18,
            "Prime",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\x6b\xb5\x99\xa1\x2c\x82\x64\x76\xe3\xa7\x71\x45\x46\x97\xea\x52\xe9\xe2\x20",
            "PRO",
            8,
            "Propy",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x82\xbb\x4b\xa8\x60\x45\xaf\x6f\x32\x73\x23\xa4\x6e\x80\x41\x2b\x91\xb2\x7d",
            "PROM",
            18,
            "Token Prometeus Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\xef\x15\xd9\x38\xd4\xe7\x20\x56\xac\x92\xea\x4b\xdd\x0d\x76\xb1\xc4\xad\x29",
            "PROVE",
            18,
            "Succinct",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\xa4\x42\x85\x6c\x23\x4a\x39\xa8\x1a\x08\x9c\x06\x45\x1e\xba\xa4\x30\x6a\x72",
            "pufETH",
            18,
            "pufETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\xd1\x0b\x98\x99\x88\x2a\x6f\x2f\xcb\x5c\x37\x1e\x17\xe7\x0f\xde\xe0\x0c\x38",
            "PUNDIX",
            18,
            "Pundi X Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x0e\xf9\x26\x3e\x76\xda\xe6\x3c\x84\x29\x2c\x34\x09\xd6\x1c\x59\x8e\x96\x82",
            "PYR",
            18,
            "PYR Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x3e\xa9\x03\x64\x06\x85\x20\x06\x29\x07\x70\xbe\xdf\xca\xba\x0e\x23\xa0\xe8",
            "PYUSD",
            6,
            "Hopper",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x95\x32\xa6\x0e\x0e\x7c\x6b\xbd\x2b\x2c\x13\x03\xf6\x3a\xce\x1c\x3e\x98\x11",
            "pzETH",
            18,
            "Renzo Restaked LST",
        )
        yield (  # address, symbol, decimals, name
            b"\xcb\x21\x31\x1d\x3b\x91\xb5\x32\x4f\x6c\x11\xb4\xf5\xa6\x56\xfc\xac\xbf\xf1\x22",
            "QAI",
            18,
            "QuantixAI",
        )
        yield (  # address, symbol, decimals, name
            b"\x4a\x22\x0e\x60\x96\xb2\x5e\xad\xb8\x83\x58\xcb\x44\x06\x8a\x32\x48\x25\x46\x75",
            "QNT",
            18,
            "QNT",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\x3c\x6b\xfe\xda\x06\x5f\xe2\xc4\xc1\x17\x65\xbf\x83\x87\x89\xbd\x0b\xb5\xde",
            "RED",
            18,
            "Redstone",
        )
        yield (  # address, symbol, decimals, name
            b"\xdd\x3b\x11\xef\x34\xcd\x51\x1a\x2d\xa1\x59\x03\x4a\x05\xfc\xb9\x4d\x80\x66\x86",
            "REKT",
            18,
            "Rekt",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x82\x21\xaf\xbb\x33\x99\x8d\x85\x84\xa2\xb0\x57\x49\xba\x73\xc3\x7a\x93\x8a",
            "REQ",
            18,
            "REQ",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x78\x73\x6c\xd6\x15\xf3\x74\xd3\x08\x51\x23\xa2\x10\x44\x8e\x74\xfc\x63\x93",
            "rETH",
            18,
            "Rocket Pool ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\xb6\x03\xbe\x3d\x54\x5f\x09\x60\x15\x74\x1e\x65\x03\x44\x02\x82\xbf\x45\xfb",
            "RIF",
            18,
            "RIF",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x6e\xee\x2c\xc3\x18\x80\x71\xc0\x2b\xfc\x17\x45\xa6\xb1\x7c\x65\x6e\x3f\x3d",
            "RLB",
            18,
            "Rollbit Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\x7f\x4c\x5b\xb6\x72\x23\x0e\x86\x72\x08\x55\x32\xf7\xe9\x01\x54\x4a\x73\x75",
            "RLC",
            9,
            "RLC",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\x92\xbb\x45\xbf\x1e\xe4\xd1\x40\x12\x70\x49\x75\x7c\x2e\x0f\xf0\x63\x17\xed",
            "RLUSD",
            18,
            "RLUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x6d\xe0\x37\xef\x9a\xd2\x72\x5e\xb4\x01\x18\xbb\x17\x02\xeb\xb2\x7e\x4a\xeb\x24",
            "RNDR",
            18,
            "Render Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x35\x26\x06\x8d\x11\x6c\xe6\x9f\x19\xa9\xee\x46\xf0\xbd\x30\x4f\x21\xa5\x1f",
            "RPL",
            18,
            "Rocket Pool Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\x29\x0d\x69\xc6\x5a\x6f\xe4\xdf\x75\x2f\x95\x82\x3f\xae\x25\xcb\x99\xe5\xa7",
            "rsETH",
            18,
            "rsETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x32\x06\x23\xb8\xe4\xff\x03\x37\x39\x31\x76\x9a\x31\xfc\x52\xa4\xe7\x8b\x5d\x70",
            "RSR",
            18,
            "Reserve Rights",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\xe1\x03\xdc\x9c\xf1\x90\xed\x75\x35\x07\x61\xe9\x54\x03\xb7\xb8\xaf\xa6\xc0",
            "rswETH",
            18,
            "rswETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\x7f\xe8\xf9\x7d\xb9\x7c\xbc\xcc\x76\x98\x9a\xb7\x42\xaf\xc6\x6c\xa6\xe7\x5c",
            "RYO",
            18,
            "RYO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\x53\x04\x08\xc3\xe5\x52\xb0\x20\xa2\x30\x0d\xeb\xc7\xbd\x18\x82\x0f\xb4\x2f",
            "RYU",
            18,
            "RyuJin",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\xfe\x38\x55\x35\x8e\x11\x2b\x56\x47\xb9\x52\x70\x9e\x61\x65\xe1\xc1\xee\xee",
            "SAFE",
            18,
            "Safe Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\xff\xb4\x11\xc4\xa7\x0a\xa7\xc9\x5d\x5c\x98\x1a\x6f\xb4\xda\x86\x7e\x11\x11",
            "SAHARA",
            18,
            "Sahara AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x38\x45\xba\xda\xde\x8e\x6d\xff\x04\x98\x20\x68\x0d\x1f\x14\xbd\x39\x03\xa5\xd0",
            "SAND",
            18,
            "SAND",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\x5b\x19\x6a\xbe\x0d\x54\x48\x59\x75\xd1\xec\x29\x61\x7d\x42\xd9\x19\x83\x26",
            "sdeUSD",
            18,
            "Staked deUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x12\xe2\xb8\x03\x34\x20\x27\x0d\xb2\xf3\xb3\x28\xe3\x23\x70\xcb\x5b\x2c\xa1\x34",
            "SFP",
            18,
            "SafePalToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\xe2\xf4\x83\x0b\x9e\x45\xd5\x2f\x80\xac\x08\xcb\x2b\xec\x0f\xef\x72\xed\x9c",
            "SGETH",
            18,
            "Stargate Ether Vault",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\x81\x56\x27\x83\x02\x8f\x5c\x1b\xcb\x98\x5d\x22\x83\xd5\xe1\x70\xd8\x88\x88",
            "SHFL",
            18,
            "Shuffle",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\xad\x61\xb0\xa1\x50\xd7\x92\x19\xdc\xf6\x4e\x1e\x6c\xc0\x1f\x0b\x64\xc4\xce",
            "SHIB",
            18,
            "SHIBA INU",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x75\x27\x84\x1a\x93\x2d\x29\x12\x22\x4e\x20\xa4\x05\xe1\xa1\xff\x74\x70\x84",
            "SHX",
            7,
            "Stronghold SHx",
        )
        yield (  # address, symbol, decimals, name
            b"\x86\x8f\xce\xd6\x5e\xdb\xf0\x05\x6c\x41\x63\x51\x5d\xd8\x40\xe9\xf2\x87\xa4\xc3",
            "SIGN",
            18,
            "Sign",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\xc8\x3a\xec\xc7\x90\xe8\xa4\x45\x3e\x5d\xd3\xb0\xb4\xb3\x68\x05\x01\xa7\xa7",
            "SKL",
            18,
            "SKALE",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\x07\x2c\x95\xfa\xa7\x01\x25\x60\x59\xaa\x12\x26\x97\xb1\x33\xad\xed\x92\x79",
            "SKY",
            18,
            "SKY Governance Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xcc\x8f\xa2\x25\xd8\x0b\x9c\x7d\x42\xf9\x6e\x95\x70\x15\x6c\x65\xd6\xca\xaa\x25",
            "SLP",
            0,
            "Smooth Love Potion",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\x4d\x70\xfd\xbe\x2b\xa4\xcf\x95\x13\x16\x26\x61\x4a\x17\x63\xdf\x80\x5b\x9e",
            "SNT",
            18,
            "SNT",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x11\xa7\x3e\xe8\x57\x6f\xb4\x6f\x5e\x1c\x57\x51\xca\x3b\x9f\xe0\xaf\x2a\x6f",
            "SNX",
            18,
            "Synthetix Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x1a\x59\xc8\x5a\xe9\xd8\xed\xef\xec\x41\x1d\x44\x8f\x90\x84\x15\x71\xb8\x9c",
            "SOL",
            9,
            "(Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x56\xe1\xc5\x7c\x74\x75\xcc\xf7\x42\xa1\x83\x2b\x02\x8f\x04\x56\x65\x2f\x97",
            "SolvBTC",
            18,
            "Solv BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\xd9\x20\xaa\x40\xf5\x78\xab\x79\x44\x26\xf5\xc9\x0f\x6c\x73\x1d\x15\x9d\xef",
            "SolvBTC.BBN",
            18,
            "SolvBTC Babylon",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\xa0\xe2\x76\x18\x46\x2b\xda\xc7\xa2\x91\x04\xbd\xcf\xff\x4e\x6b\xfc\xea\x2d",
            "SOSO",
            18,
            "SoSoValue",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x01\x85\xf2\x13\x53\x08\xba\xd1\x75\x27\x00\x43\x64\xeb\xcc\x2d\x37\xe5\xf6",
            "SPELL",
            18,
            "Spell Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\x00\x59\xe0\x31\x7d\xe9\x17\x38\xd1\x3a\xf0\x27\xdf\xc4\xa5\x07\x81\xb0\x66",
            "SPK",
            18,
            "Spark",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\xf6\x3a\x42\x4a\x44\x39\xcb\xe4\x57\xd8\x0e\x4f\x4b\x51\xad\x25\xb2\xc5\x6c",
            "SPX",
            8,
            "SPX6900",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\x65\xff\x81\xa3\xc4\x88\xd5\x85\xbb\xfb\x0b\xfe\x3c\x77\x07\xc7\x91\x7f\x54",
            "SSV",
            18,
            "SSV Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x7a\xb9\x65\x20\xde\x3a\x18\xe5\xe1\x11\xb5\xea\xab\x09\x53\x12\xd7\xfe\x84",
            "stETH",
            18,
            "Liquid staked Ether 2.0",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x51\x91\xb0\xde\x27\x8c\x72\x86\xd6\xc7\xcc\x6a\xb6\xbb\x8a\x73\xba\x2c\xd6",
            "STG",
            18,
            "StargateToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\xa2\x7a\x54\x5c\x0c\x5b\x75\x8a\x6b\xa1\x00\xe3\xa0\x49\x00\x1d\xe8\x70\xf5",
            "stkAAVE",
            18,
            "Staked Aave",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\x4e\xf5\x1c\x88\x89\x72\xc9\x08\xcf\xac\xf5\x9b\x47\xc1\xaf\xbc\x0a\xb8\xac",
            "STORJ",
            8,
            "STORJ",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\x14\x00\x7e\xff\x0d\xb1\xf8\x13\x5f\x4c\x25\xb3\x4d\xe4\x9a\xb0\xd4\x27\x66",
            "STRK",
            18,
            "StarkNet Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe5\x3e\xc7\x27\xdb\xde\xb9\xe2\xd5\x45\x6c\x3b\xe4\x0c\xff\x03\x1a\xb4\x0a\x55",
            "SUPER",
            18,
            "SuperFarm",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\x39\xa5\xde\x30\xe5\x74\x43\xbf\xf2\xa8\x30\x7a\x42\x56\xc8\x79\x7a\x34\x97",
            "sUSDe",
            18,
            "Staked USDe",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\x35\x95\x06\x87\x78\xdd\x59\x2e\x39\xa1\x22\xf4\xf5\xa5\xcf\x09\xc9\x0f\xe2",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\xb2\x17\xe4\x0f\x8a\x5c\xb7\x9a\xdf\x04\xe1\xaa\xb6\x0e\x5a\xbd\x0d\xfc\x1e",
            "SWFTC",
            8,
            "SwftCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\xe9\x13\x7d\x39\x32\x6a\xd0\xcd\x64\x91\xfb\x5c\xc0\xcb\xa0\xe0\x89\xb6\xa9",
            "SXP",
            18,
            "Swipe",
        )
        yield (  # address, symbol, decimals, name
            b"\xe6\xbf\xd3\x3f\x52\xd8\x2c\xcb\x5b\x37\xe1\x6d\x3d\xd8\x1f\x9f\xfd\xab\xb1\x95",
            "SXT",
            18,
            "Space and Time",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\x3c\x4e\x15\xd7\xd6\x2a\xd0\xab\xec\x4a\x9b\xd4\xb0\x01\xaa\x3e\xf5\x2d\x66",
            "SYRUP",
            18,
            "Syrup Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x80\xac\x24\xaa\x92\x9e\xaf\x50\x13\xf6\x43\x6c\xda\x2a\x7b\xa1\x90\xf5\xcc\x0b",
            "syrupUSDC",
            6,
            "Syrup USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\xf7\x02\x8c\xea\xb8\x1f\xa0\xc6\x97\x12\x08\xe8\x3f\xa7\x87\x29\x94\xbe\xe5",
            "T",
            18,
            "Threshold Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\xde\xa6\x74\x78\xc5\xf8\xc5\xe2\xd9\x0e\x5e\x9b\x26\xdb\xe6\x0c\x54\xd8\x00",
            "TAIKO",
            18,
            "Taiko Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\x08\x4f\xba\x66\x6a\x33\xd3\x75\x92\xfa\x26\x33\xfd\x49\xa7\x4d\xd9\x3a\x88",
            "tBTC",
            18,
            "tBTC v2",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\xae\xba\xde\x92\x2d\xf7\x35\xc3\x8c\x80\xc7\xeb\xd7\x08\xaf\x50\x81\x5f\xaa",
            "TBTC",
            18,
            "tBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x7b\xcc\xd9\xd2\x9f\x22\x3b\xce\x80\x43\xb8\x4e\x8c\x8b\x28\x28\x27\x79\x0f",
            "TEL",
            2,
            "Telcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\xe0\x76\x36\x1c\xc8\x13\xa9\x08\xff\x67\x2f\x9b\xad\x15\x41\x47\x44\x02\xb2",
            "TEL",
            2,
            "Telcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\x1c\x45\x2f\xc9\x9c\xf4\x05\x03\x4e\xe4\x46\x80\x3b\x6f\x6c\x1f\x6d\x5e\xd8",
            "tETH",
            18,
            "Treehouse ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\x71\x02\xbd\x34\x13\xbf\xea\xa3\xdf\xfb\x48\xfa\x82\x88\x81\x9e\x48\x0a\x88",
            "TKX",
            8,
            "Tokenize Emblem",
        )
        yield (  # address, symbol, decimals, name
            b"\x58\x2d\x87\x2a\x1b\x09\x4f\xc4\x8f\x5d\xe3\x1d\x3b\x73\xf2\xd9\xbe\x47\xde\xf1",
            "TONCOIN",
            9,
            "Wrapped TON Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\x7a\x9c\xa8\x7d\x36\x94\xb5\x75\x5f\x21\x3b\x5d\x04\x09\x4b\x8d\x0f\x0a\x6f",
            "TRAC",
            18,
            "Trace Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\xdf\x59\x2f\x8e\xb5\xd7\xbd\x38\xbf\xef\x7d\xeb\x0f\xbc\x02\xcf\x37\x78\xa0",
            "TRB",
            18,
            "Tellor Tributes",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x28\x3b\x66\xeb\x1e\xb5\xfb\x86\x32\x7f\x08\xe1\xb5\x81\x6b\x07\x20\x21\x2b",
            "TRIBE",
            18,
            "Tribe",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\x32\x7c\x6c\x5a\x14\xdc\xad\xe7\x07\xab\xad\x2e\x27\xeb\x51\x7d\xf8\x7a\xb5",
            "TRX",
            6,
            "TRON",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\x59\x23\x16\x2c\x49\xcf\x95\xe6\xbf\x26\x62\x33\x85\xeb\x43\x1a\xd9\x20\xd3",
            "TURBO",
            18,
            "Turbo",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x00\x00\x00\x00\x08\x5d\x47\x80\xb7\x31\x19\xb6\x44\xae\x5e\xcd\x22\xb3\x76",
            "TUSD",
            18,
            "TrueUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x2b\xd4\xbe\xb5\x4c\x6b\x95\x82\x67\xd9\xdb\x02\x59\xab\xdb\xb0\xbf\xf6\x06",
            "UDS",
            18,
            "UndeadServiceToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\x08\x9d\xbf\xed\x19\xd1\xbc\xd4\x31\x46\xbb\xdc\xa2\xb8\xf9\xd9\xf8\x4a\x9a",
            "UGOLD",
            18,
            "UGOLD Inc.",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\xfa\x0d\x23\x5c\x4a\xbf\x4b\xcf\x47\x87\xaf\x4c\xf4\x47\xde\x57\x2e\xf8\x28",
            "UMA",
            18,
            "UMA Voting Token v1",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x98\x40\xa8\x5d\x5a\xf5\xbf\x1d\x17\x62\xf9\x25\xbd\xad\xdc\x42\x01\xf9\x84",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\x48\x7d\x62\x46\x82\x82\xbd\x04\xdd\xf9\x76\x63\x1c\x23\x12\x8a\x42\x55\x55\xee",
            "UPC",
            5,
            "UPCX",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\xa1\x5f\xed\x60\xbf\x67\x63\x1d\xc6\xcd\x7b\xc5\xb6\xe8\xda\x81\x90\xac\xf5",
            "USD0",
            18,
            "Usual USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\xd8\x94\x93\x72\xd4\x6b\x7a\x3d\x5a\x56\x00\x6a\xe7\x7b\x21\x5f\xc6\x9b\xc0",
            "USD0++",
            18,
            "USD0 Liquid Bond",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x0d\x00\x0e\xe4\x49\x48\xfc\x98\xc9\xb9\x8a\x4f\xa4\x92\x14\x76\xf0\x8b\x0d",
            "USD1",
            18,
            "World Liberty Financial USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\xb8\x69\x91\xc6\x21\x8b\x36\xc1\xd1\x9d\x4a\x2e\x9e\xb0\xce\x36\x06\xeb\x48",
            "USDC",
            6,
            "USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x10\xbf\x8f\xcb\x7b\xf5\x41\x21\x87\xa5\x95\xab\x97\xa3\x60\x91\x60\xb5\xc6",
            "USDD",
            18,
            "Decentralized USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x3d\x79\x75\xec\xcf\xc6\x1a\x21\x02\xb0\x89\x25\xcb\xba\x0a\x4d\x4d\xbb\x65\x55",
            "USDD",
            18,
            "Decentralized USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\x9e\xdd\x58\x52\xcd\x90\x5f\x08\x6c\x75\x9e\x83\x83\xe0\x9b\xff\x1e\x68\xb3",
            "USDe",
            18,
            "USDe",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x2b\x94\x7e\xec\x36\x8f\x42\x19\x5f\x24\xf3\x6d\x2a\xf2\x9f\x7c\x24\xce\xc2",
            "USDf",
            18,
            "Falcon USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\x43\x16\x76\x31\xd8\x9b\x6f\xfc\x58\xb8\x8d\x6b\x7f\xb0\x22\x87\x95\x49\x1d",
            "USDG",
            6,
            "Global Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\xbd\xc7\xc0\x85\x92\xee\x4a\xa5\x1d\x06\xc2\x7e\xe2\x3d\x50\x87\xd6\x5a\xdb\xcd",
            "USDL",
            18,
            "Lift Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\x38\x88\x4e\xc9\x66\x8e\xf7\x7b\x90\xc6\xdf\xf4\xd1\xa9\xf4\xf4\x82\x3b\xfe",
            "USDO",
            18,
            "OpenEden Open Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\x87\x0d\x67\xf6\x60\xd9\x5d\x5b\xe5\x30\x38\x0d\x0e\xc0\xbd\x38\x82\x89\xe1",
            "USDP",
            18,
            "USDP",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\x03\x5d\x45\xd9\x73\xe3\xec\x16\x9d\x22\x76\xdd\xab\x16\xf1\xe4\x07\x38\x4f",
            "USDS",
            18,
            "USDS Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\xc1\x7f\x95\x8d\x2e\xe5\x23\xa2\x20\x62\x06\x99\x45\x97\xc1\x3d\x83\x1e\xc7",
            "USDT",
            6,
            "USDT",
        )
        yield (  # address, symbol, decimals, name
            b"\xc1\x39\x19\x0f\x44\x7e\x92\x9f\x09\x0e\xde\xb5\x54\xd9\x5a\xbb\x8b\x18\xac\x1c",
            "USDtb",
            18,
            "USDtb",
        )
        yield (  # address, symbol, decimals, name
            b"\x96\xf6\xef\x95\x18\x40\x72\x1a\xdb\xf4\x6a\xc9\x96\xb5\x9e\x02\x35\xcb\x98\x5c",
            "USDY",
            18,
            "Ondo U.S. Dollar Yield",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\xa1\xe3\x7c\x9b\x0e\xad\xdc\xa1\x7d\x36\x62\xd6\xc0\x5f\x4d\xec\xf3\xe1\x10",
            "USR",
            18,
            "Resolv USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xa4\x7c\x8b\xf3\x7f\x92\xab\xed\x4a\x12\x6b\xda\x80\x7a\x7b\x74\x98\x66\x1a\xcd",
            "UST",
            18,
            "Wrapped UST Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\x44\x1c\x2b\xe5\xd8\xfa\x81\x26\x82\x2b\x99\x29\xca\x0b\x81\xea\x0d\xe3\x8e",
            "USUAL",
            18,
            "USUAL",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\x64\x71\xa3\x4f\x6e\xf1\x9f\xe5\x71\xef\xfc\x1c\xa7\x11\xfd\xb8\xe4\x9f\x2b",
            "USYC",
            6,
            "Short Duration Yield Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\xf7\xfa\x94\xb8\xb6\x6e\xf3\x13\xf7\x97\x0d\x4e\xeb\xd2\xcb\x31\x03\xa2\xc0",
            "VANA",
            18,
            "Vana",
        )
        yield (  # address, symbol, decimals, name
            b"\x3b\xe7\x75\xb6\x99\xfe\xe9\x16\xe7\xde\x11\x79\x94\x35\x8f\xf8\xf4\x8e\x45\x69",
            "VCNT",
            18,
            "ViciCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x34\x0d\x2b\xde\x5e\xb2\x8c\x1e\xed\x91\xb2\xf7\x90\x72\x3e\x3b\x16\x06\x13\xb7",
            "VEE",
            18,
            "BLOCKv Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\xff\x86\x20\xb8\xca\x30\x90\x23\x95\xa7\xbd\x3f\x24\x07\xe1\xa0\x91\xbf\x73",
            "VIRTUAL",
            18,
            "Virtual Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x9c\xcf\x91\x9c\x1d\xfd\xfa\x4c\x37\x42\x92\xf4\x2c\xad\xc9\x89\x9b\xf7\x53",
            "VSN",
            18,
            "Vision",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x9e\x71\x61\x3f\x9a\xa0\x6e\x57\x01\xcf\x6d\xe6\x3e\x30\x36\x16\xb0\xdd\xe3",
            "VVS",
            18,
            "VVS",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xff\xa8\x00\x08\x86\xe5\x7f\x86\xdd\x52\x64\xb9\x58\x2b\x2a\xd8\x7b\x2b\x91",
            "W",
            18,
            "Wormhole Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\xf4\x59\x2e\xbf\xfd\x73\x0c\x7d\xc9\x2c\x1b\xdf\xfd\xfc\x3b\x9e\xfc\xf2\x9a",
            "WAVES",
            18,
            "WAVES",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\x96\xea\xa2\x25\x80\x44\x51\xa9\x16\x16\xb2\x9d\x04\x0c\xab\x05\x43\x5f\x0d",
            "WAVES",
            8,
            "WAVES",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\x79\x32\x4c\x19\xef\x2b\x89\xea\x98\xb2\x3b\xc6\x69\xb7\xe7\xc9\xf8\xa5\x17",
            "WAXP",
            8,
            "WAXP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\x8d\x75\xf6\x5a\x02\xb3\xd5\x3b\x24\x18\xfb\x8e\x1f\xe4\x93\x75\x9c\x76\x05",
            "WBNB",
            18,
            "Wrapped BNB (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x92\x52\x06\xb8\xa7\x07\x09\x6e\xd2\x6a\xe4\x7c\x84\x74\x7f\xe0\xbb\x73\x4f\x59",
            "WBT",
            8,
            "WBT",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\x60\xfa\xc5\xe5\x54\x2a\x77\x3a\xa4\x4f\xbc\xfe\xdf\x7c\x19\x3b\xc2\xc5\x99",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\x5f\xe2\x3c\x85\x82\x0f\x7b\x72\xd0\x92\x6f\xc9\xb0\x5b\x43\xe3\x59\xb7\xee",
            "weETH",
            18,
            "EtherFi wrapped ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x2a\xaa\x39\xb2\x23\xfe\x8d\x0a\x0e\x5c\x4f\x27\xea\xd9\x08\x3c\x75\x6c\xc2",
            "WETH",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xf3\x16\x98\xdd\xad\x0d\x11\x16\x0f\xe8\x5c\x50\x03\x97\xa4\x70\xcd\x3d\x49\x2e",
            "WEXO",
            18,
            "WEXO",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\x14\x7e\x74\xd6\x3b\x1d\x31\xaa\x3f\xd7\x8e\xb2\x29\xb6\x51\x61\x98\x3b\x2b",
            "WFLOW",
            18,
            "Wrapped Flow",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\xdf\x24\x2e\xf7\x97\x5d\x8c\x68\xd5\xc1\xf5\xb6\x90\x58\x01\x69\x9b\x19\x40",
            "WHITE",
            18,
            "WhiteRock",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\x3b\xff\x78\xb7\x9a\x00\x99\x76\xee\xa0\x96\xa5\x1a\x94\x8a\x3d\xc0\x0e\x34",
            "WILD",
            18,
            "Wilder",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\x3f\x8c\x24\x67\x92\x4b\xe0\xae\x7b\x53\x47\x22\x8c\xab\xf2\x60\x31\x87\x53",
            "WLD",
            18,
            "Worldcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x5e\x19\x88\x09\x72\x97\xdc\xdc\x1f\x90\xd4\xdf\xe7\x90\x9e\x84\x7c\xbe\xf6",
            "WLFI",
            18,
            "World Liberty Financial",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\xb5\xcf\x12\x40\x8a\x3a\xc1\x7d\x66\x80\x37\xce\x28\x9f\x9e\xa7\x54\x39\xd7",
            "WMTX",
            6,
            "WorldMobileToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x91\x93\x7a\x75\x08\x86\x0f\x87\x6c\x9c\x0a\x2a\x61\x7e\x7d\x9e\x94\x5d\x4b",
            "WOO",
            18,
            "Wootrade Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x26\xb8\x0f\xbf\xc0\x1b\x71\x49\x5f\x47\x7d\x52\x37\x07\x12\x42\xe0\xd9\x59\xd7",
            "wROSE",
            18,
            "Wrapped ROSE (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\x39\xc5\x81\xf5\x95\xb5\x3c\x5c\xb1\x9b\xd0\xb3\xf8\xda\x6c\x93\x5e\x2c\xa0",
            "wstETH",
            18,
            "Wrapped liquid staked Ether 2.0",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\x74\x96\x65\xff\x8d\x2d\x11\x2f\xa8\x59\xaa\x29\x3f\x07\xa6\x22\x78\x2f\x38",
            "XAUt",
            6,
            "Tether Gold",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xcd\x3d\x43\xc7\x75\x97\x8a\x96\xbd\xbf\x12\xd7\x33\xd5\xa1\xed\x94\xfb\x18",
            "XCN",
            18,
            "Chain",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\xef\xb0\x0d\x12\xc2\xc1\x31\x31\xfd\x31\x93\x36\xfd\xf9\x52\x52\x5d\xa2\xaf",
            "XPR",
            4,
            "Proton",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\xcc\x9d\x8f\x36\x89\xb8\x3c\x91\xb7\xb5\x9c\xab\x49\x46\xb0\x63\xeb\x89\x4a",
            "XVS",
            18,
            "Venus XVS",
        )
        yield (  # address, symbol, decimals, name
            b"\x55\x29\x6f\x69\xf4\x0e\xa6\xd2\x0e\x47\x85\x33\xc1\x5a\x6b\x08\xb6\x54\xe7\x58",
            "XYO",
            18,
            "XYO",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\xc5\x29\xc0\x0c\x64\x01\xae\xf6\xd2\x20\xbe\x8c\x6e\xa1\x66\x7f\x6a\xd9\x3e",
            "YFI",
            18,
            "yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\xf8\x08\x7e\xad\x17\x3b\x73\xd6\xe8\xb8\x43\x29\x98\x9a\x8e\xea\x16\xcf\x73",
            "YGG",
            18,
            "Yield Guild Games Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe8\x68\x08\x4c\xf0\x8f\x3c\x3d\xb1\x1f\x4b\x73\xa9\x54\x73\x76\x2d\x94\x63\xf7",
            "YU",
            18,
            "Yala stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x9b\x45\x25\x68\x1f\x3e\xa6\xe4\x3b\x8e\x0a\x57\xbf\xff\x86\xc0\xa1\xdd\x2e",
            "ZBU",
            18,
            "ZEEBU",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\x7f\x6a\xcd\x24\x18\x5e\x14\x9e\x32\x9c\x1c\x0f\x47\x92\x01\xb9\xec\x2f\x4b",
            "ZBU",
            18,
            "ZEEBU",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\xb7\xa3\x4b\xf1\x01\x69\xd6\xd2\xd0\xd0\x2a\x6c\xbb\x43\x6c\xf4\x38\x1b\xfa",
            "ZENT",
            18,
            "Zentry",
        )
        yield (  # address, symbol, decimals, name
            b"\xf0\x91\x86\x7e\xc6\x03\xa6\x62\x8e\xd8\x3d\x27\x4e\x83\x55\x39\xd8\x2e\x9c\xc8",
            "ZETA",
            18,
            "Zeta",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\x61\x72\x46\xd0\xc6\xc0\x08\x7f\x18\x70\x3d\x57\x68\x31\x89\x9c\xa9\x4f\x01",
            "ZIG",
            18,
            "ZigCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x05\xf4\xa4\x2e\x25\x1f\x2d\x52\xb8\xed\x15\xe9\xfe\xda\xac\xfc\xef\x1f\xad\x27",
            "ZIL",
            12,
            "ZIL",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x1b\x5f\x63\x13\x54\xbe\x68\x53\xef\xe9\xc3\xab\x6b\x95\x90\xf8\x30\x2e\x81",
            "ZK",
            18,
            "Polyhedra Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x85\x88\x4c\x43\x92\xd3\x48\x58\x7b\x19\xcb\x9e\xaa\xf1\x57\xf1\x32\x71\xcd",
            "ZRO",
            18,
            "LayerZero",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x1d\x24\x89\x57\x1d\x32\x21\x89\x24\x6d\xaf\xa5\xeb\xde\x1f\x46\x99\xf4\x98",
            "ZRX",
            18,
            "0x",
        )
    if chain_id == 10:  # oeth
        yield (  # address, symbol, decimals, name
            b"\x76\xfb\x31\xfb\x4a\xf5\x68\x92\xa2\x5e\x32\xcf\xc4\x3d\xe7\x17\x95\x0c\x92\x78",
            "AAVE",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\x98\xc3\x03\x30\xd5\x77\x1a\xe9\xf9\x83\x97\x92\x09\x48\x6a\xe2\x6d\xe8\x75",
            "AI",
            18,
            "Any Inu",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x51\x1d\xc1\x85\x72\xd3\x1c\x2c\x3f\x7b\x15\x05\xcb\x2b\xbc\x08\x28\x2f\xcc",
            "AIPO",
            18,
            "Aipocalypto",
        )
        yield (  # address, symbol, decimals, name
            b"\xcb\x8f\xa9\xa7\x6b\x8e\x20\x3d\x8c\x37\x97\xbf\x43\x8d\x8f\xb8\x1e\xa3\x32\x6a",
            "alUSD",
            18,
            "Alchemix USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\xae\xed\x23\x47\x8c\x3a\x4b\x79\x8e\x4e\xd4\x0d\x8b\x7f\x41\x36\x6a\xe8\x61",
            "ANKR",
            18,
            "Ankr Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\x09\x70\x6a\x6c\x66\xca\x54\x9f\xf0\xcb\x46\x4d\xe8\x82\x31\xdd\xbe\x21\x3b",
            "AURA",
            18,
            "Aura",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\xee\x23\x43\xb8\x92\xb1\xbb\x63\x50\x3a\x4f\xab\xc8\x40\xe0\xe2\xc6\x81\x0f",
            "AXL",
            6,
            "Axelar",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\x2c\x72\xdd\x7e\x77\x07\x07\x40\xf0\x1d\x2d\x35\x85\x1c\x46\x15\x85\xd5\xd0",
            "axlLqdr",
            18,
            "Axelar Wrapped Lqdr",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x46\x63\x42\xc4\xd4\x49\xbc\x9f\x53\xa8\x65\xd5\xcb\x90\x58\x6f\x40\x52\x15",
            "axlUSDC",
            6,
            "Axelar Wrapped USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\xfe\x8b\x12\x8b\xa8\xc7\x8a\xab\xc5\x9d\x4c\x64\xce\xe7\xff\x28\xe9\x37\x99\x21",
            "BAL",
            18,
            "Balancer",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x36\xc7\x4b\x84\x0f\x99\x62\xcf\x2c\x66\x6f\x86\x66\xd6\xd6\x1e\xc2\x44\x40",
            "BALN",
            18,
            "Balanced Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\xcd\x21\x39\x51\xd8\x21\x4f\xba\xca\x72\x07\x28\x47\x4e\x2c\xef\x9d\x24\x7b",
            "bnUSD",
            18,
            "Balanced Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\x08\x7c\x44\x0f\x25\x1f\xf6\xcf\xe6\x2b\x86\xdd\xe1\xbe\x55\x8b\x95\xb4\xbb\x9b",
            "Bold",
            18,
            "Bold Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x7e\xf8\xf5\x02\x46\xf7\x25\x88\x51\x02\xe8\x23\x8c\xbb\xa3\x3f\x27\x67\x47",
            "BOND",
            18,
            "BarnBridge Governance Token (Opt",
        )
        yield (  # address, symbol, decimals, name
            b"\x58\xb9\xcb\x81\x0a\x68\xa7\xf3\xe1\xe4\xf8\xcb\x45\xd1\xb9\xb3\xc7\x97\x05\xe8",
            "CLEAR",
            18,
            "Connext",
        )
        yield (  # address, symbol, decimals, name
            b"\x7e\x7d\x44\x67\x11\x26\x89\x32\x9f\x7e\x06\x57\x1e\xd0\xe8\xcb\xad\x49\x10\xee",
            "COMP",
            18,
            "Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x94\x20\x6d\xfe\x8d\xe6\xec\x69\x20\xff\x4d\x77\x9b\x0d\x95\x06\x05\xfb\x53",
            "CRV",
            18,
            "Curve DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc5\x2d\x7f\x23\xa2\xe4\x60\x24\x8d\xb6\xee\x19\x2c\xb2\x3d\xd1\x2b\xdd\xcb\xf6",
            "crvUSD",
            18,
            "Curve.Fi USD Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\x6a\xde\xf5\xe1\x00\x6b\xb3\x05\xbb\x19\x75\x33\x3e\x8f\xc4\x07\x12\x95\xbf",
            "CTSI",
            18,
            "Cartesi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\x77\x88\x60\xe9\x37\xf5\x09\xe6\x51\x19\x2a\x90\x58\x9d\xe7\x11\xfb\x88\xa9",
            "CYBER",
            18,
            "CyberConnect",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x10\x00\x9c\xbd\x5d\x07\xdd\x0c\xec\xc6\x61\x61\xfc\x93\xd7\xc9\x00\x0d\xa1",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x5f\x7f\xa4\x6e\xd0\xf4\x87\xf0\x49\xc4\x2f\x37\x4c\xa4\x82\x7d\x65\xa2\x64",
            "DEURO",
            18,
            "Decentralized EURO",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x9f\xe3\xb5\xcc\xda\xe7\x81\x88\xb1\xf8\xb9\xa4\x9d\xa7\xae\x95\x10\xf1\x51",
            "DHT",
            18,
            "dHEDGE DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x69\xee\x04\x3d\x52\x16\x1f\xd2\x91\x37\xae\xdf\x63\xf5\xe7\x0c\xd5\x04\xd5",
            "DOG",
            18,
            "The Doge NFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\xe1\x25\xe8\x65\x38\x21\xe8\x51\xf1\x2a\x49\xf7\x76\x5d\xb9\xa9\xce\x73\x84",
            "DOLA",
            18,
            "Dola USD Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x01\x0b\xf9\xc2\x68\x81\x78\x8b\x4e\x6b\xf5\xfd\x1b\xdc\x35\x8c\x8f\x90\xb8",
            "DOT",
            18,
            "Polkadot Token (Relay Chain)",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\x80\x0d\xe7\xe8\x17\xa7\x0a\x69\x4f\x31\x47\x63\x13\xa7\xc5\x72\xbb\xa1\x00",
            "DRV",
            18,
            "Derive",
        )
        yield (  # address, symbol, decimals, name
            b"\x81\xab\x7e\x0d\x57\x0b\x01\x41\x1f\xcc\x4a\xfd\x3d\x50\xec\x8c\x24\x1c\xb7\x4b",
            "EQZ",
            18,
            "Equalizer",
        )
        yield (  # address, symbol, decimals, name
            b"\xc5\xb0\x01\xdc\x33\x72\x7f\x8f\x26\x88\x0b\x18\x40\x90\xd3\xe2\x52\x47\x0d\x45",
            "ERN",
            18,
            "Ethos Reserve Note",
        )
        yield (  # address, symbol, decimals, name
            b"\x1e\x92\x5d\xe1\xc6\x8e\xf8\x3b\xd9\x8e\xe3\xe1\x30\xef\x14\xa5\x03\x09\xc0\x1b",
            "EXA",
            18,
            "exactly",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\xad\x3a\x13\xef\x0c\x63\x66\x22\x0f\x98\x91\x57\x00\x9e\x50\x1e\x79\x38\xf8",
            "EXTRA",
            18,
            "Extra Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xf1\xa0\xda\x33\x67\xbc\x7a\xa0\x4f\x8d\x94\xba\x57\xb8\x62\xff\x37\xce\xd1\x74",
            "FOX",
            18,
            "ShapeShift FOX",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x68\xdc\xa5\xce\x2a\x4d\xb5\x30\xc0\xf6\xe5\x35\xd9\x0b\x68\x26\x42\x8d\xee",
            "FPIS",
            18,
            "Frax Price Index Share",
        )
        yield (  # address, symbol, decimals, name
            b"\x2e\x3d\x87\x07\x90\xdc\x77\xa8\x3d\xd1\xd1\x81\x84\xac\xc7\x43\x9a\x53\xf4\x75",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\x06\x41\x17\x65\xaf\x15\xbd\xdd\x26\xf8\xf5\x44\xa3\x4c\xc4\x0c\xb9\x83\x8b",
            "frxETH",
            18,
            "Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x80\xee\xde\x49\x66\x55\xfb\x90\x47\xdd\x39\xd9\xf4\x18\xd5\x48\x3e\xd6\x00\xdf",
            "FRXUSD",
            18,
            "Frax USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x53\xd6\x64\x96\x43\xf1\xf4\x60\xc3\x71\xdc\x3d\x1d\xa9\x8f\x79\x22\xfe\x51",
            "FUSE",
            18,
            "Fuse Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\xcc\xea\x5b\xb1\x61\x81\xe7\xb4\x10\x9c\x9c\x21\x43\xc2\x4a\x1c\x22\x05\xbe",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\x52\x8c\xdc\x92\xea\xb0\x44\xe1\xe3\x9f\xe4\x3b\x95\x14\xbf\xda\xb4\x41\x2b\x98",
            "GIV",
            18,
            "Giveth Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\xf4\x62\x57\xed\x31\xc3\x29\xf2\x04\xeb\x43\xe2\x54\xc6\x09\xde\xe1\x43\xb3",
            "GRG",
            18,
            "Rigo Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xbc\xe6\x43\x97\xc7\x54\x88\x46\x52\x53\xc0\xa0\x34\xb8\x09\x7f\xea\x65\x78",
            "HAN",
            18,
            "HanChain",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\x23\xdb\x8d\x7f\xba\xcc\x2e\x69\xe8\x7f\xad\x19\xb8\x86\xc8\x1c\xd7\x49\x79",
            "HYPER",
            18,
            "Hyperlane",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\x0e\x3a\x35\x11\xaa\xc3\x5c\xa7\x85\x30\xc8\x50\x07\xaf\xcd\x31\x75\x38\x19",
            "KNC",
            18,
            "Kyber Network Crystal v2",
        )
        yield (  # address, symbol, decimals, name
            b"\x2e\xd6\x22\x2c\xb7\x5e\x35\x3b\x87\x89\xbe\xc7\xbb\x44\x3b\x7e\xc9\x02\x20\x21",
            "KRL",
            18,
            "Kryll",
        )
        yield (  # address, symbol, decimals, name
            b"\x92\x0c\xf6\x26\xa2\x71\x32\x1c\x15\x1d\x02\x70\x30\xd5\xd0\x8a\xf6\x99\x45\x6b",
            "KWENTA",
            18,
            "Kwenta",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\xb7\x94\x69\x27\x24\x15\x3d\x14\x88\xcc\xdb\xe0\xc5\x6c\x25\x25\x96\x73\x5f",
            "LDO",
            18,
            "Lido DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x0a\x79\x1b\xfc\x2c\x21\xf9\xed\x5d\x10\x98\x0d\xad\x2e\x26\x38\xff\xa7\xf6",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\x0f\x94\x9f\x8a\x4e\x09\x4d\x1b\x49\xa2\x3e\xa9\x24\x1d\x28\x9b\x7b\x28\x19",
            "LUSD",
            18,
            "LUSD Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xc5\x72\x59\x49\xa6\xf0\xc7\x2e\x6c\x4a\x64\x1f\x24\x04\x9a\x91\x7d\xb0\xcb",
            "LYRA",
            18,
            "Lyra Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\x8d\x72\x67\x6e\x37\x19\x7b\x50\x11\xaf\xc6\x4d\x86\x41\xaf\x2f\xe4\x1f\xdb",
            "MDT",
            18,
            "Measurable Data Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x42\x0f\x9e\x38\xa4\xe6\x0a\x62\x22\x4c\x48\x9b\xe4\xbf\x7a\x94\x40\x24\x96",
            "MONEY",
            18,
            "Defi.Money Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x98\x01\x66\x9c\x61\x38\xe8\x4b\xd5\x0d\xeb\x50\x08\x27\xb7\x76\x77\x7d\x28",
            "O3",
            18,
            "O3 Swap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\x61\xaa\x2b\xb1\xd2\xeb\x66\x29\xed\xd7\xb0\x93\x8d\x76\x79\xb8\xb4\x9f\x9e",
            "OCEAN",
            18,
            "Ocean Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x2e\x6e\x6b\xcb\xd4\x9c\xcf\x3a\x5f\x02\x9c\x79\x98\x43\x72\xdc\xbf\xe5\x27",
            "OLAS",
            18,
            "Autonolas",
        )
        yield (  # address, symbol, decimals, name
            b"\x42\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x42",
            "OP",
            18,
            "Optimism",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\xd5\x20\x69\x62\x26\x7c\x7b\x4b\x4a\x8b\x3d\x76\xac\x2e\x1b\x2a\x5c\x4d\x5e",
            "OSAK",
            18,
            "Osaka Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\x7b\x1f\xf1\xc6\x98\x9f\x00\x6a\x11\x85\x31\x8e\xd4\xe7\xb5\x79\x6e\x66\xe1",
            "PENDLE",
            18,
            "Pendle",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\x10\x28\xf5\xf1\xd5\xed\xe5\x97\x48\xff\xce\xe5\x53\x25\x09\x97\x68\x40\xe0",
            "PERP",
            18,
            "Perpetual",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\x60\x1c\x5b\xb3\x60\x81\x1d\x96\xa2\x36\x89\x06\x6a\xf3\x16\xa3\x0c\x30\x27",
            "PIKA",
            18,
            "Pika",
        )
        yield (  # address, symbol, decimals, name
            b"\xb4\x35\x70\x54\xc3\xda\x8d\x46\xed\x64\x23\x83\xf0\x31\x39\xac\x7f\x09\x03\x43",
            "PORT3",
            18,
            "Port3 Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x59\x4e\x87\x9b\x35\x8f\x43\x0e\x20\xf8\x2b\xea\x61\xe8\x35\x62\xd4\x9d\x48",
            "PSP",
            18,
            "ParaSwap",
        )
        yield (  # address, symbol, decimals, name
            b"\x02\x35\x50\xad\xde\x4f\xa2\xf9\x0d\x63\xa4\x1d\x92\x82\xbe\xe0\x29\x4c\x04\xcd",
            "PSTAKE",
            18,
            "pSTAKE Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\xb6\x88\xcc\xf6\x82\xd5\x8f\x86\xd7\xe3\x8e\x03\xf9\xd2\x2e\x77\x05\x44\x8b",
            "RAI",
            18,
            "Rai Reflex Index",
        )
        yield (  # address, symbol, decimals, name
            b"\x9b\xce\xf7\x2b\xe8\x71\xe6\x1e\xd4\xfb\xbc\x76\x30\x88\x9b\xee\x75\x8e\xb8\x1d",
            "rETH",
            18,
            "Rocket Pool ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\x2c\x13\xe6\x6a\xde\x1f\x72\xf7\x18\x34\xf2\xfc\x50\x82\xdb\x8c\x09\x13\x58",
            "ROOBEE",
            18,
            "ROOBEE",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x05\xde\x8f\x52\xba\x75\x59\xf9\xdf\x3c\x36\x85\x00\xb6\xe6\xae\x6c\xee\x49",
            "sETH",
            18,
            "Synth sETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x48\x4c\x2d\x6e\x3c\xdd\x94\x5a\x8b\x2d\xf7\x35\xe0\x79\x17\x8c\x10\x36\x57\x8c",
            "sfrxETH",
            18,
            "Staked Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\x9c\x8c\xbc\xd9\x0a\x39\xd8\xd3\xd3\xcc\xf0\xa3\xe9\x68\x51\x1c\x38\x56\xa0",
            "SGETH",
            18,
            "Stargate Ether Vault",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\xdc\xa9\x69\x62\x47\x95\x98\x5f\x2f\x08\x3b\xcd\x0b\x67\x43\x37\xba\x13\x0a",
            "SKR",
            18,
            "Saakuru",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\xb4\x9f\xcb\x41\x89\x9e\x31\xe4\x42\xa4\xd7\x43\x99\x64\x96\x6e\x0c\x9b\x28",
            "SLN",
            18,
            "Smart Layer Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x87\x00\xda\xec\x35\xaf\x8f\xf8\x8c\x16\xbd\xf0\x41\x87\x74\xcb\x3d\x75\x99\xb4",
            "SNX",
            18,
            "Synthetix Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\x8f\x43\xb6\x22\xce\x2f\x35\xa1\x4d\xb3\xfc\x52\x82\x84\x73\x0b\x61\x9c\xd5",
            "SPECTRA",
            18,
            "Spectra Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x29\x6f\x55\xf8\xfb\x28\xe4\x98\xb8\x58\xd0\xbc\xda\x06\xd9\x55\xb2\xcb\x3f\x97",
            "STG",
            18,
            "StargateToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x6f\x28\xf2\xf1\xa3\xc8\x7f\x0f\x93\x8b\x96\xd2\x75\x20\xd9\x75\x1e\xc8\xd9",
            "SUSD",
            18,
            "Synth sUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xae\xb7\x7b\x03\xdb\xc0\xf6\x32\x1a\xe1\xb7\x2b\x2e\x9a\xdb\x0f\x60\x11\x2b",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x38\x76\x17\x49\x33\x0c\x23\xd6\x81\xd1\x36\xe8\xea\x3f\xc4\x7d\x4b\xb9\x3d\xb3",
            "SWEAT",
            18,
            "SWEAT",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\x5f\xff\x6f\x75\x3d\x7c\x11\xa5\x6a\x52\xfe\x47\xa1\x77\xa8\x7e\x43\x16\x55",
            "SYN",
            18,
            "Synapse",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x51\x4a\x61\xbc\xde\x34\xf9\x4b\xc3\x97\x31\x23\x56\x90\xab\x9d\xa7\x37\xf7",
            "TAROT",
            18,
            "Tarot",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x7d\x47\x01\x1b\x23\xbb\x96\x1e\xb6\xd9\x3c\xa9\x94\x5b\x75\x01\xa5\xbb\x11",
            "THALES",
            18,
            "Thales DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\xcc\x3d\x70\xe7\x30\x50\x3e\x7f\x28\xc1\xb4\x07\x38\x91\x98\xc4\xb7\x5f\xa2",
            "TLX",
            18,
            "TLX DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x8c\xa6\x53\xfa\x27\x72\xd5\x8f\x43\x68\xb0\xa7\x19\x80\xe9\xe3\xce\xb8\x88",
            "TRB",
            18,
            "Tellor",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x19\x60\xe2\x9b\x2c\xa5\x81\xa3\x8c\x5c\x47\x4e\x12\x3f\x42\x0f\x80\x92\xdb",
            "UBXS",
            6,
            "UBXS Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\xd9\xd7\xad\x17\x24\x2c\x41\xf7\x13\x1d\x25\x72\x12\xc5\x4a\x0e\x81\x66\x91",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x2c\x63\x9c\x53\x38\x13\xf4\xaa\x9d\x78\x37\xca\xf6\x26\x53\xd0\x97\xff\x85",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\x5c\x76\x4c\xbc\x14\xf9\x66\x9b\x88\x83\x7c\xa1\x49\x0c\xca\x17\xc3\x16\x07",
            "USDC.e",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x5d\x3a\x1f\xf2\xb6\xba\xb8\x3b\x63\xcd\x9a\xd0\x78\x70\x74\x08\x1a\x52\xef\x34",
            "USDe",
            18,
            "USDe",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\xd9\x35\x6e\x56\x5a\xb3\xa3\x6d\xd7\x77\x63\xfc\x0d\x87\xfe\xaf\x85\x50\x8c",
            "USDM",
            18,
            "Mountain Protocol USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\xb0\x08\xaa\x00\x57\x9c\x13\x07\xb0\xef\x2c\x49\x9a\xd9\x8a\x8c\xe5\x8e\x58",
            "USDT",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\xd2\x91\xda\x8a\x40\x3d\xaa\xf7\xe5\xe9\xdc\x1e\xc0\xac\xea\xcd\x48\x48\xb9",
            "USX",
            18,
            "dForce USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\xc3\x32\xa5\xdc\xe0\xe1\xd9\xbc\x2c\xc7\x2a\x68\x43\x77\x90\x57\x0c\x28\xa4",
            "VEE",
            18,
            "BLOCKv",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\x60\xe8\x27\xaf\x36\xc9\x4d\x2a\xc3\x3a\x39\xbc\xe1\xfe\x78\x63\x10\x88\xdb",
            "VELO",
            18,
            "VelodromeV2",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xff\xa8\x00\x08\x86\xe5\x7f\x86\xdd\x52\x64\xb9\x58\x2b\x2a\xd8\x7b\x2b\x91",
            "W",
            18,
            "Wormhole Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xef\x44\x61\x89\x1d\xfb\x3a\xc8\x57\x2c\xcf\x7c\x79\x46\x64\xa8\xdd\x92\x79\x45",
            "WCT",
            18,
            "WalletConnect",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\x6f\xf4\x4d\x5d\x93\x2c\xbd\x77\xb5\x2e\x56\x12\xba\x05\x29\xdc\x62\x26\xf1",
            "WLD",
            18,
            "Worldcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x32\xb1\xc2\x34\x55\x38\xc0\xc6\xf5\x82\xfc\xb0\x22\x73\x9c\x4a\x19\x4e\xbb",
            "wstETH",
            18,
            "Wrapped liquid staked Ether 2.0",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\xf5\xe0\x98\xca\xd7\xa3\xd1\xee\xd5\x39\x91\xd4\xd6\x6c\x45\xc9\xaf\x78\x12",
            "wUSDM",
            18,
            "Wrapped Mountain Protocol USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\xed\xf7\xb7\xb3\x66\x7a\x06\x99\x25\x08\xe7\xb1\x56\xef\xf7\x94\xa9\xe1\xc8",
            "XPRT",
            6,
            "XPRT native asset",
        )
        yield (  # address, symbol, decimals, name
            b"\x4a\x97\x1e\x87\xad\x1f\x61\xf7\xf3\x08\x16\x45\xf5\x2a\x99\x27\x7a\xe9\x17\xcf",
            "XVS",
            18,
            "Venus XVS",
        )
        yield (  # address, symbol, decimals, name
            b"\x90\x46\xd3\x64\x40\x29\x0f\xfd\xe5\x4f\xe0\xdd\x84\xdb\x8b\x1c\xfe\xe9\x10\x7b",
            "YFI",
            18,
            "yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\x31\xdb\x5f\x34\x8d\x70\x92\x39\xef\x17\x41\xea\x30\x96\x1b\x3b\x63\x5a\x61",
            "ynETHx",
            18,
            "ynETH MAX",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x85\x88\x4c\x43\x92\xd3\x48\x58\x7b\x19\xcb\x9e\xaa\xf1\x57\xf1\x32\x71\xcd",
            "ZRO",
            18,
            "LayerZero",
        )
    if chain_id == 56:  # bnb
        yield (  # address, symbol, decimals, name
            b"\x11\x11\x11\x11\x11\x17\xdc\x0a\xa7\x8b\x77\x0f\xa6\xa7\x38\x03\x41\x20\xc3\x02",
            "1INCH",
            18,
            "1INCH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xfb\x61\x15\x44\x5b\xff\x7b\x52\xfe\xb9\x86\x50\xc8\x7f\x44\x90\x7e\x58\xf8\x02",
            "AAVE",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\x03\x4f\x65\x3d\x5d\x16\x18\x90\x83\x6a\xd2\xb6\xb8\xcc\x49\xd1\x4e\x02\x9a",
            "AB",
            18,
            "AB",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\x7a\x71\x91\x05\xa9\x87\xb4\xc3\x41\x16\x22\x3c\xae\x8b\xd8\xf4\xb5\xde\xf4",
            "ACE",
            18,
            "ACEToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\x7d\x6b\x50\x61\x69\x89\x65\x5a\xfd\x68\x2f\xb4\x27\x43\x50\x70\x03\x05\x6d",
            "ACH",
            8,
            "Alchemy",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xe2\x20\x0e\xfb\x34\x00\xfa\xbb\x9a\xac\xf3\x12\x97\xcb\xdd\x1d\x43\x5d\x47",
            "ADA",
            18,
            "Cardano Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xbd\xa0\x11\xd7\xf8\xec\x00\xf6\x6c\x19\x23\xb0\x49\xb9\x4c\x67\xd1\x48\xd8\xb2",
            "AI",
            18,
            "Sleepless AI Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xbe\x6a\xd1\xeb\x98\x76\xcf\x3d\x3f\x9b\x85\xfe\xec\xfb\x40\x02\x98\xe8\x01\x43",
            "AIC",
            18,
            "AI Companions",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\xd0\x8d\x8c\x7a\x16\x83\x33\xa8\x52\x85\xa6\x8c\x00\x42\xb3\x9f\xc3\x74\x1d",
            "AIOZ",
            18,
            "AIOZ Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\x51\x06\x6d\x7b\xec\x65\xdc\x45\x89\x36\x8d\xa3\x68\xb2\x12\x74\x5d\x63\xe8",
            "ALICE",
            6,
            "ALICE",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\xfa\xa1\x13\xcb\xe5\x34\x36\xdf\x28\xff\x0a\xee\x54\x27\x5c\x13\xb4\x09\x75",
            "ALPHA",
            18,
            "AlphaToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xf3\x07\x91\x0a\x4c\x7b\xbc\x79\x69\x1f\xd3\x74\x88\x9b\x36\xd8\x53\x1b\x08\xe3",
            "ANKR",
            18,
            "Ankr",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\x76\x9e\x65\xc1\x4e\xbd\x1f\x68\x81\x7f\x5f\x1d\xcd\xb6\x1c\xfa\x2d\x6f\x7e",
            "ARPA",
            18,
            "ARPA Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x85\x0d\x97\x01\x8f\x66\x5e\xb7\x46\xfb\xb8\xf1\x83\x51\xe9\x77\xd1\xb0\xd6",
            "ATLAS",
            8,
            "Star Atlas (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\xb3\xa7\x05\xfc\x54\x72\x50\x37\xcc\x9e\x00\x8b\xde\xde\x69\x7f\x62\xf3\x35",
            "ATOM",
            18,
            "Cosmos Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\xe0\xc2\x82\x7e\x2e\xf1\x4d\x5c\x4f\x29\xa0\x91\xd7\x35\xa2\x04\x79\x40\x41",
            "AVAX",
            18,
            "Avalanche",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\x1f\x44\x32\xf9\x43\xc4\x65\xa9\x73\xfe\xdc\x6d\x7a\xa5\x0f\xc9\x6f\x1f\x65",
            "AXL",
            6,
            "Axelar",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x5d\x40\x0f\x88\xc1\x67\x88\x4b\xbc\xc4\x1c\x5f\xea\x40\x7e\xd4\xd2\xf8\xa0",
            "AXS",
            18,
            "Axie Infinity Shard",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\xdc\xce\x4a\x55\x90\x76\xe3\x77\x55\xa7\x8c\xe0\xc0\x62\x14\xe5\x9e\x44\x44",
            "B",
            18,
            "B",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\x2a\x5e\xa2\x5b\x16\x1b\xef\xa6\xa8\x44\x4c\x71\xae\x3a\x89\xc3\x99\x33\xc6",
            "B2M",
            18,
            "Bit2Me",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x48\x67\x30\x57\x86\x1a\x79\x72\x75\xcd\x8a\x06\x8a\xbb\x95\xa9\x02\xe8\xde",
            "BabyDoge",
            9,
            "Baby Doge Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\x1d\x82\x42\x84\x37\x12\x7b\xf1\x60\x8f\x69\x9c\xd6\x51\xe6\xab\xf9\x76\x6e",
            "BAT",
            18,
            "Basic Attention Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\xf7\x95\xa6\xf4\xd9\x7e\x78\x87\xc7\x9b\xea\x79\xab\xa5\xcc\x76\x44\x4a\xdf",
            "BCH",
            18,
            "Bitcoin Cash Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x62\xd0\xa8\x45\x8e\xd7\x71\x9f\xda\xf9\x78\xfe\x59\x29\xc6\xd3\x42\xb0\xbf\xce",
            "BEAM",
            18,
            "Beam",
        )
        yield (  # address, symbol, decimals, name
            b"\x84\x43\xf0\x91\x99\x7f\x06\xa6\x16\x70\xb7\x35\xed\x92\x73\x4f\x56\x28\x69\x2f",
            "BEL",
            18,
            "Bella Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\x06\x32\x37\x8e\x57\x3c\x6b\xe1\xac\x2f\x97\xfc\xdf\x00\x51\x5d\x0a\xa9\x1b",
            "BETH",
            18,
            "Binance Beacon ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x93\x5a\x54\x4b\xf5\x81\x6e\x3a\x7c\x13\xdb\x2e\xfe\x30\x09\xff\xda\x0a\xcd\xa2",
            "BLZ",
            18,
            "Bluzelle Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\x58\x8e\xfa\xf8\xeb\x57\xe3\x83\x74\x86\xe8\x34\xfc\x5a\x4e\x07\x76\x8d\x98",
            "bMVL",
            18,
            "Binance Wrapped MVL",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\x69\x00\x8a\x66\x9e\x2a\xf0\x0a\x86\x67\x3d\x9d\x58\x4c\xfb\x52\x4a\x42\xcc",
            "BNT",
            18,
            "Bancor Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5b\x1f\x87\x4d\x0b\x0c\x5e\xe1\x7a\x49\x5c\xbb\x70\xab\x8b\xf6\x41\x07\xa3\xbd",
            "BNX",
            18,
            "BinaryX",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\x97\xe2\x72\xa7\x37\x44\xb3\x43\x52\x8c\x3b\xc4\x70\x2f\x25\x65\xb2\xf4\x22",
            "BONK",
            5,
            "Bonk",
        )
        yield (  # address, symbol, decimals, name
            b"\x48\xdc\x01\x90\xdf\x5e\xce\x99\x0c\x64\x9a\x7a\x07\xba\x19\xd3\x65\x0a\x95\x72",
            "BOT",
            18,
            "BOT Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x30\xd2\xa1\x2b\x9b\xcb\xfa\xe4\xf2\x63\x4d\x86\x4a\x1e\xe1\xce\x3e\xad\x9c",
            "BTCB",
            18,
            "BTCB Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x78\x65\x0b\x13\x94\x71\x52\x06\x56\xb9\xe7\xaa\x7a\x5e\x92\x76\x81\x4a\x38\xe9",
            "BTCST",
            18,
            "StandardBTCHashrateToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x2c\xb5\xe1\x9b\x12\xfc\x21\x65\x48\xa2\x67\x7b\xd0\xfc\xe8\x3b\xae\x43\x4b",
            "BTT",
            18,
            "BitTorrent",
        )
        yield (  # address, symbol, decimals, name
            b"\xe9\xe7\xce\xa3\xde\xdc\xa5\x98\x47\x80\xba\xfc\x59\x9b\xd6\x9a\xdd\x08\x7d\x56",
            "BUSD",
            18,
            "BUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x2e\xa7\xf1\x04\xde\x4b\x31\x8a\x3a\xae\xdd\xc6\x29\x33\x4e\x8a\x80\x67\xbd\xd8",
            "CAIR",
            18,
            "Crypto-AI-Robo",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\x09\xfa\xbb\x73\xbd\x3a\xde\x0a\x17\xec\xc3\x21\xfd\x13\xa1\x9e\x81\xce\x82",
            "Cake",
            18,
            "PancakeSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x9f\x6a\x69\x6c\x6f\xd1\x09\xcd\x39\x56\xf4\x5d\xc7\x09\xd2\xb3\x90\x21\x63",
            "CELR",
            18,
            "CelerToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x1c\x90\xae\xb2\xfd\x13\xea\x97\x2f\x0a\x71\xe3\x5c\x07\x53\x84\x8e\x3d\xb0",
            "CHEEL",
            18,
            "CHEELEE",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\xf0\x58\x72\x16\xa4\xa1\xbb\x7d\x50\x82\xfd\xc4\x91\xd9\x3d\x2d\xd4\xb4\x13",
            "CHEEMS",
            18,
            "Cheems",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\xce\xc8\xd5\x0f\x6c\x8a\xd3\xfb\x6d\xcc\xec\x57\x7e\x05\xaa\x32\xb2\x24\xfe",
            "CHR",
            6,
            "Chroma",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\xe1\x16\x22\x44\x59\x29\x6a\xbc\x78\x58\x62\x7a\xbd\x58\x79\x51\x4b\xc6\x29",
            "COL",
            18,
            "Clash of lilliput",
        )
        yield (  # address, symbol, decimals, name
            b"\x52\xce\x07\x1b\xd9\xb1\xc4\xb0\x0a\x0b\x92\xd2\x98\xc5\x12\x47\x8c\xad\x67\xe8",
            "COMP",
            18,
            "Compound Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x21\x28\xab\xc4\x11\x94\x74\xd1\x6b\xb0\xa0\x42\x00\xb6\x3b\x0e\x68\xa9\x71",
            "CONX",
            18,
            "Connex",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\xba\xf8\x8b\x39\xd3\x7d\xc6\x87\x75\xed\x15\x41\xf1\xbf\x83\xa5\xa4\x5f\xeb",
            "COTI",
            18,
            "COTI Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa8\xc2\xb8\xee\xc3\xd3\x68\xc0\x25\x3a\xd3\xda\xe6\x5a\x5f\x2b\xbb\x89\xc9\x29",
            "CTK",
            6,
            "CertiK Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\xa4\x43\xf8\x4f\xea\x71\x02\x66\xc8\xeb\x6b\xc3\x4b\x71\x70\x2d\x03\x3e\xf2",
            "CTSI",
            18,
            "Cartesi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\xf3\xf3\x29\xe8\xbe\x15\x40\x74\xd8\x76\x9d\x1f\xfa\x4e\xe0\x58\xb1\xdb\xc3",
            "DAI",
            18,
            "Dai Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x3d\xa9\x32\x45\x6d\x08\x2c\xba\x20\x8f\xeb\x0b\x09\x6d\x49\xb2\x02\xbf\x89\xc8",
            "DEGOV2",
            18,
            "dego.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\x88\x05\x6e\x83\x76\xae\x77\x09\x49\x6b\xa6\x4d\x37\xfa\x2f\x80\x15\xce\x3e",
            "DEXE",
            18,
            "Dexe",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\x8b\xbc\x59\x9e\xa0\x30\xaa\x69\xd0\x29\x80\x35\xdf\xe2\x63\x74\x0a\x95\xbc",
            "DHN",
            18,
            "Dohrnii",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\x95\x6d\x38\x05\x9c\xf7\xbe\xda\x96\xec\x91\xaa\x7b\xb2\x47\x7e\x09\x01\xdd",
            "DIA",
            18,
            "DIAToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\xee\x3c\xb0\x86\xf8\xa1\x6f\x34\xbe\xe3\xca\x72\xfa\xd3\x6f\x7d\xb9\x29\xe2",
            "DODO",
            18,
            "DODO bird",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\x88\xc6\x03\xd1\x42\xc3\x71\xea\x0e\xac\x87\x56\x12\x3c\x58\x05\xed\xee\x03",
            "DOG",
            18,
            "The Doge NFT",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x2a\xe4\x24\xd9\x60\xc2\x62\x47\xdd\x6c\x32\xed\xc7\x0b\x29\x5c\x74\x4c\x43",
            "DOGE",
            8,
            "DOGE",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x29\xbc\x0f\xfa\xf9\xbf\xf3\x37\xb3\x1c\xbe\x6c\xb5\xfb\x3b\xf1\x2e\x58\x40",
            "DOLA",
            18,
            "Dola USD Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x70\x83\x60\x9f\xce\x4d\x1d\x8d\xc0\xc9\x79\xaa\xb8\xc8\x69\xea\x2c\x87\x34\x02",
            "DOT",
            18,
            "Polkadot Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\xbd\x07\x49\xdb\xe2\x1f\x62\x3d\x9b\xab\xa8\x56\xd3\xb0\xf0\xe1\xbf\xec\x9c",
            "DUSK",
            18,
            "Dusk Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\xf0\x20\xa5\xc9\x2e\x15\xbe\x13\xca\xf0\xee\x5c\x95\xcf\x79\x58\x5e\xec\xc9",
            "ELF",
            18,
            "ELF Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\x53\x41\xc4\x9f\x44\x27\xe4\x3d\x99\xd8\x25\x4a\x8d\xd8\x70\x56\xf1\xee\x00",
            "ELG",
            18,
            "EscoinToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\xd6\xfa\xbd\x64\x81\x3c\x48\x54\x5c\x9c\x0e\x31\x2a\x00\x99\xd9\xbe\x25\x40",
            "ELON",
            18,
            "Dogelon Mars",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\xb6\xfb\x70\x8f\xc5\x73\x2d\xec\x1a\xfc\x8d\x85\x56\x42\x3a\x2e\xdc\xcb\xd6",
            "EOS",
            18,
            "EOS Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x3d\x65\x45\xb0\x86\x93\xda\xe0\x87\xe9\x57\xcb\x11\x80\xee\x38\xb9\xe3\xc2\x5e",
            "ETC",
            18,
            "Ethereum Classic",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x70\xed\x08\x80\xac\x9a\x75\x5f\xd2\x9b\x26\x88\x95\x6b\xd9\x59\xf9\x33\xf8",
            "ETH",
            18,
            "Ethereum Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\x16\x09\x2f\x14\x33\x78\x75\x0b\xb2\x9b\x79\xed\x96\x1a\xb1\x95\xcc\xee\xa5",
            "ezETH",
            18,
            "Renzo Restaked ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x6d\xe2\x60\x18\xa5\x4d\x51\xc0\x97\x16\x05\x68\x75\x2c\x4e\x3b\xd6\xc3\x64",
            "FBTC",
            8,
            "Fire Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xc5\xf0\xf7\xb6\x67\x64\xf6\xec\x8c\x8d\xff\x7b\xa6\x83\x10\x22\x95\xe1\x64\x09",
            "FDUSD",
            18,
            "First Digital USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\x1b\x41\xe5\x04\x67\x78\x79\x37\x0e\x9d\xbc\xf9\x37\x28\x3a\x86\x91\xfa\x7f",
            "FET",
            18,
            "Fetch",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x8c\xe2\xa9\x9b\xb6\xe3\xb7\xdb\x58\x0e\xd8\x48\x24\x0e\x4a\x0f\x9a\xe1\x53",
            "FIL",
            18,
            "Filecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xfb\x5b\x83\x8b\x6c\xfe\xed\xc2\x87\x3a\xb2\x78\x66\x07\x9a\xc5\x53\x63\xd3\x7e",
            "FLOKI",
            9,
            "FLOKI",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x43\xc5\x32\x0b\x9c\x18\xc1\x53\xd1\xe2\xd1\x2c\xc3\x07\x4b\xeb\xfb\x31\xa2",
            "FLOW",
            18,
            "FLOW",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\xf9\x08\x4f\x23\x74\x58\x58\x79\xe8\xb4\x34\xc3\x99\xe2\x9e\x80\xcc\xe6\x35",
            "FLUX",
            8,
            "Flux",
        )
        yield (  # address, symbol, decimals, name
            b"\x5b\x73\xa9\x3b\x4e\x5e\x4f\x1f\xd2\x7d\x8b\x3f\x8c\x97\xd6\x99\x08\xb5\xe2\x84",
            "FORM",
            18,
            "FORM Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x90\xc9\x7f\x71\xe1\x87\x23\xb0\xcf\x0d\xfa\x30\xee\x17\x6a\xb6\x53\xe8\x9f\x40",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\x29\xab\xb3\x18\x79\x1d\x57\x94\x33\xd8\x31\xed\x12\x2a\xfe\xaf\x29\xdc\xfe",
            "FTM",
            18,
            "Fantom",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\x45\x97\x1c\x16\x8b\x52\x94\xac\xbc\x87\x27\xa4\xf1\xc9\xe1\xaf\x99\xf6\xd0",
            "FTN",
            18,
            "Bridged FTN (OrtakSea)",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x8a\x3d\x7d\x0b\xc8\x8d\x55\x2f\x73\x0b\x62\xc0\x06\xbc\x92\x5e\xad\xb9\xee",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\xcc\x45\xbb\x5d\xbd\xa0\x6d\xb6\x18\x3e\x8b\xf0\x16\x56\x9f\x40\x49\x7a\xa5",
            "GAL",
            18,
            "Project Galaxy",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\x19\xbf\x2a\x2e\xf8\x04\x0c\x24\x2c\x9a\x4c\x5c\x4b\xd4\xc8\x16\x78\xb2\xa1",
            "GMT",
            8,
            "Green Metaverse Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x32\x87\xca\xe9\x9c\x98\x25\x86\xc0\x74\x01\xc0\xd9\x11\xbf\x7d\xe6\xcd\x82",
            "H2O",
            18,
            "H2O DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\xec\x80\x7c\xe2\xf4\xa6\xf2\x73\x7a\x92\xe9\x85\xf3\x18\xd0\x35\x88\x3e\x47",
            "HFT",
            18,
            "Hashflow",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xb7\x26\xb1\x14\x5a\x47\x73\xf6\x85\x93\xcf\x17\x11\x87\xd8\xeb\xe4\xd4\x95",
            "INJ",
            18,
            "Injective Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xe4\xfe\xa6\xe4\xf3\x63\x70\x25\xc7\xbc\xd8\x78\xe2\xb2\x38\xb0\x1f\x7d\x4e",
            "insurance",
            18,
            "insurance",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\x44\xf1\xd1\xe9\xd5\xf9\xbb\x90\xb6\x2f\x9d\x45\xe4\x47\xd9\x89\x58\x07\x82",
            "IOTA",
            6,
            "MIOTAC",
        )
        yield (  # address, symbol, decimals, name
            b"\x96\x78\xe4\x2c\xeb\xeb\x63\xf2\x31\x97\xd7\x26\xb2\x9b\x1c\xb2\x0d\x00\x64\xe5",
            "IOTX",
            18,
            "IoTeX Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\x37\xd7\x0b\x51\xff\xa2\xb9\x8b\x4d\x34\xa5\x71\x2c\x52\x91\x11\x54\x64\xe3",
            "IQ",
            18,
            "Everipedia IQ",
        )
        yield (  # address, symbol, decimals, name
            b"\x9b\xaf\xc8\xd4\xb4\x87\xce\xbf\xf2\x01\x72\x17\x02\x50\x7a\x3e\x2c\x67\xad\x79",
            "KAVA",
            18,
            "Kava",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\x03\xf7\xba\x57\xea\x95\x6b\x05\xb8\x5c\x60\xb4\xb2\xce\xea\x9b\x11\x12\x56",
            "KMD",
            18,
            "Komodo",
        )
        yield (  # address, symbol, decimals, name
            b"\xfe\x56\xd5\x89\x2b\xdf\xfc\x7b\xf5\x8f\x2e\x84\xbe\x1b\x2c\x32\xd2\x1c\x30\x8b",
            "KNC",
            18,
            "Kyber Network Crystal",
        )
        yield (  # address, symbol, decimals, name
            b"\xe6\xdf\x05\xce\x8c\x83\x01\x22\x33\x73\xcf\x5b\x96\x9a\xfc\xb1\x49\x8c\x55\x28",
            "KOGE",
            18,
            "BNB48 Club Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\x20\x80\x5e\x0b\xc7\xf0\xe3\x53\xd1\x34\x3d\x67\xa2\x39\xf4\x17\xd5\x7b\xbf",
            "KRD",
            18,
            "Krypton DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x98\x68\x54\x77\x98\x04\x79\x9c\x1d\x68\x86\x7f\x5e\x03\xe6\x01\xe7\x81\xe4\x1b",
            "LDO",
            18,
            "Lido DAO Token (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\xa0\xbf\x9c\xf5\x4b\xb9\x2f\x17\x37\x4d\x9e\x9a\x32\x1e\x6a\x11\x1a\x51\xbd",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x38\x66\x5c\xbb\x7b\x24\x85\xa8\x85\x5a\x13\x9b\x75\xd5\xe3\x4a\xb0\xdb\x94",
            "LTC",
            18,
            "Litecoin Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x7b\x22\x2f\xc7\x9e\x1c\xbb\xf8\xca\x5f\x78\xcb\x13\x3d\x1b\x7c\xf3\x4b\xbd",
            "LTO",
            18,
            "LTO Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\x6a\xb3\x34\x68\x23\xb6\x51\x29\x47\x66\xe2\x3e\x6c\xf8\x72\x54\xd6\x89\x62",
            "LUNA",
            6,
            "LUNA (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\xb1\x45\x8e\x78\x0f\x8f\xa7\x1e\x2f\x84\x50\x2c\xee\x8b\x5a\x3c\xc7\x31\xfa",
            "M",
            18,
            "MemeCore",
        )
        yield (  # address, symbol, decimals, name
            b"\xcc\x42\x72\x4c\x66\x83\xb7\xe5\x73\x34\xc4\xe8\x56\xf4\xc9\x96\x5e\xd6\x82\xbd",
            "MATIC",
            18,
            "Matic Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x67\x77\x13\xa2\xc6\x61\xec\xd0\xb2\xbd\x4d\x74\x85\x17\x0a\xa8\xc1\xec\xeb",
            "MCT",
            18,
            "MetaCraftToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\x73\xbb\x25\x05\xac\x46\x43\xd5\xc0\xa9\x9c\x2a\x1f\x34\xb3\xdf\xd0\x9d\x11",
            "MGC",
            9,
            "Meta Games Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\x0d\xa5\x99\xbb\x2c\xcc\xfc\xf6\xfd\xfd\x7d\x81\x74\x3b\x60\x20\x86\x43\x50",
            "MKR",
            18,
            "Maker",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x81\xaa\x8d\x68\x13\xec\x9d\x7e\x6d\xdb\x4e\x52\x3f\xb1\x60\x1a\x0e\x86\xf3",
            "MNT",
            18,
            "Mr Mint",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\x0a\x48\x21\xfd\xef\x15\x6b\x0d\x05\x1d\x08\xa1\x66\xde\x5d\xf2\x78\x8c\xf7",
            "MOG",
            18,
            "Mog Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\x8d\x02\xee\x3f\x80\xfb\xf5\xe3\x81\xf0\x49\x61\x60\x25\xc1\x86\x88\x9b\x68",
            "MRS",
            18,
            "Metars",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\x62\x95\xeb\xd6\xa9\x38\xc7\xaa\xf6\x13\x50\xeb\x51\x61\xa9\x93\x9a\xb2\xb7",
            "Mura",
            18,
            "Murasaki",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\x88\x25\x67\xa6\x2a\x55\x60\xd1\x47\xd6\x48\x71\x77\x6e\xea\x72\xdf\x41\xd3",
            "MX",
            18,
            "MX Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\xa4\xa7\x3a\x3f\x01\x33\xf0\x02\x53\x78\xaf\x00\x23\x6f\x3a\xbd\xee\x5d\x63",
            "NEAR",
            18,
            "NEAR Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\xc9\x00\x4e\xc7\xe5\x72\x28\x91\xf5\xf3\x8b\xae\x76\x78\xef\xcb\x11\xd3\x4d",
            "NFT",
            6,
            "APENFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\xee\x7b\x72\x0f\x4e\x4c\x4f\xfc\xb0\x0c\x40\x65\xcd\xae\x55\x27\x1a\xec\xca",
            "NFT",
            6,
            "APENFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x76\x96\x30\xb2\x36\x39\x8c\x24\x71\xeb\x26\xe6\xa5\x29\x44\x80\x30\xd9\x4f",
            "NKYC",
            18,
            "NONKYC",
        )
        yield (  # address, symbol, decimals, name
            b"\xf2\xb5\x1c\xc1\x85\x0f\xed\x93\x96\x58\x31\x7a\x22\xd7\x3d\x34\x82\x76\x75\x91",
            "NXPC",
            18,
            "NXPC",
        )
        yield (  # address, symbol, decimals, name
            b"\xf7\x8d\x2e\x79\x36\xf5\xfe\x18\x30\x8a\x3b\x29\x51\xa9\x3b\x6c\x4a\x41\xf5\xe2",
            "OM",
            18,
            "MANTRA DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\x8b\xfa\xea\xac\x8b\xda\xb6\xe9\xfc\x5e\xad\x8e\xdc\xb5\xf9\x5b\x05\x99\xd9",
            "ONG",
            9,
            "Poly-Peg ONG",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\x7b\x3a\x77\x84\x8f\x1c\x2d\x67\xe0\x5e\x54\xd7\x8d\x17\x4a\x0c\x85\x03\x35",
            "ONT",
            18,
            "Ontology Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\xca\x1f\x75\xec\xa6\x21\x43\x93\xfc\xe1\xc1\xb3\x16\xc2\x37\x66\x4e\xaa\x8e",
            "ORN",
            8,
            "Orion Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x0d\xda\x87\x62\xc2\x4d\xa9\x48\x7f\x5f\xa0\x26\xa9\xb6\x4b\x69\x5a\x07\xea",
            "OX",
            18,
            "OX Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x50\x86\x5a\x91\x40\xcb\x51\x93\x42\x43\x31\x46\xed\x5b\x40\xc6\xf2\x10\xf7",
            "PAXG",
            18,
            "PAX Gold",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\xed\x0a\x42\x61\x55\xb7\x9b\x89\x88\x49\x80\x3e\x3b\x36\x55\x2f\x7e\xd5\x07",
            "PENDLE",
            18,
            "Pendle",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\x12\xe5\x57\xd4\x00\x47\x47\x17\x05\x6c\x4e\x6d\x40\xed\xd8\x46\xf3\x83\x51",
            "PHA",
            18,
            "Phala Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x09\x63\x3a\x72\xd8\x46\xfc\x5b\xbe\x2f\x98\xd8\x85\x64\xd3\x59\x87\x90\x4d",
            "PHB",
            18,
            "Phoenix Global",
        )
        yield (  # address, symbol, decimals, name
            b"\x63\x1c\x2f\x0e\xda\xba\xc7\x99\xf0\x75\x50\xae\xe4\xff\x0b\xf7\xfd\x35\x21\x2b",
            "PLT",
            18,
            "Poollotto.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x53\xd5\x6f\xf9\x9f\x13\x22\x51\x5e\x54\xfd\xde\x93\xff\x8b\x3b\x7d\xaf\xd5",
            "PROM",
            18,
            "Prometeus",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x79\xc8\x5c\x6d\x63\x3a\xe8\x1c\x97\xbe\x71\xe1\x69\x1e\xe7\xdc\x6e\x13\x2d",
            "PXT",
            18,
            "PIXER",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\x49\xa5\x3e\x1f\x15\xfd\x7f\xd5\x22\xe0\x69\x1c\xb5\x70\xf4\x42\xe9\xca\x6c",
            "QGold",
            3,
            "Quorium",
        )
        yield (  # address, symbol, decimals, name
            b"\x12\xbb\x89\x05\x08\xc1\x25\x66\x1e\x03\xb0\x9e\xc0\x6e\x40\x4b\xc9\x28\x90\x40",
            "RACA",
            18,
            "Radio Caca V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\xb6\xa5\x56\x62\xf6\x59\x1f\x8b\x84\x08\xaf\x1c\x73\xb0\x17\xe3\x2e\xed\xb8",
            "RAY",
            6,
            "(Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x49\xa5\x10\x75\x62\x24\xe8\x87\xb3\xd9\x9d\x00\xd9\x59\xf2\xd8\x6d\xda\x1c",
            "RIO",
            18,
            "Realio Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\xc5\xab\xcc\x56\x87\x4d\x7f\xac\xb9\x0c\x2c\x38\x12\xcc\x19\xaa\xf9\xb2\x04",
            "RZ",
            18,
            "RZ coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\xff\xb4\x11\xc4\xa7\x0a\xa7\xc9\x5d\x5c\x98\x1a\x6f\xb4\xda\x86\x7e\x11\x11",
            "SAHARA",
            18,
            "Sahara AI",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x1f\xdb\x03\xba\x84\x76\x2d\xd6\x6a\x0a\xf1\xa6\xc8\x54\x0f\xf1\xba\x5d\xfb",
            "SFP",
            18,
            "SafePal Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x3c\xd5\x53\x56\x43\x3c\x89\xe5\x0d\xc5\x1a\xb0\x7e\xe0\xfa\x0a\x95\x62\x3d\x53",
            "sfrxETH",
            18,
            "Staked Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x28\x59\xe4\x54\x4c\x4b\xb0\x39\x66\x80\x3b\x04\x4a\x93\x56\x3b\xd2\xd0\xdd\x4d",
            "SHIB",
            18,
            "SHIBA INU",
        )
        yield (  # address, symbol, decimals, name
            b"\x87\x26\x64\x13\xe5\xb6\x4d\xb7\x2f\x11\xbb\x67\x95\xee\x97\x65\x45\xdb\xaf\x43",
            "SHIBTC",
            18,
            "Shibabitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x91\xa4\x83\x53\x8d\xee\xa8\xef\x58\x3b\x41\x31\x66\xa9\x1e\x70\x9b\xd2\xc8\xf6",
            "SIR",
            18,
            "Poo Chi",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x08\x51\x47\x61\x80\xbf\xc4\x99\xea\x68\x45\x0a\x53\x27\xd2\x1c\x9b\x05\x0e",
            "SLAM",
            18,
            "SLAM TOKEN",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xb8\x4d\x29\x4e\x0c\x75\xa6\xab\xe6\x01\x71\xb7\x0e\xde\xb2\xef\xd1\x4a\x1b",
            "slisBNB",
            18,
            "Synclub Staked BNB",
        )
        yield (  # address, symbol, decimals, name
            b"\x07\x0a\x08\xbe\xef\x8d\x36\x73\x4d\xd6\x7a\x49\x12\x02\xff\x35\xa6\xa1\x6d\x97",
            "SLP",
            18,
            "Smooth Love Potion",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\xc9\x83\x82\x60\x58\xb8\xa9\xc7\xaa\x1c\x91\x71\x44\x11\x91\x23\x2e\x84\x04",
            "SNX",
            18,
            "Synthetix Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\x0a\x5d\x26\xf7\x76\x5e\xcb\x71\x2c\x09\x24\xe4\xde\x54\x5b\x89\xfd\x43\xdf",
            "SOL",
            18,
            "SOLANA",
        )
        yield (  # address, symbol, decimals, name
            b"\x4a\xae\x82\x3a\x6a\x0b\x37\x6d\xe6\xa7\x8e\x74\xec\xc5\xb0\x79\xd3\x8c\xbc\xf7",
            "SolvBTC",
            18,
            "Solv BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\x46\xb6\x18\xdc\x92\x81\x0e\xc7\x41\x63\xe4\xc2\x70\x04\xc9\x21\xd4\x46\xa5",
            "SolvBTC.BBN",
            18,
            "SolvBTC Babylon",
        )
        yield (  # address, symbol, decimals, name
            b"\xf6\x71\x8b\x27\x01\xd4\xa6\x49\x8e\xf7\x7d\x7c\x15\x2b\x21\x37\xab\x28\xb8\xa3",
            "stBTC",
            18,
            "Lorenzo stBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x9b\x17\xba\xad\xf0\xf2\x1f\x03\xe3\x52\x49\xe0\xe5\x97\x23\xf3\x49\x94\xf8\x06",
            "SURE",
            18,
            "inSure",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\x79\x50\xbc\xc7\x48\x88\xa4\x0f\xfa\x25\x93\xc5\x79\x8f\x11\xfc\x91\x24\xc4",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\xbe\xad\x25\x63\xdc\xbf\x3b\xf2\xc9\x40\x7f\xea\x4d\xc2\x36\xfa\xba\x48\x5a",
            "SXP",
            18,
            "Swipe",
        )
        yield (  # address, symbol, decimals, name
            b"\xa4\x08\x0f\x17\x78\xe6\x94\x67\xe9\x05\xb8\xd6\xf7\x2f\x6e\x44\x1f\x9e\x94\x84",
            "SYN",
            18,
            "Synapse",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\x22\x22\x7e\x22\x10\x2f\xe3\x32\x20\x98\xe4\xcb\xfe\x18\xcf\xeb\xd5\x7c\x95",
            "TLM",
            4,
            "Alien Worlds Trilium",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\xa7\x97\xa5\x9b\xa2\xc1\x77\x26\x89\x69\x76\xb7\xb3\x74\x7b\xfd\x1d\x22\x0f",
            "TONCOIN",
            9,
            "Wrapped TON Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\xea\xc5\xac\x2f\x75\x86\x18\xdf\xa0\x9b\xdb\xe0\xcf\x17\x4e\x7d\x57\x4d\x5b",
            "TRX",
            18,
            "TRON",
        )
        yield (  # address, symbol, decimals, name
            b"\xce\x7d\xe6\x46\xe7\x20\x8a\x4e\xf1\x12\xcb\x6e\xd5\x03\x8f\xa6\xcc\x6b\x12\xe3",
            "TRX",
            6,
            "TRON",
        )
        yield (  # address, symbol, decimals, name
            b"\xc1\xfd\xbe\xd7\xda\xc3\x9c\xae\x2c\xcc\x07\x48\xf7\xa8\x0d\xc4\x46\xf6\xa5\x94",
            "TRYB",
            6,
            "BiLira",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\xaf\x38\x27\xf3\x9d\x0e\xac\xbf\x4a\x16\x8f\x8d\x4e\xe6\x7c\x12\x1d\x11\xc9",
            "TUSD",
            0,
            "TrueUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x4b\x0f\x18\x12\xe5\xdf\x2a\x09\x79\x64\x81\xff\x14\x01\x7e\x60\x05\x50\x80\x03",
            "TWT",
            18,
            "Trust Wallet",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\x51\x40\xa2\x25\x78\x16\x8f\xd5\x62\xdc\xcf\x23\x5e\x5d\x43\xa0\x2c\xe9\xb1",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\x8a\x3e\x0b\x1f\x84\x2c\x93\x06\xb7\x83\xf8\xa4\x07\x8c\x6c\x8c\x03\xa2\x70",
            "USD0",
            18,
            "Usual USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x0d\x00\x0e\xe4\x49\x48\xfc\x98\xc9\xb9\x8a\x4f\xa4\x92\x14\x76\xf0\x8b\x0d",
            "USD1",
            18,
            "World Liberty Financial USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\xc7\x6a\x51\xcc\x95\x0d\x98\x22\xd6\x8b\x83\xfe\x1a\xd9\x7b\x32\xcd\x58\x0d",
            "USDC",
            18,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x39\x20\x04\xbe\xe2\x13\xf1\xff\x58\x0c\x86\x73\x59\xc2\x46\x92\x4f\x21\xe6\xad",
            "USDD",
            18,
            "Decentralized USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\x74\x79\x99\x7f\x34\xdd\x91\x56\xde\xef\x8f\x95\xa5\x2d\x81\xd2\x65\xbe\x9c",
            "USDD",
            18,
            "Decentralized USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x5d\x3a\x1f\xf2\xb6\xba\xb8\x3b\x63\xcd\x9a\xd0\x78\x70\x74\x08\x1a\x52\xef\x34",
            "USDe",
            18,
            "USDe",
        )
        yield (  # address, symbol, decimals, name
            b"\x55\xd3\x98\x32\x6f\x99\x05\x9f\xf7\x75\x48\x52\x46\x99\x90\x27\xb3\x19\x79\x55",
            "USDT",
            18,
            "USDT",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\x7c\x5b\x74\xc9\xb4\x04\x47\xa9\x54\xe1\x46\x69\x38\xb8\x65\xb6\xbb\xea\x36",
            "vBNB",
            8,
            "Venus BNB",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\x2c\x17\x3b\xc7\xff\x3b\x77\x86\xca\x16\xdf\xed\x3d\xff\xfb\x9e\xe7\x84\x7b",
            "vBTC",
            8,
            "Venus BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\xc7\x82\x22\xb3\xd6\xe2\x62\x42\x64\x83\xd4\x2c\xfa\x53\x68\x5a\x67\xab\x9d",
            "vBUSD",
            8,
            "Venus BUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xe6\xe4\xd9\xe1\xdd\xd7\x83\xb6\xbe\xec\xcc\x05\x9a\xbc\x17\xbe\x88\xee\x1a\x03",
            "VERUM",
            8,
            "Verum Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\xdc\xdf\xef\x7c\x49\x64\x07\xcc\xb0\xce\xc9\x0f\x9c\x5a\xaa\x1c\xc8\xd8\x88",
            "VET",
            18,
            "VeChain",
        )
        yield (  # address, symbol, decimals, name
            b"\xf5\x08\xfc\xd8\x9b\x8b\xd1\x55\x79\xdc\x79\xa6\x82\x7c\xb4\x68\x6a\x35\x92\xc8",
            "vETH",
            8,
            "Venus ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\xf3\xd0\xf6\x99\x0a\x40\x26\x1c\x66\xe1\xff\x20\x17\xac\xbc\x28\x2e\xb6\xd0",
            "vSXP",
            8,
            "Venus SXP",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\xa8\x81\x25\xa5\xad\xbe\x82\x61\x4f\xfc\x12\xd0\xdb\x55\x4e\x2e\x28\x67\xc8",
            "vUSDC",
            8,
            "Venus USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\x58\x40\xcd\x36\xd9\x4d\x72\x29\x43\x98\x59\xc0\x11\x2a\x41\x85\xbc\x02\x55",
            "vUSDT",
            8,
            "Venus USDT",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\xaf\x24\x0c\x90\xf9\x89\x84\x7c\x56\xac\x9d\xee\x75\x4f\x76\xf4\x1c\x58\x33",
            "wARRR",
            8,
            "Wrapped Pirate Chain",
        )
        yield (  # address, symbol, decimals, name
            b"\x90\xbb\xdd\xbf\x32\x23\x36\x38\x98\x06\x5b\x9c\x73\x6e\x2b\x86\xc6\x55\x76\x2b",
            "wBDX",
            9,
            "Wrapped BELDEX",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\x4c\xdb\x9c\xbd\x36\xb0\x1b\xd1\xcb\xae\xbf\x2d\xe0\x8d\x91\x73\xbc\x09\x5c",
            "WBNB",
            18,
            "Wrapped BNB",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\xb5\xa6\x6e\x93\x7a\x9f\x44\x73\xfa\x95\xb1\xca\xf1\xd1\xe1\xd6\x2e\x29\xea",
            "WETH",
            18,
            "Wrapped Ether (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\x47\x47\x47\x47\x7b\x19\x92\x88\xbf\x72\xa1\xd7\x02\xf7\xfe\x0f\xb1\xde\xea",
            "WLFI",
            18,
            "World Liberty Financial",
        )
        yield (  # address, symbol, decimals, name
            b"\xc8\x36\xd8\xdc\x36\x1e\x44\xdb\xe6\x4c\x48\x62\xd5\x5b\xa0\x41\xf8\x8d\xdd\x39",
            "WMATIC",
            18,
            "Wrapped Matic (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x91\x93\x7a\x75\x08\x86\x0f\x87\x6c\x9c\x0a\x2a\x61\x7e\x7d\x9e\x94\x5d\x4b",
            "WOO",
            18,
            "Wootrade Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xf0\x06\x00\xeb\xc7\x63\x34\x62\xbc\x4f\x9c\x61\xea\x2c\xe9\x9f\x5a\xae\xbd\x4a",
            "wROSE",
            18,
            "Wrapped ROSE",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\x75\x56\xa7\x49\x3f\xb7\xa2\x2d\x2f\xd1\x58\xdd\x4c\x76\x6a\x98\x70\x5f\x96",
            "WZEDX",
            18,
            "Wrapped ZEDXION from ZEDX",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\x24\xc7\xc0\xd9\x5c\xeb\xc7\x3e\xea\x7e\x85\xcb\xaa\xc0\xdb\xdf\x88\xa0\x5b",
            "XCN",
            18,
            "Chain (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\x2f\x0d\xa1\x69\xce\xb9\xfc\x7b\x31\x44\x62\x8d\xb1\x56\xf3\xf6\xc6\x0d\xbe",
            "XRP",
            18,
            "XRP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\xf1\xa5\xae\x2a\x3b\xf9\x8a\xea\xf3\x42\xd2\x6b\x30\xa7\x94\x38\xc9\x14\x2e",
            "YFI",
            18,
            "yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\xa4\x2e\x51\x93\xdf\xa8\xb0\x3d\x15\xdd\x1b\x86\xa3\x11\x3b\xbb\xef\x8e\xeb",
            "ZEC",
            18,
            "Zcash Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x90\x7e\x0a\x72\xc3\xd5\x56\x27\xe8\x53\xf4\xec\x6a\x96\xb0\xc8\x77\x11\x45",
            "ZIG",
            18,
            "ZigCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xb8\x6a\xbc\xb3\x7c\x3a\x4b\x64\xf7\x4f\x59\x30\x1a\xff\x13\x1a\x1b\xec\xc7\x87",
            "ZIL",
            12,
            "Zilliqa",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x1b\x5f\x63\x13\x54\xbe\x68\x53\xef\xe9\xc3\xab\x6b\x95\x90\xf8\x30\x2e\x81",
            "ZK",
            18,
            "Polyhedra Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x85\x88\x4c\x43\x92\xd3\x48\x58\x7b\x19\xcb\x9e\xaa\xf1\x57\xf1\x32\x71\xcd",
            "ZRO",
            18,
            "LayerZero",
        )
    if chain_id == 66:  # okt
        yield (  # address, symbol, decimals, name
            b"\x7b\x2d\xc8\x21\xea\x26\x67\x46\x2b\x86\x35\xf3\xc2\x92\x8f\x52\xe3\xad\x4d\x27",
            "ACMD",
            18,
            "ACMD",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x3b\x65\x2b\x0e\xf7\xb0\xfa\x6e\x22\x38\xc6\xe8\xa8\xde\x3a\x32\xa5\xd7\x25",
            "BabyNewY",
            9,
            "BabyNewY",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xa5\x43\x94\xaf\x05\x61\x07\x3f\x5d\xdd\xee\x23\xf1\x1e\x17\x88\xb5\xa3\x82",
            "Bamboo",
            6,
            "Bamboo",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\xc4\xfd\x75\x79\x13\x37\x01\xfa\x27\x69\xb6\x95\x5e\x7e\x56\xbb\x38\x6d\xb1",
            "BRG",
            18,
            "Bridge",
        )
        yield (  # address, symbol, decimals, name
            b"\x54\xe4\x62\x2d\xc5\x04\x17\x6b\x3b\xb4\x32\xdc\xca\xf5\x04\x56\x96\x99\xa7\xff",
            "BTCK",
            18,
            "BTCK",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\xb6\x22\x49\x4a\xb7\xc5\xe8\x19\x11\x55\x8c\x95\x52\xdb\xd5\x17\xf4\x03\xfb",
            "Celt",
            18,
            "Celestial",
        )
        yield (  # address, symbol, decimals, name
            b"\x81\x79\xd9\x7e\xb6\x48\x88\x60\xd8\x16\xe3\xec\xaf\xe6\x94\xa4\x15\x3f\x21\x6c",
            "CHE",
            18,
            "CherrySwap",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x3e\x31\xec\x03\x96\xa4\x2a\xfe\xe2\xac\x01\xe8\x1b\x7f\x22\x92\x29\xed\x35",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x45\xf3\x60\xb9\x1e\xb6\x98\x3a\x71\x9e\xd4\x52\x0a\x1b\xd5\x9c\x83\x30\x8e",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xb8\xc9\x8d\x89\xd1\xa9\xd7\x09\x57\xf8\xc4\xe3\xac\x1d\xef\xfa\xa9\x65\x99",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xdf\xcd\x08\x7a\x25\x5e\x80\xf7\x3d\x65\xa0\x21\xff\x1f\xa8\xd1\x12\xcf\x3f",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xfe\x72\x4f\xef\x7d\x88\x6f\x09\xc3\x84\xeb\xdc\xfe\x7f\xf5\x63\x9b\x15\xaf",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x09\x10\x8f\x44\x56\xe3\x90\xe6\x71\x8e\x79\xc9\xf6\xa9\x0d\x1e\x46\x9b\xcd",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x17\x53\x13\xfb\x7e\x1e\x9d\x9d\x13\xfb\xe4\x26\x16\x4a\x7d\x23\xd3\x1f\x16",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xae\x13\x89\xe7\x6a\x4b\xaa\xca\x6f\x37\x1d\xc7\xd8\x36\x72\x68\x37\x39\x0e",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xc1\x36\x2e\x13\x68\x9b\x22\xbb\x89\xf1\xd3\x88\x72\x4d\x28\xc0\x91\x94\x57",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xe2\xa2\xea\x2f\x4b\x7c\xb7\x3b\x88\x29\xfb\x03\x57\x2d\x08\xf1\xb8\xaf\x13",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xf6\x11\x77\x08\x68\x23\x75\x7b\xce\x91\x24\x5f\x52\x4f\x98\xd6\xd4\x5f\x40",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x08\xc9\x5f\x0c\xe0\x44\x23\x58\x39\x28\xc0\xc0\xbb\xeb\x51\xbe\x3c\xe4\xee",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x22\xee\xca\xb2\x9f\xfe\xcf\x49\x43\x51\xb8\xd4\x16\xc6\x55\xb2\x1b\x4c\xcf",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x47\xd1\x59\x1e\x01\xc4\xac\xbc\xcd\x95\xb7\xbc\xe7\x21\x84\x74\xff\x55\x60",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x59\x9b\x56\xe8\xf8\xeb\x7d\xcd\x82\x1e\x65\x88\x9c\xa6\xce\xb0\x1e\x55\x50",
            "Che-LP",
            18,
            "Cherry LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xdb\xa7\x79\x4b\x3a\xbc\xb7\x0c\x3b\x46\x08\x21\x4e\x52\x19\xed\x5e\xff\x76",
            "demo",
            18,
            "demo",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x39\xaa\x66\xf2\xd1\xa4\xbb\x65\x4b\x67\x0c\xbb\x9f\x30\x21\xe0\x19\xa6\xb8",
            "HDTT",
            18,
            "HDTT",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xea\x1e\xc0\xb1\x1f\x24\x9e\x2c\x15\x0a\x37\x30\x50\xe8\x1a\xb7\x10\x35\x41",
            "HMD",
            9,
            "HMD",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xf9\x63\xc6\x3c\x7b\xae\xa5\x93\x79\x07\xf4\x86\x3d\xd8\x3d\x77\x1f\x6c\x4b",
            "iDFOX",
            18,
            "iDeFiFox Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x36\x17\x2a\x9a\x3c\x15\x26\xec\x61\x41\x31\xb2\x56\x83\x4a\xc0\x2f\xd0\x20",
            "JLP",
            18,
            "Jswap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x4b\x2b\xdb\x16\x0d\xf2\x4b\xc7\x5c\x2f\x36\xe5\xcc\xe9\x1d\x75\x78\x14\x5d",
            "JLP",
            18,
            "Jswap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xf9\x74\xac\x75\xe0\x97\x43\xb5\x37\xe4\x7a\xdc\xb9\x68\x74\xe9\x35\x5e\xbf",
            "JLP",
            18,
            "Jswap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x42\xb4\x68\x04\xeb\x9b\x21\x01\x3a\x24\xc7\x26\x8c\xd1\x58\x8d\x8d\x2a\xaf",
            "JLP",
            18,
            "Jswap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xb7\x2f\x9d\xd5\xd6\x9d\xfa\x29\x0f\x69\x94\x84\xc8\x7c\x35\x70\xbf\xb1\xb1",
            "JLP",
            18,
            "Jswap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xd6\x17\xfa\xaf\x11\xf9\xe5\xbd\xf1\x0a\x3b\x2b\x6a\x25\xf5\xd7\xb4\xc8\xa0",
            "JLP",
            18,
            "Jswap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xca\x72\x03\x69\x2b\x0b\x6b\x63\x07\x66\x30\xb4\xb7\x02\xe1\x6f\xb1\xb6\xf0",
            "l0WordToken",
            18,
            "l0Word.com",
        )
        yield (  # address, symbol, decimals, name
            b"\x08\x96\x3d\xb7\x42\xab\x15\x9f\x27\x51\x8d\x1d\x12\x18\x8f\x69\xaa\x73\x87\xfb",
            "lowb",
            18,
            "loser coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xc7\x52\x70\xe8\x94\x32\xcc\x05\xfa\xd8\xc8\xf2\x90\x45\x84\x72\xe6\xaa\xcb",
            "META",
            8,
            "cpmeta",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x87\xfc\x0f\x05\xba\x3e\xf0\x0d\x83\x4f\x7b\x2a\xda\xce\xb0\x88\x22\xbe\x1d",
            "MontyPython_Dividend",
            18,
            "MontyPython_Dividend_Tracker",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\xdf\x88\x37\x15\xa1\x2f\x08\x5f\xfe\x54\xe9\x07\x33\xa1\x2c\x7c\x9c\x81\x28",
            "NABOX",
            18,
            "Nabox Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xa1\x15\x54\x30\x39\x9a\x78\x8d\xb8\x57\xa8\xb4\x9c\xea\x1e\xa9\x0c\xb1\xd8",
            "NOS",
            8,
            "NOSF",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\x35\x1e\xc1\x5e\x85\x11\x65\x8c\x2b\xa1\xe1\xe8\x1e\x1e\x60\xaa\x39\xc9\xcd",
            "NVT",
            8,
            "NerveNetwork",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x98\x01\x66\x9c\x61\x38\xe8\x4b\xd5\x0d\xeb\x50\x08\x27\xb7\x76\x77\x7d\x28",
            "O3",
            18,
            "O3 SwapToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x54\xb6\xc6\x19\x5e\xa4\xd9\x48\xd0\x3b\xfd\x81\x8d\x36\x5c\xf1\x75\xcf\xc2",
            "OKB",
            18,
            "OKB",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x5d\x5f\x0e\x64\x8d\xeb\x4f\x38\xcc\xa7\xda\x0d\xa8\x87\xa7\x44\xfc\x37\x95",
            "OkBaby",
            18,
            "OKBABY",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x2e\x41\x6a\x66\xf2\x26\xdf\x3e\x77\xcc\xa0\xaa\x86\xac\x88\x9d\x6b\xf8\x87",
            "OKM",
            9,
            "OKmom",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xcb\x16\x5a\xbe\xff\x45\x05\xf0\xe0\xa2\xfa\xb7\x63\xf5\xf4\x14\xc6\x1c\x38",
            "OKMINICHE",
            9,
            "OKMINICHE",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x8b\x08\x41\x77\xb2\xdd\x27\xaa\x97\x26\xd9\xae\xc1\x6a\xbf\xbb\xe6\x51\xfe",
            "OKXBABYDOGE",
            6,
            "OKXBABYDOGE",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xf4\xcf\xe6\x08\xac\xa7\xe5\x64\x4e\x66\x45\x00\x1b\xee\xed\xeb\xbd\x36\xec",
            "PoolToken",
            18,
            "PoolToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x67\x1b\x75\x19\x68\x01\xb1\xa0\xa3\xeb\xc4\xac\x63\xd5\x96\x08\x56\x25\x01",
            "R U OK",
            9,
            "R U OK",
        )
        yield (  # address, symbol, decimals, name
            b"\x12\xbb\x89\x05\x08\xc1\x25\x66\x1e\x03\xb0\x9e\xc0\x6e\x40\x4b\xc9\x28\x90\x40",
            "RACA",
            18,
            "Radio Caca V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x77\xa9\xc1\x60\x2e\xc3\x5c\xe9\xe6\xc2\xca\x64\x9d\xe1\xf7\x88\x6d\xb3\xbf",
            "SCHE",
            18,
            "SincereCherry",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x66\xe4\xde\x88\x41\xe8\x94\x71\x8f\x96\x41\x5d\x58\xaa\x66\xff\x11\xb1\x61",
            "SLP",
            18,
            "SakeSwap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x3e\x92\xa8\x89\x64\x01\xd2\x56\x10\x59\xd0\xc2\x6f\x3e\x26\xab\xd9\x91\x35",
            "STK",
            18,
            "Stakeprotocal",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x42\x26\x9b\x7d\xd2\x67\xcc\xcc\xcb\x73\x5e\x5c\x7d\x5c\xed\x2f\x7f\xc7\xb5",
            "sun",
            9,
            "sun",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xa8\xe4\xd4\x87\x79\xe8\x14\x71\xb3\x3a\x68\xf4\xd0\x76\xea\x3b\x84\xaa\x48",
            "TIMING",
            18,
            "TimingToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x46\x8d\xb5\x5a\xa3\x37\xe1\x18\x7c\x5e\xf2\xdf\xde\x4f\xd8\x0a\xf6\x08\xfc",
            "TOK",
            18,
            "TOK Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xb7\x47\x16\x56\x8c\x34\x6e\xb0\xe3\x75\xd4\x8c\xaf\xfa\xb7\x3f\x4d\x4a\x7d",
            "TST",
            4,
            "TST",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x46\xda\xf8\x1b\x08\x14\x6b\x1c\x7a\x8d\xa2\xa8\x51\xdd\xf2\xb3\xea\xaf\x85",
            "USDC",
            18,
            "USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\xac\x52\xe0\x01\xf5\xbd\x41\x3a\xa6\xea\x83\x95\x64\x38\xf2\x90\x98\x16\x6b",
            "USDK",
            18,
            "USDK",
        )
        yield (  # address, symbol, decimals, name
            b"\x38\x2b\xb3\x69\xd3\x43\x12\x5b\xfb\x21\x17\xaf\x9c\x14\x97\x95\xc6\xc6\x5c\x50",
            "USDT",
            18,
            "USDT",
        )
        yield (  # address, symbol, decimals, name
            b"\x2c\x9a\x1d\x0e\x12\x26\x93\x9e\xdb\x7b\xbb\x68\xc4\x3a\x08\x0c\x28\x74\x3c\x5c",
            "VEMP",
            18,
            "vEmpire Gamer Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\xcd\xd4\xf6\x66\xc8\x4f\xed\x1d\x8b\xd8\x25\xaa\x76\x2e\x37\x14\xf6\x52\xc9",
            "VINU",
            18,
            "Vita Inu",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x47\xab\x30\x5b\x8a\x2a\x3f\x40\x20\xd1\x3f\xa9\xef\x73\xcd\xdc\xc0\xe7\xd4",
            "WING",
            9,
            "Wing Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xc5\x05\x1c\xf0\x5a\xf7\xed\x17\x16\xa0\xb0\xc4\x58\xa5\x63\xce\xd8\x0c\x2f",
            "XBXH",
            18,
            "XBXH",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\xa6\x6b\x67\x88\xb2\x21\xec\x34\x8d\x1e\xca\x65\xf0\x56\x7e\x47\x28\x1c\x8c",
            "_Dividend_Tracker",
            18,
            "_Dividend_Tracker",
        )
    if chain_id == 137:  # pol
        yield (  # address, symbol, decimals, name
            b"\x9c\x2c\x5f\xd7\xb0\x7e\x95\xee\x04\x4d\xde\xba\x0e\x97\xa6\x65\xf1\x42\x39\x4f",
            "1INCH",
            18,
            "1Inch (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xd6\xdf\x93\x2a\x45\xc0\xf2\x55\xf8\x51\x45\xf2\x86\xea\x0b\x29\x2b\x21\xc9\x0b",
            "AAVE",
            18,
            "Aave (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\x7e\x49\x12\x5a\x65\x95\x58\x8c\x95\x56\xf0\x7a\x4c\x00\x64\x61\xb2\x45\x45",
            "AKI",
            18,
            "Aki Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\xc7\x05\x07\x38\x40\x47\xaa\x74\xc2\x9c\xdc\x8c\x5c\xb8\x8d\x0f\x72\x13\xac",
            "ALI",
            18,
            "Artificial Liquid Intelligence Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\x1a\x02\x32\x70\x36\x8c\x0d\x50\xbf\xfb\x62\x78\x0f\x4a\xfd\x4e\xa7\x9c\x35",
            "ANKR",
            18,
            "Ankr (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xb7\xb3\x1a\x6b\xc1\x8e\x48\x88\x85\x45\xce\x79\xe8\x3e\x06\x00\x3b\xe7\x09\x30",
            "APE",
            18,
            "ApeCoin (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\xf7\x51\x66\x2e\x28\x2e\x83\xec\x3c\xbc\x38\x7d\x22\x5c\xa5\x6d\xd6\x3d\x3a",
            "APEPE",
            18,
            "Ape and Pepe",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x0a\xb1\x20\xdb\xd1\x1b\xa7\x01\xaf\xf6\x74\x85\x68\x31\x16\x68\xf6\x3f\xe0",
            "APW",
            18,
            "APWine Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\x37\x17\x09\x0c\xdd\xc9\xb2\x27\xe4\x9d\x0d\x84\xa2\x8a\xc0\xa9\x96\xe6\xff",
            "ASK",
            18,
            "Permission Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\x51\xc4\xc4\x8d\xc3\x11\x64\x87\xed\x4b\xc1\x65\x42\xe2\x7b\x56\x94\xda\x1b",
            "ATOM",
            6,
            "Cosmos (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x2c\x89\xbb\xc9\x2b\xd8\x6f\x80\x75\xd1\xde\xcc\x58\xc7\xf4\xe0\x10\x7f\x28\x6b",
            "AVAX",
            18,
            "Avalanche Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\x4e\x62\x41\x06\xcb\x12\xe1\x68\xe6\x53\x3f\x8e\xc7\xc8\x22\x63\x35\x89\x40",
            "AXL",
            6,
            "Axelar",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\xcb\xe5\x93\x7b\x0c\xc2\xad\xf6\x97\x72\xd2\x28\xfa\x42\x05\xac\xf4\xd9\xb2",
            "BADGER",
            18,
            "Badger (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\x71\x01\x2b\x13\xca\x4d\x3d\x0c\xdc\x72\xa1\x77\xdf\x3e\xf0\x3b\x0e\x76\xa3",
            "BAL",
            18,
            "Balancer (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x3c\xef\x98\xbb\x43\xd7\x32\xe2\xf2\x85\xee\x60\x5a\x81\x58\xcd\xe9\x67\xd2\x19",
            "BAT",
            18,
            "Basic Attention Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x7c\xc8\xef\x14\xf3\xdc\x76\xee\x2f\xb6\x00\x28\x74\x9e\x1b\x61\xce\xa1\x62",
            "BB",
            18,
            "BitBoard",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x80\x0d\xb0\xfd\xb5\x22\x3b\x3c\x3f\x35\x48\x86\xd9\x07\xa6\x71\x41\x4a\x7f",
            "BCT",
            18,
            "Toucan Protocol: Base Carbon Tonne",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\x86\x49\x87\x4b\xcd\xb7\xa9\xd1\x66\x6e\x66\x5f\x34\x07\x23\xa0\x18\x74\x82",
            "BIM",
            18,
            "BIM",
        )
        yield (  # address, symbol, decimals, name
            b"\xe5\xb4\x98\x20\xe5\xa1\x06\x3f\x6f\x4d\xdf\x85\x13\x27\xb5\xe8\xb2\x30\x10\x48",
            "BONK",
            5,
            "Bonk",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\x9e\x5f\xd8\xbb\xc2\x59\x84\xb1\x78\xfd\xce\x61\x17\xde\xfa\x39\xd2\xdb\x39",
            "BUSD",
            18,
            "BUSD Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x9b\x76\x57\x35\xc8\x2b\xb0\x00\x85\xe9\xdb\xf1\x94\xf2\x0e\x3f\xa7\x54\x25\x8e",
            "CARR",
            18,
            "Carnomaly",
        )
        yield (  # address, symbol, decimals, name
            b"\x4e\xc2\x03\xdd\x06\x99\xfa\xc6\xad\xaf\x48\x3c\xdd\x25\x19\xbc\x05\xd2\xc5\x73",
            "CBK",
            18,
            "Cobak Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xd8\x5d\x1e\x94\x57\x66\xfe\xa5\xed\xa9\x10\x3f\x91\x8b\xd9\x15\xfb\xca\x63\xe6",
            "CEL",
            4,
            "Celsius (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xf1\x93\x8c\xe1\x24\x00\xf9\xa7\x61\x08\x4e\x7a\x80\xd3\x7e\x73\x2a\x4d\xa0\x56",
            "CHZ",
            18,
            "CHZ (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x12\x59\x8e\x36\x16\x26\x1d\xf1\xc0\x5e\xc2\x8d\xe0\xd2\xfb\x10\xc1\xf2\x06",
            "COCA",
            18,
            "COCA",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x05\xb9\xd2\x25\x4a\x7a\xe4\x68\xc0\xe9\xdd\x10\xcc\xea\x3a\x83\x7a\xef\x5c",
            "COMP",
            18,
            "(PoS) Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\x40\x48\x04\xba\x58\x3c\x02\x5f\xa6\x4c\x9a\x27\x6a\x61\x27\xce\xb3\x55\xc6",
            "CPR",
            2,
            "CIPHER",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\x26\x55\xaf\x32\x6d\xe3\x10\x49\x1c\xb5\x4f\x12\x0e\x02\xee\x0d\xa9\x2b\x55",
            "CRETA",
            18,
            "CRETA TOKEN",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\x23\x70\xd5\xcd\x63\x27\x9e\xfa\x6d\x50\x2d\xab\x29\x17\x19\x33\xa6\x10\xaf",
            "CRV",
            18,
            "CRV (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x3b\xd1\x26\x75\xc6\xb9\xd6\x99\x3e\xb8\x12\x83\xcb\x68\xe6\xeb\x92\x60\xb5",
            "CTF",
            18,
            "Crypto Trading Fund",
        )
        yield (  # address, symbol, decimals, name
            b"\x27\x27\xab\x1c\x2d\x22\x17\x0a\xbc\x9b\x59\x51\x77\xb2\xd5\xc6\xe1\xab\x7b\x7b",
            "CTSI",
            18,
            "Cartesi Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x3c\xf7\xad\x23\xcd\x3c\xad\xbd\x97\x35\xaf\xf9\x58\x02\x32\x39\xc6\xa0\x63",
            "DAI",
            18,
            "(PoS) Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xc3\xeb\x88\xc9\x72\x39\x01\x20\xbb\x4a\xbd\x2f\x70\x5c\x48\xf6\x2e\x21\x2c",
            "DEFI",
            18,
            "Defiway Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\xee\x10\x93\x52\xc6\x11\x6d\xb0\xf1\x7f\x4a\xa8\x51\x9c\xdb\xfc\x7e\x78\x87",
            "DEP",
            18,
            "DEAPCOIN (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\xbf\x28\x64\xeb\xec\x7b\x7f\xdf\x6e\xec\xa9\xba\xca\xe7\xcd\xfd\xaf\xfe\x78",
            "DODO",
            18,
            "DODO bird (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x95\x50\x46\xdf\x46\x68\xe1\xdd\x36\x9d\x2d\xe9\xf3\xae\xb9\x8d\xd2\xa3\x69",
            "DPI",
            18,
            "DefiPulse Index (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\x26\xac\x9e\xdb\x26\xff\x16\xda\x19\x15\x10\x42\xa8\xba\x3b\xb2\xcc\x23\x7f",
            "ELG",
            18,
            "EscoinToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\x33\x9c\x80\xff\xde\x91\xf3\xe2\x04\x94\xdf\x88\xd4\x20\x6d\x86\x02\x4c\xdf",
            "ELON",
            18,
            "Dogelon (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x70\x83\x83\xae\x0e\x80\xe7\x53\x77\xd6\x64\xe4\xd6\x34\x44\x04\xde\xde\x11\x9a",
            "EMT",
            18,
            "Earthmeta",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\xfe\x63\x29\x4d\xac\x27\xce\x94\x1d\x42\xa7\x78\xa3\x7f\x2b\x35\xfe\xa2\x1b",
            "ETUS",
            18,
            "Ethernus",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\x11\x17\x8a\x87\xa3\xbf\xf0\xc8\xd1\x8d\xec\xba\x57\x98\x82\x75\x39\xae\x99",
            "EURS",
            2,
            "STASIS EURS Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x3b\x9e\x91\x00\xdb\x13\x89\xc5\x18\xd4\x7c\x63\x5d\x80\xa9\x0a\xd4\xc4\xf4\x1b",
            "FAN",
            8,
            "FAN",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\x32\xab\xee\xbd\x3c\x2f\xac\x1e\x74\x59\xa2\x7e\x1a\xe9\xf1\xc1\x6c\xcc\xca",
            "FAR",
            18,
            "FARCANA",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\x15\xcd\xb9\xd4\x38\x24\xdc\xa6\x7f\xce\xb1\x20\x1e\x5a\xbe\xbe\x0b\x2c\xbc",
            "FARM",
            6,
            "CryptoFarmers",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x8c\x96\x34\xde\x65\x90\xb9\x6a\xea\xdc\x4b\xc6\xdb\x5c\x28\xfd\x17\xe3\xc2",
            "FIRE",
            18,
            "Matr1x Fire Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xbb\x7a\x68\xc4\x6b\x53\xf6\xbb\xf6\xcc\x91\xc8\x65\xae\x24\x7a\x82\xe9\x9b",
            "FLUX",
            8,
            "Flux",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\xc3\x2f\xa6\xdf\x82\xea\xd1\xe2\xef\x74\xd1\x7b\x76\x54\x7e\xdd\xfa\xff\x89",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\xed\x22\xee\xe9\x2a\x38\x72\x70\x98\x23\xa6\x97\x00\x69\xe1\x2a\x45\x40\xeb",
            "FRONT",
            18,
            "Frontier Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x32\x7f\x88\x9d\x59\x47\xc1\xdc\x19\x34\xbb\x20\x8a\x1e\x79\x2f\x95\x3e\x96",
            "frxETH",
            18,
            "Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\x42\xfe\x47\x7b\x2a\xfe\xd9\x2c\x25\xd0\x92\x41\x8b\xac\x06\xcd\x07\x6c\xea",
            "FURI",
            18,
            "FURI",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\x15\xfd\xda\x4c\x88\x27\x31\xc0\x45\x6a\x42\x14\x54\x8c\xd1\x3a\x82\x28\x86",
            "FUSE",
            18,
            "Fuse",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\x3a\xcf\x6d\x19\x26\x7e\x2d\x3e\x7f\x89\x8f\x42\x80\x3e\x90\xc9\x21\x90\x62",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\x0f\x66\x37\x9a\x6d\x78\x01\xd7\x72\x6d\x5a\x94\x33\x56\xa1\x72\x54\x9a\xdb",
            "GEOD",
            18,
            "Geodnet Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x22\x0b\x82\xf3\xea\x3b\x7f\x6d\x9a\x1d\x8a\xb5\x89\x30\xc0\x64\xa2\xb5\xbf",
            "GLM",
            18,
            "Golem Network Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x4d\xb5\x50\xb5\x74\xb3\xe9\x27\xaf\x3d\x93\xe2\x61\x27\xd1\x57\x21\xd4\xc2",
            "GMT",
            8,
            "GreenMetaverseToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xe5\x41\x7a\xf5\x64\xe4\xbf\xda\x1c\x48\x36\x42\xdb\x72\x00\x78\x71\x39\x78\x96",
            "GNS",
            18,
            "Gains Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\xe2\xb5\x8c\x01\x3d\x76\x01\x14\x7d\xcd\xd6\x8c\x14\x3a\x77\x49\x9f\x55\x31",
            "GRT",
            18,
            "Graph Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\x5c\xa3\x66\xe2\x64\x09\x84\x56\x24\xe2\x9b\x62\xc3\x88\xa0\x69\x61\xa7\x92",
            "HLSCOPE",
            6,
            "Hamilton Lane SCOPE Securitize Tokenized Feeder Fund LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\x72\xda\xce\xd7\xc6\xb0\x32\x23\x71\x0c\x11\x41\x30\x36\xd1\x7e\xb2\x98\xf6",
            "IBEX",
            18,
            "Impermax (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\xdf\x5f\xda\x1c\x43\x5c\xd4\x01\x7a\x1f\x1f\x91\x11\x99\x65\x20\xb6\x44\x39",
            "IBS",
            18,
            "IBSToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xfb\xd8\xa3\xb9\x08\xe7\x64\xdb\xcd\x51\xe2\x79\x92\x46\x4b\x44\x32\xa1\x13\x2b",
            "INDEX",
            18,
            "Index (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xf5\x0d\x05\xa1\x40\x2d\x0a\xda\xfa\x88\x0d\x36\x05\x07\x36\xf9\xf6\xee\x7d\xee",
            "INST",
            18,
            "Instadapp (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xf6\x37\x2c\xdb\x9c\x1d\x36\x74\xe8\x38\x42\xe3\x80\x0f\x2a\x62\xac\x9f\x3c\x66",
            "IOTX",
            18,
            "IoTeX Network (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\x22\xc9\x4f\xfb\x6b\xb5\xd1\xdf\x18\xbe\xb5\xfd\x1d\xfe\x75\x83\xd3\xb2\x14",
            "IPMB",
            18,
            "IPMB Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\x63\x82\x72\xad\x69\x98\x70\x8d\xe5\x6b\xbc\x0a\x29\x0a\x1d\xe5\x34\xa5\x78",
            "IQ",
            18,
            "Everipedia IQ (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\x6b\xd4\xf5\xaa\xc8\xd0\xaa\x33\x7d\x13\xec\x88\xdb\x6d\xef\xc6\xea\xee\xfe",
            "IXT",
            18,
            "PlanetIX",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\x15\xdf\x92\xfc\xa1\x2a\xd4\x0b\x8f\x64\x2f\x46\x16\x7b\x62\x3a\x03\xd6\x46",
            "JET",
            8,
            "Satoshi Airline",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\x57\x37\xdb\xb5\x6e\x85\x17\x39\x14\x73\xf7\xc9\x64\xdb\x31\xfa\x6e\xf2\x80",
            "KASTA",
            18,
            "KastaToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\x95\x4e\x8f\xe7\x37\xf9\x9f\x68\xfa\x1c\xcd\xa3\xe5\x1e\xbd\xb2\x91\x94\x8c",
            "KNC",
            18,
            "Kyber Network Crystal v2 (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x28\xe6\xfc\xec\x94\x77\xc2\x82\x98\xc9\xf0\x2d\x74\x0d\xd8\x7a\x16\x83\xe5",
            "KPN",
            18,
            "KonnektVPN",
        )
        yield (  # address, symbol, decimals, name
            b"\xc3\xc7\xd4\x22\x80\x98\x52\x03\x1b\x44\xab\x29\xee\xc9\xf1\xef\xf2\xa5\x87\x56",
            "LDO",
            18,
            "Lido DAO Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x53\xe0\xbc\xa3\x5e\xc3\x56\xbd\x5d\xdd\xfe\xbb\xd1\xfc\x0f\xd0\x3f\xab\xad\x39",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\x83\x4b\xbe\xc7\xe3\x9e\xf4\x2f\x4a\x75\xea\xf8\xe5\xb6\x48\x6d\x3f\x0e\x57",
            "LUNA",
            18,
            "Wrapped LUNA Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\xd6\x74\x66\x65\xd9\x55\x7e\x1b\x9a\x77\x58\x19\x62\x57\x11\xd0\x69\x34\x39",
            "LUNA",
            6,
            "LUNA (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\x00\x1f\x89\x2c\x0c\x82\xb7\x93\x03\xed\xc9\xb9\x03\x3c\xd1\x90\xbb\x21\xc7",
            "LUSD",
            18,
            "LUSD Stablecoin (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\xc5\x7f\x48\xf0\xde\xb8\x9f\x56\x9d\xfb\xe6\xe2\xb7\xf4\x6d\x33\x60\x6f\xd4",
            "MANA",
            18,
            "(PoS) Decentraland MANA",
        )
        yield (  # address, symbol, decimals, name
            b"\x2b\x9e\x7c\xcd\xf0\xf4\xe5\xb2\x47\x57\xc1\xe1\xa8\x0e\x31\x1e\x34\xcb\x10\xc7",
            "MASK",
            18,
            "Mask Network (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\x0c\x23\x47\x21\xb5\xe5\x49\xc3\x69\x3c\xcc\x93\xcf\x19\x1f\x90\xdc\x2a\xf9",
            "METAL",
            18,
            "METAL",
        )
        yield (  # address, symbol, decimals, name
            b"\x05\x66\xc5\x06\x47\x7c\xd2\xd8\xdf\x4e\x01\x23\x51\x2d\xbc\x34\x4b\xd9\xd1\x11",
            "MLC",
            18,
            "MyLovelyCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x26\xd3\xc0\xd9\xf4\xcc\x4c\x13\x00\x97\xb6\xaf\xde\xbe\x4f\x5e\x49\x7e\x6b\xdf",
            "MNT",
            6,
            "Mynth",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x8e\xc2\xd9\xbf\xbd\xd2\x0a\x7f\x5a\x4e\x89\xd6\x40\xf7\xe7\xce\xba\x44\x99",
            "MSQ",
            18,
            "MSQUARE",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\xa9\x10\x36\xe4\xa3\xc1\x44\xef\xed\x95\x3e\x0b\x6c\xc5\xf6\xb9\x8a\xd2\x56",
            "MUBI",
            18,
            "MUBI",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\xfe\xab\x21\xb4\x29\x19\xc5\xc9\x60\xed\x2b\x4b\xdf\xfc\x52\x1e\x26\x88\x1f",
            "MUT",
            18,
            "MUT",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\x14\x34\x16\x0d\x75\x37\xbe\x35\x89\x30\xde\xf3\x17\xaf\xb6\x06\xc0\xd7\x37",
            "NAKA",
            18,
            "Nakamoto.Games",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\xb3\x96\x6b\x4f\xf7\xb4\x27\x96\x9d\xdf\x5d\xa3\x62\x7d\x6a\xea\xe9\xa4\x8e",
            "NEXO",
            18,
            "Nexo (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\x61\x53\x02\xbc\xb3\x63\x09\x37\x1e\xa7\x45\x4f\x3e\x99\xa4\x00\x21\x05\xde",
            "NRS",
            18,
            "Nereus Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc3\xec\x80\x34\x3d\x2b\xae\x2f\x8e\x68\x0f\xda\xdd\xe7\xc1\x7e\x71\xe1\x14\xea",
            "OM",
            18,
            "MANTRA DAO (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x81\x47\x70\x06\x5f\x63\x40\x03\xa8\xd8\xd7\x0b\x47\x43\xe0\xc3\xf3\x34\xad",
            "ONT",
            9,
            "Poly-Peg ONT",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\x43\x89\xea\xae\x0a\x68\x21\xdc\x49\x06\x2d\x56\xbd\xa3\xd9\xd4\x5f\xa2\xff",
            "ORBS",
            18,
            "Orbs (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\xcd\x72\xf7\xa4\xb6\x99\xc6\x7f\x22\x5c\xa8\xab\xb2\x0b\xc9\xf8\xdb\x90\xc7",
            "OSAK",
            18,
            "Osaka Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x7a\x56\xf2\xf2\xfd\x97\xda\x0f\x5b\x4f\xad\xe1\x80\xfa\x8d\xc3\xfb\xea\x8d",
            "OSMO",
            6,
            "Osmosis (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x11\xc5\xef\xfa\x33\xc4\xd6\xf8\xf5\x93\xcf\xa3\x94\x24\x1c\xfe\x92\x58\x11",
            "OUSG",
            18,
            "Ondo Short-Term U.S. Government",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\xa8\xe9\x6c\x8c\x7c\x54\x85\x48\x12\x4e\x4a\x92\x38\x1a\x8d\x97\xfd\x81\xaa",
            "P-USDC",
            6,
            "Primex pToken USD Coin (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x95\x3c\xfe\x44\x2c\x5e\x88\x55\xcc\x6c\x61\xb1\x29\x3f\xa6\x48\xba\xe4\x72",
            "PolyDoge",
            18,
            "PolyDoge",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\xa7\x19\x5c\xef\x46\x05\xbd\x80\xb9\x49\xcd\x7e\xb1\xf7\xad\x15\x66\xc8\x50",
            "PWR",
            18,
            "Gamebitcoin Power",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x0e\xf9\x26\x3e\x76\xda\xe6\x3c\x84\x29\x2c\x34\x09\xd6\x1c\x59\x8e\x96\x82",
            "PYR",
            18,
            "PYR Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\xe5\x64\x6f\x60\xac\x6f\xb4\x46\xf6\x21\xd1\x46\xb6\xe1\x88\x6f\x00\x29\x05",
            "RAI",
            18,
            "Rai Reflex Index (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\x67\x7c\xa1\x70\x65\xed\x74\x67\x5b\xc2\x7b\xca\xba\xdb\x7e\xef\x10\xa2\x92",
            "RAIN",
            18,
            "Rain Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\xce\xbf\xcf\x60\x4a\xd2\x45\xea\xf0\xd5\xbd\x68\x45\x9c\x3a\x7a\x63\x99\xc2",
            "RAMP",
            18,
            "RAMP (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\x5e\x20\xde\x2f\x2e\xbb\x4c\xff\xd4\xd1\x6a\x55\xc7\xb3\x95\xe8\xa9\x47\x62",
            "REQ",
            18,
            "Request",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\x29\x97\x74\x02\x0d\xa4\x44\xaf\x13\x4c\x82\xfa\x83\xe3\x81\x0b\x30\x99\x91",
            "RNDR",
            18,
            "Render Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x27\xab\x6e\x82\xf3\x45\x8e\xdb\xc0\x70\x3d\xb2\x75\x63\x91\xb8\x99\xce\x63\x24",
            "RNT",
            18,
            "Reental Utility Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\x05\x70\x57\x71\x54\x7c\xf7\x92\x01\x11\x1b\x4b\xd8\xaa\xf2\x94\x67\xb9\xec",
            "RPL",
            18,
            "Rocket Pool (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\xba\x07\x3c\x31\xbf\x03\xb8\xac\xf7\xc2\x8e\xf0\x73\x8d\xec\xf3\x69\x56\x83",
            "SAND",
            18,
            "SAND",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\x99\xfa\xce\x15\xc1\x43\x48\xe1\x75\x93\x71\x04\x9a\xb6\x4a\x3a\x06\xbf\xa6",
            "SDEX",
            18,
            "SmarDex Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x36\x1a\x5a\x49\x93\x49\x3c\xe0\x0f\x61\xc3\x2d\x4e\xcc\xa5\x51\x2b\x82\xce\x90",
            "SDT",
            18,
            "Stake DAO Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x6d\x1f\xdb\xb2\x66\xfc\xc0\x9a\x16\xa2\x20\x16\x36\x92\x10\xa1\x5b\xb9\x57\x61",
            "sfrxETH",
            18,
            "Staked Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\x44\xe4\xd2\xb1\x45\xa8\xe7\x83\x28\x89\xfc\x46\x09\xf6\x54\x44\x6c\x22\xf9",
            "SITY",
            18,
            "Sity Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xb7\x28\xd8\xd9\x64\xfd\x00\xc2\xd0\xaa\xd8\x17\x18\xb7\x13\x11\xfe\xf6\x8a",
            "SNX",
            18,
            "Synthetix Network Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\xc7\x3b\x90\xe0\xc2\xa3\x55\x78\x4d\xcf\x0d\xa1\x2f\x47\x77\x29\xb3\x1e\x77",
            "SOIL",
            18,
            "Soil",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\x3f\x7e\x27\x1c\xb8\x7c\x23\xaa\xa7\x3e\xdc\x00\x8a\x79\x64\x6d\x1f\x99\x12",
            "SOL",
            9,
            "Wrapped SOL (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\x42\x81\x74\xf5\x16\xf5\x27\xfa\xfd\xd1\x46\xb8\x83\xbb\x44\x28\x68\x27\x37",
            "SUPER",
            18,
            "SuperFarm (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\x83\x32\x54\x7c\x68\x0f\x75\x54\x81\xbf\x48\x9d\x89\x04\x26\x24\x8b\xb2\x75",
            "SURE",
            18,
            "inSure (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x3f\x86\x8e\x0b\xe5\x59\x7d\x5d\xb7\xfe\xb5\x9e\x1c\xad\xbb\x0f\xdd\xa5\x0a",
            "SUSHI",
            18,
            "SushiToken (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x3c\xe1\x32\x78\x67\x07\x7b\x55\x1a\xe9\xa6\x98\x7b\xf1\x0c\x9f\xd0\x8e\xdc\xe1",
            "SWCH",
            18,
            "SwissCheese Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\xf9\xef\xc0\xdb\x77\xd8\x88\x15\x00\xbb\x06\xff\x5d\x6a\xbc\x30\x70\xe6\x95",
            "SYN",
            18,
            "Synapse",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x78\x37\xde\x1f\x2f\xa4\x63\x1d\x71\x6c\xf2\x50\x2f\x8b\x23\x0f\x1d\xcc\x32",
            "TEL",
            2,
            "Telcoin (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xd2\x8b\x3f\xd3\x50\xa1\x4f\x0b\x1d\x14\x63\x3e\x3d\x14\xdb\x7b\x80\x40\x63\x91",
            "TRUMP",
            18,
            "Meme TrumpCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x78\x44\x54\x85\xa8\xd5\xb3\xbe\x76\x5e\x30\x27\xbc\x33\x6e\x3c\x27\x2a\x23\xc9",
            "UBU",
            18,
            "Ubuntu",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\x19\x4f\xea\xca\x97\x0a\x4e\x98\xa1\x9c\x36\x5f\xe1\x44\xfb\x54\xf6\x57\xdb",
            "UFO",
            18,
            "THE TRUTH (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\x3e\xaa\xd8\xd9\x22\xb1\x08\x34\x46\xdc\x23\xf6\x10\xc2\x56\x7f\xb5\x18\x0f",
            "UNI",
            18,
            "Uniswap (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\xbf\xc1\xcb\xf7\xa1\xe7\x11\x50\x3a\x29\xb4\xb5\xf9\x13\x0e\xbe\xcc\xac\x96",
            "UPO",
            18,
            "UpOnly",
        )
        yield (  # address, symbol, decimals, name
            b"\x27\x91\xbc\xa1\xf2\xde\x46\x61\xed\x88\xa3\x0c\x99\xa7\xa9\x44\x9a\xa8\x41\x74",
            "USDC.e",
            6,
            "USD Coin (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x3c\x49\x9c\x54\x2c\xef\x5e\x38\x11\xe1\x19\x2c\xe7\x0d\x8c\xc0\x3d\x5c\x33\x59",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x18\xcb\x63\xa2\xb8\xed\xf2\xde\x97\x1e\x2f\x17\xf7\x70\x97\xe4\x99\x45\x9d",
            "USDC",
            6,
            "USD Coin (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\x3f\x9c\x3f\xac\x3e\x27\x59\xd7\xe9\x0a\xca\x4f\x95\x40\xf7\x51\x94\xa0\xd7",
            "USDN",
            18,
            "Neutrino USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\x13\x2d\x05\xd3\x1c\x91\x4a\x87\xc6\x61\x1c\x10\x74\x8a\xeb\x04\xb5\x8e\x8f",
            "USDT",
            6,
            "(PoS) Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x25\x97\xb0\x09\xd1\x3c\x40\x49\xa9\x47\xca\xb2\x23\x9b\x7d\x65\x17\x87\x5f",
            "UST",
            18,
            "Wrapped UST Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x88\x02\x49\x74\x5e\xac\x5f\x1e\xd9\xd8\xf7\xdf\x84\x47\x92\xd5\x60\xe7\x50",
            "USTBL",
            5,
            "Spiko US T-Bills Money Market Fund",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\xe5\xb8\x0a\x0c\x1b\x02\xfe\x49\x76\x85\x1d\x03\x0b\x36\x12\x2d\xbb\x86\x24",
            "VANRY",
            18,
            "VANRY",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x16\xd4\xbf\x8a\x0a\x71\x60\x17\xe8\xd2\x26\x2c\x4a\xc3\x29\x27\x79\x7a\x2f",
            "VCNT",
            18,
            "ViciCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xf1\xc1\xa3\xc2\x48\x1a\x3a\x8a\x3f\x17\x3a\x9a\xb5\xad\xe2\x75\x29\x2a\x6f\xa3",
            "VEE",
            18,
            "BLOCKv Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\x25\x8a\x3f\xd0\x0f\x38\xaa\x80\x90\xdf\xee\x34\x3f\x10\xa9\xd4\xd3\x0d\x3f",
            "VOXEL",
            18,
            "VOXEL Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\x49\x06\xec\x1d\xf8\x39\x9f\x00\xe4\xad\x32\xc0\xec\xac\x04\x04\xa2\x7a\x1c",
            "WALLET",
            18,
            "Ambire Wallet",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\x0b\x9e\x24\x6d\xb5\xa0\xd2\x1b\xf9\x20\x9e\x48\x58\xbc\x9a\x3f\xf7\xa0\x34",
            "wBAN",
            18,
            "Wrapped Banano",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\xdc\xb5\xb8\x8f\x8e\x3c\x15\xf9\x5c\x72\x0c\x51\xc7\x1c\x9e\x20\x80\x52\x5d",
            "WBNB",
            18,
            "Wrapped BNB (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\xf0\x1f\x6a\x04\x13\x41\x66\x79\xd8\xbe\x06\xf7\x82\x86\x6a\xe9\xe3\xea\xe6",
            "WEMIX",
            18,
            "Orbit Bridge Matic Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\xcd\x37\xbb\x86\xf6\x54\x19\x71\x3f\x30\x67\x3a\x48\x0e\xa3\x3c\x82\x68\x72",
            "WETH",
            18,
            "Wrapped Ether (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x7c\xeb\x23\xfd\x6b\xc0\xad\xd5\x9e\x62\xac\x25\x57\x82\x70\xcf\xf1\xb9\xf6\x19",
            "WETH",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\x58\x5e\xc9\xd4\xc9\x7a\xd9\xde\xd7\x25\x0b\xb9\xa1\x99\xfe\x8e\xed\x0e\xca",
            "WHALE",
            4,
            "WHALE (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\x38\xec\xb4\x2c\x42\x4e\x87\x76\x52\xad\x82\xd8\xa9\x39\x18\x3a\x04\xc3\x5f",
            "WIFI",
            18,
            "WiFi Map",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x34\x41\xe7\xb9\xaa\x8a\x30\xa5\x42\xdd\xe0\x48\xdd\x06\x7d\xe2\x80\x2e\x9b",
            "WINK",
            18,
            "WINK",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x50\x0b\x1d\x8e\x8e\xf3\x1e\x21\xc9\x9d\x1d\xb9\xa6\x44\x4d\x3a\xdf\x12\x70",
            "WMATIC",
            18,
            "Wrapped Matic",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x81\x5d\x12\x0b\x3e\xf0\x20\x39\xee\x11\xdc\x2d\x33\xde\x7a\xa4\xa8\xc6\x03",
            "WOO",
            18,
            "Wootrade Network (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\x2f\x88\x18\x00\x2d\xc6\x47\x53\xda\xed\xf4\xa6\xcb\x2c\xcb\x75\x7c\xd2\x20",
            "WSDM",
            6,
            "Wisdomise",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\x5e\xb8\x4c\x05\x2f\xd0\x12\xbb\x90\x2c\x25\x8c\x9f\xd2\x41\xb1\x7c\x00\x05",
            "XNET",
            18,
            "XNET Mobile",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x53\x71\x04\xd6\xa5\xed\xd5\x3c\x6f\xbb\xa9\xa8\x98\x70\x8e\x46\x52\x60\xb6",
            "YFI",
            18,
            "(PoS) yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\x61\x7a\xa5\x2d\xdd\xf5\xed\x9b\xb7\xb3\x70\xed\x77\x7b\x31\x82\xa3\x0f\xd1",
            "YGG",
            18,
            "Yield Guild Games Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\xeb\xd2\x26\x15\x4e\x86\x59\x54\xa8\x76\x50\xfa\xef\xa8\xf4\x85\xd3\x60\x81",
            "ZIG",
            18,
            "ZigCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x85\x88\x4c\x43\x92\xd3\x48\x58\x7b\x19\xcb\x9e\xaa\xf1\x57\xf1\x32\x71\xcd",
            "ZRO",
            18,
            "LayerZero",
        )
    if chain_id == 250:  # ftm
        yield (  # address, symbol, decimals, name
            b"\x03\xe5\x86\x20\x4a\x83\x53\x2e\x46\xea\xf1\x51\xe3\x86\x46\xda\x00\x1e\x4a\xec",
            "$BABYMAN",
            18,
            "Baby Spiderman",
        )
        yield (  # address, symbol, decimals, name
            b"\x6d\x17\xb2\xf4\x00\xf5\x24\x4c\x21\x1e\x8e\x96\x87\x95\x78\xee\xbd\x9f\xa6\x3e",
            "$BALD",
            18,
            "Bald Sam",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\x8d\x4b\xb8\x86\xb5\x34\xb5\xc4\x06\x38\x11\x9f\xfb\xa2\x6d\x63\x8e\xfc\xa8",
            "$BOND",
            18,
            "Bond",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\xba\x12\xa3\xa8\x6f\xf8\x7e\x61\xf0\x8a\x18\x33\x2a\xca\xa1\xdc\x3e\x16\x6e",
            "$SODA",
            18,
            "SonicSoda",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x0e\xfa\xdf\xae\x31\xc1\x34\xce\x80\x08\x39\xc1\x2b\x9b\x24\xaf\xa2\xbd\x51",
            "1234",
            18,
            "allax",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\xc3\x25\x84\x8d\x7c\x6e\x29\xb5\x74\xcb\x07\x89\x99\x8b\x2f\xf9\x01\xf1\x7e",
            "1ART",
            18,
            "ArtWallet",
        )
        yield (  # address, symbol, decimals, name
            b"\xf7\xd1\x4c\x68\x3a\xce\x25\xdc\x48\x5a\xc0\x2b\x69\xd8\xf2\xef\xf0\xbc\xc4\x21",
            "33",
            18,
            "MaSONIC",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\xa3\x9a\x90\xe6\xe7\x81\xc1\x29\x05\x80\xc9\xd5\xac\xfd\xd3\xcb\x82\xcc\x5f",
            "aa1",
            18,
            "aa1",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x07\xa7\x92\xab\x29\x65\xc7\x2a\x5b\x80\x88\xd3\xa0\x69\xa7\xac\x3a\x99\x3b",
            "AAVE",
            18,
            "Aave",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\xc9\x89\xd0\xc0\x3f\x38\x57\x54\x99\x1e\x06\xd9\x0a\xca\xb9\x36\xa2\xe0\xa6",
            "AGATA",
            18,
            "Agatech",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\x98\xc3\x03\x30\xd5\x77\x1a\xe9\xf9\x83\x97\x92\x09\x48\x6a\xe2\x6d\xe8\x75",
            "AI",
            18,
            "Any Inu",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\xab\x45\x02\x77\x1e\x82\x04\x6f\xb0\x1c\xce\xae\x7a\xb5\xd9\xed\xa2\xbc\x13",
            "AIDOGE",
            18,
            "AIDOGE",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\x79\x11\xbb\x87\x04\xf1\xda\xc8\xbb\x71\xee\x99\xe7\x80\x7e\x31\xb7\xcc\xca",
            "Alpha",
            18,
            "Alpha",
        )
        yield (  # address, symbol, decimals, name
            b"\x38\x8b\x5d\x99\x60\xc6\xd1\x48\x6d\x8f\xfc\x92\x27\x6a\x00\xcf\x88\xa3\xdd\xc5",
            "AMFD",
            17,
            "Amfetamina",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x47\x4b\x71\x09\xb7\x3b\x7d\x57\x92\x6d\x43\x59\x8d\x59\x34\x13\x11\x36\xb2",
            "ANKR",
            18,
            "Ankr Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\x02\x5b\xe3\x89\xfc\xa8\x2d\x10\x3a\xbc\x46\x92\xe9\x82\xd3\xd8\x45\xd6\x84",
            "ANON",
            18,
            "Act Now Or Not",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\xbd\xb5\xcf\x9b\x19\x9b\xa5\x38\x7e\x48\x8c\xcf\xef\xd4\xf0\x83\xab\xfd\xb3",
            "BABYGOGLZ",
            18,
            "BABYGOGLZ",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\x2e\x6a\x4f\xe9\x4a\xb5\x52\xbc\x35\xdc\x3e\xb9\x62\xd9\x3d\x83\xe0\x3d\x8c",
            "BABYLUMOS",
            10,
            "Baby Lumos",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x43\xa8\xe6\xc2\x39\x26\x1c\x8d\xd8\x3b\x6d\xda\x6f\xf9\x81\xac\x4a\x86\x46",
            "babysafuu",
            18,
            "babysafuu",
        )
        yield (  # address, symbol, decimals, name
            b"\x87\x64\x97\x1b\xac\xc3\x5c\x44\xc8\x29\xb8\x22\x99\x13\x45\xf6\x45\x2d\x93\xc6",
            "BAGH",
            18,
            "Baby Ghost",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\xe7\x62\x8e\x8b\x43\x50\xb2\x71\x6a\xb4\x70\xee\x0b\xa1\xfa\x9e\x76\xc6\xc5",
            "BAND",
            18,
            "Band",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x7d\x34\x09\x88\x1b\x51\x46\x6b\x48\x3b\x11\xea\x1b\x8a\x03\xcd\xed\x89\xae",
            "BASED",
            18,
            "BASED",
        )
        yield (  # address, symbol, decimals, name
            b"\xf3\x21\xb3\xd3\x03\x67\x00\xcd\x14\x36\xe2\xd9\x39\xa9\x76\x60\xd6\xb8\x91\x06",
            "BBBS",
            18,
            "BabyBatmanBinSuparman",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xc7\xad\xf6\xe6\x18\x6c\xe6\xc7\xf0\x93\xf0\x88\x5b\xa1\x22\x77\x16\x91\xb9",
            "BBENIS",
            18,
            "Blue Benis",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\xd1\xb7\xe8\x42\x2a\x27\x1e\xf4\x4d\x50\x20\xcc\x6b\xdc\xde\xc2\x48\x0d\xe8",
            "BDOGS",
            18,
            "Black Dogs",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\xe3\x01\x51\x50\xf3\x64\x6b\x39\xaa\x78\x7d\x35\x9f\x79\x93\xe3\xca\x2e\x0a",
            "BEEP",
            18,
            "Beep Boop",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\xb3\xbe\xc5\xbb\xf3\x08\xc3\x79\xee\xf4\x4e\x4a\xa2\x94\x07\xcb\x3f\x45\x25",
            "BEN",
            18,
            "Ben",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\xfd\x94\x94\x36\x77\x2e\x07\xcb\xbf\x35\xfb\x65\x24\xe7\x99\x24\xc5\x4c\xb3",
            "BLACKY",
            18,
            "Blacky",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\xa4\xe5\xef\x4a\x17\xeb\xac\xfb\xb0\x7b\xf3\xd8\x86\x96\x2c\x7f\x3b\xbe\xd5",
            "BON",
            18,
            "Bonjour Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x84\x1f\xad\x6e\xae\x12\xc2\x86\xd1\xfd\x18\xd1\xd5\x25\xdf\xfa\x75\xc7\xef\xfe",
            "BOO",
            18,
            "SpookyToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x81\x83\xf0\xc6\x7e\x8a\x84\xbe\xd6\x41\xb9\xec\x8f\x5a\x2b\xa2\xf5\x28\x5f",
            "bpt-4test",
            18,
            "4 test",
        )
        yield (  # address, symbol, decimals, name
            b"\x27\xdc\x29\xa5\x85\x29\x12\xac\x94\x2a\x45\xd4\x92\x28\x58\x82\xea\x7c\x4c\xb1",
            "BRF",
            9,
            "BullRunFinance",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\xde\xc8\xc4\xb2\x68\x07\x93\x66\x1b\xca\x91\xa8\xf1\x29\x60\x75\x71\x86\x3d",
            "BRUSH",
            18,
            "PaintSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x0a\x4b\x8c\x64\xb3\x90\xd7\x26\x2d\x40\xae\x8a\x71\xd8\xa3\x68\xcd\x80\x05\x0d",
            "BSHARE",
            18,
            "BSHARE",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\x86\x84\xeb\xe0\x84\xf9\x02\x9f\x73\x20\x32\xd5\x17\x7c\x69\xe8\xa7\x37\x2c",
            "BTC",
            18,
            "Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\x15\xec\x9c\x4f\xc4\x30\x26\x8b\x91\xd6\xf4\x43\x85\x81\x72\xde\xa8\x3d\x36",
            "BTC",
            6,
            "BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\x37\xd2\xad\x46\x4f\x19\x2d\xb3\x4d\xdb\x12\x88\xe8\xb0\x3b\x9c\x53\x27\x3f",
            "BTC",
            8,
            "WBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\x9c\xb9\xf0\x11\x54\x58\xe5\x5c\x08\x48\x50\x8a\x55\x03\xc5\xae\x38\xcf\xbf",
            "bth",
            18,
            "beet",
        )
        yield (  # address, symbol, decimals, name
            b"\x3d\x6c\xad\xd6\xc7\xb3\x67\xf3\xc5\x95\xab\x49\x21\xc5\xf8\xb4\x0d\xfc\x07\x26",
            "ccc",
            18,
            "chingchongcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\xa6\x1e\x42\x39\xce\xf5\x50\x5c\xff\xa4\x00\x0e\x6b\x72\x91\x3b\xa7\x7b\xed",
            "CELO",
            18,
            "Celo",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x7d\x95\x7f\x83\xf8\x88\x70\x63\x15\x0a\xaf\x71\x87\x41\x13\x51\x64\x33\x92",
            "CHILL",
            18,
            "ChillPill",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x76\x98\x51\x07\x28\x92\xd6\xf9\x31\xfd\x50\xa0\xc6\x47\x68\x30\x0f\x0a\x86",
            "Claim on: layerzero-",
            0,
            "LayerZero (ZRO)",
        )
        yield (  # address, symbol, decimals, name
            b"\xd2\x2d\xbb\x46\x4f\xbf\x0a\x23\x66\xae\x44\x9e\x4a\x8e\x21\xb0\x51\x43\x66\x36",
            "Claim on: layerzero-",
            0,
            "LayerZero (ZRO)",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\xa4\xf8\x29\xc1\x23\x6e\xc1\x29\x6b\x8e\x67\x10\xe4\xfd\x0d\x47\x22\x5f\x78",
            "CODE",
            0,
            "MTM Code Token Round 7",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x3a\xb5\x3b\xaf\xfa\xf1\xe8\x21\x84\x5c\xf7\x08\x04\x28\x36\x6f\x03\x0a\x9c",
            "COFFIN",
            18,
            "CoffinToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x65\x7a\x18\x61\xc1\x5a\x3d\xed\x9a\xf0\xb6\x79\x9a\x19\x5a\x24\x9e\xbd\xcb\xc6",
            "CREAM",
            18,
            "Cream",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\x78\x4d\x50\x67\xaf\x5a\xe6\x29\x2c\xa8\xad\x94\xd5\xd5\xc2\x3e\xa6\x09\x75",
            "CSHARE",
            18,
            "CSHARE",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\xc8\xca\x46\x54\x15\x98\x5f\x16\x9f\x67\xb3\x3b\xe0\x58\xfd\x5c\x3b\xa7\x38",
            "cTAROT",
            18,
            "Tarot Collateral",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x30\x9d\x85\x88\xba\x01\xde\xcf\x51\x4b\xc1\xe5\x52\x79\x54\xba\x89\x25\x2e",
            "cTAROT",
            18,
            "Tarot Collateral",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x54\x2e\x4b\x7a\x9f\x18\xe8\xfd\xbd\x4d\x5f\x2e\x83\x5b\x91\xc1\x56\xd0\x12",
            "DADDYDOG",
            18,
            "DADDYDOG",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x37\x6f\xc1\xea\x0c\x27\xd8\x29\xc0\x5d\x3c\xd8\x68\x4a\x45\xfe\x5a\x2d\x89",
            "DAI",
            18,
            "DAIUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\x9a\x0a\x75\xe4\x98\x40\x8d\x5d\xf6\xc3\xa9\x2b\x61\xd5\xac\x66\x2d\xd6\x66",
            "DAI",
            18,
            "DAIUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x11\xec\x38\xa3\xeb\x5e\x95\x6b\x05\x2f\x67\xda\x8b\xdc\x9b\xef\x8a\xbf\x3e",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xb7\x3a\x4b\x26\x08\x44\x31\x5c\xad\xcf\xb2\xc2\x3b\x48\xbd\x49\x9f\x3a\x83\xc2",
            "DAI",
            18,
            "DAI",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\xb5\xe6\xb0\x8d\x4d\xf3\x3d\x06\xe9\x25\x1a\xe6\xc6\x9c\xdc\x1b\xea\x9b\x4d",
            "deg",
            18,
            "degen express",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\xb0\x11\xcf\x9e\xb6\xd4\x7e\x5b\x9e\x38\xdd\xab\x1a\x13\x5d\x4c\x48\xe2\x65",
            "Dewdz",
            18,
            "Derpe Dewdz",
        )
        yield (  # address, symbol, decimals, name
            b"\xdd\xa0\xf0\xe1\x08\x1b\x8d\x64\xab\x1d\x64\x62\x1e\xb2\x67\x9f\x93\x08\x67\x05",
            "DIAMOND",
            18,
            "Diamond",
        )
        yield (  # address, symbol, decimals, name
            b"\xd2\x27\x27\x7c\xb9\x19\x94\x41\x1c\x89\x36\x81\xa0\xa5\x4e\x1f\x79\xe0\x5e\x21",
            "DIGIT",
            18,
            "Digit",
        )
        yield (  # address, symbol, decimals, name
            b"\x42\x49\x19\x98\xb3\xdc\xd0\xc9\x6e\xa6\x55\x11\x76\xf3\xd8\x69\x27\xa6\xfc\x20",
            "DML",
            18,
            "Dark Mana League",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x53\x83\x0f\xe6\x7a\x60\x30\x63\x83\xc3\x7b\x5c\x77\xa9\x0a\x2a\xf1\xa6\x8e",
            "doNotBuy",
            18,
            "stakingTest",
        )
        yield (  # address, symbol, decimals, name
            b"\x42\xbe\x90\x4f\xca\x36\x7f\xb1\xb2\x1b\x8c\xc9\x85\x6f\x35\xec\xb7\x67\xbd\x53",
            "DSHARE Token Demo",
            18,
            "DShare",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\xa5\x28\xaf\x62\xe5\x65\x12\xa1\x9c\xe8\xc3\xca\xb4\x27\x80\x7c\x28\xcc\x19",
            "DSLA",
            18,
            "DSLA",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\xc8\x73\x9f\x18\x14\xee\xaa\xc0\xfc\x72\x33\xbb\x96\x45\x75\x19\xd4\xe1\x38",
            "DSTRUMP",
            18,
            "DonaldSonicTrump",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\x2a\x3a\xd3\x11\xe6\x6b\x6f\x5e\x56\x7a\x13\x01\x6b\x71\x2a\xba\x06\x25\xc6",
            "DUCK",
            18,
            "Unit Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x47\x82\x25\x6e\xe3\x3c\x64\xa3\x25\xae\x45\xd1\x98\x87\x2f\xd7\x27\x7c\xf7",
            "DW",
            18,
            "Dark Wizards",
        )
        yield (  # address, symbol, decimals, name
            b"\x53\x0a\x4f\x8d\x9c\x41\xad\xbc\x10\xe3\x16\x52\xfc\x5c\xc8\xe3\xab\xdb\x91\x14",
            "E3.G:eliteFmoneyWorm",
            6,
            "Equalizer V3 Gauge for eliteFmoneyWormholeUSDC.e is Eliteness Wrapped wormUSDC.e representing 1:1 wo",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\x08\x2e\xd5\x04\x86\xbd\xc8\xa3\x4b\x2d\xaa\x77\xd6\xbb\xf2\x20\x75\x1e\xde",
            "ECTO",
            18,
            "ec\u00b7to\u00b7plasm",
        )
        yield (  # address, symbol, decimals, name
            b"\x37\x71\x80\xa6\x3f\xcd\xe9\x97\x32\x3e\x1c\x61\xb8\xce\x04\xb1\x89\xe0\x0c\x06",
            "EETH",
            18,
            "ERCWETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xf4\x3c\xc2\x35\xe6\x86\xd7\xbc\x51\x3f\x53\xfb\xff\xb6\x1f\x76\x0c\x3a\x18\x82",
            "ELITE",
            18,
            "ftm.guru",
        )
        yield (  # address, symbol, decimals, name
            b"\xc8\xbe\x6b\x18\x23\x57\xeb\x7a\xf2\x1f\x45\x25\x0b\xff\x10\x8c\x5a\x61\xf4\x41",
            "ELMA.gauge:eliteFmon",
            6,
            "Elite Lending Market Aggregator Gauge for eliteFmoneyWormholeUSDC.e is Eliteness Wrapped wormUSDC.e",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\x03\x8b\x6c\x5b\x8c\x96\xeb\x08\x13\x82\xd9\x52\x8d\x43\x11\xcb\xac\xcd\x38",
            "eLOCKS:vAMM-DUDADUWF",
            18,
            "eLOCKS Financial NFT containing",
        )
        yield (  # address, symbol, decimals, name
            b"\x3f\xd3\xa0\xc8\x5b\x70\x75\x4e\xfc\x07\xac\x9a\xc0\xcb\xbd\xce\x66\x48\x65\xa6",
            "EQUAL",
            18,
            "Equalizer",
        )
        yield (  # address, symbol, decimals, name
            b"\x90\x4f\x51\xa2\xe7\xee\xaf\x76\xaa\xf0\x41\x8c\xba\xf0\xb7\x11\x49\x68\x6f\x4a",
            "FAME",
            18,
            "FAME",
        )
        yield (  # address, symbol, decimals, name
            b"\x6d\xc5\xf8\xfa\x41\x51\xd6\xf7\x0b\xb9\xb2\x02\xca\x83\x2d\x3f\x54\x14\xca\xb9",
            "FANTOM",
            18,
            "HarryPotterObamaSonic10Inu",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x77\xb9\xb3\xd2\x8c\xf1\x97\x64\xe4\x32\x73\xe7\x4c\x7b\xf2\xb2\x63\x6b\x35",
            "FCKGOAT",
            18,
            "FuK The Goat Club",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x69\x01\xb1\xc2\xb6\x7d\xa0\x8c\xd2\xb4\x37\xae\x69\x07\x86\xe1\x99\xac\x95",
            "FDSS",
            18,
            "FDFDFF",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\x0a\xb1\x85\x48\x6e\x19\xe7\xb0\x6b\x09\x9f\x4f\xd5\xde\x2d\x7f\x3e\xd4\x26",
            "Fizo",
            0,
            "Fizo",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\xb3\x0c\x14\x63\x48\xdd\xb0\xdd\xa0\xfa\xd0\x84\xaa\x4f\xb4\x8b\x9b\x8a\x90",
            "FMG",
            18,
            "FANTOM MAGA",
        )
        yield (  # address, symbol, decimals, name
            b"\x90\x03\xe8\x8c\x9b\xf0\x61\x63\x37\x86\xe8\xae\x86\x3b\xb8\x62\x97\x3c\xbe\x7e",
            "fNON",
            18,
            "fNON",
        )
        yield (  # address, symbol, decimals, name
            b"\x52\x98\x25\x08\xf5\x70\x6d\x10\x6f\x1b\x37\x3c\xe2\x36\xea\x23\x87\x71\x48\x97",
            "FOO1",
            3,
            "FooToken1.fordev",
        )
        yield (  # address, symbol, decimals, name
            b"\x90\x08\x25\xea\x29\x7c\x7c\x5d\x1f\x6f\xa5\x41\x46\x84\x9b\xc4\x1e\xdd\xaf\x29",
            "fPUP",
            18,
            "FTM PUP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xf8\xcb\x20\xc1\x4f\x13\x4f\xe6\xeb\xf7\xac\x3b\x90\x3b\x21\x17\xaa\xfa\x62",
            "FRAX",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\x30\x16\x22\xe6\x21\x16\x6b\xd8\xe8\x2f\x2c\xa0\xa2\x6c\x13\xad\x0b\xe3\x55",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\x95\x05\x09\x8d\x56\x1b\x5d\x73\x57\xcd\x74\x60\xe0\x86\xf8\xbf\x36\x09\xbe",
            "FROGZ",
            9,
            "Fantom Frogz",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\x0e\x28\x7d\xcc\x16\x82\x70\xf9\x98\xe7\xde\x4f\x35\x68\x22\x0a\x6e\xa7\xd9",
            "FS",
            18,
            "Friend Snipper Bot",
        )
        yield (  # address, symbol, decimals, name
            b"\x39\x52\x15\xaa\xe8\x1c\x5d\x26\x6d\xca\x89\xce\x82\xb5\x94\x0f\x7f\x5c\x19\x3e",
            "FS",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\xe1\x3e\x6a\x23\x42\x5b\x7b\x0b\x1a\x08\xa8\xa2\x0d\xbc\xd1\x3f\x81\x9d\x34",
            "FTMBAG",
            18,
            "Fantom Bag",
        )
        yield (  # address, symbol, decimals, name
            b"\x58\xd2\x3b\x45\x51\x3b\x31\xfa\x40\x1e\x79\x6d\x7e\xd2\x8a\x5b\x68\xdc\xc4\x39",
            "FUCK",
            18,
            "FUCK DEGENS",
        )
        yield (  # address, symbol, decimals, name
            b"\x07\xbb\x65\xfa\xac\x50\x2d\x49\x96\x53\x2f\x83\x4a\x1b\x7b\xa5\xdc\x32\xff\x96",
            "FVM",
            18,
            "FantomVelociMeter",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\x5f\x5e\xda\xa2\x18\xb4\x58\x66\x28\x52\x52\xcb\x56\x62\xcd\x19\xca\xb9\x9b",
            "FWC",
            9,
            "Fwc",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\xc1\xb6\xcb\xc8\x6e\x85\x6c\x99\x2a\x61\x30\x9b\xd1\x3c\x63\xca\x3e\xac\xd8",
            "Fyde Points (Claim:",
            18,
            "\u200b \u200b \u200b \u200b \u200b \u200b Fyd",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\x43\x3e\x6c\x3f\x95\x0f\x0a\x81\xcf\x96\xe0\xd6\xdf\xac\xee\xcf\x07\xa1\xcb",
            "Fyde Points (Claim:",
            18,
            "\u200b \u200b Fyde",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\x22\xfc\x19\x51\x7f\xa3\x9b\x07\x8f\xcd\x6e\x2b\xcf\x43\x5e\xab\xcd\x6c\x32",
            "gaigna",
            18,
            "again",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\xb7\xc0\xc9\x07\xe4\xc6\xb9\xad\xaa\xaa\xbc\x30\x0c\x08\x99\x1d\x6c\xea\x05",
            "GEL",
            18,
            "Gelato Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x6b\xe8\xa3\x74\xf9\x93\x2e\xbe\x23\xd5\x8c\x38\x2b\xce\x39\x9e\x39\x65\x75",
            "GIO",
            18,
            "GIOTocken",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\xc8\xe4\x58\x66\x87\x44\xc9\x5d\xd8\x27\x5c\xe4\x35\x19\x36\x95\x95\x1a\xd9",
            "GOAT",
            18,
            "Goatseus Maximus",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\x2b\x3d\x31\x9e\x69\x3a\xa5\x78\xed\xd4\xbd\x8a\x5c\x93\x95\xbc\x49\xe9\xf4",
            "GOGLZ",
            18,
            "GOGGLES",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\x16\xd7\x85\x5c\xcc\xb4\x5c\x47\x61\x75\x20\x6e\x14\x83\x2d\x15\xaa\x3e\xac",
            "GOLD",
            18,
            "GOLD",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\xf0\x9d\x21\x8e\x57\xb0\x94\x33\xfc\x84\x57\x22\xf4\x0e\x52\x2d\xb9\xe6\x06",
            "GORK",
            18,
            "Gork",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\x26\xe4\x12\x00\x5c\x9e\x38\x75\xcc\x5d\xeb\x82\xf8\xf0\x89\x0a\x5f\x1b\x3f",
            "HACHI",
            18,
            "Hachiko",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x73\x21\x51\x8e\xb7\xad\x11\xcc\xa3\xbd\xa7\xf4\x5a\xaf\xbf\x26\xd0\x0d\xc0",
            "Haunted",
            18,
            "Haunted",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\xef\xd0\x60\xdc\xce\x7e\x98\x4b\x78\x96\x63\x18\x73\xad\x68\x59\x22\x64\xe3",
            "HawkTuah",
            18,
            "HawkTuahFTM",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\xc2\x21\x05\x30\x1c\xfd\x85\xfd\x0f\x91\x19\x20\xd2\x82\xd1\x8c\xc1\x75\xc1",
            "HEDGY",
            18,
            "Hedgy the hedgehog",
        )
        yield (  # address, symbol, decimals, name
            b"\x5e\x33\x63\xe4\x4d\x8e\xfc\x46\x90\xbd\xb3\xb7\xe4\xa2\x09\xff\xa2\x11\x18\xa1",
            "HLP",
            18,
            "HyperSwap LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x15\x16\xe4\x78\xd7\x3c\xfa\xb0\x34\x22\x9c\x4d\x7a\x94\xa2\xcb\xf5\x78\x7f",
            "HOOPS",
            18,
            "HOOPS",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\x82\x00\x62\xb6\x55\xeb\xbc\x95\x1c\x30\x80\x36\x6d\x0a\xb8\xd8\xd9\x40\xf6",
            "ICO",
            18,
            "IntellCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\xce\x96\x72\x37\xd1\x5e\x85\x62\x67\x3d\x0f\x92\xd7\x69\x82\x9c\x1b\x93\x7d",
            "INU",
            18,
            "InuKoroshi",
        )
        yield (  # address, symbol, decimals, name
            b"\x80\x51\x34\xff\x52\x91\x6d\x86\xbd\xa6\x56\x71\x11\xd6\x00\xc3\xb5\x91\x00\x63",
            "JITSU",
            18,
            "Jitsu",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x31\x7b\xfd\xfa\xd3\x1c\x40\x03\x2d\xcd\xb0\xa6\x22\x78\x09\xf8\x07\xc4\xc5",
            "JTK12",
            18,
            "JollofToken12",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\x5e\xee\xdf\x1d\x82\xcb\xe0\xf7\x34\xbf\x0f\xb9\x64\xb9\x3b\x3f\xa6\x54\x55",
            "JTK15",
            18,
            "JollofToken15",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\xaf\x1d\x57\xd5\x87\xd9\xbd\xdf\xea\x7f\x09\x89\x63\x5d\x70\xff\x37\x78\x1e",
            "JTK7",
            18,
            "JollofToken7",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\x4c\xaf\x66\xc4\x5e\x48\x3f\x7e\xe6\x47\xcc\xad\x27\x5b\x35\xb4\xc7\x57\x19",
            "KENSHI",
            18,
            "Kenshi",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\x70\x57\xe9\x56\x1d\x4b\xb0\x1f\x49\x82\x6c\xed\x43\x82\x9d\xc0\xd0\xca\x7f",
            "KIT",
            18,
            "KITTEN",
        )
        yield (  # address, symbol, decimals, name
            b"\x2b\xa4\xbe\x30\x60\xaa\xe2\x00\xec\x55\x0d\x09\x6d\x0e\xd3\xcb\x56\xf9\x8d\x08",
            "LATINA",
            18,
            "LATINA",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\xab\x3c\x55\x05\x0d\xc1\xe2\xc5\x09\x8e\x5e\xf7\x11\x1c\x56\xd5\x47\x30\xd0",
            "LEG",
            18,
            "LEG",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\x65\x4d\xc3\xd1\x0e\xa7\x64\x5f\x83\x19\x66\x8e\x8f\x54\xd2\x57\x4f\xbd\xc8",
            "LINK",
            18,
            "ChainLink",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\xfb\xe8\x60\xad\x69\x96\x70\xa2\x29\x3d\x19\x4c\xf1\x37\x6e\xf5\x8c\x01\x4a",
            "LUMOS",
            18,
            "Lumos",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\x2a\xe3\x54\xe5\x7c\x6c\x6a\x4a\x71\xc1\xa3\xd2\x7b\x92\x5d\xf6\x85\x3f\x9b",
            "m",
            18,
            "sonic moon",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\x2a\x31\x0a\x60\x09\xd3\x07\xda\xb0\xb9\x42\x01\x13\x88\x37\x91\x75\xe0\xe6",
            "MAGIK-LP",
            18,
            "Magik LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x0e\xde\x16\x6d\x95\x8e\x9d\xf5\x29\x8b\xd3\x0a\x99\x66\x0c\x6e\x6d\x2c\xd0",
            "mAMM-dicpicDWBeer",
            18,
            "Solidly V2 Memecore - dicpic/DWB",
        )
        yield (  # address, symbol, decimals, name
            b"\xce\x95\xb3\x81\xcf\x1d\xc1\x1c\xa4\xce\x2f\x9c\x32\x15\xce\x6d\xca\xe3\x4e\x79",
            "mAMM-dicpicElonCrypt",
            18,
            "Solidly V2 Memecore - dicpic/Elo",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\xb4\x9d\x67\xc6\x29\xb6\xb8\x94\xe7\x3c\xeb\xd1\x6e\x3e\x4f\xd4\x22\x6b\x89",
            "mAMM-SMOONWFTM",
            18,
            "Solidly V2 Memecore - SMOON/WFTM",
        )
        yield (  # address, symbol, decimals, name
            b"\x93\xd0\x5a\xac\x33\x4c\xa5\xdd\x87\x5b\x2a\x25\x24\x96\xff\x3f\x3f\x61\x2f\x27",
            "mAMM-TROLLdicpic",
            18,
            "Solidly V2 Memecore - TROLL/dicp",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\xf8\x4a\x5b\xc7\x56\xac\x64\xd9\x51\xe4\xff\xad\xda\x3a\x8a\x82\xc1\xe4\x74",
            "mAMM-WFTMCOB",
            18,
            "Solidly V2 Memecore - WFTM/COB",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x29\x78\xc3\xfb\x8a\x60\xed\xdc\x1d\x07\x01\x7e\xbe\x1e\x60\xc0\x03\xf3\xef",
            "mAMM-WFTMDR",
            18,
            "Solidly V2 Memecore - WFTM/DR",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\x7b\xe4\xf2\xe3\xa1\x23\x5c\x1b\x27\x91\x1c\x73\x0d\x85\x12\xd9\xd3\x64\x89",
            "Meow",
            18,
            "Meow",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\x93\x61\xa8\x6d\x03\x8c\x8a\xda\x3d\xb2\x45\x76\x08\xe2\x27\x5b\x3e\x08\xd4",
            "MESO",
            18,
            "FTM-Meso",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\xdf\x6b\xe2\x1b\xc2\xa3\x01\x09\x3b\x52\x13\xa4\xfb\xf4\x01\x61\x4a\xad\x14",
            "META",
            18,
            "META token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe6\x4b\x9f\xd0\x40\xd1\xf9\xd4\x71\x5c\x64\x5e\x0d\x56\x7e\xf6\x99\x58\xd3\xd9",
            "MOD",
            18,
            "Modefi",
        )
        yield (  # address, symbol, decimals, name
            b"\x5e\xb1\x48\xa5\x71\xb0\xb3\xd8\xb8\xeb\x46\x05\x35\x45\xe7\xdf\x83\x38\x98\xdc",
            "MOOSCREAMSPELL",
            18,
            "Moo Scream SPELL",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\xee\xd5\xff\x17\x01\xe6\xed\x84\x70\xdc\x39\x1f\x05\xe2\x7b\x1d\x06\x57\xeb",
            "MPX",
            18,
            "MPX",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\x1b\xc0\x0b\x78\x26\x04\xc0\x85\xa3\x48\xa7\x4b\xde\x00\x88\x87\x77\x23\xc2",
            "MRAT",
            10,
            "MRAT",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\xb9\xa3\x39\x56\x35\x1c\xf4\xfa\x04\x0f\x65\xa1\x3b\x83\x5a\x3c\x87\x64\xe3",
            "MULTI",
            18,
            "Multichain",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\xb2\x51\x9e\x31\x5f\x1b\x4a\x71\x29\x2f\xf7\x4b\x59\x38\x69\x87\x6a\xd4\x14",
            "MULTI2",
            18,
            "Multi V2",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\xf9\x9c\xcc\xf3\x56\x94\x12\x9e\xf6\x02\xf7\x53\xe1\x03\x72\x6f\xaa\xfd\xd6",
            "mxvWFTMCHILLEQUAL",
            18,
            "vWFTM-CHILL",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x11\x4e\x7e\xc1\xe2\x89\x87\x2b\x12\xf1\xb2\x73\x8e\xde\xf9\xe1\xa3\x18\x36",
            "NIPS-LP",
            18,
            "NIPS LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\xd2\x98\xdc\x93\x8d\xe3\x4d\xdb\x03\xf7\xed\x7c\xb3\x30\x3c\x90\xa5\x79\xf1",
            "NRB",
            18,
            "Nico Robin",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\x39\xc6\x30\x37\xd9\x5f\x84\xa5\x98\x1f\x96\xe4\x38\x50\xd1\x45\x1b\x62\x16",
            "OBOL",
            18,
            "OBOL",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\x67\x58\x15\x69\x8c\x2a\xac\xde\xb4\x6b\x34\x43\x1c\x2b\x06\x33\xd9\x36\x44",
            "OBROS",
            7,
            "Oxygen Bros",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\x7d\x11\x48\x89\xc0\x0c\x18\xfb\x6a\x61\x88\x34\x9f\x29\x9d\x7b\x86\xcc\xcc",
            "OMP",
            18,
            "Olimpus Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\xe6\xaf\xeb\x3a\xc2\xfb\xc8\x2a\x8d\x31\x2b\xea\x3b\x47\xdc\x6b\x48\x48\xd2",
            "OOZE",
            0,
            "Ooze",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\xa8\xca\xb1\x5d\x06\xd3\xa5\xfe\x58\x54\xd7\x14\xc3\x7e\x7e\x92\x46\xf1\x70",
            "ORBS",
            18,
            "Orbs",
        )
        yield (  # address, symbol, decimals, name
            b"\xa8\x82\xce\xac\x81\xb2\x2f\xc2\xbe\xf8\xe1\xa8\x2e\x82\x3e\x3e\x96\x03\x31\x0b",
            "PANIC",
            18,
            "PanicSwap",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\xf6\xca\xbb\xbc\x14\x04\xbd\x0d\x23\x7c\x36\x8a\x59\x78\xdc\x37\x5b\x3a\x06",
            "PAPU",
            18,
            "PumpkinApu",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x1f\xad\xa4\x76\x4c\x22\x05\x96\xa7\x43\x4b\xf7\x96\x0d\xef\x09\xab\x28\x3f",
            "PEPE",
            18,
            "PEPE DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\xd2\x95\xa5\xe1\x17\x6f\x9f\xef\xe1\xda\x42\x17\x77\xcd\x5e\xc2\x60\xbf\x6c\x97",
            "PEPE",
            18,
            "PEPE DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xf3\x2a\x01\x7a\xc6\x10\xf5\xb7\x81\x9e\x60\x73\x76\x35\x5f\x27\xb5\xe4\x3e",
            "POL",
            18,
            "Polygon Ecosystem Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\x48\x9c\x92\x56\xf8\x5b\xa6\xc7\xb6\xbd\x4e\xa4\x15\x62\xb7\x50\xb0\x8c\x86",
            "PROBIT",
            18,
            "Probit",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\xe6\x6c\x0a\x02\x7c\x24\xa8\x9d\x93\xec\x24\x4e\xd2\x98\x85\x6f\x16\xab\xe0",
            "PUBGIRLS",
            8,
            "Pub Girls",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\x57\xd4\x2e\xeb\x74\xb4\xf4\x62\x87\x31\x32\x10\xbf\x46\x39\xaf\x16\x0d\x31",
            "QHZ",
            18,
            "QHZ Digit",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\x62\xf4\x74\x8f\x01\xf4\xbc\x2f\x9d\xa1\x4b\x30\xbf\x60\x4a\xab\xc4\x5d\x24",
            "RABBIT",
            18,
            "RABBIT",
        )
        yield (  # address, symbol, decimals, name
            b"\xb8\xe6\x93\x85\x51\xd4\x5c\x4a\xe0\x77\xe6\x34\x26\xd6\xf9\x40\x19\x5e\x21\xb0",
            "RGOAT",
            18,
            "REDHEAD GOAT",
        )
        yield (  # address, symbol, decimals, name
            b"\xc1\x7c\x30\xe9\x85\x41\x18\x86\x14\xdf\x99\x23\x9c\xab\xd4\x02\x80\x81\x0c\xa3",
            "RISE",
            18,
            "EverRise",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\xc6\x04\x5a\xdc\x74\xb1\x06\x09\xcf\x00\xae\x93\x49\x99\x27\xc4\xc9\x32\xfb",
            "ROAD",
            9,
            "ROAD",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\x12\x86\xfc\x88\x71\x89\xc5\x62\x41\x0a\xf1\x2e\xd5\x21\xc8\xe5\x8e\x5f\xa3",
            "s3crypto_e",
            18,
            "s3crypto_e",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\x7a\x91\x03\x8c\x48\x97\x4d\xda\xff\x16\xa7\x89\x57\xd3\x94\x8b\x93\x79\x3b",
            "S404",
            18,
            "Sonic404",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x7e\xff\x21\xc0\x80\x4f\x3a\x4d\xff\x95\x01\xcb\x65\x57\x56\x04\xb9\xcd\x4e",
            "S5X",
            18,
            "Sigmoid 5X",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\x60\x63\x7c\x79\xbc\xa8\x3e\x43\x89\xf5\x16\x10\x16\x0a\x3a\x8b\x98\x9b\x62",
            "SAM",
            18,
            "Samurai DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\xa4\x55\x64\x38\x62\xd9\x25\x31\x9a\xa4\x5c\x1c\xb0\x22\xe8\xff\x3f\xfb\x6f",
            "sbtc",
            18,
            "sbtc",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x31\xd4\x52\xe4\xbc\xa8\x66\x72\xfd\x61\x09\xde\x94\x68\x8d\x1e\x17\xaa\xe5",
            "SCC",
            9,
            "ScaryChainCapital V2",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\x65\x4c\x8e\x6f\xd4\xd7\x33\x34\x9a\xc7\xe0\x9f\x6f\x23\xda\x25\x6b\xf4\x75",
            "SCREAM",
            18,
            "Scream",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\x28\xd7\x6a\x49\xd2\x5d\x3c\x97\xf5\x0a\xc9\x3d\xa4\x33\xeb\xed\xbe\x05\xc8",
            "scrvUSDT_o",
            18,
            "scrvUSDT_o",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\x2a\x13\xc1\x09\xac\x30\xf0\xdb\x80\xad\x3b\xd1\xde\xfd\x5d\x0a\x6c\x0a\xc6",
            "SD",
            18,
            "Stader (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x1c\x44\x70\x39\x4a\x37\x78\x63\x77\x04\x98\xfc\x5a\x73\x64\xae\x4b\x0d\xb3",
            "SEGA",
            18,
            "SEGA",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\xec\xa7\x53\x5f\x1e\x4b\x09\xfd\x10\x91\x61\x5d\x30\x2e\xba\x2d\x04\x90\x1e",
            "sex-sAMM-SPIRITRAINS",
            18,
            "Solidex sAMM-SPIRIT/RAINSPIRIT D",
        )
        yield (  # address, symbol, decimals, name
            b"\x89\x5b\x8a\xf7\xa0\x90\x9a\x92\x68\xfb\x11\xc6\xe5\xe6\xdb\xde\x2f\x04\x29\x9b",
            "sex-sAMM-USDCMIM",
            18,
            "Solidex sAMM-USDC/MIM Deposit",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x1f\xcd\x1f\x7b\xa1\x90\xdb\xc7\x53\x54\x04\x6f\x60\x24\xa9\xb8\x60\x14\xd7",
            "SEX",
            18,
            "Solidex",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\x48\x33\x91\xf6\x55\xfd\x74\x5f\x4e\x41\xa1\x3d\xa3\xe5\x80\x6a\xc4\x79\xce",
            "SEX",
            18,
            "SEX",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\x0c\xcd\x56\x39\x18\xff\x90\x09\x28\xdc\x52\x9a\xa0\x10\x46\x79\x5c\xcb\x4a",
            "sfrxETH",
            18,
            "Staked Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\x02\x80\x92\xc8\x30\xb5\xc8\xfc\xe0\x61\xaf\x2e\x59\x34\x13\xeb\xbc\x1f\xc1",
            "sFTMX",
            18,
            "sFTMX",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\xe3\x08\xaf\x39\x37\xe0\x6f\xfb\x9c\x61\x0b\x83\x7f\xe3\x92\xea\x5d\x1b\xc7",
            "SHARDED",
            18,
            "SHARDED in my pant",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\x9a\x6b\x00\x8d\x86\x0b\x36\x29\xec\x65\x35\x87\xd4\xb6\x5d\xc7\xb3\x57\x18",
            "Shiba-LP",
            18,
            "Shiba LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\xb2\x1b\xcd\x09\x88\xca\x50\xc4\x28\x5d\x6b\xd8\x56\xda\xa9\x50\x3d\x61\xd3",
            "ska",
            8,
            "suka",
        )
        yield (  # address, symbol, decimals, name
            b"\x1e\xb9\x4e\x9e\xfd\x43\x65\xa1\xe1\xc4\x96\x58\xca\xc7\x6c\x8e\xf9\x80\xa1\xd0",
            "SLP",
            18,
            "SushiSwap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\x03\x2e\xd3\xff\xc2\xee\x5b\x1c\xd8\xda\x61\x47\x97\x5c\x6e\xcd\xb3\xae\x2d",
            "SLP",
            18,
            "SushiSwap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x08\x69\x05\x69\x03\x52\xbe\x51\x79\xd1\x35\x79\x58\xc2\xe4\xcd\x53\x95\x2c\x77",
            "SM64",
            18,
            "Super Mario 64",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\x01\xd1\xaf\x7f\x00\xff\xf4\x1f\x8e\xcb\x25\x0e\xee\x39\x99\x72\xd2\x35\x30",
            "sMMY",
            18,
            "Staked MMY",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\x02\x99\x36\x13\x68\x6a\xb0\xf7\x06\xef\x07\x88\x38\x70\xa9\x7d\x36\xfd\xcf",
            "SNS",
            18,
            "Sonic Name Service",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\xee\x92\x6b\xd8\xc7\x2b\x2d\x5f\xa1\xaf\x4d\x9e\x4c\xbb\x51\x5a\x1e\x3a\xdc",
            "SNX",
            18,
            "Synthetix Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\x7c\xf5\xba\x9c\x29\x1a\x1a\x8f\x57\xff\x14\x83\x6f\x6f\x9d\xc5\xc0\xf9\xdd",
            "SOLID",
            18,
            "Solidly",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x45\xea\x4a\x61\x6d\x21\x8c\xf1\xa5\xb4\xe1\x26\x25\x63\xbc\x00\x7e\x8d\x07",
            "SOPUCA",
            18,
            "sonic pussy cat",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\xfb\x17\x70\x09\xff\x39\xf5\x2c\x01\x34\xe8\x00\x7f\xa0\xe4\xba\xac\xbd\x07",
            "SOUL",
            18,
            "SoulPower",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\xaf\x0f\x0d\x7d\xe4\x4d\x96\x2c\x99\xee\x60\x70\xc9\x53\xff\x62\x77\x13\x75",
            "soyboysolid",
            18,
            "solidsoyboylananobonerinu",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x80\x03\xb6\x88\x94\x39\x77\xe6\x13\x0f\x4f\x68\xf2\x3a\xad\x93\x9a\x10\x40",
            "SPELL",
            18,
            "Spell Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\xc6\x1a\x78\xf1\x64\x88\x57\x76\xaa\x61\x0f\xb0\xfe\x12\x57\xdf\x78\xe5\x9b",
            "SPIRIT",
            18,
            "SpiritSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\xf4\x10\xf7\xe5\xff\xe5\x4b\x5d\xcc\x25\x37\x82\x0d\x39\xaf\x57\xe3\x4e\xeb",
            "SPIRIT-LP",
            18,
            "Spirit LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x98\xf7\x0f\xc7\x17\xad\xca\xde\x6b\x0a\x24\xdf\x54\xc4\x9e\xab\xdc\x5e\xfc\x46",
            "SPIRIT-LP",
            18,
            "Spirit LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x52\xa6\x66\xbc\x9b\x7a\x09\x33\x4c\x8a\x09\x3e\xcc\x6a\x3f\x82\x8f\xf3\xaa",
            "Spit",
            18,
            "Hawk Tuah",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\xd4\x65\xac\x93\x8f\x9b\x65\xc6\xc4\xf0\xef\x28\x18\xf9\xe1\x40\x33\xb9\xa4",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\x58\xb0\x8b\x66\x9f\x11\xb1\x56\xd7\x4e\xb0\xb9\xd8\x2b\x7b\x09\x80\x46\xbe",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\xb2\xd1\x14\x08\xf3\xbf\xfe\x2c\xd4\x1d\xb1\xea\x2f\xd7\x0d\x60\x23\x35\x0d",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x06\x3f\x8f\x5d\x45\x68\xe1\xc8\x24\xf6\x47\x5e\x7a\x11\x02\x3e\xae\x37\xec\xcd",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x03\x06\x08\xfc\x5d\xd3\xb6\x35\xfd\xf5\x78\xfc\x26\x96\x80\x65\x78\xb2\xe5",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x0a\x07\xaf\xf8\x76\x58\x6a\x07\x0e\xef\x36\x99\xb7\x35\x4e\x5a\x50\x42\x75\xca",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\xe0\x5e\x40\x05\x49\x48\x43\x99\x76\x13\xad\x21\xb5\x82\x10\xe8\x1a\x7a\xac",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\x06\x0c\xbb\xb8\x40\x51\xda\x39\xd9\x1a\xa6\x9c\x09\x93\xff\x64\xc1\xb2\xf7",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\xa8\x87\x2c\x74\xcf\x83\xd9\x9d\x54\xa0\xea\x4d\xa1\xa3\xbd\xaf\xb7\x0a\x52",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\x71\x5e\x94\x91\xdb\xea\x28\xa3\x14\x49\x30\x59\xc4\x3d\xd3\x05\x64\x01\x97",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xfc\x31\x1f\x95\x86\x24\xd5\x8f\x73\x34\x5d\xd6\xbc\x3d\x4d\x1f\x2d\x45\x9f",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x19\x08\x37\x54\x8f\x95\x5d\x2f\x73\x52\x05\xa7\x8c\xe4\xc2\xd8\x2a\xf3\xce\x0d",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\xb3\x75\x2f\xe8\x3d\x76\x57\x44\xea\xf6\x6a\xa3\xa1\x4c\x63\x3e\x82\xa0\x55",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\xe5\xf4\xfa\x6e\xa4\xde\xd6\xb5\xcd\xf2\x17\x03\x3b\xf9\xc1\x3f\x53\x1d\x4a",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\xb7\x73\x5f\x17\x98\xc6\xb6\x18\x79\x60\x7a\x0f\xe3\x63\xb5\xa5\x22\xd8\xa5",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\xde\xff\xee\xe4\xdb\xd0\x23\xb5\xb5\xf2\x27\x90\x37\xf9\x66\x73\x42\x15\x8b",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x26\x5a\xc8\x41\xeb\x6b\xdb\xea\x90\x67\x63\xd0\x77\x90\x04\xa2\x84\x4b\x47\xb7",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x28\x03\x62\x72\x26\xfc\xd4\xa9\x48\x51\x4d\xbc\xf1\xe1\x49\x3e\x27\x84\x96\xb1",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x2c\x6d\x14\xce\x6e\xba\x28\xab\x2a\x9c\x39\x1a\xad\x37\x27\x6d\xb2\xc5\x43\x9a",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x34\x61\xf5\x2c\xc9\x3f\x52\x23\xfd\x3b\xdc\x7f\x5c\xfd\x50\xa3\x5a\x02\xa5\xa9",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x36\x49\x1e\x0b\x9a\x50\x70\x58\x94\x89\xfa\x95\x91\x4d\x4e\x77\xa9\x90\xb2\x2c",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xc1\x80\xd1\x52\x78\xd8\xde\x40\x20\xa4\x2a\x10\xdd\x95\x11\x57\xab\x6a\xa2",
            "SPLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\x1f\xca\x7e\xc4\x37\x44\xae\x4f\x8c\x37\xf5\x1c\xee\x47\x69\x24\xa7\xb6\xce",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\x24\xbc\xb5\x79\x02\xe3\x6d\x66\x26\x63\xfe\x28\x7b\x06\x92\x6a\xed\x7c\x61",
            "SPLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x39\x07\x0c\x57\xa4\x0c\x8b\x69\x5d\x4b\x16\x3b\x3d\x7f\x1a\xe6\x3a\xc2\x60",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\x8c\x2e\xba\x9a\x2a\xa7\x21\x2a\x10\x68\xc7\x00\x59\xa6\xa3\x70\x48\x78\x43",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x91\x0e\x30\xf0\x20\xbd\x39\xbb\x0b\x99\x46\x10\xf9\x08\xfd\x6c\x46\xb2\x73",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\xdb\x7a\x9f\x33\x8b\xfc\xfc\xe5\x73\x9e\xc7\x70\x38\x42\x14\xb7\xff\x83\xf9",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x48\xf4\xf7\xbe\x4a\xc2\xd5\x83\xc0\x0d\xf2\x3f\x49\x30\xf8\x1b\x39\x2a\xb5\x2b",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\xae\x53\x94\x7c\xc8\xf5\x21\x58\x35\x05\x16\xd6\x1e\x0c\x95\x95\xa6\xf7\x3b",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x54\x9d\x44\xc9\xc7\x6e\xde\x84\xd4\x02\x25\x54\x55\x24\x70\xb8\x3c\x09\x7a\xff",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x22\xe6\xfa\x35\xa7\x05\x65\xee\x66\xa5\xc5\xe1\xfb\x5b\x37\x3b\x4d\xda\x4c",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x4e\xe6\x93\x1e\xfa\xb6\x54\x28\xbc\x45\xbb\x82\xd3\x4a\xf6\x41\x85\x39\x60",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\xf6\x54\x38\x44\x5e\x3d\xb4\x6b\xe3\x44\x22\x3d\xcb\xea\x17\x9b\x56\x60\x1b",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x5b\x36\x0b\x2f\x57\x72\x04\xd0\x00\xe4\x2b\x0b\x8d\x20\x0b\xa6\x92\x5e\x2b\x87",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x5b\xac\x93\x57\xa7\xf6\x0e\x9c\xe3\xcd\xfb\xfc\x66\x93\x6b\xa6\x0a\x2a\xf2\xd8",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\xf1\xc9\x66\x93\xec\xc7\xdf\x07\x5f\x1a\xc0\x66\xf5\xb1\xd3\xa1\x3d\x79\x41",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x63\x97\xa3\x89\x55\x07\x19\xb8\x8c\x47\x04\x1b\x46\x9b\x9d\x36\x68\xe4\x8e\x7a",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xd3\xc7\x42\x0f\xf6\x28\xaf\xc1\x8c\x52\xfa\x8c\xd5\x97\xa1\x5e\x56\xb7\x35",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x0b\x4c\xac\x02\xa5\xa5\xdd\x4d\xc3\xc3\xbd\x24\x1e\x47\xdd\xa0\x20\x28\xf4",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x27\x3c\x55\x0a\x3e\x56\x79\x63\x14\x6a\x10\xfe\x38\x73\x05\x58\x52\xa0\x09",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x58\x8e\xdc\xc8\xce\xb7\x29\xa3\xde\x4e\x0a\x50\xb8\x38\x29\x85\xce\xcb\x90",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x5c\x6c\xfb\x6d\x94\x4e\x8d\x02\xed\xc5\x02\x57\x06\x4c\x4b\x5a\xf8\x79\x7e",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x85\xf5\xad\x57\x86\x3f\x0b\x87\x59\x0c\x74\x05\xed\x1d\xe4\x5d\x39\xf8\x29",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\xb9\x92\xc1\xa9\xe6\xb5\x52\xca\x72\x0f\x73\x66\xc3\x86\xa0\x02\x69\x3a\xb3",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\x8b\x8f\x61\x6c\xe2\x40\x5f\xd0\x57\x9c\x3c\x02\x38\xd3\x8a\x75\x9d\x67\xaa",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\x0f\x5f\xe8\xe8\xf4\x0c\xa2\xe1\xc8\xa6\x3a\x9b\xb8\x65\x92\xf9\x65\x0b\x9f",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x0b\xc6\x18\xe3\xe0\x8d\xec\x2f\x1b\x89\x71\x3b\xd6\x4f\xba\x33\xb7\x29\x97",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\xbe\x88\x10\xcd\x77\x93\xbd\x6c\x80\x74\xe4\xb8\x0d\x7c\x57\x4a\x96\x63\x2f",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x4d\x9e\xcb\x91\xc9\x0a\xc3\x1d\x7b\x33\xb2\x80\x6c\x2e\xb3\x5a\x67\xf0\xdd",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\x83\xc4\xac\xc0\xae\xf7\xf3\x96\xca\x33\xcb\xc6\x92\x11\x69\xba\xfe\x8b\xb9",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\xeb\x13\xc7\xda\xfd\x26\xcf\x69\x50\xb2\x9e\xd2\xf8\x83\x55\xe3\x4c\x58\x89",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x04\xc7\xb8\xe8\x6d\x5b\x4d\x3d\x3d\xe6\x60\x85\x7e\xd9\x94\xd1\xb6\xd8\x78",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x16\x4d\xc1\x60\xa0\x06\x54\x11\xcc\x62\x82\xdc\x4a\xb0\x0d\x3d\x53\x31\x02",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\x17\x74\x61\x14\xf1\xd7\x57\xde\x0f\xc2\x14\xc1\x76\xf4\x1f\x4a\x6b\x0c\x04",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x96\xbb\x44\x6c\x05\x2e\x21\x2a\xb5\xc3\x31\x7a\xbb\x09\xad\x19\x1e\xba\x2d\x5d",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\x6b\x87\xf4\x34\xb8\xdc\x14\xe9\x75\x08\xfb\xee\x38\x74\x51\x64\xcf\xdb\x05",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\x6e\xe3\xc6\xd2\x1d\xab\x29\xd8\xb2\x49\x8f\x77\x81\x2a\x8c\x6b\xba\x50\x66",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\xeb\x0d\xbc\x35\x77\x39\xa0\x4c\x69\xc8\x65\xbf\x54\xc7\x1e\x92\x56\x3e\xba",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\x82\x63\x10\x6a\x2b\x98\x73\x0e\x1b\x41\x75\x9a\xa0\xf9\xc5\x5d\x2b\x3b\xc9",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\xc8\x43\x52\x9b\xb7\x30\xad\xbb\x59\x48\x90\xec\xc7\x2a\x52\x14\xc8\x1b\x74",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\x56\xc7\x3c\xea\xb8\x37\x18\x15\x4d\x28\xb5\xb5\xa8\x11\xce\xd3\xf3\x68\x50",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xc5\x33\xb6\xc1\xbf\xc6\x76\x67\x6b\x2c\xf4\x79\xf5\x4e\xa0\x51\x30\x49\xa4\xc3",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\xb4\xb0\xef\x31\xe7\x30\xd3\xb4\x2f\xaa\xd8\x78\x31\x31\x0b\xa4\xe9\xb1\x77",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x3e\xc3\x80\xff\xdc\x17\x00\x13\x1c\xd3\xf6\x0f\x5f\xb8\x24\x6d\xf6\x0d\x94",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xcb\x52\x81\x77\xf4\x41\x89\x1e\x99\x93\x7f\x29\xf9\x45\xbe\x09\xb9\x2e\x0b\xb1",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\xee\x53\x15\x27\xc9\xb1\xe6\x7c\xad\x71\x25\x11\xb8\x01\x33\x42\x2f\x0a\x0f",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xd2\x57\x3e\x97\xcc\xb8\xda\xf0\x41\x25\x72\xf4\x48\x7a\x4a\xf0\x48\x0f\x41\xfd",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xe8\xf9\x64\xaa\x73\xd3\x74\xed\x5e\x19\xad\xb7\x3b\x39\x9c\x32\x20\x76\x00\xc3",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xe9\x6f\x7c\xdd\x05\xec\x90\xe6\x31\x73\x06\x6f\xa0\xe7\xb0\xcf\x87\x6a\x3f\xc8",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x26\xf5\xbb\x0f\x94\x2b\x61\x98\x2f\x70\xc6\xc1\xc6\xd3\xf8\x14\x0a\x35\x9e",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x36\xdd\x09\x59\xc5\xce\x5c\x4e\x24\x6d\x58\xf3\xf2\xf6\x6b\xc7\xe6\x70\x29",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x88\xa0\x25\xc7\x1c\x6a\xc3\x54\x02\x53\x4a\x08\x08\x83\xf2\x78\x83\xcf\x3d",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xf3\x08\x0a\x8e\x3f\xb0\xfa\x43\x11\x95\x07\xd4\x10\xba\x20\xa3\xfc\xad\xd6\x96",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\xf2\xd3\xae\x71\x5c\x88\x53\xcc\x43\x6d\xe7\xdd\xe2\x8d\xc0\xe2\x10\xd1\xa6",
            "spLP",
            18,
            "Spooky LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xfa\x96\x9f\x25\xaf\x02\x02\x02\x9f\xdf\x87\x2f\x7b\xd1\x23\xbe\x1c\x10\xb4",
            "SRUSH",
            18,
            "SonicRush",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\x8a\x14\x63\x35\x01\x4a\x77\xa6\x14\x32\x80\x92\x9c\x0b\xac\x20\x28\x2a\x25",
            "SS",
            9,
            "SpookyShiba",
        )
        yield (  # address, symbol, decimals, name
            b"\x84\x0c\x7f\x99\x7c\x56\xb9\xa4\xca\x81\xf9\x18\x9c\x5c\xa7\xa9\x26\x63\x5e\x6f",
            "StCock",
            18,
            "StableCoockie",
        )
        yield (  # address, symbol, decimals, name
            b"\xd8\x50\xaf\xc4\x51\x1c\x2d\x14\x30\x90\x76\xb3\x8e\xbf\x97\x4f\xd9\x79\x48\x18",
            "stpyreFTM",
            18,
            "Staked PyreSwap Vibratile Fantom",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x94\xbc\x72\xf9\x06\x4d\x78\x67\x0e\xa5\x3d\xec\xfa\xef\x95\x9d\x41\xc9\x97",
            "SUPER2",
            18,
            "SUPER3",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x75\xa4\x38\xb2\xe0\xcb\x8b\xb0\x1e\xc1\xe1\xe3\x76\xde\x11\xd4\x44\x77\xcc",
            "SUSHI",
            18,
            "Sushi",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\xcf\x4d\xc8\x79\x97\x9c\x68\xfa\x25\x5f\x73\x1f\xe8\xdc\xf7\x19\x70\xc9\xbc",
            "SYF",
            18,
            "Syfin",
        )
        yield (  # address, symbol, decimals, name
            b"\xe5\x5e\x19\xfb\x4f\x2d\x85\xaf\x75\x89\x50\x95\x77\x14\x29\x2d\xac\x1e\x25\xb2",
            "SYN",
            18,
            "Synapse",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x9f\xf3\xef\xd5\x08\x74\x14\x92\xb5\xbd\x45\x26\x9a\xa4\x5a\x43\xbe\x59\xc8",
            "TANGO",
            18,
            "TANGO",
        )
        yield (  # address, symbol, decimals, name
            b"\xb7\xc2\xdd\xb1\xeb\xac\x10\x56\x23\x1e\xf2\x2c\x1b\x0a\x13\x98\x85\x37\xa2\x74",
            "TAROT",
            18,
            "Tarot",
        )
        yield (  # address, symbol, decimals, name
            b"\xfb\xfa\xe0\xdd\x49\x88\x2e\x50\x39\x82\xf8\xeb\x4b\x8b\x1e\x46\x4e\xca\x0b\x91",
            "TCS",
            18,
            "Timechain Swap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\xbb\x13\x34\x14\x83\x5d\xe6\xcf\xd8\xc9\x3b\xb6\x0e\x4a\x1f\xbb\x77\x19\xb3",
            "TEST",
            18,
            "test",
        )
        yield (  # address, symbol, decimals, name
            b"\x05\xd1\x1e\xcb\x9a\x42\x19\xb8\x12\x0e\x99\xe1\x2c\x8b\xe5\x01\x1a\xee\x61\xc5",
            "test",
            18,
            "whodis",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\x07\x77\xb2\x14\x4e\x86\xf2\x6f\x1d\xca\x2b\xe1\x94\x0b\xdc\x4a\xe5\x2a\xa6",
            "test",
            9,
            "test",
        )
        yield (  # address, symbol, decimals, name
            b"\xea\xf5\xca\xda\x52\x47\xea\x15\xc9\xc1\x68\xae\x66\x82\x57\x0b\x95\x9d\x3a\x6d",
            "TestDo",
            18,
            "TestD",
        )
        yield (  # address, symbol, decimals, name
            b"\xf7\x9d\x07\xf0\x6e\xad\xf0\x4d\x78\x42\xd9\x1b\xf3\x22\xb1\x28\xc5\xe8\xd9\xac",
            "TLC",
            18,
            "TLCash",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x79\xc2\x6d\xc9\xbb\x73\x65\x4a\x65\x35\x46\xa3\x6c\xbe\x41\x72\x95\x1b\x8b",
            "TOMB-V2-LP",
            18,
            "TombSwap V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\x19\xc5\xad\xb2\x54\x60\x11\x78\xbe\xbc\xc5\x35\x3a\x71\x9d\xbc\x1b\x11\x9a",
            "toona",
            18,
            "toona",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\xa3\x7a\xed\xc0\xc1\x8a\xa1\x39\xe1\x20\xe1\xcd\xc6\x73\xbb\xb2\x06\x3e\x6f",
            "TOTEM",
            18,
            "Totem Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\x7a\x9d\x5d\xf9\xbe\xda\x06\xf6\xb0\x21\x13\x6a\x2e\xfe\x7b\xe2\x42\xfc\xc9",
            "TRAVA",
            18,
            "TravaFinance Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\x14\x6c\x87\xd2\x15\x96\xe3\x0d\xd8\x9d\x45\x46\x6c\xc6\x3f\x48\x3f\x21\xff",
            "TRUMPMUSK5",
            9,
            "newsmemecoin.com Elon Musk and D",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\xdf\x39\x28\x5d\x7c\xa8\xeb\x3f\x09\x0f\xda\x0c\x06\x9b\xa5\xf4\x14\x5b\x37",
            "TSHARE",
            18,
            "TSHARE",
        )
        yield (  # address, symbol, decimals, name
            b"\x4e\x4f\x84\xaf\xf9\xfc\xef\x48\x32\x8b\x5d\x3c\xdb\xa7\xf7\x67\x83\xb9\xf6\x48",
            "tt6_presale",
            18,
            "tt6_presale",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x58\x39\x44\x59\xe2\xa4\x76\x23\xca\x65\x90\x65\x8a\xb4\x53\x2b\x4a\x0f\xcb",
            "tt7_presale",
            18,
            "tt7_presale",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x77\x59\x40\xf5\xda\x16\xde\xc9\xcb\xa0\x9d\x20\x44\x26\x8f\xb4\xc9\x5d\x43",
            "tt_presale",
            18,
            "Tt_presale",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x33\xaa\xce\x31\xb1\xed\xa9\x61\x06\x1a\x2a\x78\x60\xba\x2c\x77\x93\x65\x54",
            "TURD",
            18,
            "Heisenturd",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\xf4\xb7\x4f\x21\xa6\x4b\xa1\x61\x19\x59\x51\xab\x66\xf2\xe8\xae\xed\x24\xdc",
            "TUSD",
            6,
            "Test USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x19\x60\xe2\x9b\x2c\xa5\x81\xa3\x8c\x5c\x47\x4e\x12\x3f\x42\x0f\x80\x92\xdb",
            "UBXS",
            6,
            "UBXS Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x78\x5f\x89\xb7\x41\xa8\xec\xac\xd7\x88\xf4\xf2\x92\x73\xf4\x3f\x0e\xd8\x7e\xc7",
            "UMAN",
            18,
            "ANCESTRAL",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x56\x86\xf3\x87\xbf\xb1\xf0\x58\xb4\xfb\xee\x09\xbf\x05\x55\x0f\x3a\xdd\x8d",
            "Uman",
            18,
            "Worldofuman",
        )
        yield (  # address, symbol, decimals, name
            b"\xa4\x25\xaa\xe4\x32\xba\x89\x20\x07\x9b\xf7\xd3\xd1\xdd\xe5\x31\xfc\x68\x28\x13",
            "USA",
            18,
            "American Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x73\x30\x95\xb8\x0a\x04\xb3\x8b\x0d\x10\xcc\x88\x45\x24\xa3\xd0\x9b\x83\x6a",
            "USDC.e",
            6,
            "Bridged USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\xa8\x8b\xa9\x80\x01\x6c\x96\x80\x33\x3e\x11\x25\xc4\x66\x8c\x0a\x0b\xae\x25\x83",
            "USDC2",
            18,
            "USDC 2.0",
        )
        yield (  # address, symbol, decimals, name
            b"\x28\xa9\x2d\xde\x19\xd9\x98\x9f\x39\xa4\x99\x05\xd7\xc9\xc2\xfa\xc7\x79\x9b\xdf",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\x8d\xb3\xd7\x49\x1c\xfa\xe4\x4a\x14\x87\xba\xd2\xe5\xf4\x7b\x1a\x37\x9d\x8c",
            "USDC",
            9,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x9d\x68\x02\x96\x88\xea\xbf\x47\x30\x97\xa2\xfc\x38\xef\x61\x63\x3a\x3c\x7a",
            "USDT",
            6,
            "Frapped USDT",
        )
        yield (  # address, symbol, decimals, name
            b"\xcc\x1b\x99\xdd\xac\x1a\x33\xc2\x01\xa7\x42\xa1\x85\x16\x62\xe8\x7b\xc7\xf2\x2c",
            "USDT",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x84\x6e\x4d\x51\xd7\xe2\x04\x3c\x1a\x87\xe0\xab\x74\x90\xb9\x3f\xb9\x40\x35\x7b",
            "USTC",
            6,
            "UST (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x82\x42\x78\x79\x95\xbb\x5e\x05\xb1\xce\x0b\xc5\x99\x12\x3c\x8f\x3d\x44\x0c",
            "vAMM-eliteFmoneySFTM",
            18,
            "VolatileV1 AMM - eliteFmoneySFTM",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\x95\x6a\x94\x85\x99\xec\xa3\x49\x05\xb9\x29\x15\x4e\x92\xb0\xa2\xf9\x97\x61",
            "vAMM-MAATALETHEIA",
            18,
            "VolatileV1 AMM - MAAT/ALETHEIA",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\x89\xca\x16\x19\xe6\x68\x85\xe4\x3d\x32\x97\xd0\x0a\xe7\x23\x72\x26\x99\x63",
            "vAMM-SOLIDOXD",
            18,
            "VolatileV1 AMM - SOLID/OXD",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\xe6\x10\xf1\xea\x79\x50\xbc\x89\xeb\x4d\xcc\xed\x6b\x6f\x0c\x92\xa4\x62\xa5",
            "vAMM-WFTMJOINT",
            18,
            "VolatileV1 AMM - WFTM/JOINT",
        )
        yield (  # address, symbol, decimals, name
            b"\x26\xfb\x59\x82\xfe\x04\xc5\xcd\x9c\xe5\xcf\xd7\xc9\x81\x4b\xf3\x58\x3b\x15\x1a",
            "vAMM-WFTMscc",
            18,
            "VolatileV1 AMM - WFTM/scc",
        )
        yield (  # address, symbol, decimals, name
            b"\x1e\x62\x88\xda\xb1\x35\x29\x53\x2f\xea\x83\x77\x3d\x57\xfe\x42\x2b\xe0\xe8\xd7",
            "vAMM-WFTMsFTMX",
            18,
            "VolatileV1 AMM - WFTM/sFTMX",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\x26\xd9\x22\xa7\x79\xee\xc8\xc3\xed\xfe\x44\x51\x31\x6e\x47\x1b\x28\x19\x89",
            "vAMM-WFTMsKitty",
            18,
            "VolatileV1 AMM - WFTM/sKitty",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x5a\xac\x60\x7d\xc2\x8b\x32\x25\xfc\x5f\xbd\x03\x63\x5b\x6f\xbb\x26\x24\x10",
            "VAMPBOND",
            18,
            "VAMPBOND",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x13\xf3\x55\x1c\x4d\x39\x84\xff\xba\xdf\xb4\x2f\x78\x0d\x0c\x87\x63\xce\x94",
            "veEQUAL",
            18,
            "veEQUAL",
        )
        yield (  # address, symbol, decimals, name
            b"\x92\x2d\x64\x1a\x42\x6d\xcf\xfa\xef\x11\x68\x0e\x53\x58\xf3\x4d\x97\xd1\x12\xe1",
            "VID",
            18,
            "VideoCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xc3\xd6\x98\x39\x7f\x97\xe3\x23\x97\x9b\xed\x25\x9f\x85\xd7\xdd\xe4\xe1\xed\x07",
            "VMX",
            18,
            "Voodoo Trade",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\xa9\x66\x08\xcf\x53\x84\x54\x3a\xcd\x11\x4f\xa8\x2f\x6f\xc6\xb9\xe8\x58\x36",
            "VNP",
            9,
            "VND For Panda",
        )
        yield (  # address, symbol, decimals, name
            b"\xbe\xbb\x3a\x8a\x1e\x01\xee\x54\xcf\xbe\xbb\xe3\x4f\xfe\xb6\x1c\x90\x00\x2d\x6d",
            "vTAROT",
            18,
            "Tarot Vault Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\x76\xbf\x71\xbc\x72\xb1\x91\x15\x24\xdf\xd3\x58\x2f\x2b\xef\xac\x3d\x86\xe9",
            "WAIFU",
            18,
            "Fantom waifus",
        )
        yield (  # address, symbol, decimals, name
            b"\x70\x68\x69\x97\xe4\x4e\xcd\x17\xd2\x6c\xe5\x82\x2c\x61\x49\xd9\x4c\xe0\x14\xe8",
            "Wei",
            8,
            "WeiSwap",
        )
        yield (  # address, symbol, decimals, name
            b"\x91\x1d\xa0\x2c\x12\x32\xa3\xc3\xe1\x41\x8b\x83\x4a\x31\x19\x21\x14\x3b\x04\xd7",
            "WeVE",
            18,
            "veDAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\xbe\x37\x0d\x53\x12\xf4\x4c\xb4\x2c\xe3\x77\xbc\x9b\x8a\x0c\xef\x1a\x4c\x83",
            "WFTM",
            18,
            "Wrapped Fantom",
        )
        yield (  # address, symbol, decimals, name
            b"\xe9\x92\xbe\xab\x66\x59\xbf\xf4\x47\x89\x36\x41\xa3\x78\xfb\xbf\x03\x1c\x5b\xd6",
            "WIGO",
            18,
            "WigoSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x9e\x48\x19\xcd\x67\x5b\x48\x17\xed\x49\xe5\xb6\xbc\x22\xb9\x8b\x22\xcd\xb1",
            "Wigo-LP",
            18,
            "Wigo LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\xe7\xf7\x97\xcc\x1f\x24\x6c\xd5\x29\xd6\x7a\xe0\x6b\x27\xd2\x6e\x53\x67\xda",
            "WOOF",
            18,
            "DogDog",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\xcc\xf6\x0f\x70\x01\x46\xbe\xa8\xef\x78\x32\x82\x08\x00\xe2\xdf\xa9\x2e\xda",
            "wsHEC",
            18,
            "Wrapped sHEC",
        )
        yield (  # address, symbol, decimals, name
            b"\x29\xb0\xda\x86\xe4\x84\xe1\xc0\x02\x9b\x56\xe8\x17\x91\x2d\x77\x8a\xc0\xec\x69",
            "YFI",
            18,
            "yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xcf\x70\x4f\x9c\x44\xb3\x05\xa2\xb6\x85\xc3\xa9\x6b\xb1\x96\x96\x61\x2c\xc6\xba",
            "YLP",
            18,
            "YOSHI LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xce\x2f\xc0\xbd\xc1\x8b\xd6\xa4\xd9\xa7\x25\x79\x1a\x3d\xee\x33\xf3\xa2\x3b\xb7",
            "yvWETH",
            18,
            "WETH yVault",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x5d\xb6\xc2\xa2\xfd\x53\xa2\xd3\xfc\x42\x46\xbb\xd8\xac\xce\x5f\x93\xba\xe1",
            "ZOMBI",
            18,
            "ZombI",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\x38\x45\x07\x3e\xd0\xfd\x9c\xe8\xf1\x60\x04\xed\xeb\x3d\xf0\xe0\xef\x33\x63",
            "\u9884\u6d4b\u90fd\u662fsb",
            18,
            "\u9884\u6d4b\u90fd\u662fsb",
        )
    if chain_id == 324:  # zksync
        yield (  # address, symbol, decimals, name
            b"\x17\x18\x9a\x66\x20\xc7\xb5\xfe\x59\x1f\x18\x42\x3f\xbd\xa2\x69\x4a\x7c\x76\x01",
            "$BOLT",
            9,
            "BOLT",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\xa8\x3b\xfc\xae\x5f\xa4\x92\x19\xc2\x6b\xa4\x5c\xa3\xbb\xc3\x8b\xba\x04\xad",
            "6y6y6",
            18,
            "kmj",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\xce\x82\xfa\xcd\x20\x60\x20\xf4\xe7\xea\x29\x58\xbd\x9a\xc8\x77\x4b\xd7\x2c",
            "aaaa",
            18,
            "aaaa",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\x66\x87\xba\xc8\x93\xec\x5a\xf0\xf9\x08\x40\x98\xe2\xb2\xfc\x65\xac\xbd\x0b",
            "AAI",
            18,
            "AirdropSnipa AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\x4b\x83\x55\x5d\x8a\x31\x19\xb0\xa6\x9a\x7b\xc2\xf0\xa0\x38\x83\x08\xfe\xe3",
            "AAI",
            18,
            "AutoAir AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\xc6\x1c\xff\x9d\xd4\x78\xc7\x8c\x09\xf9\xb7\xcf\x91\xf1\xc3\xa8\xe8\xc0\x90",
            "AEYE",
            18,
            "Agamotto by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\xe6\x02\x42\x91\x86\xeb\x21\x18\xf3\x29\xb2\xb5\xdf\x8f\xc7\xe4\xfc\x72\xc6\xbd",
            "AEYE",
            18,
            "AEYE by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\x6f\xf1\x42\xb6\x4d\x76\xd2\xff\xd7\x16\xda\x0f\x82\x02\xe0\xdd\x4c\x68\x22",
            "aishell",
            18,
            "AiShell by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x8e\x19\xcf\x4b\xc3\x52\x70\x5a\x25\x19\x6d\x35\x2f\x6d\x81\x30\x08\x8b\xcf",
            "AIson",
            18,
            "AIson by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x9c\x74\x7d\xb4\x76\x02\x21\x0e\xa7\x51\x3c\x9d\x00\xab\xf3\x56\xb5\x38\x80",
            "aiws",
            18,
            "aiws",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\x88\x70\x6a\xbb\x0b\x33\x22\x1e\x65\x25\x90\xb1\x35\xc5\x5e\x9d\x6f\x33\xd7",
            "aizure",
            18,
            "aizure",
        )
        yield (  # address, symbol, decimals, name
            b"\xa5\x2b\x83\x26\x1b\x8b\xc3\x1f\xad\x96\xe1\x8a\x17\x2f\x05\x55\x9c\xae\xc1\xb0",
            "ALI",
            18,
            "Artificial Liquid Intelligence T",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\xc6\x17\x1a\xf9\x97\x6e\x76\xe1\x81\x1f\x2d\x40\x24\xde\x99\xad\x16\x94\xf0",
            "almZFZK-WETH",
            18,
            "alm zkswapFinance ZK-WETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\x96\x43\x65\x7e\x48\xe2\xfa\x8e\xcf\x84\x82\x35\x5d\x46\x44\xa4\x4b\x01\x33",
            "alo",
            18,
            "alo",
        )
        yield (  # address, symbol, decimals, name
            b"\xa8\x6f\x37\x7c\x67\xab\x81\x18\xc0\x01\x19\x7d\xc2\x52\xe9\x29\xaa\x65\x05\x8b",
            "AMIS",
            9,
            "AMIS",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\x0b\x37\xc4\x4e\x14\xa4\x20\xfb\x98\x44\xd4\x84\x62\x56\x80\x0b\xea\x7c\xe1",
            "ANDs",
            18,
            "ANDERS",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\x5c\x58\x34\x4b\x78\xbb\xbd\xe0\x21\xe3\x78\x24\x87\xe7\x3b\xb3\x32\xc1\xef",
            "APPA",
            18,
            "Dappad",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\x15\x4b\x6a\x14\x90\x81\xe7\x96\x04\x9f\x2a\x3b\x17\xe1\xc3\x56\xd2\xba\x61",
            "apxETH",
            18,
            "Autocompounding Pirex Ether OFT",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\xb9\xd5\x84\x3f\xf8\x4e\xda\xcf\x5f\xab\x0b\xcb\x5a\xe9\xc3\xf8\xdb\xef\x6c",
            "ATH",
            18,
            "Aethir OFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\x9c\x05\x04\xc4\x4a\xc8\x01\x75\xcf\x19\xc2\xf3\xdc\x70\xec\x65\x84\x3c\x04",
            "auxs",
            18,
            "auxs",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x52\x79\xe9\x9c\xa7\x78\x6f\xb1\x3f\x82\x7f\xc1\xfb\x4f\x61\x68\x49\x33\xd6",
            "AVAX",
            18,
            "Avalanche",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\x61\xb7\xf7\xc2\x58\x69\x46\x44\x31\x31\x7d\x76\x77\x40\x32\xb1\xfa\x89\xd8",
            "AVI",
            18,
            "Avi",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xf0\xfb\x82\x13\xf4\x94\x0e\xa0\xb3\x0e\x80\xf8\x5e\x71\xdd\x85\x67\x05\x16",
            "bad guy",
            18,
            "bad guy",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\x73\x7e\xb2\xc6\x0d\xa3\x18\x8e\x15\x80\x7e\x20\xa7\xf8\x56\x3a\xe1\x0f\xd5",
            "bAInance",
            18,
            "bAInace Labs",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\xfe\xba\x72\xf5\x5e\x7b\xdd\x08\xe4\x77\x3b\x20\x3c\x04\xaa\x3f\x00\x84\xb8",
            "Bean",
            18,
            "Mr. Bean by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\xcc\x99\xa9\x02\x4d\xea\x82\x95\xe5\xe8\x6b\xce\xf6\xbd\x66\x84\xb8\x10\x1d\x4f",
            "BEG",
            18,
            "eBeggar",
        )
        yield (  # address, symbol, decimals, name
            b"\xa4\x8d\xba\x37\xf9\x34\x8c\x30\xec\x43\x3e\xf2\x78\xc8\xfb\xa4\xad\x52\x8b\x98",
            "BIC",
            18,
            "Bicycle",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\xd4\xa7\x1f\x34\xf0\xe1\x47\xc8\xbd\x86\x34\x85\x76\xc3\x5f\x39\xcf\x36\x14",
            "BLADE",
            18,
            "Crypto Valkyrie",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\xfc\xf1\x58\xb9\x8e\xc9\x36\x7a\x68\x78\xe4\xaa\xee\x42\xe4\x5f\x1f\xe9\xe0",
            "BLINK",
            18,
            "BlackPink",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\x44\x7b\x87\xb7\xdf\x51\x27\xc3\x24\x51\x60\x17\x28\x86\x92\xea\x2c\x28\x02",
            "BLO",
            18,
            "Blo",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\xe9\x55\x52\xc5\x51\xa9\x48\xba\xc3\xa4\x34\x66\x32\x07\xb5\x13\xb1\x09\xe4",
            "BOMB",
            18,
            "Bomb Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x07\x45\x7e\xbb\xbc\x50\xd0\xa0\x78\x9c\xe1\x54\xa1\x48\xc2\x90\x48\xff\x8b\xc2",
            "BPT",
            18,
            "BullishGPT by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x78\x8d\x66\xf8\x52\x8e\xc5\x37\xa7\x8c\x2b\xbd\x55\x6f\x02\xba\x1c\x31\xb0\x8d",
            "BRAIN",
            18,
            "Brainrot by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\x39\xbb\x41\x16\xb4\xef\xc1\x45\xec\x4f\x0e\x2e\xa7\x50\x12\xd6\xc0\xf1\x81",
            "BUSD",
            18,
            "Binance USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\xd0\xf6\xd0\x6d\x21\xde\x76\x51\xd1\xe5\xac\x3d\x33\xfe\xff\xb9\xd7\x8b\x27",
            "Cake-LP",
            18,
            "Pancake LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\xc8\x2f\x5f\x38\x18\xeb\xf6\xb9\xd7\x5c\x63\xd8\x6a\xe6\x76\x71\x2d\x6c\xfb\x59",
            "Cake-LP",
            18,
            "Pancake LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x21\xc3\xd6\x38\x82\x6b\x76\x8d\x44\x57\x01\xbf\xae\x13\xd3\x84\xc3\xd8\x12",
            "Cake-LP",
            18,
            "Pancake LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x11\x46\xc4\xb2\xae\xcd\x98\x04\x51\xa6\x77\x17\xc3\x30\x50\x68\x0e\x08\x5b",
            "CakeWETH-A",
            18,
            "SyncSwap Aqua LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\x4c\xcc\xb2\x21\x99\x09\x1c\x02\x18\x64\xee\xbb\x68\x41\xe3\xd5\xd8\xed\x4d",
            "CAR",
            18,
            "Cards",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\xaf\xb6\x1b\xc9\x1c\x59\x49\x77\xe6\x5a\xf5\x4f\xf8\xb5\xec\xc1\xc3\x89\x95",
            "CAS",
            18,
            "Casa",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\xfd\x26\x0f\x7f\x91\x53\x8d\x8e\xd5\x40\x3d\x49\x89\x77\xdd\x91\x01\x6c\xbf",
            "chAIn",
            18,
            "chAIn by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\xe1\x64\x33\x62\x10\x20\xfe\xac\xf8\xb0\x3a\x11\x77\x54\x8d\x7f\xfa\x0e\x2a",
            "CHRM",
            18,
            "ChatCharm AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\xc7\x94\xab\xd8\x3a\x61\xd5\x33\xc0\xf8\x45\x69\x33\x34\x99\x00\x0f\x83\xdf",
            "chubby",
            18,
            "chubby girl",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x01\x01\x94\x04\xee\xc4\x49\xca\x71\xf6\x70\x90\x19\x50\x10\xd6\x45\x75\x22",
            "COCO",
            18,
            "COCO",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\x3b\x17\x86\xcf\x7f\x51\x10\x0a\xf5\x19\xbd\xce\xcd\xb0\xde\x98\x53\x31\x13",
            "COIN",
            18,
            "coinbAIse Ventures",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\x5a\x74\x26\x27\x7c\x28\x8e\x96\x3a\x9b\x6a\xb2\xc9\xe5\xe6\xc4\xb5\xf0\x5c",
            "CROWD",
            18,
            "CrowdTokenWrapper",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\xa6\x18\x69\x45\x4d\x41\x4e\x1e\xb4\x59\xa5\xde\xe9\x12\x20\xfa\x29\x25\x1d",
            "CUV",
            18,
            "Curvic by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x4b\x9e\xb6\xc0\xb6\xea\x15\x17\x6b\xbf\x62\x84\x1c\x6b\x2a\x8a\x39\x8c\xb6\x56",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xcf\xc8\x1f\x9e\x86\x63\xcf\xd3\x4b\x39\x99\x35\xcb\x27\x00\xab\x50\xf5\x5c\x7a",
            "DEAN",
            18,
            "DEAN AI by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x76\xd3\x63\xdc\xb5\xcb\x77\x38\x24\x25\x6c\x2d\x15\x2b\x55\x23\x42\x45\xe0",
            "DES",
            18,
            "Destinations",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\x29\xbc\xac\x44\x17\xa2\x1d\x7e\x6f\xc8\x6f\x6d\xae\x1c\xc7\xf2\x7a\x2e\x41",
            "DEXTF",
            18,
            "DEXTF Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\x1c\xc5\xdc\xb6\xf1\xac\x2c\x88\xb8\xa4\x7b\x21\x29\x16\xbe\xfb\x8e\xda\xe3",
            "DOF",
            18,
            "DogeFarmZK",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\x00\x5c\x32\x9c\x14\x06\x4e\xb3\x89\xb2\x37\x52\x62\xe1\xf5\x8a\x17\x3b\x45",
            "DOGE",
            18,
            "DOGE",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\xe1\x35\xad\x23\xac\x90\xf9\xf7\x39\x67\xfe\xa4\x9c\x5f\x52\x0e\x60\x02\x98",
            "Ducat",
            18,
            "Ducat",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\xdb\x70\x78\x17\x85\x01\xe0\x60\x35\x8d\x0d\xc1\xd1\x44\x4e\x89\x25\x86\x3b",
            "Egg",
            18,
            "Egg by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\x5a\xc5\x81\xc6\xf0\x5e\x71\x82\x8a\x61\x4c\xca\x91\xcb\x06\x42\x02\x30\x8d",
            "ELLAI",
            18,
            "ELLAI",
        )
        yield (  # address, symbol, decimals, name
            b"\x2b\x95\x15\x57\x04\x9a\x58\xc8\xf8\xac\xdd\x6b\xec\xc5\x15\xdb\xe4\x68\x73\x43",
            "EU",
            18,
            "Eu",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\xff\xff\xff\xff\xf1\x8b\x96\xed\x12\xe2\xd8\xbf\x95\x82\x46\xbf\xcd\x58\xc4",
            "F????",
            18,
            "Many Fs",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\xb1\x05\xa3\xea\xd2\x27\x31\x08\x2b\x79\x0c\xa9\xa0\x0d\x9a\x3a\x76\x27\xf9",
            "FIUSD",
            2,
            "Fidelity ILF USD Fund Class G Ac",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xf3\x36\x09\x57\x5b\xea\x01\x92\x87\x50\xa4\x0b\xdd\x21\xa7\x75\x9a\xb4\xa1",
            "fLUSD",
            18,
            "FARM_LUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\x4a\xae\x4a\x07\x43\xae\xec\x1d\x58\x4f\x2b\x2a\xbc\x1e\xbd\xc1\x2f\x1b\x0f",
            "frxETH",
            18,
            "Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\xd0\xf1\xa9\x2c\x3b\xe7\x4a\xcb\x58\xa0\xab\xe2\x57\x5d\x75\x52\x93\x14\x54",
            "FTM",
            18,
            "Fantom Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe5\x93\x85\x3b\x4d\x60\x3d\x5b\x8f\x21\x03\x6b\xb4\xad\x0d\x18\x80\x09\x7a\x6e",
            "FUL",
            18,
            "FUL",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\xbb\x4b\x94\xc8\xcc\x07\xdf\x50\x7f\x26\x5c\x9e\x44\xf2\x64\x1e\xc4\x60\xf5",
            "FUTURE",
            18,
            "ZK Future",
        )
        yield (  # address, symbol, decimals, name
            b"\x2c\xd2\xc6\x87\xbc\x5e\x49\xb9\x63\xcf\xc7\xc0\xb3\x29\xfe\xea\xaf\x08\x8d\x5b",
            "fZK",
            18,
            "FARM_ZK",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\x6a\x8b\xcf\x8a\xcb\xab\x1a\x38\xe5\x3d\xb8\x3c\x74\xa4\xb1\xa5\x3b\xf7\x82",
            "fZK",
            18,
            "FARM_ZK",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\xf6\x45\x03\xdd\x4b\xf8\x6b\x44\xb6\x1b\x2c\xb5\xfb\xd6\x6d\x0f\x88\x32\xde",
            "fZK",
            18,
            "FARM_ZK",
        )
        yield (  # address, symbol, decimals, name
            b"\x29\xcd\x92\xee\x12\xec\x64\x0e\x6c\xba\x96\xc8\xed\xa1\x08\x2d\x5f\xfa\xba\xb5",
            "G63",
            18,
            "Mercedes-AMG G63",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\x65\x01\xc0\x59\x59\xe3\x41\xef\xed\x90\x86\x34\xef\x71\x17\x4f\x0b\x3f\x4a",
            "GLP",
            18,
            "GemSwap LPs",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\xc2\x63\x4e\x60\xd4\x0e\xee\x6b\xe1\xe9\xf7\x76\x62\xea\x25\x55\x26\x39\xe6",
            "GOAT",
            18,
            "GOAT",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x56\x23\x40\xa3\x95\xba\x2f\x5d\x72\xa5\xee\x1b\x1b\x60\xc5\x78\x76\x85\x78",
            "GORILLA",
            18,
            "GORILLA ARMY",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\xef\x25\x25\xc7\x9f\x79\x95\xaa\x59\x07\x74\xda\x60\x49\x63\xe3\x48\xee\xda",
            "GRLAI",
            18,
            "GORILLA AI",
        )
        yield (  # address, symbol, decimals, name
            b"\xc3\xf5\xf4\x2a\x5a\x4e\xf0\x05\x9a\xcc\x87\xeb\x6e\xab\xcb\x09\x4e\x92\x84\x63",
            "GT3RS",
            18,
            "Porsche 911 GT3 RS",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\x95\xba\x7c\x02\xd2\x85\x8f\xb1\xb9\xbc\x8b\x90\xdd\x19\xad\xd0\xdf\x81\x92",
            "GTATE",
            18,
            "GRETA LOVE TATE",
        )
        yield (  # address, symbol, decimals, name
            b"\x26\x9d\x60\x84\xff\x49\x3e\x8b\x49\x3a\xc7\x9b\xf3\x4c\x75\xa8\x51\xa1\xf2\x75",
            "Heeee",
            18,
            "Uuuuu",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xde\xd3\x98\xb8\x3d\x3c\xb2\xf6\x12\xf8\x3d\xdb\xe0\xbb\xd0\x1c\x14\xdf\xd0",
            "HEU",
            18,
            "HEURIST by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x27\x9e\x08\x13\x0a\x53\x45\xc0\xb1\x11\x50\x7b\x72\xcf\x52\x99\xcd\xc3\xa8",
            "HIDE",
            18,
            "HIDE",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x40\x40\xfd\x47\x62\x9e\x7c\x8f\xbb\x7d\xa7\x6b\xb5\x0b\x3e\x76\x95\xf0\xf2",
            "HOLD",
            18,
            "Holdstation",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\xa0\xae\x94\x43\x6c\x83\xf1\x9b\x43\x2d\xc8\xd0\x91\x5b\x0e\x63\x70\x05\x20",
            "HOLDXBT",
            18,
            "HOLDXBT",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x57\x20\x6e\xbc\xc2\xbf\xf5\xd8\x09\x96\x17\xa5\x88\x9f\x59\x37\x2e\x6a\xdd",
            "HOTTIE",
            18,
            "HOTTIE",
        )
        yield (  # address, symbol, decimals, name
            b"\x98\x25\xe5\x93\xcd\x71\xc4\x63\xc8\x69\x75\x9b\xe8\xe1\x57\x92\x34\xed\x17\x1d",
            "HUHU",
            18,
            "HUHUHU",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\x65\x6c\x02\xaa\xe8\x56\x44\x37\x17\xc3\x41\x59\x87\x0b\x90\xd1\x28\x82\x03",
            "HYCO",
            18,
            "HYCO",
        )
        yield (  # address, symbol, decimals, name
            b"\xbe\x9f\x8c\x0d\x6f\x0f\xd7\xe4\x6c\xda\xcc\xa3\x40\x74\x7e\xa2\xf2\x47\x99\x1d",
            "IBEX",
            18,
            "Impermax",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\x25\x6f\xce\x92\xd7\x41\x66\xe1\xc2\x32\x60\xee\x83\x7c\xe9\xd0\x4e\x69\x13",
            "IPS",
            18,
            "IPS",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\xa9\x49\x4e\x25\x77\x03\x79\x7d\x74\x75\x40\xf0\x16\x83\x95\x25\x47\xee\x5b",
            "iZi",
            18,
            "izumi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\xf2\x4b\xe3\x2f\x64\x83\xa6\xdd\x32\xc9\xe4\x98\xd5\x3d\x14\x7a\xa8\x64\x1a",
            "JD",
            18,
            "Jd",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x6c\xa7\xfb\x54\xcc\x74\x8f\x3a\xd8\xa5\x2b\x77\x19\x36\x04\x49\x77\x59\x90",
            "JRT",
            18,
            "JR. TIGER",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\xb7\xd2\x60\xc1\x07\x49\x9c\x80\xb4\xb7\x48\xe8\x33\x1c\x64\x59\x59\x72\xa1",
            "KAT",
            18,
            "Karat Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\x86\x57\x8f\x4b\x93\x5b\xed\x83\x98\x35\x3c\x89\xbf\x0f\xca\x98\x7d\xce\xf2",
            "KITTEN",
            18,
            "BabyZeek",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\xe4\x6c\xb7\xcd\x2f\x15\xee\x1e\xc9\x53\x4c\xf2\x9a\x5b\x51\xc8\x32\x83\xe6",
            "KNC",
            18,
            "Kyber Network Crystal v2",
        )
        yield (  # address, symbol, decimals, name
            b"\xa9\x95\xad\x25\xce\x5e\xb7\x69\x72\xab\x35\x61\x68\xf5\xe1\xd9\x25\x7e\x4d\x05",
            "KOI",
            18,
            "Koi",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\xd3\x7f\x12\x02\x95\x11\xf1\x7c\x18\x98\x47\x7f\x7f\xb6\x85\xcf\x6c\x0c\x0f",
            "KZ",
            18,
            "KZ",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\x50\xa7\x46\x35\x9c\xd3\xd7\xe0\xa0\x65\x20\x0c\xbf\xfe\x91\xfa\x18\x80\xd4",
            "LA",
            18,
            "Lover AI by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\xc1\x56\xaf\x69\x07\x91\xf7\xac\xdc\x50\xf9\x30\xee\xbd\x9c\xab\xf6\xc4\x01",
            "LAB",
            18,
            "Laboratories",
        )
        yield (  # address, symbol, decimals, name
            b"\xf6\xd9\xa0\x93\xa1\xc6\x9a\x15\x2d\x87\xe2\x69\xa7\xd9\x09\xe9\xd7\x6b\x18\x15",
            "LAUNCH",
            18,
            "Super Launcher",
        )
        yield (  # address, symbol, decimals, name
            b"\xe6\x7f\xc9\xc8\x45\xb7\x5c\x3c\xd4\x7e\x20\x53\xe4\xea\x45\xd1\xbe\xd3\x0b\xae",
            "LIB",
            18,
            "Lib",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\x60\x83\xaf\x80\x2b\xf1\x9d\x93\x0f\xd2\x5f\x7a\xc3\xd6\x93\xb0\x87\x8c\x9f",
            "LILZK",
            18,
            "lilzk",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xc2\xbd\xc4\x25\xfd\x01\xc3\x3d\x85\x14\xf8\xbe\x01\x60\x70\x21\x2b\xdc\x6a",
            "LMAO",
            18,
            "LONGMAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x08\x04\x33\x44\x49\xd8\x48\x80\x68\xc9\x67\x08\xc4\xdc\x03\x65\x7c\x1e\x04\x9c",
            "LOCKED",
            18,
            "Lock in NOW",
        )
        yield (  # address, symbol, decimals, name
            b"\xde\x6b\x70\x54\x6a\x67\x0f\x0c\x93\x02\x95\xb4\x51\xf3\x3e\x15\xcb\xf5\x8b\x74",
            "lonely",
            18,
            "lonely dude",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\x65\xec\x33\xb4\x91\xd7\xb6\x72\x60\xb3\x14\x3f\x96\xbb\x4a\xc4\x73\x63\x98",
            "LONG",
            18,
            "Long",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\xa3\xcf\x6d\xb6\xc3\x6b\xcf\x83\x6d\xf9\xd3\x36\x7f\x79\x62\x28\x5d\x64\x5f",
            "LORA",
            18,
            "Isilora by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\xd0\xff\x2e\x01\xfd\x3e\x53\x40\xe9\x0d\x0c\x3c\x2e\x57\x0f\x33\x13\x71\xfe",
            "LOU",
            18,
            "Louisiana",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x08\x14\xa1\x41\x4c\x86\x10\x74\x02\x79\x21\x1d\xa9\x9a\xe1\xde\xb6\x9e\x47",
            "LUMEN",
            18,
            "AILumen",
        )
        yield (  # address, symbol, decimals, name
            b"\x28\xa4\x87\x24\x0e\x4d\x45\xcf\xf4\xa2\x98\x0d\x33\x4c\xc9\x33\xb7\x48\x38\x42",
            "MATIC",
            18,
            "Matic Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xcc\x10\x45\x25\xf8\x76\x11\xee\xda\xc7\x78\xbd\x15\xba\xaa\x50\x45\x8e\x61\x23",
            "MB",
            18,
            "MBtoken",
        )
        yield (  # address, symbol, decimals, name
            b"\x93\xed\x44\x71\x5a\x4d\xb0\xb1\xb9\x3b\x3a\xc8\x13\x42\xe8\x70\x72\x0a\xde\xd8",
            "MBP-WETH-weETH-44",
            18,
            "Maverick BP-WETH-weETH-44",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\xaf\xfa\x90\xde\xa7\x00\x36\xa1\x4c\xf7\x89\x6e\xea\x69\x5b\x98\x08\xa7\x6e",
            "MBP-WETH-wstETH-48",
            18,
            "Maverick BP-WETH-wstETH-48",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\xd4\xac\x81\x77\xc2\x6c\x68\xa8\x56\x0f\xcb\xa0\xb8\x9a\x57\xf5\xe5\xa5\x82",
            "MBP-WETH-zkETH-47",
            18,
            "Maverick BP-WETH-zkETH-47",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\xe4\xca\x0b\xbe\x62\x15\xcb\xda\x12\x85\x7e\x72\x31\x34\xbc\x38\x09\xf7\x66",
            "MCRN",
            18,
            "MacaronSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x81\x86\xc7\x46\x6d\xea\x1e\xb6\x9f\x7e\xdc\xdb\x46\x54\xd9\x23\xa0\x6c\x9d",
            "MEAI",
            18,
            "Meme Master",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xdb\x8c\x67\xd0\xc3\x32\x03\xda\x4e\xfb\x58\xf7\xd3\x25\xe1\xe0\xd4\xd6\x92",
            "MEOW",
            18,
            "Zeek Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\x87\x9c\x23\x94\x87\xbd\xcd\xd0\x8a\x94\x95\x3f\x25\xd8\x67\x30\x63\x37\x1f",
            "MG AI",
            18,
            "MG AI",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\xca\x4f\xd0\x96\x54\xd3\x7f\xe3\xb4\x36\x42\x0b\x0e\x77\x98\x31\xe8\x9b\xc2",
            "MIRA",
            18,
            "MIRA",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x24\x75\x2a\x7d\xa8\xc0\xd5\x8f\xe1\x43\x9c\x7f\x17\xb7\xab\x44\x11\x69\x30",
            "MKG",
            18,
            "Monkey Gang",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\x29\x88\x2a\xb7\xba\x6b\x87\x17\xad\x85\x8d\x10\x93\x0d\x68\xea\xfd\x78\xc3",
            "MOLTEN",
            18,
            "Molten",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\x2c\x7b\x35\xdf\x7e\x99\x2d\xd5\x10\x4d\x90\x51\x27\xf1\x2d\xed\x7f\x1e\xd6",
            "MON",
            18,
            "Monstera",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\xa8\xc5\x50\xe4\xbe\xf3\x71\xfa\x56\x3e\x5a\x2d\x17\x3f\xd0\x56\xb6\x28\x6a",
            "MONK",
            18,
            "monkey fest",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xbd\xe1\x60\x5e\x8f\xff\xe1\x7c\xea\x23\x64\xf8\x4f\x67\x41\x3c\x04\xe6\x0b",
            "MONKEYS",
            18,
            "Monkeys Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\xf8\xb4\x99\x57\xe7\x79\x96\x81\xd4\x12\x2c\x42\x0d\xf6\x7d\xaf\xff\x23\x99",
            "monrich",
            18,
            "RichMonkey",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\x5f\x8a\xdf\xc8\x26\x52\xed\x3d\x20\x97\x2e\xf4\xab\x44\x9e\xa5\x0e\x9e\xd9",
            "MONST",
            18,
            "MONSTER",
        )
        yield (  # address, symbol, decimals, name
            b"\x5e\x9a\x02\xc0\xb5\x87\xd8\x96\x6d\x17\xef\x73\x75\x32\x04\xf4\x57\xfb\x36\xac",
            "mooVenusWBTC",
            18,
            "Moo Venus WBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\x19\xd2\x43\x65\x0e\x1d\x03\xf6\x0f\xbd\xfd\xde\xae\xfd\x85\x8d\x84\xb1\x6b",
            "MTK",
            18,
            "MyToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xd7\x46\xcc\x67\xab\xcf\x95\x39\xb8\xdf\x10\x96\x42\x42\xc0\xd7\x6a\x41\x60\x85",
            "MUSDT",
            6,
            "Mock USDT",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\x97\xc7\xa0\xf8\xb2\xc9\x88\x5c\x8a\xc9\xfc\x61\x36\xe8\x29\xcb\xc2\x1d\x42",
            "MUTE",
            18,
            "Mute.io",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\xb5\x50\x49\x1f\x5a\x66\xbe\x3a\x21\xa7\x50\xdb\xdf\xa2\xef\xbf\x85\xe9\xa3",
            "MUZK",
            18,
            "ELON MUZK",
        )
        yield (  # address, symbol, decimals, name
            b"\xc8\xac\x61\x91\xcd\xc9\xc7\xbf\x84\x6a\xd6\xb5\x2a\xaa\xa7\xa0\x75\x7e\xe3\x05",
            "MVX",
            18,
            "Metavault Trade",
        )
        yield (  # address, symbol, decimals, name
            b"\x29\x48\x54\x1d\xe0\xee\x3c\xba\x18\x7d\x39\xd3\x03\x66\x43\x70\xcd\x58\xf7\xd7",
            "N2O",
            18,
            "happy gas",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\xf6\x36\xeb\x37\x65\xa7\x26\xe2\xb7\x26\x8b\x88\xe5\x4c\x55\x65\xaa\xe9\x44",
            "netflix",
            18,
            "netflix",
        )
        yield (  # address, symbol, decimals, name
            b"\x02\x50\xb2\x00\x42\x2e\xa4\xed\xc3\x4d\xbe\xcc\x0f\x15\xaf\x18\x28\x53\xbf\x7b",
            "Never",
            18,
            "@Never",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\x7b\x48\x85\x1d\x39\x7e\x77\xe4\xc5\x51\xee\x95\xcf\xfc\x6f\x51\x3c\x43\x5b",
            "NGGT",
            18,
            "NUGGET",
        )
        yield (  # address, symbol, decimals, name
            b"\xa5\x12\xd3\x2e\xd8\x42\xc4\xa6\x75\xf7\x8d\x35\x35\xba\xa0\x7d\x48\xa9\xb3\xad",
            "NIKO",
            18,
            "NIKOAI",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\xae\x94\x2c\x23\xcd\x15\x4c\x9f\x3b\xea\x9e\x6f\xa2\xa4\xa8\xa4\xec\x82\x5b",
            "nnmm",
            18,
            "noname",
        )
        yield (  # address, symbol, decimals, name
            b"\xbd\x43\x72\xe4\x4c\x5e\xe6\x54\xdd\x83\x83\x04\x00\x6e\x1f\x0f\x69\x98\x31\x54",
            "NODL",
            18,
            "Nodle Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\xb5\x16\x2e\xd3\x71\x16\xa7\xd0\x08\xea\xaa\xe5\x3e\x3a\x18\x06\x39\x68\xb7",
            "NUC",
            18,
            "Nuclear",
        )
        yield (  # address, symbol, decimals, name
            b"\x36\x95\x4f\x89\xd4\x08\x36\x97\x02\xd6\x53\xa8\xc0\x3e\x18\x32\xeb\x80\xbb\x78",
            "oneML",
            18,
            "Mercedes-AMG ONE",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\x8b\x79\x40\xcc\xa7\x1a\xc5\x45\x7a\x0e\xde\x7b\x43\x42\x6a\x21\x78\x59\x4b",
            "OOGWAI",
            18,
            "oogwAI",
        )
        yield (  # address, symbol, decimals, name
            b"\x89\xdd\xaf\x5d\xc1\x5b\x57\x77\x02\xc7\xf4\x8a\xca\x92\xfd\x8a\x4c\x9d\x6d\x93",
            "opr",
            18,
            "oper",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\xd1\x5f\x61\x0c\x87\x39\xfa\x19\x84\xd7\x77\x9c\xda\x61\x13\x0f\x78\x2d\x29",
            "ORB",
            18,
            "Orbiter Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\xea\x21\xba\x66\xb6\x7b\xe6\x36\xde\x1e\xc4\xbd\x96\x96\xeb\x8c\x61\xe9\xaa",
            "OT",
            18,
            "OT",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\xd3\x6c\x9c\x19\xc7\x1d\x08\x4a\x37\xa2\x7e\xb4\xcf\x78\xfd\xae\x86\xf3\x75",
            "PEG",
            18,
            "Pepe Gangster",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\xe4\xa4\x7d\xc6\x5e\x34\xcd\x12\x92\x49\xe5\xec\xa7\xd2\xc5\x15\xbf\xa8\x9b",
            "pfUSDC",
            18,
            "pfUSDC",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\x0e\xb1\x1a\xc0\xd5\xfd\x7d\xfd\xef\x23\x33\x48\x29\x6f\x1a\x58\x15\xde\x76",
            "PHX",
            18,
            "Phoenix",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\x25\x9f\x26\xdb\xe4\xa1\x65\x30\x5a\xa9\x76\xdd\x8c\xe0\x3c\x8f\x1a\x54\x7e",
            "PIKATXAI",
            18,
            "Pikachu Taxi AI by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\xc6\xda\x1b\xbd\xc3\x1e\xa5\xf9\x68\xac\xe7\x6e\x93\x16\x85\xca\x7f\x99\x62",
            "PIKO",
            18,
            "PIKO",
        )
        yield (  # address, symbol, decimals, name
            b"\x96\x0c\x02\x0d\x84\x6e\xf2\xf1\xb7\x49\xa3\x70\x3a\x0f\xf9\x6c\xbc\xc1\xe2\xe6",
            "PROD",
            18,
            "ProdigyDAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\x96\x0b\xd7\xce\x91\x6b\xd2\xef\xf8\x8a\xd4\x7b\x30\xa8\x61\xe6\x2a\xfa\x86",
            "PUMPRA",
            18,
            "Pump Rabbit",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\xa0\xaa\xe0\x56\xde\x87\xae\xfc\xbc\x97\x36\x23\xd9\xd4\x34\x70\xea\xe2\x48",
            "pxETH",
            18,
            "Pirex Ether OFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\x7c\xbc\xb3\x91\xd3\x39\x88\xda\xd7\x4d\x6f\xd6\x83\xaa\xdd\xa1\x12\x3e\x4d",
            "RF",
            18,
            "ReactorFusion",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\x09\xe0\x25\x92\xd9\x2d\x8a\xd1\xee\x30\x62\xe1\x45\x96\xce\xab\x6d\x82\xed",
            "Ribbit",
            9,
            "Ribbit",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\xa3\xcb\x75\x5b\x52\x6f\xe0\xce\x9a\x5f\x2a\x6b\x64\x62\xed\x9a\x88\xaf\x0f",
            "RICEWETH-C",
            18,
            "SyncSwap Classic LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\xd2\x08\xa7\xfc\x4f\xff\xaf\x46\xed\x4d\x97\xd2\x37\x04\x31\x4e\x14\x6f\xdd",
            "Rio",
            18,
            "PrakharWeb",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\xff\x08\x01\x52\x96\xd5\x3f\xb0\x96\x81\xb8\x6a\x47\xe3\xd2\x46\x34\x66\xba",
            "ROKO",
            18,
            "Basilisk",
        )
        yield (  # address, symbol, decimals, name
            b"\x2e\x86\x53\x38\x38\xa5\x13\x43\x67\x53\x15\xce\xe8\xe6\x5a\xee\x11\xa7\x2d\x9e",
            "RP",
            18,
            "RFX Pool",
        )
        yield (  # address, symbol, decimals, name
            b"\xa8\x65\x17\x96\xd8\x6a\x8b\x02\x30\x8d\x58\x17\x9c\xa2\x58\xad\x6d\x8a\x02\xff",
            "S3X",
            18,
            "KayeBaby",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\xa1\xf3\x4e\x39\x59\x10\x73\xe7\x65\xdd\xcb\x4b\xfe\x7f\xb1\xc0\xd4\xac\x51",
            "sAMM-USDCVS",
            18,
            "StableV1 AMM - USDC/VS",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\xc9\x1f\xbe\xf7\x78\x14\x3a\x19\xb0\x79\x9b\xc1\xca\x88\x57\xa3\x27\xa4\x90",
            "SAZK",
            18,
            "SatoshiZK",
        )
        yield (  # address, symbol, decimals, name
            b"\x19\x1a\xd8\xbe\x62\x4d\x99\x9a\x52\x56\x08\xc9\x72\x92\x2e\x9f\x01\x58\x2d\xe8",
            "SEQ",
            18,
            "Sequences",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\xad\x5b\x42\x48\xbd\xea\xaa\x03\x34\x40\x23\xd5\x0d\x8f\xaf\x1f\xd9\x64\x42",
            "SHIT",
            18,
            "Shit",
        )
        yield (  # address, symbol, decimals, name
            b"\xdd\x9f\x72\xaf\xed\x36\x31\xa6\xc8\x5b\x53\x69\xd8\x48\x75\xe6\xc4\x2f\x18\x27",
            "SIS",
            18,
            "Symbiosis",
        )
        yield (  # address, symbol, decimals, name
            b"\xa9\x84\x37\x5b\x8e\x07\x4d\x3e\x71\xf8\x70\xda\x66\xfb\x49\xc8\x94\xc3\x54\x74",
            "SLP",
            18,
            "SpaceSwap LP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\x27\xd9\x39\xf7\xde\x6f\x52\x16\x75\x90\x7c\xf0\x86\xf5\x9e\x4d\x75\xb8\x76",
            "SLR",
            18,
            "SolarCoin on Mainnet",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\xc1\xf2\xc8\x24\x45\xb3\xc6\x72\x52\xe4\x03\x04\xa9\x85\xda\x41\xa3\xe8\x71",
            "SNT",
            18,
            "SINISTRO",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\x26\x00\x90\xce\x5e\x83\x45\x4d\x5f\x05\xa0\xab\xbb\x2c\x95\x38\x35\xf7\x77",
            "SPACE",
            18,
            "SPACE",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x25\xc0\x63\x13\x65\xcd\xe9\x1c\x57\xed\xf7\x4b\xd6\xde\x6a\xfb\xf3\x09\xe3",
            "STARAI",
            18,
            "Star AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x0b\xe3\x79\xfe\xd9\xb5\x78\xe3\x0d\xc1\xab\xe3\x27\xf7\x7d\x1a\x54\x3a\xe1",
            "SWAG",
            18,
            "SWAG",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x0c\x95\xeb\xe5\xa3\xe6\x87\xcb\x22\x24\x68\x70\x24\xfe\xc6\x51\x8e\x68\x3e",
            "syBTC",
            8,
            "syBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\xe4\x9e\x26\x71\xcc\x40\xf0\x04\x69\xf6\xc6\x07\xd0\x39\x51\xd4\x34\xcc\xea",
            "SYC",
            18,
            "SYNC",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x52\x9e\xa6\xaa\xf9\x1f\x4d\x0e\x53\xa9\xa8\x55\x86\x6b\x93\xe6\xbf\xda\xe7",
            "Symbol",
            18,
            "Name",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\xc8\x1b\xff\xf1\x37\x85\x18\xe7\xf4\x43\x43\xa8\xba\xd7\x75\xbd\x24\xd9\x8d",
            "Symbol",
            18,
            "Name",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\x20\xb8\x89\x1f\x2f\x9e\xd0\xeb\xf1\xb1\x79\xb2\x27\x9f\x93\x6d\xec\x92\x82",
            "SYNC",
            9,
            "Syncus",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\xd9\xbc\x18\x75\x6c\xcb\x26\xb0\xc3\xcb\xc0\xcf\xbf\x9a\xd1\x7a\xfe\xc8\x84",
            "TAI",
            18,
            "Tradable AI by AIgentFi",
        )
        yield (  # address, symbol, decimals, name
            b"\xa5\x47\xdd\xf6\xd3\x2a\xfa\xd0\xac\x35\x37\x29\xde\x29\x6a\x93\x1e\x2f\x7a\xcc",
            "TEA-PL-USDT-WETH-500",
            18,
            "PL-USDT-WETH-500",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\xff\x7c\x6d\x36\x89\x04\x68\x07\x06\x80\x46\x45\xca\xfa\x4d\xef\xa3\xc2\x24",
            "TEVA",
            18,
            "Tevaera",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\x9d\x48\x87\x9c\x13\xd7\xea\xd0\x10\xaf\xc8\xa2\x73\x1d\xb3\x53\xd0\x8c\x85",
            "TGB",
            18,
            "TG.Bet",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\x44\x26\x85\x48\x24\xbe\x73\x9c\x10\x23\x43\x0a\x2e\x82\x1b\x6a\xd5\xfa\x22",
            "THI",
            18,
            "thientt11",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\xfa\x0a\xb4\x81\x59\x53\x62\x99\xc4\x86\x0e\x94\x40\x4c\xa1\xab\x1f\x31\xa6",
            "tomo",
            18,
            "Tomorrow",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\xd0\x2c\x65\x53\xd0\xee\x04\x42\x4f\x0b\x4a\x2f\x2a\x7e\x84\x14\x6d\x1c\x29",
            "TR",
            18,
            "Turkey Army",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\x5e\x08\x2e\x38\x4a\x9f\xc8\x6e\x95\xee\xa8\x05\xdc\x90\x12\xf1\xb7\x6c\xb7",
            "TRALA",
            18,
            "TRALA TOKEN",
        )
        yield (  # address, symbol, decimals, name
            b"\xf1\xb8\x27\x02\xa8\x03\xe1\xe3\x18\x5d\xe5\x7b\xee\xf1\x61\x3e\xb8\x34\xb5\xe9",
            "TRU",
            18,
            "TRUE",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xca\x32\x88\x2e\x25\xcf\xbf\x1d\xd7\x50\x15\xd0\x7a\x32\x6f\x6b\xcf\x64\x5c",
            "TRUMP",
            18,
            "President of ZKsync Memecoins",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\x5f\xb3\xe2\x7b\xaa\x52\x9d\x53\x37\x73\x2e\x47\x6e\xf7\xc9\x78\x81\x26\x1a",
            "unkMav",
            18,
            "unlock Maverick",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\x17\xcb\xcf\x0d\x6d\x14\x31\x35\xae\x90\x23\x65\xd2\xe5\xe2\xa1\x65\x38\xd4",
            "USDC",
            6,
            "USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\x62\x0f\x17\x09\xc0\xaa\x33\x47\x44\x89\xcf\xa4\xe6\x13\x4a\x5b\xc5\x1e\xaa\x9a",
            "USDC.eKZ-C",
            18,
            "SyncSwap Classic LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xe8\x7d\x92\x92\x8a\xb0\x46\x6e\xb0\x40\x56\xd3\x0b\x72\x82\x20\x8f\x67\x04\x85",
            "USDC.eZKS-C",
            18,
            "SyncSwap Classic LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\xc5\x35\x9e\x98\x85\x09\x1c\x5f\xfc\x08\xcb\x2f\x6d\xf3\x1d\x4a\xf8\x6f\xf9",
            "USDCHOLD-A",
            18,
            "SyncSwap Aqua LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\x15\xc2\x06\xa1\x4a\xc9\x3c\xb1\xa6\xc0\x31\x6a\x6e\x5f\x8a\xd7\xc9\xdc\x31",
            "USDM",
            18,
            "Mountain Protocol USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\x32\x57\xfd\x37\xed\xb3\x44\x51\xf6\x2e\xdf\x8d\x2a\x0c\x41\x88\x52\xba\x4c",
            "USDT",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\xd8\x4c\x77\x4c\xf8\xe9\xff\x85\x34\x26\x84\xb0\xe7\x95\xdf\x72\xa2\x49\x08",
            "VC",
            18,
            "Velocore",
        )
        yield (  # address, symbol, decimals, name
            b"\x19\x25\xab\x9f\x9b\xcd\xb9\xe2\xd2\x86\x1c\xc7\xc4\xc1\x57\x64\x51\x26\xd9\xd9",
            "veVS",
            18,
            "veVS",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\x40\x22\x9b\xde\x30\xc1\x0a\x3d\xd2\x27\x90\x78\xd2\x16\xc1\x67\x3f\xdd\xfd",
            "vienAI",
            18,
            "Super Beautiful AI Girl",
        )
        yield (  # address, symbol, decimals, name
            b"\x89\x07\x08\xca\xae\x86\x92\x80\x70\x92\x53\x37\x40\xbd\x9b\xd2\xb0\x36\x74\x8c",
            "vMLP (WETHPirate )",
            18,
            "Volatile Mute LP (WETH/Pirate )",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\x5a\x17\xb4\xf5\xc4\xf8\x44\x68\x8d\x56\x70\xb6\x84\x51\x5d\x7c\x78\x5e\x63",
            "VNO",
            18,
            "VenoToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\x00\x79\x3a\xad\x94\xc8\xca\x80\x1a\xa0\x36\x35\x7d\x10\xf5\xfd\x0c\xe0\x8f",
            "WBNB",
            18,
            "Binance Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\xeb\x51\x6f\xb0\x2a\x01\x61\x1c\xbb\xe0\x45\x3f\xe3\xc5\x80\xd7\x28\x10\x11",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\xf5\x26\xd2\x12\x86\x19\x33\xb1\xa9\x37\x7a\x8e\x8b\x7a\x6c\x0c\xd4\x1f\x38",
            "WEGMI",
            18,
            "WEGMI",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\xea\x57\x75\x95\x9f\xbc\x25\x57\xcc\x87\x89\xbc\x1b\xf9\x0a\x23\x9d\x9a\x91",
            "WETH",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\xb4\x3d\xd4\xdd\xc4\xf1\xe5\x87\x06\x9a\x14\xe8\x90\xaa\x4e\xcc\xe1\x7d\x05",
            "WETHKOI-C",
            18,
            "SyncSwap Classic LP",
        )
        yield (  # address, symbol, decimals, name
            b"\x32\x5b\xd9\x07\xa6\x4f\x4f\xe9\x41\x51\x98\xcd\x4a\x25\x0a\x57\xdb\x8e\xf3\x58",
            "WETHPROD-C",
            18,
            "SyncSwap Classic LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\xa3\x24\xbd\xd4\xeb\xb6\xb8\xa1\x80\x29\x59\x32\x4c\xe1\x25\xb5\xd5\x79\x21",
            "weUSDC",
            18,
            "WOOFi Super Charger USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x63\x71\xe7\xc8\x5e\x22\x9d\xdd\x94\x2f\xf8\x0c\xbd\x56\xd1\x04\xc9\xdb\xd4",
            "WTFWETH cSLP",
            18,
            "SyncSwap WTF/WETH Classic LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xfc\xa9\x74\xc9\xcc\x63\x0f\xd0\x3f\x49\xd3\x1c\x99\x2c\xe3\xcc\xe1\xe1\x3e",
            "xiaojie",
            18,
            "yang",
        )
        yield (  # address, symbol, decimals, name
            b"\xa4\xc3\x8c\x1c\x75\xbd\xb1\xb1\x4d\xe3\xf7\x21\xb1\x7a\xbb\x0c\x41\xfb\x72\x7e",
            "z12",
            18,
            "fduhuhz12",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\x89\xf0\x9d\x64\xcb\xd5\xb2\x42\x06\xf6\x55\x81\x73\x2b\xcb\xc2\xb7\xc0\xd1",
            "z23",
            18,
            "unhuz23",
        )
        yield (  # address, symbol, decimals, name
            b"\xcf\x11\xe0\xf5\x23\xe6\xac\x28\x46\x23\xea\x56\x57\x35\x27\x9c\xf5\x55\xcb\x26",
            "z3",
            18,
            "zkk37",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\xf8\x0a\xd7\x38\x97\x35\x96\x49\xa4\xf8\x99\x2f\xbb\x9f\xd9\x14\xc2\x4b\xc1",
            "ZAI",
            18,
            "Artificial intelligence on ZkSyn",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\xef\x4a\x56\x41\x99\x2a\x72\xcf\xd5\x7b\x94\x06\xc9\xd9\xce\xfe\xe8\xe0\xc4",
            "ZAT",
            18,
            "zkApes token",
        )
        yield (  # address, symbol, decimals, name
            b"\x27\x0f\x98\x3e\xaf\x87\x51\xa3\x36\x4c\x33\x25\xa0\xcc\x03\xe5\xab\x9f\xb7\xe4",
            "zdoge",
            18,
            "zdoge",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\xc9\x0a\x20\x59\xe2\x26\xa1\xc8\x93\xbe\xcd\x10\xf5\xb7\xae\x9a\x2c\x75\xc3",
            "ZERO",
            18,
            "Zero",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\xc2\xc0\x31\xfd\xc9\xd3\x3e\x97\x4f\x32\x7a\xb0\xd9\x88\x3e\xae\x06\xca\x4a",
            "ZF",
            18,
            "zkSwap Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x5d\x0d\x7b\xca\x05\x0e\x2e\x98\xfd\x4a\x5e\x8d\x3b\xa8\x23\xb4\x9f\x39\x86\x8d",
            "ZFI",
            18,
            "Zyfi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\x7d\x6b\x2f\x92\xc7\x7f\xad\x6c\xca\xbd\x7e\xe0\x62\x4e\x64\x90\x7e\xaf\x3e",
            "ZK",
            18,
            "ZKsync",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\xd6\x46\x42\xda\xb0\x96\x53\x24\x6e\xb4\x0f\xcb\xe3\x25\x45\xfb\xbe\x43\x24",
            "Zk1",
            18,
            "Zk1",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x98\x5a\xed\x21\x3c\x2c\x61\xb3\xc3\x83\x91\x2f\xa9\x9d\xbe\x4e\x1e\x55\x64",
            "ZKBONK",
            18,
            "ZksyncBONK",
        )
        yield (  # address, symbol, decimals, name
            b"\x7e\x18\x3d\xf0\x4d\xfa\xbe\xdd\x6a\xab\x5f\xf3\xd5\x85\x3e\x2f\x19\x92\xb8\xc7",
            "ZKCAT",
            18,
            "ZKCat",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x74\x85\x2f\x54\x83\x51\xad\x5f\xe7\xf7\x12\x00\x93\x62\x99\x7c\x1d\x14\xbb",
            "ZKDERPINA",
            18,
            "ZkDerpina",
        )
        yield (  # address, symbol, decimals, name
            b"\x05\xc1\x60\x14\x0f\xd2\xb9\x61\xf3\xc8\x7b\x74\x5e\x64\x06\x6d\x66\xe6\x71\xc3",
            "zkDIA",
            18,
            "zkDiamondSync",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\xb4\xb5\x61\x60\x44\xed\xed\x03\xe5\xb1\xad\x75\x14\x1f\x0d\x9c\xb1\x49\x9b",
            "ZKDOGE",
            18,
            "zkDoge",
        )
        yield (  # address, symbol, decimals, name
            b"\xb7\x22\x07\xe1\xfb\x50\xf3\x41\x41\x59\x99\x73\x2a\x20\xb6\xd2\x5d\x81\x27\xaa",
            "zkETH",
            18,
            "ZK Liquid Staked Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x94\x1d\x75\xc8\xc4\xa5\xb4\xed\xab\xe8\x91\x31\x4b\xd8\xb2\x42\x51\xac\x75",
            "ZKG",
            18,
            "zkGame Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\x48\xdc\x4b\x1b\x7f\xc5\xaf\xeb\x2d\x68\x5d\x8e\x1e\x7c\xbd\xf3\x33\xdf\x3f",
            "ZKM",
            18,
            "ZKT",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x83\xf6\x06\x2e\x1e\x67\x25\xdb\x8c\x86\xd8\xd8\x6e\x2e\x3a\x24\xf7\xe2\xe8",
            "ZKpablo",
            18,
            "EarlyPablo",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\x54\xa3\x11\xd5\x69\x57\xfa\x3c\x9a\x3e\x39\x7c\xa9\xdc\x60\x61\x11\x3a\xb3",
            "ZKPEPE",
            18,
            "ZKPEPE",
        )
        yield (  # address, symbol, decimals, name
            b"\x3a\x6d\xa1\xc6\x91\x77\x6d\x10\x99\xc7\xdf\x69\x69\x3e\x80\x0d\xd5\x53\xc6\x71",
            "ZKPOTATO",
            9,
            "zkPOTATO",
        )
        yield (  # address, symbol, decimals, name
            b"\x7b\x1e\x0f\x38\xe1\x41\xe2\x51\x34\xc0\x3d\x2a\xbf\xa2\xa9\xb1\x72\x78\x84\x1d",
            "ZKR",
            18,
            "zkSync Robots",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x21\x95\x95\x0d\xd0\x2d\xab\x47\xfb\x02\xfa\xe0\x47\x6a\x29\x20\x72\xb3\x07",
            "ZKS",
            18,
            "zkSarah",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\x5b\xf5\xf8\x09\xfb\x9b\xd4\xc9\xb5\xa4\xa7\x4e\xb2\x81\xf9\x37\x5f\x6b\x0c",
            "zksync",
            18,
            "ZKSYNC",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x8e\x6f\x44\x95\xf0\x89\xde\x76\xa7\xac\x75\x83\x2f\x7d\x7a\x55\xb2\x97\x1c",
            "zksync",
            18,
            "ZKSYNC",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\x84\x93\x20\x68\xca\x5d\xbd\xe7\x2b\x1c\x9d\xc4\x56\x0f\x46\x52\x37\x5e\x1b",
            "ZKT",
            18,
            "ZKToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\x8e\x15\x45\xae\x4e\x30\x26\x5c\xf0\x74\x15\x0e\x04\xf0\x3f\xff\x67\xe7\xf1",
            "zkTate",
            18,
            "zkTate",
        )
        yield (  # address, symbol, decimals, name
            b"\x7c\x1c\x46\xa7\x8d\x89\xda\x23\x39\xf6\x6a\xe0\x7a\xc5\xd2\x16\x02\x62\xcd\x2b",
            "zkup",
            18,
            "zkup",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\xcd\x48\x94\x17\x62\xe8\x19\x5d\x71\xc6\x76\x29\x94\xec\x47\x5f\x2b\x0e\xb9",
            "ZKzkUSD-C",
            18,
            "SyncSwap Classic LP",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\x54\xbd\x14\xc8\x88\x78\xdd\xf4\xc8\x38\xd2\x3c\x92\x6a\xaf\x0f\xf9\xc8\xb4",
            "ZLOKI",
            18,
            "zkFloki",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\x54\xed\xc5\x59\x15\x3a\xdc\x95\xae\xb1\x2d\x6f\xf6\x67\x64\x31\xa5\x47\x44",
            "Zm",
            18,
            "zksync gm",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\xd4\x79\x37\xfe\xa1\x59\xba\xe4\x13\x04\xda\xe3\xec\x9b\xbe\x7d\x02\xf8\xbc",
            "ZOEY",
            18,
            "ZkSync Mascot",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\xe4\x1f\xfa\x12\x3a\x77\x94\x28\xed\xe6\x7b\xf9\xb8\x2e\xa2\x78\x35\x72\xb1",
            "ZOHM",
            9,
            "ZOHM",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\x4c\x23\x83\x25\xfc\x1b\xdf\x6e\xde\xd7\x26\xee\x1b\x47\xd5\x58\x95\xd9\x44",
            "ZORRO",
            18,
            "Zorro",
        )
        yield (  # address, symbol, decimals, name
            b"\x54\xe9\x3c\x17\x1d\x6f\x5c\xb3\x7a\xcc\xd4\xbf\x31\x48\xb6\xa8\x71\x1f\xc6\xd2",
            "ZWIFMEOW cSLP",
            18,
            "SyncSwap ZWIF/MEOW Classic LP",
        )
    if chain_id == 8453:  # base
        yield (  # address, symbol, decimals, name
            b"\xe3\x08\x68\x52\xa4\xb1\x25\x80\x3c\x81\x5a\x15\x82\x49\xae\x46\x8a\x32\x54\xca",
            "$mfer",
            18,
            "mfercoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x1c\x69\x5f\x6b\x4a\x3f\x8b\x05\xf2\x49\x2c\xef\x94\x74\xaf\xb6\xd6\xad\x69",
            "A1C",
            18,
            "Sally",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xda\xcb\x99\xa8\x69\x80\x52\xa9\x89\x8e\x81\xfd\xf8\x83\xc2\x9e\xfb\x93\xcb",
            "ACOLYT",
            18,
            "Acolyte by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\x0a\x4b\xd0\x59\xf5\x91\x4a\x2f\x8b\x9c\x18\x88\x1c\x63\x7f\x79\xef\xb7\xdf",
            "ADS",
            11,
            "Adshares",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\x01\x81\xa9\x4a\x35\xa4\x56\x9e\x45\x29\xa3\xcd\xfb\x74\xe3\x8f\xd9\x86\x31",
            "AERO",
            18,
            "Aerodrome",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\xd8\xcb\x75\x42\x30\xdb\xfd\x24\x9d\xb0\xe8\xec\xcb\x51\x42\xdd\x67\x5a\x0d",
            "AEROBUD",
            18,
            "AEROBUD",
        )
        yield (  # address, symbol, decimals, name
            b"\xf5\xbc\x34\x39\xf5\x3a\x45\x60\x7c\xca\xd6\x67\xab\xc7\xda\xf5\xa5\x83\x63\x3f",
            "AGENT",
            18,
            "AgentLayer",
        )
        yield (  # address, symbol, decimals, name
            b"\x81\x49\x6f\x85\xab\xaf\x8b\xd2\xe1\x3d\x90\x37\x9f\xde\x86\xc5\x33\xd8\x67\x0d",
            "AGIXBT",
            18,
            "AGIXBT by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\xbd\xf3\x17\xf9\xc1\x53\x24\x6c\x42\x9f\x23\xf4\x09\x30\x87\x16\x4b\x14\x53\x90",
            "AIFUN",
            18,
            "AI Agent Layer",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\x53\xf0\xc0\x59\xc2\x75\x27\xd3\x3d\x02\x37\x8e\x5e\x4f\x6d\x5a\xfb\x57\x4a",
            "AIINU",
            18,
            "AI INU",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x48\x31\x4a\xd4\xad\x5b\xd3\x6a\x84\xe8\x30\x7b\x86\xa6\x8a\x01\xd9\x5d\x9c",
            "AION",
            18,
            "AION 5100",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x9f\xd6\xbe\x4a\x90\xf2\x62\x08\x60\xd6\x80\xc0\xd4\xd5\xfb\x53\xd1\xa8\x25",
            "AIXBT",
            18,
            "aixbt by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\xc7\x1f\x17\x03\xfb\xf1\x9f\xfd\xcf\x30\x51\xe1\xe6\x84\xcb\x99\x34\x51\x0f",
            "AIXCB",
            18,
            "aixCB by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x20\xcf\x34\x66\xf8\x0a\x5f\x7f\x53\x2f\xca\x55\x3c\x8c\xbc\x97\x27\xfe\xf6",
            "AKUMA",
            18,
            "Akuma Inu",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\xd2\xd6\x31\xc9\x2b\x1a\xcd\xfc\xdd\x51\xa0\xf7\x14\x5a\x50\x13\x00\x50\xc4",
            "ALB",
            18,
            "AlienBase Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xa8\xbd\x1f\xe1\x7a\x1b\xb9\xff\xb3\x9e\xcd\x83\xe9\x48\x9c\xfd\x17\xa0\x22",
            "ANDY",
            18,
            "Andy",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\xb5\x10\xe7\x99\x09\x66\x6d\x6d\xec\x7f\x5e\x49\x37\x08\x38\xc1\x6d\x95\x0f",
            "ANON",
            18,
            "Super Anon",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\xe6\xb6\x48\xc9\x1d\x22\x2b\x2f\x63\x18\xe8\xce\xee\xd4\xb6\x91\xd5\x32\x3f",
            "ANON",
            18,
            "AnonFi",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\xc4\x68\x33\x3c\xcc\x16\xd4\xbf\x1c\xef\x49\x7a\x56\xcf\x8c\x0a\xae\x4c\xa3",
            "ANZ",
            18,
            "Anzen Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\x56\x9c\xfb\x8f\xd2\x65\xe9\x1a\xa4\x78\xd8\x6a\xe8\xc7\x8b\x8a\xf5\x5d\xf4",
            "AUKI",
            18,
            "Auki Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x81\xbc\x7d\x56\x40\xbf\x51\x4a\x50\x8e\x27\x32\xec\x52\x2c\x65\x0c\x41\x4c",
            "AURA",
            18,
            "Aura",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x46\x17\x73\x4c\x43\xf6\x15\x9f\x3a\x70\xb7\xe0\x6d\x88\x36\x47\x51\x27\x78",
            "AWE",
            18,
            "AWE Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\xee\x23\x43\xb8\x92\xb1\xbb\x63\x50\x3a\x4f\xab\xc8\x40\xe0\xe2\xc6\x81\x0f",
            "AXL",
            6,
            "Axelar",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x46\x63\x42\xc4\xd4\x49\xbc\x9f\x53\xa8\x65\xd5\xcb\x90\x58\x6f\x40\x52\x15",
            "axlUSDC",
            6,
            "Axelar Wrapped USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\xb3\x2f\x9f\x88\x27\xd4\x63\x4f\xe7\xd9\x73\xfa\x10\x34\xec\x9f\xdd\xb3\xb3",
            "B3",
            18,
            "B3",
        )
        yield (  # address, symbol, decimals, name
            b"\x58\xec\xef\x26\x33\x5a\xf7\xb0\x4a\x99\x81\x05\xa6\x60\x3b\x0d\xc4\x75\xaf\x33",
            "BabyDoge",
            9,
            "Baby Doge Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x7c\x6b\x91\xd9\xbe\x15\x5a\x6d\xb0\x1f\x74\x92\x17\xd7\x6f\xf0\x2a\x72\x27\xf2",
            "BAL",
            18,
            "Balancer",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\x96\x44\xb8\x60\x75\xed\x61\xc6\x47\x59\x68\x62\xc7\x40\x3e\x1c\x47\x4d\xbf",
            "BAMBOO",
            18,
            "Bamboo on Base",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\x73\x79\xa7\x55\xa8\xf1\x1b\x57\x61\x01\x54\x86\x1d\x69\x4b\x2a\x0f\x61\x5a",
            "BASE",
            18,
            "BASE",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x63\x8e\xa7\x9f\x71\xf3\xb9\x1b\xdc\x96\xbb\xdf\x9f\xb2\x7c\x93\x01\x3d\x60",
            "BBT",
            5,
            "Baby Tiger",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\x09\xf1\x08\xd1\x72\x8e\x6b\x6e\xd2\x41\xcc\xd2\x54\x77\x5e\x32\x2f\x1e\xd6",
            "BEE",
            18,
            "BeeBase",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\x45\x64\x7e\xa8\x94\x03\x0a\x4e\x98\x01\xec\x03\x47\x97\x39\xfa\x24\x85\xf0",
            "BENJI",
            18,
            "Basenji",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x36\x18\x67\x72\xa8\xfd\xa4\xbe\x10\x0d\xba\xcc\x0b\x48\xef\x00\xc5\x30\x89",
            "BERRY",
            18,
            "Strawberry AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x55\x5f\xff\x48\x54\x9c\x1a\x25\xa7\x23\xbd\x8e\x7e\xd1\x08\x70\xd8\x2e\x83\x79",
            "BIM",
            18,
            "BIM",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\x5e\x03\xa6\xcc\xef\x48\xa1\x8d\xdb\x6c\xfd\xc3\x06\x6d\x94\x43\x87\x56\x72",
            "BLOCK",
            18,
            "BLOCK",
        )
        yield (  # address, symbol, decimals, name
            b"\x54\x8f\x93\x77\x9f\xbc\x99\x20\x10\xc0\x74\x67\xcb\xaf\x32\x9d\xd5\xf0\x59\xb7",
            "BMX",
            18,
            "BMX",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\xaf\x33\xfe\x49\xfd\x1f\xa8\x0c\x71\x49\x77\x3d\xde\x58\x90\xd3\xc7\x6f\x3b",
            "BNKR",
            18,
            "BankrCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x08\x7c\x44\x0f\x25\x1f\xf6\xcf\xe6\x2b\x86\xdd\xe1\xbe\x55\x8b\x95\xb4\xbb\x9b",
            "Bold",
            18,
            "Bold Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x1c\xf2\x11\xd3\x8e\x77\x62\xc9\x69\x1b\xe4\xd7\x79\xa4\x41\xa1\x7a\x6c\xfc",
            "BONK",
            5,
            "Bonk",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\xe1\x72\xdc\x5f\xfc\x46\xd2\x28\x83\x84\x46\xc5\x7c\x12\x27\xe0\xb8\x20\x49",
            "BOOMER",
            18,
            "Boomer",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x92\x60\x7c\x3c\xba\x9e\xc3\xd4\x90\x20\x6d\x10\xd8\x31\xe6\x8e\x5f\x3c\x97",
            "BOSON",
            18,
            "Boson Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x53\x2f\x27\x10\x19\x65\xdd\x16\x44\x2e\x59\xd4\x06\x70\xfa\xf5\xeb\xb1\x42\xe4",
            "BRETT",
            18,
            "Brett",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x96\xe4\x99\xcc\x8f\x59\x9a\x2a\x82\x80\x82\x5d\x8b\xda\x92\xf7\xa8\x95\xe0",
            "BRO",
            18,
            "Neurobro by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\xcb\x32\x7b\x99\xff\x83\x1b\xf8\x22\x3c\xce\xd1\x2b\x13\x38\xff\x3a\xa3\x22\xff",
            "bsdETH",
            18,
            "Based ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x41\xf1\xfc\x90\x22\xfe\xb6\x9a\xf6\xdc\x66\x6a\xbf\xe7\x3c\x9f\xfd\xa7\xce",
            "BTCB",
            18,
            "Bitcoin on Base",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x3d\xc7\xae\x7a\x4f\xa4\x05\x17\xaa\xfa\x94\x1a\xe1\xee\x43\x6b\x91\xa1\x2c",
            "BUZ",
            4,
            "BUZ",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\x55\x91\x3c\x90\xfc\xc1\xa6\xce\x9a\x35\x89\x11\x72\x1e\xeb\x94\x20\x13\xa1",
            "CAKE",
            18,
            "PancakeSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x8c\xd2\x64\x74\x72\x2c\xe9\x3f\x4d\x0c\x34\xd1\x62\x01\x46\x1c\x10\xaa\x8c",
            "CARV",
            18,
            "CARV",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\xe3\xf1\xec\x7f\x1f\x50\x12\xcf\xea\xb0\x18\x5b\xfc\x7a\xa3\xcf\x0d\xec\x22",
            "cbETH",
            18,
            "Coinbase Wrapped Staked ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\xaf\x81\xeb\x42\xcd\x30\x01\x4e\xb4\x41\x30\xb8\x9b\xcd\x6d\x4a\xd9\x8b\x92",
            "CHAD",
            18,
            "Based Chad",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\x6d\x78\x14\x8f\x00\x1f\x3a\xa2\xf5\x88\x99\x7c\x5e\x10\x2e\x48\x9a\xd3\x41",
            "CHAMP",
            18,
            "Super Champs",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\xff\x2d\xb6\x43\xcf\x95\x52\x47\x33\x9c\x8c\x6b\xcd\x84\x06\x30\x8c\xa4\x37",
            "CHOMP",
            18,
            "ChompCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\xc0\xc4\x22\x15\x58\x2d\x5a\x08\x57\x95\xf4\xba\xdb\xac\x3f\xf3\x6d\x1b\xcb",
            "CLANKER",
            18,
            "tokenbot",
        )
        yield (  # address, symbol, decimals, name
            b"\x4b\x36\x1e\x60\xcf\x25\x6b\x92\x6b\xa1\x5f\x15\x7d\x69\xca\xc9\xcd\x03\x74\x26",
            "CLUSTR",
            18,
            "Clustr",
        )
        yield (  # address, symbol, decimals, name
            b"\xab\x96\x4f\x7b\x7b\x63\x91\xbd\x6c\x4e\x85\x12\xef\x00\xd0\x1f\x25\x5d\x9c\x0d",
            "CONVO",
            18,
            "Prefrontal Cortex Convo Agent by",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x04\x1e\xf3\x57\xb1\x83\x44\x8b\x23\x5a\x8e\xa7\x3c\xe4\xe4\xec\x8c\x26\x5f",
            "COOKIE",
            18,
            "Cookie",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\x94\xa9\x1e\x6b\x07\x1b\xf0\x30\xa1\x8b\xd3\x05\x3a\x7f\xe0\x9b\x6d\xae\x69",
            "COW",
            18,
            "CoW Protocol Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\xd9\x07\x72\x69\xdd\x08\x89\x9f\x2a\x9e\x73\x50\x71\x25\x96\x2b\x5b\xc8\x7f",
            "Crash",
            18,
            "Crash",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\xe7\x3c\x48\x4a\x26\xe0\xa5\xdf\x2e\xe2\xa4\x96\x0b\x78\x99\x67\xdd\x04\x15",
            "CRV",
            18,
            "Curve DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\x7a\xc0\xe0\x78\x39\x8c\x15\x4e\xdf\xad\xd9\xef\x67\x5d\x30\xbe\x60\xaf\x93",
            "crvUSD",
            18,
            "Curve.Fi USD Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\x9f\xac\x10\xc5\xcb\xfe\xfe\x3e\x71\x0e\x1d\x94\x67\xf7\x0a\x76\x13\x8d\x45",
            "CTSI",
            18,
            "Cartesi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\x77\x88\x60\xe9\x37\xf5\x09\xe6\x51\x19\x2a\x90\x58\x9d\xe7\x11\xfb\x88\xa9",
            "CYBER",
            18,
            "CyberConnect",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\x29\x9a\x71\x8b\x2c\x44\x48\x3a\x27\x32\x5d\x77\x25\xf0\xb2\x64\x6d\xe3\xb1",
            "DAG",
            8,
            "Constellation",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xc5\x72\x59\x49\xa6\xf0\xc7\x2e\x6c\x4a\x64\x1f\x24\x04\x9a\x91\x7d\xb0\xcb",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x16\x4f\xfd\xae\x2f\xe3\x89\x17\x14\xbc\x29\x68\xf1\x87\x5c\xa4\xfa\x10\x79\xd0",
            "DEFI.ssi",
            8,
            "DEFI.ssi",
        )
        yield (  # address, symbol, decimals, name
            b"\x4e\xd4\xe8\x62\x86\x0b\xed\x51\xa9\x57\x0b\x96\xd8\x9a\xf5\xe1\xb0\xef\xef\xed",
            "DEGEN",
            18,
            "Degen",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x21\xb1\x30\xd2\x97\xcc\x43\x75\x4a\xfb\xa2\x2e\x5e\xac\x0f\xbf\x8d\xb7\x5b",
            "doginme",
            18,
            "doginme",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x01\x0b\xf9\xc2\x68\x81\x78\x8b\x4e\x6b\xf5\xfd\x1b\xdc\x35\x8c\x8f\x90\xb8",
            "DOT",
            18,
            "Polkadot Token (Relay Chain)",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xc2\x15\x6d\x4c\x0a\x9c\xbd\xab\x4a\x01\x66\x33\xb7\xbc\xf6\xa8\xd6\x8e\xa2",
            "DRB",
            18,
            "DebtReliefBot",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x6e\x00\x0d\xef\x95\x78\x0f\xb8\x97\x34\xc0\x7e\xe2\xce\x9f\x6d\xca\xf1\x10",
            "EDGE",
            18,
            "Edge",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\x6c\xcc\xdc\xe1\x93\x69\x8d\x33\xde\xb9\xff\xd4\xbe\x74\xea\xa7\x4c\x48\x98",
            "ElonRWA",
            18,
            "ElonRWA",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\xa3\xe3\x5c\xc3\x02\xbf\xa4\x4c\xb2\x88\xbc\x5a\x4f\x31\x6f\xdb\x1a\xdb\x42",
            "EURC",
            6,
            "EURC",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\x3f\xf5\x4b\x9f\x72\x42\xef\x15\x93\xd2\xc9\xbc\xd8\xf9\xdf\x46\xc7\x79\x35",
            "FAI",
            18,
            "FAI",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x6c\x17\xfa\x9f\x9b\xc3\x60\x03\x46\xab\x4e\x48\xc0\x70\x1e\x1d\x59\x62\xae",
            "Fartcoin",
            18,
            "Based Fartcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\xb3\xd4\xc3\x85\xb4\x00\xf3\xab\xb4\x9e\x80\xde\x2f\xaf\x6a\x88\xa7\xb6\x91",
            "FLOCK",
            18,
            "FLock.io",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\x6a\xae\xf8\xd8\x76\x01\x29\xa0\x39\x8c\xf8\x67\x4e\xe2\x8c\xef\xc0\xea\xb9",
            "FLOPPA",
            18,
            "Floppa",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\x08\xbd\xcf\x9c\xdf\xf9\xda\x68\x4a\x19\x09\x41\xdc\x3d\xca\x8c\x2c\xdd\x44",
            "FLUX",
            8,
            "Flux",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\x0c\x53\x2f\xdb\x8c\xd5\x66\xae\x16\x9c\x1c\xb1\x57\xff\x2b\xdc\x83\xe1\x05",
            "Fren Pet",
            18,
            "Fren Pet",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\xd4\x88\x7f\x7d\x41\xb3\x5c\xd7\x5d\xff\x9f\xfe\xe2\x85\x61\x06\xf8\x66\x70",
            "FRIEND",
            18,
            "FRIEND",
        )
        yield (  # address, symbol, decimals, name
            b"\x42\x06\x9b\xab\xe1\x4f\xb1\x80\x2c\x5c\xb0\xf5\x0b\xb9\xd2\xad\x6f\xef\x55\xe2",
            "FROK",
            18,
            "frok.ai",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\x45\x97\x1c\x16\x8b\x52\x94\xac\xbc\x87\x27\xa4\xf1\xc9\xe1\xaf\x99\xf6\xd0",
            "FTN",
            18,
            "Bridged FTN (OrtakSea)",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\x7b\xeb\xa8\xf6\xef\x66\x43\xab\xd7\x25\xe4\x5a\x4e\x83\x87\xef\x26\x06\x49",
            "G",
            18,
            "Gravity",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\x4c\xca\x7c\x5d\xb0\x03\x82\x42\x08\xad\xda\x61\xbd\x74\x9e\x55\xf4\x63\xa3",
            "GAME",
            18,
            "GAME by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\xaa\xe7\x45\xcf\x28\x30\xfb\x8d\xdc\x62\x48\xb1\x74\x36\xdc\x3a\x5e\x70\x1c",
            "GOCHU",
            18,
            "Gochujangcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x1d\xc7\x31\x59\xe3\x0c\x4b\x06\x17\x0f\x25\x93\xd3\x11\x89\x68\xa0\xdc\xa5",
            "GPS",
            18,
            "GoPlus Security",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\xe2\xa6\x32\x03\x14\x88\x3f\xf8\xcc\x08\xb5\x3f\x14\x60\xa5\xf4\xc4\x7f\x2c",
            "HAI",
            8,
            "HackenAI",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\xa9\x66\x80\xcc\xde\x03\xbd\x4b\xdd\x9a\x3e\x9a\x0c\xb5\x6a\x5d\x27\xf7\xc9",
            "HENLO",
            18,
            "henlo",
        )
        yield (  # address, symbol, decimals, name
            b"\x05\x78\xd8\xa4\x4d\xb9\x8b\x23\xbf\x09\x6a\x38\x2e\x01\x6e\x29\xa5\xce\x0f\xfe",
            "HIGHER",
            18,
            "higher",
        )
        yield (  # address, symbol, decimals, name
            b"\x4b\xfa\xa7\x76\x99\x1e\x85\xe5\xf8\xb1\x25\x54\x61\xcb\xbd\x21\x6c\xfc\x71\x4f",
            "HOME",
            18,
            "Home",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\xd2\x3e\xd2\xad\xb0\xf5\x51\x36\x99\x46\xbd\x37\x7f\x86\x44\xce\x1c\xa5\xc4",
            "HYPER",
            18,
            "Hyperlane",
        )
        yield (  # address, symbol, decimals, name
            b"\xcc\x7f\xf2\x30\x36\x5b\xd7\x30\xee\x4b\x35\x2c\xc2\x49\x2c\xed\xac\x49\x38\x3e",
            "hyUSD",
            18,
            "High Yield USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\xba\xf3\x11\xce\xc8\xa4\xea\xc0\x43\x01\x93\xa5\x28\xd9\xff\x27\xae\x38\xc1",
            "IOTX",
            18,
            "IoTeX Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x98\xd0\xba\xa5\x2b\x2d\x06\x3e\x78\x0d\xe1\x2f\x61\x5f\x96\x3f\xe8\x53\x75\x53",
            "KAITO",
            18,
            "KAITO",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\x26\xf5\x43\x36\x71\x75\x1c\x32\x76\xa0\x65\xf5\x7e\x5a\x02\xd2\x81\x79\x73",
            "KEYCAT",
            18,
            "Keyboard Cat",
        )
        yield (  # address, symbol, decimals, name
            b"\xbe\x31\x11\x85\x6e\x4a\xca\x82\x85\x93\x27\x4e\xa6\x87\x2f\x27\x96\x8c\x8d\xd6",
            "KRAV",
            18,
            "KRAV",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x63\x40\x90\xf2\xfe\x6c\x6d\x75\xe6\x1b\xe2\xb9\x49\x46\x4a\xbb\x49\x89\x73",
            "KTA",
            18,
            "Keeta",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\xa3\x8e\x72\x16\x30\x41\x08\xe8\x41\x57\x90\x41\x24\x9f\xeb\x23\x6c\x88\x7b",
            "LBM",
            18,
            "Libertum",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\xac\x9c\x5f\x70\x4e\x95\x49\x31\x34\x9d\xa3\x7f\x60\xe3\x9f\x51\x5c\x11\xc1",
            "LBTC",
            8,
            "Lombard Staked Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xd3\x8b\x30\x5c\xac\x06\x99\x0c\x08\x87\x03\x2a\x02\xc0\x3d\x68\x39\xf7\x70\xa8",
            "LGCT",
            18,
            "LEGACY TOKEN",
        )
        yield (  # address, symbol, decimals, name
            b"\xfb\x42\xda\x27\x31\x58\xb0\xf6\x42\xf5\x9f\x2b\xa7\xcc\x1d\x54\x57\x48\x16\x77",
            "LINGO",
            18,
            "Lingo",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\xd7\xa3\x01\xb5\x1d\x0a\x83\xfc\xaf\x67\x18\x62\x81\x74\xd5\x27\xb3\x73\xb6",
            "LUM",
            18,
            "luminous",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\x6a\x46\xf2\x94\xbb\x67\xc2\x0f\x1d\x1e\x7a\xfb\x0b\xbe\xf6\x14\x40\x3b\x55",
            "MAG7.ssi",
            8,
            "MAG7.ssi",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\xd5\x79\xdd\x6a\xeb\x23\x2e\x95\xa1\x5d\x96\x4a\x13\x5a\x61\x92\x5b\x5c\x93",
            "MARSO",
            18,
            "Marso.Tech",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\x48\x24\x29\x0c\xc1\x2f\x31\xae\x18\xef\x51\x21\x6a\x22\x3b\xa4\x06\x30\x92",
            "MASA",
            18,
            "Masa Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\xbd\x06\x48\x97\x1d\x56\xf1\xf2\xc3\x5f\xa0\x75\xff\x5b\xc7\x5f\xb0\xe3\x9d",
            "MBS",
            18,
            "MBS",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\xfa\xc1\xb3\x75\xbf\x28\x94\xd6\xf1\x2f\xb2\xeb\x48\xb1\xc1\xa7\x91\x67\x89",
            "MEY",
            18,
            "Mey Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\xa0\x3e\xda\x10\x34\x25\x29\xbb\xf8\xeb\x70\x0a\x06\xc6\x04\x41\xfe\xf2\x5d",
            "MIGGLES",
            18,
            "Mister Miggles",
        )
        yield (  # address, symbol, decimals, name
            b"\x2b\x11\x83\x4e\xd1\xfe\xae\xd4\xb4\xb3\xa8\x6a\x6f\x57\x13\x15\xe2\x5a\x88\x4d",
            "MOCA",
            18,
            "Moca",
        )
        yield (  # address, symbol, decimals, name
            b"\xf6\xe9\x32\xca\x12\xaf\xa2\x66\x65\xdc\x4d\xde\x7e\x27\xbe\x02\xa7\xc0\x2e\x50",
            "MOCHI",
            18,
            "Mochi",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\xa5\x6a\xcb\x9e\xa7\x83\x30\xf9\x47\xbd\x57\xc5\x41\x19\xde\xbd\xa7\xaf\x71",
            "MOG",
            18,
            "Mog Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x15\x94\xb0\xe8\x96\xbf\x16\x5d\x92\x59\x56\xe0\xdf\x73\x3b\x84\x43\xaf\x6a",
            "MORK",
            18,
            "Mork",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\xa5\xcc\x21\xfd\x48\x7b\x8f\xcc\x2f\x63\x2f\x3f\x4e\x8d\x37\x26\x2a\x08\x42",
            "MORPHO",
            18,
            "Morpho Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\x90\x37\xd1\xef\x5c\x6d\x1f\x68\x16\x27\x8c\x7a\xaf\x54\x91\xd2\x4c\xd5\x27",
            "MOXIE",
            18,
            "Moxie",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\x55\xc3\x31\xd1\xaa\x7f\x96\xc2\x52\xf1\xf4\x0c\xe1\x3d\x80\xea\xc5\x35\x04",
            "MUSIC",
            18,
            "MUSIC by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\xe4\x30\x87\x0c\x46\x04\xcc\xfc\x7b\x38\xca\x78\x45\xb1\xff\x65\x3d\x0f\xf1",
            "mwETH",
            18,
            "Moonwell Flagship ETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\xa3\x5e\xbc\x4f\x25\xb0\x42\xd2\xca\xe7\x59\x14\xc7\xe8\x82\xe6\x31\xfa\x9a",
            "NEIRO",
            9,
            "Neiro",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\x66\xe8\xb1\x40\xd3\x5d\x9d\x82\x26\xe4\x0c\x09\xf7\x57\xba\xc5\xa4\xd8\x7d",
            "NPC",
            18,
            "Non-Playable Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\x73\xed\x18\x15\xe5\x91\x54\x89\x57\x00\x14\xe0\x24\xb7\xeb\xe6\x5d\xe6\x79",
            "ODOS",
            18,
            "Odos Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x39\x92\xb2\x7d\xa2\x68\x48\xc2\xb1\x9c\xea\x6f\xd2\x5a\xd5\x56\x8b\x68\xab\x98",
            "OM",
            18,
            "MANTRA",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\x33\x33\xc4\x65\xa1\x9c\x85\xf8\x5c\x6c\xfb\xed\x7b\x16\xb0\xb2\x6e\x33\x33",
            "ORA",
            18,
            "ORA Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\xd5\x20\x69\x62\x26\x7c\x7b\x4b\x4a\x8b\x3d\x76\xac\x2e\x1b\x2a\x5c\x4d\x5e",
            "OSAK",
            18,
            "Osaka Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\x23\x33\x44\x1c\x05\x53\xfa\xcb\x25\x96\x00\xfa\x83\x3a\x69\x18\x68\x93\xa5",
            "PAAL",
            18,
            "PAAL AI",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x26\x4f\x02\xd3\x01\x28\x1f\x33\x93\xe1\x38\x5c\x0a\xef\xd4\x46\xeb\x0f\x00",
            "PARTI",
            18,
            "PARTI Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x37\xe0\xb5\x0d\xdb\xd7\x19\xa4\x6b\x96\x60\x39\x61\x14\xc7\xf3\xfc\x9a\xa0\x76",
            "PATRIOT",
            18,
            "Patriot on Base",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x6a\x56\x9d\xd6\x1e\xdc\xe3\xc3\x83\xf6\xd5\x65\xe2\xf7\x9e\xc3\xa1\x29\x80",
            "PEEZY",
            18,
            "Young Peezy AKA Pepe",
        )
        yield (  # address, symbol, decimals, name
            b"\x52\xb4\x92\xa3\x3e\x44\x7c\xdb\x85\x4c\x7f\xc1\x9f\x1e\x57\xe8\xbf\xa1\x77\x7d",
            "PEPE",
            18,
            "BasedPepe",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x8d\xc4\x5e\x4f\x10\x96\x6f\x6d\x1d\x98\xe3\xbf\xd7\x07\x1d\x81\x44\xc2\x33",
            "PEPE",
            9,
            "Pepe",
        )
        yield (  # address, symbol, decimals, name
            b"\x91\x7f\x39\xbb\x33\xb2\x48\x3d\xd1\x95\x46\xb1\xe8\xd2\xf0\x9c\xe4\x81\xee\x44",
            "PKT",
            18,
            "PKT",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x4a\x72\x6d\x9c\xed\x04\x33\xa8\xd7\x64\x33\x35\x91\x9d\xeb\x03\xa9\xa9\x35",
            "POKT",
            6,
            "Pocket Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x26\x76\xe4\xe0\xe2\xeb\x58\xd9\xbd\xb5\x07\x83\x58\xff\x8a\x3a\x96\x4c\xed\xf5",
            "POLY",
            18,
            "Polytrader by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x98\x0c\xed\x68\x95\xac\x31\x4e\x7d\xe3\x4e\xf1\xbf\xae\x90\xa5\xad\xd2\x1b",
            "PRIME",
            18,
            "Prime",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xdd\x5b\x08\x7b\xca\x99\x20\x56\x2a\xff\x7a\x01\x99\xb9\x6b\x92\x30\x43\x8b",
            "PRO",
            8,
            "Propy",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\x25\x04\xa0\x3c\xa4\xd4\x3d\x0d\x73\x77\x6f\x6a\xd4\x6d\xab\x2f\x2a\x4c\xfd",
            "REI",
            18,
            "Unit 00 - Rei",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\xe3\xc8\x9b\x8d\x9c\x88\xb1\xfe\x96\x85\x6e\x38\x29\x59\xee\x62\x91\xeb\xba",
            "REKT",
            18,
            "Rekt",
        )
        yield (  # address, symbol, decimals, name
            b"\x19\x86\xcc\x18\xd8\xec\x75\x74\x47\x25\x43\x10\xd2\x60\x4f\x85\x74\x1a\xa7\x32",
            "REWARD",
            18,
            "Rewardable",
        )
        yield (  # address, symbol, decimals, name
            b"\xf0\x26\x8c\x5f\x9a\xa9\x5b\xaf\x5c\x25\xd6\x46\xaa\xbb\x90\x0a\xc1\x2f\x08\x00",
            "RGOAT",
            8,
            "RealGOAT",
        )
        yield (  # address, symbol, decimals, name
            b"\xab\x36\x45\x2d\xba\xc1\x51\xbe\x02\xb1\x6c\xa1\x7d\x89\x19\x82\x60\x72\xf6\x4a",
            "RSR",
            18,
            "Reserve Rights",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x51\x42\xbc\x58\xf9\xa6\x1a\xb8\xc3\xd2\x08\x5d\xd2\xf4\xe5\x50\xc5\xce\x0b",
            "RUSSELL",
            18,
            "RUSSELL",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\x43\x30\xb0\x31\x2f\xde\xec\x6d\x42\x25\x07\x5b\x82\xe0\x04\x93\xff\x2e\x3f",
            "SDEX",
            18,
            "SmarDex Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\x85\xcb\x51\x22\xed\xad\x19\x9b\xdb\xc0\xcb\xd7\xa0\x45\x7e\x44\x8f\x23\xc7",
            "SEKOIA",
            18,
            "sekoia by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x81\x83\x7c\x2f\x4a\x18\x9a\x0b\x69\x37\x00\x27\xcc\x26\x27\xd9\x37\x85\xb4",
            "SERAPH",
            18,
            "Seraph by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x86\x8f\xce\xd6\x5e\xdb\xf0\x05\x6c\x41\x63\x51\x5d\xd8\x40\xe9\xf2\x87\xa4\xc3",
            "SIGN",
            18,
            "Sign",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x8b\xe1\x3e\x16\x80\xb5\xeb\xe0\x02\x4c\x42\xc8\x96\xe3\xdb\x59\xec\x01\x49",
            "SKI",
            9,
            "SKI MASK DOG",
        )
        yield (  # address, symbol, decimals, name
            b"\x6d\x3b\x8c\x76\xc5\x39\x66\x42\x96\x02\x43\xfe\xbf\x73\x6c\x6b\xe8\xb6\x05\x62",
            "SKOP",
            18,
            "SKOP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x29\x74\xdc\x64\x6e\x37\x5e\x83\xbd\x1c\x03\x42\x62\x5b\x49\xf2\x88\x98\x7f\xa4",
            "SMT",
            18,
            "Swarm Markets",
        )
        yield (  # address, symbol, decimals, name
            b"\x70\x3d\x57\x16\x4c\xa2\x70\xb0\xb3\x30\xa8\x7f\xd1\x59\xcf\xef\x14\x90\xc0\xa5",
            "SOFI",
            18,
            "Rai.Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xfc\xc3\xa0\x2e\xee\xba\x05\xef\x70\x1b\x7e\xed\x06\x6c\x6e\xbd\x5d\x4e\x51",
            "SPECTRA",
            18,
            "Spectra Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xda\x64\x5f\x14\x87\x98\xf6\x8e\xf2\xd7\xdb\x7c\x1c\xb2\x2a\x68\x19\xbb\x2c",
            "SPX",
            8,
            "SPX6900",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x55\x4b\xea\x54\x6e\xfa\x83\xc1\xe6\xb3\x89\xec\xac\x40\xea\x99\x9b\x3e\x78",
            "SQD",
            18,
            "Subsquid",
        )
        yield (  # address, symbol, decimals, name
            b"\x39\x13\x59\xab\x0c\xce\xf5\x72\xdc\xac\x78\xf7\x4e\x47\xd7\xc0\x6d\xb0\xb9\x82",
            "SUPER",
            18,
            "SuperVerse",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\xfe\xfd\x2e\x84\x60\xa6\xee\x49\x55\xa6\x85\x82\xf8\x57\x08\xba\xea\x60\xa3",
            "superOETHb",
            18,
            "Super OETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\x49\xa0\x65\xd1\x7d\x6d\x4a\x55\xdc\x13\x64\x99\x01\xfd\xbb\x98\xb2\xaf\xba",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xc2\x22\x52\xcd\xc8\xb7\xcd\xde\xe1\xb0\xb2\xe2\x42\x81\x85\x09\xfc\xf7\xb8",
            "SXT",
            18,
            "Space and Time",
        )
        yield (  # address, symbol, decimals, name
            b"\x80\x08\x22\xd3\x61\x33\x5b\x4d\x5f\x35\x2d\xac\x29\x3c\xa4\x12\x8b\x5b\x60\x5f",
            "SYMM",
            18,
            "Symmio",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\x8a\xee\x02\x2a\xa5\x44\xf1\x50\x67\x8b\x8e\x57\x20\xb6\xb9\x6a\x9e\x9a\x2f",
            "SYRUP",
            18,
            "Syrup Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x55\x02\x7a\x5b\x06\xf4\x34\x0c\xc4\xc8\x2d\xcc\x74\xc9\x0c\xa9\x3d\xcb\x17\x3e",
            "TAD",
            18,
            "Tadpole",
        )
        yield (  # address, symbol, decimals, name
            b"\x7a\x5f\x5c\xcd\x46\xeb\xd7\xac\x30\x61\x58\x36\xd9\x88\xca\x3b\xd5\x74\x12\xb3",
            "TAOCAT",
            18,
            "TAOCat by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\xbe\x16\x92\xca\x16\xe0\x6f\x53\x6f\x00\x38\xff\x11\xd1\xda\x85\x24\xad\xb1",
            "TEL",
            2,
            "Telcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xa4\xa2\xe2\xca\x3f\xbf\xe2\x1a\xed\x83\x47\x1d\x28\xb6\xf6\x5a\x23\x3c\x6e\x00",
            "TIBBIR",
            18,
            "Ribbita by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x03\xce\x27\x0b\x48\x26\xec\x62\xe7\xdd\x00\x7f\x0b\x71\x60\x68\x63\x9f\x7b",
            "TIG",
            18,
            "The Innovation Game",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\x45\x69\x94\x96\x99\xd5\x6e\x18\x34\xef\xe9\xf5\x87\x47\xca\x0f\x15\x1b\x01",
            "TMAI",
            18,
            "Token Metrics AI",
        )
        yield (  # address, symbol, decimals, name
            b"\xb8\xd9\x8a\x10\x2b\x00\x79\xb6\x9f\xfb\xc7\x60\xc8\xd8\x57\xa3\x16\x53\xe5\x6e",
            "toby",
            18,
            "toby",
        )
        yield (  # address, symbol, decimals, name
            b"\x78\xec\x15\xc5\xfd\x8e\xfc\x5e\x92\x4e\x9e\xeb\xb9\xe5\x49\xe2\x9c\x78\x58\x67",
            "TORUS",
            18,
            "Torus",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x44\xfe\x9d\x19\x0f\xd7\xec\x52\x86\x0a\xbb\xf4\x50\x88\xe8\x1e\xe2\x4a\x8c",
            "TOSHI",
            18,
            "Toshi",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\x1b\xd2\x48\x6a\xaf\x3b\x5c\x0f\xc3\xfd\x86\x85\x58\xb0\x82\xa5\x31\xb2\xb4",
            "TOSHI",
            18,
            "Toshi",
        )
        yield (  # address, symbol, decimals, name
            b"\xa8\x1a\x52\xb4\xdd\xa0\x10\x89\x6c\xdd\x38\x6c\x7f\xbd\xc5\xcd\xc8\x35\xba\x23",
            "TRAC",
            18,
            "Trace Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x97\xf2\x61\xb1\xe8\x88\x45\x18\x4f\x67\x8e\x2d\x1e\x7a\x98\xd9\xfd\x38\xde",
            "TYBG",
            18,
            "Base God",
        )
        yield (  # address, symbol, decimals, name
            b"\xc3\xde\x83\x0e\xa0\x75\x24\xa0\x76\x16\x46\xa6\xa4\xe4\xbe\x0e\x11\x4a\x3c\x83",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\xaa\xec\x86\xb6\x5d\x86\xf6\xa7\xb5\xb1\xb0\xc4\x2f\xfa\x53\x17\x10\xb6\xca",
            "USDbC",
            6,
            "USD Base Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x35\x89\xfc\xd6\xed\xb6\xe0\x8f\x4c\x7c\x32\xd4\xf7\x1b\x54\xbd\xa0\x29\x13",
            "USDC",
            6,
            "usdc",
        )
        yield (  # address, symbol, decimals, name
            b"\x5d\x3a\x1f\xf2\xb6\xba\xb8\x3b\x63\xcd\x9a\xd0\x78\x70\x74\x08\x1a\x52\xef\x34",
            "USDe",
            18,
            "USDe",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x60\x47\x35\xc1\xcf\x31\x39\x9c\x6e\x71\x1d\x59\x62\xb2\xb3\xe0\x22\x5a\xd3",
            "USDGLO",
            18,
            "Glo Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\xd9\x35\x6e\x56\x5a\xb3\xa3\x6d\xd7\x77\x63\xfc\x0d\x87\xfe\xaf\x85\x50\x8c",
            "USDM",
            18,
            "Mountain Protocol USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\x55\xae\xbc\x9b\x8c\x03\xfc\x43\xcd\x9f\x62\x26\x03\x91\xc1\x3c\x23\xe7\xc0",
            "USDO",
            18,
            "OpenEden Open Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\xe4\xc9\x6c\x85\x93\x53\x6e\x31\xf2\x29\xea\x8f\x37\xb2\xad\xa2\x69\x9b\xb2",
            "USDT",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x3a\x46\xed\x8f\xce\xb6\xef\x1a\xda\x2e\x46\x00\xa5\x22\xae\x7e\x24\xd2\xed\x18",
            "USSI",
            8,
            "USSI",
        )
        yield (  # address, symbol, decimals, name
            b"\x4a\xcd\x4d\x03\xaf\x6f\x9c\xc0\xfb\x7c\x5f\x08\x68\xb7\xb6\x28\x7d\x79\x69\xc5",
            "USUAL",
            18,
            "USUAL",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\x18\x14\xe4\x91\x57\x1a\x2e\x9e\xe3\xc5\xb1\xf7\xf3\xb9\x62\xee\x8f\x48\x70",
            "VADER",
            18,
            "VaderAI by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\xf5\x13\x02\x74\x75\x3c\x80\x50\xab\x06\x1b\x1a\x1d\xcb\xf5\x83\xf5\xbf\xd0",
            "VCNT",
            18,
            "ViciCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x3e\x32\x84\x55\xc4\x05\x9e\xeb\x9e\x3f\x84\xb5\x54\x3f\x74\xe2\x4e\x7e\x1b",
            "VIRTUAL",
            18,
            "Virtual Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x60\xaa\x1f\xfc\xa5\x0c\xe0\x8b\x3c\xde\xc9\x62\x6c\x0b\xb9\xe9\xb0\x9b\xec",
            "VIS",
            18,
            "Envision",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\xec\xea\xf3\xbf\x88\xef\x07\x97\xe3\x92\x7d\x85\x5c\xa5\xbb\x56\x9a\x47\xfc",
            "VOID",
            18,
            "The Void",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\xfe\x60\x19\xed\x1a\x7d\xc6\xf7\xb5\x08\xc0\x2d\x1b\x04\xec\x88\xcc\x21\xbf",
            "VVV",
            18,
            "Venice Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xff\xa8\x00\x08\x86\xe5\x7f\x86\xdd\x52\x64\xb9\x58\x2b\x2a\xd8\x7b\x2b\x91",
            "W",
            18,
            "Wormhole Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\x12\xb8\x71\x42\x21\xbb\xd9\x6a\xe0\xa0\x03\x2a\x68\x3e\x38\xb4\x75\xd0\x6c",
            "WAI",
            18,
            "WAI Combinator by Virtuals",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\xbb\xea\xd6\x2f\x76\x47\xae\x83\x23\xd2\xcb\x24\x3a\x0d\xb7\x4b\x7c\x2b\x80",
            "WALLET",
            18,
            "Ambire Wallet",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x73\x4b\x57\xdf\xe8\x34\xf1\x02\xfb\x61\xe1\xeb\xf8\x44\xad\xf8\xdd\x93\x1e",
            "weirdo",
            8,
            "weirdo",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\x9b\xb3\x65\x19\x53\x8e\x0c\x07\x38\x94\xe9\x64\xe9\x01\x72\xe1\xc0\xb4\x1f",
            "WEWE",
            18,
            "WEWECOIN",
        )
        yield (  # address, symbol, decimals, name
            b"\x01\xed\xbf\xfa\x4f\x61\x40\x44\x58\xe2\x2f\xf4\x5d\xef\xfe\xf1\xc6\x22\x28\xb5",
            "wLIBRA",
            18,
            "Wrapped Libra",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\xb2\xb1\xa2\xad\xb0\x2b\x38\x22\x2a\xdc\xd0\x57\x78\x3d\x7e\x5d\x1f\xcc\x7d",
            "WLTH",
            18,
            "Common Wealth Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x31\x96\x6d\x4f\x81\xc7\x2d\x2a\x55\x31\x0a\x63\x65\xa5\x6a\x43\x93\xe9\x8d",
            "WMTX",
            6,
            "WorldMobileToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xc1\x11\x58\xc5\xda\x9d\xb1\xd5\x53\xed\x28\xf0\xc2\xba\x1c\xbe\xdd\x42\xcf\xcb",
            "wPAW",
            18,
            "Wrapped PAW",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\x63\x2e\x6a\xaa\x3e\xa7\x3f\x91\x55\x4f\x8a\x3c\xb2\xed\x2f\x29\x60\x5e\x0c",
            "XCN",
            18,
            "Onyxcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xeb\xb7\x87\x32\x13\xc8\xd1\xd9\x91\x3d\x8e\xa3\x9a\xa1\x2d\x74\xcb\x10\x79\x95",
            "XVS",
            18,
            "Venus XVS",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\xc7\x8d\x12\x19\xc0\x8a\xec\xc8\xe3\x7e\x03\x85\x8f\xe8\x85\xf5\xef\x17\x99",
            "YGG",
            18,
            "Yield Guild Games Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x2c\x8c\x89\xc4\x42\x43\x6c\xc6\xc0\xa7\x79\x43\xe0\x9c\x8d\xaf\x49\xda\x31\x61",
            "ZBU",
            18,
            "ZEEBU",
        )
        yield (  # address, symbol, decimals, name
            b"\xf4\x3e\xb8\xde\x89\x7f\xbc\x7f\x25\x02\x48\x3b\x2b\xef\x7b\xb9\xea\x17\x92\x29",
            "ZEN",
            18,
            "Horizen",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\x11\x11\x11\x11\x16\x6b\x7f\xe7\xbd\x91\x42\x77\x24\xb4\x87\x98\x0a\xfc\x69",
            "ZORA",
            18,
            "Zora",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x85\x88\x4c\x43\x92\xd3\x48\x58\x7b\x19\xcb\x9e\xaa\xf1\x57\xf1\x32\x71\xcd",
            "ZRO",
            18,
            "LayerZero",
        )
    if chain_id == 42161:  # arb1
        yield (  # address, symbol, decimals, name
            b"\x2e\x9a\x6d\xf7\x8e\x42\xa3\x07\x12\xc1\x0a\x9d\xc4\xb1\xc8\x65\x6f\x8f\x28\x79",
            "4d4b5200000000000000",
            18,
            "4d616b65720000000000000000000000",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xec\xc6\xac\x00\x73\xc0\x63\xdc\xfc\x02\x6c\xcd\xc9\x03\x9c\xae\x29\x98\xe1",
            "AA",
            18,
            "Account Abstraction Incentive",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x5d\xdd\x1f\x9d\x7f\x57\x0d\xc9\x4a\x51\x47\x9a\x00\x0e\x3b\xce\x96\x71\x96",
            "AAVE",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\xe1\x85\x90\xe8\xf7\x6b\x6c\xf4\x71\xb3\xcd\x75\xfe\x1a\x1a\x9d\x2b\x2c\x2b",
            "AIDOGE",
            6,
            "AIDOGE",
        )
        yield (  # address, symbol, decimals, name
            b"\x37\xa6\x45\x64\x8d\xf2\x92\x05\xc6\x26\x12\x89\x98\x3f\xb0\x4e\xcd\x70\xb4\xb3",
            "ANIME",
            18,
            "Animecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\xae\xed\x23\x47\x8c\x3a\x4b\x79\x8e\x4e\xd4\x0d\x8b\x7f\x41\x36\x6a\xe8\x61",
            "ANKR",
            18,
            "Ankr Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xbb\xf4\x50\x8b\x13\x91\xaf\x3a\x0f\x4b\x30\xbb\x5f\xc4\xaa\x9a\xb0\xe0\x7c",
            "Anon",
            18,
            "HeyAnon",
        )
        yield (  # address, symbol, decimals, name
            b"\xa7\x8d\x83\x21\xb2\x0c\x4e\xf9\x0e\xcd\x72\xf2\x58\x8a\xa9\x85\xa4\xbd\xb6\x84",
            "ANT",
            18,
            "Autonomi",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\xa1\xff\x55\xc5\x21\x6b\x63\x6a\x29\x4a\x07\xd7\x7c\x6f\x4d\xf1\x0d\x3b\x56",
            "APEX",
            18,
            "ApeX Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x91\x2c\xe5\x91\x44\x19\x1c\x12\x04\xe6\x45\x59\xfe\x82\x53\xa0\xe4\x9e\x65\x48",
            "ARB",
            18,
            "Arbitrum",
        )
        yield (  # address, symbol, decimals, name
            b"\xc8\x7b\x37\xa5\x81\xec\x32\x57\xb7\x34\x88\x6d\x9d\x3a\x58\x1f\x5a\x9d\x05\x6c",
            "ATH",
            18,
            "Aethir Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\xee\x23\x43\xb8\x92\xb1\xbb\x63\x50\x3a\x4f\xab\xc8\x40\xe0\xe2\xc6\x81\x0f",
            "AXL",
            6,
            "Axelar",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x0d\x1e\xdc\x95\x69\xd4\xba\xb2\xd1\x52\x87\xdc\x5a\x4f\x10\xf5\x6a\x56\xb8",
            "BAL",
            18,
            "Balancer",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\xc3\x96\x0a\xee\xaf\x4c\x25\x5a\x87\x7d\xa0\x4b\x06\x48\x7b\xba\x69\x83\x86",
            "BIC",
            18,
            "Beincom",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x19\x9d\x9a\x5f\x44\x48\xd0\x84\x8e\x43\x95\xd0\x65\xe1\xad\x9c\x4a\x1f\x74",
            "BONK",
            5,
            "Bonk",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\x89\x68\x93\xdf\xc8\x6b\xb6\x7c\xf5\x77\x67\x29\x8b\x90\x73\xd2\xc1\xba\x2c",
            "CAKE",
            18,
            "PancakeSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x8c\xd2\x64\x74\x72\x2c\xe9\x3f\x4d\x0c\x34\xd1\x62\x01\x46\x1c\x10\xaa\x8c",
            "CARV",
            18,
            "CARV",
        )
        yield (  # address, symbol, decimals, name
            b"\x3a\x8b\x78\x7f\x78\xd7\x75\xae\xcf\xee\xa1\x57\x06\xd4\x22\x1b\x40\xf3\x45\xab",
            "CELR",
            18,
            "CelerToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x4a\x6d\xa3\xfc\xde\x09\x8f\x83\x89\xca\xd8\x4b\x01\x82\x72\x5c\x6c\x91\xde",
            "COMP",
            18,
            "Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\xcb\x8b\x5c\xd2\x0b\xdc\xae\xa9\xa0\x10\xac\x1f\x8d\x83\x58\x24\xf5\xc8\x7a\x04",
            "COW",
            18,
            "CoW Protocol Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf4\xd4\x8c\xe3\xee\x1a\xc3\x65\x19\x98\x97\x15\x41\xba\xdb\xb9\xa1\x4d\x72\x34",
            "CREAM",
            18,
            "Cream",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\xcd\xb4\x2b\x0e\xb4\x6d\x95\xf9\x90\xbe\xdd\x46\x95\xa6\xe3\xfa\x03\x49\x78",
            "CRV",
            18,
            "Curve DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\x9f\x86\x5b\x28\x7f\xcc\x10\xb3\x0d\x8c\xe6\x14\x4e\x8b\x6d\x1b\x47\x69\x99",
            "CTSI",
            18,
            "Cartesi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x10\x00\x9c\xbd\x5d\x07\xdd\x0c\xec\xc6\x61\x61\xfc\x93\xd7\xc9\x00\x0d\xa1",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\x07\xf8\xa8\x2c\xb1\xaf\x14\x66\x25\x2e\x50\x5b\x7b\x7d\xde\xe1\x03\xbc\x91",
            "DEGEN",
            18,
            "DEGEN",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x6a\xab\x43\xc4\xf3\xe0\xce\xa4\xab\x83\x75\x2c\x27\x8f\x8d\xeb\xab\xa6\x89",
            "DF",
            18,
            "dForce",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\xeb\x4f\xa4\xa2\xfb\xd4\x98\xc2\x57\xc5\x7e\xa8\xb7\x65\x5a\x25\x59\xa5\x81",
            "DODO",
            18,
            "DODO bird",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\x25\x74\x2f\x1e\xc8\xd9\x87\x79\x69\x0b\x5a\x3a\x62\x76\xdb\x85\xdd\xc0\x1a",
            "DOG",
            18,
            "The Doge NFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x76\x61\x79\x5c\x37\x4c\x0b\xfc\x63\x59\x34\xef\xad\xdf\xf3\xa7\xee\x23\xb6",
            "DOLA",
            18,
            "Dola USD Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x01\x0b\xf9\xc2\x68\x81\x78\x8b\x4e\x6b\xf5\xfd\x1b\xdc\x35\x8c\x8f\x90\xb8",
            "DOT",
            18,
            "Polkadot Token (Relay Chain)",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\xb7\x78\x7a\x09\x81\x85\x02\x30\x5c\x95\xd6\x8a\x25\x71\xf0\x90\xab\xb1\x35",
            "DRV",
            18,
            "Derive",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\x25\x98\xe9\xe6\x21\x55\xd7\xfd\xfe\x65\xfd\xf0\x1e\xb5\xa5\x3a\x84\x65\xbe",
            "EMP",
            18,
            "Empyreal",
        )
        yield (  # address, symbol, decimals, name
            b"\xf4\x2a\xe1\xd5\x4f\xd6\x13\xc9\xbb\x14\x81\x0b\x05\x88\xfa\xaa\x09\xa4\x26\xca",
            "esGMX",
            18,
            "Escrowed GMX",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x89\xfb\x5b\x65\x04\xbb\xff\x6a\x85\x2b\x13\xb7\xb8\x2a\x3c\x11\x8f\xdc\x27",
            "ETHFI",
            18,
            "ether.fi governance token",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\xd9\x83\x1d\x87\x51\xb2\x32\x5f\x3d\xbf\x48\xdb\x74\x87\x23\x72\x6e\x1c\x8c",
            "EVA",
            18,
            "EverValueCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x93\xc9\x93\x2e\x4a\xfa\x59\x20\x1f\x0b\x5e\x63\xf7\xd8\x16\x51\x6f\x16\x69\xfe",
            "FDUSD",
            18,
            "First Digital USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\xed\x6b\x89\x9e\xdb\xa7\x62\xd7\x93\xf4\x4e\xd2\x95\x24\x80\x49\x44\x0e\x1e",
            "FIUSD",
            2,
            "Fidelity ILF USD Fund Class G Ac",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x7d\x4b\xf2\xbb\x00\xa2\xf4\xad\xe1\xc7\x26\x81\x9f\xca\xca\x0e\x51\x7a\x5b",
            "FLUID",
            18,
            "Fluid",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\xfc\x00\x2b\x46\x6e\xec\x40\xda\xe8\x37\xfc\x4b\xe5\xc6\x79\x93\xdd\xbd\x6f",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\x84\x12\xe7\x9c\x25\x96\x8a\x32\xe8\x9b\x11\xf6\x3b\x33\xf7\x33\x77\x0c\x2a",
            "frxETH",
            18,
            "Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\x45\x97\x1c\x16\x8b\x52\x94\xac\xbc\x87\x27\xa4\xf1\xc9\xe1\xaf\x99\xf6\xd0",
            "FTN",
            18,
            "Bridged FTN (OrtakSea)",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\x2f\x29\x97\x15\xd9\x4d\x8a\x7e\x6f\x5e\xaa\x8e\x65\x4e\x8c\x74\xa9\x88\xa7",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x5a\x1a\x6e\xb0\x76\xa2\xc7\xad\x06\xed\x22\xc9\x0d\x7e\x71\x0e\x35\xad\x0a",
            "GMX",
            18,
            "GMX",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\xb8\x62\xf6\x0e\xde\xf4\x45\x2f\x25\xb4\x16\x0f\x17\x7d\xb4\x4d\xeb\x6c\xf1",
            "GNO",
            18,
            "Gnosis Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xc1\x1f\xd2\x86\xc5\xec\x11\xc3\xb6\x83\xca\xa8\x13\xb7\x7f\x51\x63\xa1\x22",
            "GNS",
            18,
            "Gains Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x96\x23\x06\x33\x77\xad\x1b\x27\x54\x4c\x96\x5c\xcd\x73\x42\xf7\xea\x7e\x88\xc7",
            "GRT",
            18,
            "Graph Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\xd2\x3e\xd2\xad\xb0\xf5\x51\x36\x99\x46\xbd\x37\x7f\x86\x44\xce\x1c\xa5\xc4",
            "HYPER",
            18,
            "Hyperlane",
        )
        yield (  # address, symbol, decimals, name
            b"\x37\x1c\x7e\xc6\xd8\x03\x9f\xf7\x93\x3a\x2a\xa2\x8e\xb8\x27\xff\xe1\xf5\x2f\x07",
            "JOE",
            18,
            "JoeToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\xdd\xdf\xe6\x7e\x71\x64\xb0\xfe\x14\xe2\x18\xd8\x0d\xc4\xc0\x8e\xdc\x01\xcb",
            "KNC",
            18,
            "Kyber Network Crystal v2",
        )
        yield (  # address, symbol, decimals, name
            b"\x3b\x60\xff\x35\xd3\xf7\xf6\x2d\x63\x6b\x06\x7d\xd0\xdc\x0d\xfd\xad\x67\x0e\x4e",
            "LADYS",
            18,
            "Milady",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x3e\x44\xae\xa7\x14\xfb\xbc\x08\xef\x28\x34\x0d\x78\x06\x7b\x9a\x8c\xad\x73",
            "LBR",
            18,
            "Lybra",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\xad\x51\xed\x4f\x1b\x7e\x9d\xc1\x68\xd8\xa0\x0c\xb3\xf4\xdd\xd8\x5e\xfa\x60",
            "LDO",
            18,
            "Lido DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\x7f\x4d\xf7\x51\x17\xa7\x8c\x1a\x5a\x0d\xbb\x81\x4a\xf9\x24\x58\x53\x9f\xb4",
            "LINK",
            18,
            "ChainLink",
        )
        yield (  # address, symbol, decimals, name
            b"\x28\x9b\xa1\x70\x1c\x2f\x08\x8c\xf0\xfa\xf8\xb3\x70\x52\x46\x33\x1c\xb8\xa8\x39",
            "LPT",
            18,
            "Livepeer Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\xd0\xce\x7d\xe6\x24\x7b\x0a\x95\xf6\x7b\x43\xb5\x89\xb4\x04\x1b\xae\x7f\xbe",
            "LRC",
            18,
            "LoopringCoin V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x53\x9b\xde\x0d\x7d\xbd\x33\x6b\x79\x14\x8a\xa7\x42\x88\x31\x98\xbb\xf6\x03\x42",
            "MAGIC",
            18,
            "MAGIC",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\xf4\x0b\x01\xba\x9c\x46\x91\x93\xb3\x60\xf7\x27\x40\xe4\x16\xb1\x7a\xc3\x32",
            "MATH",
            18,
            "MATH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\x48\xc7\x45\x6a\x97\x76\x9f\x6c\xd0\x4f\x1e\x83\xa4\xa2\x3c\xcd\xc4\x6a\xbd",
            "MAV",
            18,
            "Maverick Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\x1f\x74\x24\x74\x55\xa4\x0b\x01\xb0\x55\x9f\xf6\x27\x44\x41\xfa\xfa\x22\xa3",
            "MGP",
            18,
            "Magpie Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x37\x4c\x5f\xb7\x97\x9d\x5f\xdb\xaa\xd2\xd9\x54\x09\xe2\x35\xe5\xcb\xdf\xd4\x3c",
            "MLK",
            8,
            "MiL.k",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x2b\xaa\xdb\x7d\xef\x4c\x39\x81\x45\x4d\xd9\xc0\xa0\xd7\xff\x07\xbc\xfc\x86",
            "MOR",
            18,
            "MOR",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\xbd\x67\x0a\x58\x23\x8e\x6e\x23\x0c\x43\x0b\xbb\x5c\xe6\xec\x0d\x40\xdf\x48",
            "MORPHO",
            18,
            "Morpho Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\xde\xca\x11\x67\xfe\x47\x49\x9e\x53\xeb\x50\xf2\x61\x10\x36\x30\x97\x49\x05",
            "NRN",
            18,
            "Neuron",
        )
        yield (  # address, symbol, decimals, name
            b"\xf0\xcb\x2d\xc0\xdb\x5e\x6c\x66\xb9\xa7\x0a\xc2\x7b\x06\xb8\x78\xda\x01\x70\x28",
            "OHM",
            9,
            "Olympus",
        )
        yield (  # address, symbol, decimals, name
            b"\xf3\xc0\x91\xed\x43\xde\x9c\x27\x05\x93\x44\x51\x63\xa4\x1a\x87\x6a\x0b\xb3\xdd",
            "ORBS",
            18,
            "Orbs",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\xd5\x20\x69\x62\x26\x7c\x7b\x4b\x4a\x8b\x3d\x76\xac\x2e\x1b\x2a\x5c\x4d\x5e",
            "OSAK",
            18,
            "Osaka Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\x0c\x88\x0f\x67\x61\xf1\xaf\x8d\x9a\xa9\xc4\x66\x98\x4b\x80\xda\xb9\xa8\xc9\xe8",
            "PENDLE",
            18,
            "Pendle",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\x3d\x22\x4b\xcf\x9a\xaf\xac\xd8\x15\x58\xc3\x23\x41\x41\x6d\xf6\x1d\x3d\xac",
            "PERP",
            18,
            "Perpetual",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x0a\x57\xb7\x10\x76\x8a\xe1\x79\x41\xa9\xfa\x33\xf8\xb7\x20\xc8\xbd\x9d\xdd",
            "POND",
            18,
            "Marlin POND",
        )
        yield (  # address, symbol, decimals, name
            b"\xb4\x35\x70\x54\xc3\xda\x8d\x46\xed\x64\x23\x83\xf0\x31\x39\xac\x7f\x09\x03\x43",
            "PORT3",
            18,
            "Port3 Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x85\x0a\xd6\x1c\x2b\x7d\x64\xd0\x8c\x9c\x75\x4f\x45\x25\x45\x96\x69\x69\x84",
            "PYUSD",
            6,
            "PayPal USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x51\x05\xec\x5b\xbb\xf1\x7d\xba\x7a\x87\xe1\xae\xd2\xc2\xc1\x53\x94\xa9\xe2",
            "RCGE",
            18,
            "RCGE",
        )
        yield (  # address, symbol, decimals, name
            b"\x30\x82\xcc\x23\x56\x8e\xa6\x40\x22\x5c\x24\x67\x65\x3d\xb9\x0e\x92\x50\xaa\xa0",
            "RDNT",
            18,
            "Radiant",
        )
        yield (  # address, symbol, decimals, name
            b"\xab\xd5\x87\xf2\x60\x75\x42\x72\x3b\x17\xf1\x4d\x00\xd9\x9b\x98\x7c\x29\xb0\x74",
            "SDEX",
            18,
            "SmarDex Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\xab\x45\x87\x5c\xff\xdb\xa1\xe5\xf4\x51\xb9\x50\xbc\x2e\x42\xc0\x05\x3f\x39",
            "sfrxETH",
            18,
            "Staked Frax Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x5b\xff\x88\xca\x14\x42\xc2\x49\x6f\x7e\x47\x5e\x9e\x77\x86\x38\x3b\xc0\x70\xc0",
            "sfrxUSD",
            18,
            "Staked Frax USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xcb\xec\xf3\x9b\xee\x52\x8b\x54\x76\xfe\x6d\x15\x50\xaf\x59\xa9\xdb\x6f\xc0",
            "SGETH",
            18,
            "Stargate Ether Vault",
        )
        yield (  # address, symbol, decimals, name
            b"\x36\x47\xc5\x4c\x4c\x2c\x65\xbc\x7a\x2d\x63\xc0\xda\x28\x09\xb3\x99\xdb\xbd\xc0",
            "SolvBTC",
            18,
            "Solv BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x34\x6c\x57\x4c\x56\xe1\xa4\xaa\xa8\xdc\x88\xcd\xa8\xf7\xeb\x12\xb3\x99\x47\xab",
            "SolvBTC.BBN",
            18,
            "SolvBTC Babylon",
        )
        yield (  # address, symbol, decimals, name
            b"\x31\xdb\xa3\xc9\x64\x81\xfd\xe3\xcd\x81\xc2\xaa\xf5\x1f\x2d\x8b\xf6\x18\xc7\x42",
            "SOPH",
            18,
            "Sophon",
        )
        yield (  # address, symbol, decimals, name
            b"\x55\x75\x55\x29\x88\xa3\xa8\x05\x04\xbb\xae\xb1\x31\x16\x74\xfc\xfd\x40\xad\x4b",
            "SPA",
            18,
            "Sperax",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x66\x48\xc5\xa7\x0a\x15\x0a\x88\xbc\xe6\x5f\x4a\xd4\xd5\x06\xfe\x15\xd2\xaf",
            "SPELL",
            18,
            "Spell Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\x37\x42\x0d\xed\x5a\xdb\x99\x80\xcf\xc3\x5f\x8f\x2b\x05\x4e\xa8\x6f\x8a\xb1",
            "SQD",
            18,
            "Subsquid",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\x94\x34\x0f\xc0\x20\xc5\xe6\xb9\x65\x67\x84\x3d\xa2\xdf\x01\xb2\xce\x1e\xb6",
            "STG",
            18,
            "StargateToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xa9\x70\xaf\x1a\x58\x45\x79\xb6\x18\xbe\x4d\x69\xad\x6f\x73\x45\x9d\x11\x2f\x95",
            "sUSD",
            18,
            "Synth sUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\xd4\x2f\x0b\x6d\xef\x4c\xe0\x38\x36\x36\x77\x0e\xf7\x73\x39\x0d\x85\xc6\x1a",
            "SUSHI",
            18,
            "Sushi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\x7d\xec\x85\x50\xf4\x3a\x5e\x46\xe3\xdf\xb9\x58\x01\xf6\x42\x80\xe7\x5b\x27",
            "SWEAT",
            18,
            "SWEAT",
        )
        yield (  # address, symbol, decimals, name
            b"\x08\x0f\x6a\xed\x32\xfc\x47\x4d\xd5\x71\x71\x05\xdb\xa5\xea\x57\x26\x8f\x46\xeb",
            "SYN",
            18,
            "Synapse",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x19\xe8\xbf\xbb\xb2\x62\x37\x28\xc3\xa6\x12\x90\x90\xda\x4f\xf4\xe4\x81\x13",
            "TEL",
            2,
            "Telcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x19\x3f\x4a\x4a\x6e\xa2\x41\x02\xf4\x9b\x93\x1d\xee\xeb\x93\x1f\x6e\x32\x40\x5d",
            "TLOS",
            18,
            "TLOS",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\x15\xa3\xa2\x28\x6d\x88\x3a\xf0\xaa\x1b\x3f\x21\x36\x78\x43\xfa\xc6\x3e\x07",
            "TUSD",
            18,
            "TrueUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x7f\x89\x80\xb0\xf1\xe6\x4a\x20\x62\x79\x1c\xc3\xb0\x87\x15\x72\xf1\xf7\xf0",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\xf1\xc5\xcb\x7f\xb9\x77\xe6\x69\xfd\x24\x4c\x56\x7d\xa9\x9d\x8a\x3a\x68\x50",
            "USD0",
            18,
            "Usual USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x88\xd0\x65\xe7\x7c\x8c\xc2\x23\x93\x27\xc5\xed\xb3\xa4\x32\x26\x8e\x58\x31",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\x97\x0a\x61\xa0\x4b\x1c\xa1\x48\x34\xa4\x3f\x5d\xe4\x53\x3e\xbd\xdb\x5c\xc8",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x5d\x3a\x1f\xf2\xb6\xba\xb8\x3b\x63\xcd\x9a\xd0\x78\x70\x74\x08\x1a\x52\xef\x34",
            "USDe",
            18,
            "USDe",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\x85\x0b\x0a\xb1\x98\x8d\xd1\x7b\x69\xac\x56\x4c\x1e\x28\x57\x94\x9e\x4d\xee",
            "USDL",
            18,
            "Lift Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\xd9\x35\x6e\x56\x5a\xb3\xa3\x6d\xd7\x77\x63\xfc\x0d\x87\xfe\xaf\x85\x50\x8c",
            "USDM",
            18,
            "Mountain Protocol USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\x91\xc0\x5a\x82\x21\x9b\x8d\x14\x79\x05\x73\x61\xff\x16\x54\x74\x9b\x87\x6b",
            "USDS",
            18,
            "USDS Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\x08\x6b\xc7\xcd\x5c\x48\x1d\xcc\x9c\x85\xeb\xe4\x78\xa1\xc0\xb6\x9f\xcb\xb9",
            "USDT",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\xe0\x50\xd3\xc0\xec\x2d\x29\xd2\x69\xa8\xec\xea\x76\x3a\x18\x3b\xdf\x9a\x9d",
            "USDY",
            18,
            "Ondo U.S. Dollar Yield",
        )
        yield (  # address, symbol, decimals, name
            b"\x02\x12\x89\x58\x8c\xd8\x1d\xc1\xac\x87\xea\x91\xe9\x16\x07\xee\xf6\x83\x03\xf5",
            "USTBL",
            5,
            "Spiko US T-Bills Money Market Fu",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\x6b\x3a\x62\x39\x1e\xcc\xaa\xa9\x92\xad\xe4\x4c\xd4\xaf\xe6\xbe\xc8\xcf\xf1",
            "UXLINK",
            18,
            "UXLINK Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\xbf\x4e\x7c\xf1\x6f\xf3\x45\x13\x51\x4b\x96\x84\x83\xb5\x4b\xef\xf4\x2a\x81",
            "VCNT",
            18,
            "ViciCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\x14\x68\x81\xb8\x6b\x3e\xe9\x9e\x63\x70\x5e\xc8\x7a\xfe\x29\xfc\xc0\x44\xd9",
            "VRTX",
            18,
            "Vertex",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\xff\xa8\x00\x08\x86\xe5\x7f\x86\xdd\x52\x64\xb9\x58\x2b\x2a\xd8\x7b\x2b\x91",
            "W",
            18,
            "Wormhole Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xaf\x49\x44\x7d\x8a\x07\xe3\xbd\x95\xbd\x0d\x56\xf3\x52\x41\x52\x3f\xba\xb1",
            "WETH",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\xb5\xcf\x12\x40\x8a\x3a\xc1\x7d\x66\x80\x37\xce\x28\x9f\x9e\xa7\x54\x39\xd7",
            "WMTX",
            6,
            "WorldMobileToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\xfc\xd8\x5d\x8c\xa7\xad\x1e\x1c\x6f\x82\xf6\x51\xfa\x15\xe3\x3a\xef\xd0\x7b",
            "WOO",
            18,
            "Wootrade Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x79\xd7\xb5\x46\xe3\x8e\x41\x4f\x7e\x98\x22\x51\x4b\xe4\x43\xa4\x80\x05\x29",
            "wstETH",
            18,
            "Wrapped liquid staked Ether 2.0",
        )
        yield (  # address, symbol, decimals, name
            b"\x4c\xb9\xa7\xae\x49\x8c\xed\xcb\xb5\xea\xe9\xf2\x57\x36\xae\x7d\x42\x8c\x9d\x66",
            "XAI",
            18,
            "Xai",
        )
        yield (  # address, symbol, decimals, name
            b"\xc1\xeb\x76\x89\x14\x7c\x81\xac\x84\x0d\x4f\xf0\xd2\x98\x48\x9f\xc7\x98\x6d\x52",
            "XVS",
            18,
            "Venus XVS",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xe3\xa8\xf0\x66\xa6\x98\x96\x66\xb0\x31\xd9\x16\xc4\x36\x72\x08\x5b\x15\x82",
            "YFI",
            18,
            "yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x85\x88\x4c\x43\x92\xd3\x48\x58\x7b\x19\xcb\x9e\xaa\xf1\x57\xf1\x32\x71\xcd",
            "ZRO",
            18,
            "LayerZero",
        )
    if chain_id == 43114:  # avax
        yield (  # address, symbol, decimals, name
            b"\xd5\x01\x28\x15\x65\xbf\x77\x89\x22\x45\x23\x14\x4f\xe5\xd9\x8e\x8b\x28\xf2\x67",
            "1INCH",
            18,
            "1INCH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\xe2\xde\xe5\x4b\xb9\x92\x1a\x2a\xe0\xa6\x3d\xbb\x2d\xf8\xed\x88\xb9\x1d\xd9",
            "AAVE",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x63\xa7\x28\x06\x09\x8b\xd3\xd9\x52\x0c\xc4\x33\x56\xdd\x78\xaf\xe5\xd3\x86\xd9",
            "AAVE.e",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x79\xea\x4e\x53\x6f\x59\x8d\xcd\x67\xc7\x6e\xe3\x82\x9f\x84\xab\x9e\x72\xa5\x58",
            "AI9000",
            18,
            "ai9000 Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\xfb\xc4\x96\x72\x59\x78\x6c\x74\x33\x61\xa5\x88\x5e\xf4\x93\x80\x47\x3d\xcf",
            "ALEPH",
            18,
            "Aleph.im V2 SuperToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x37\x83\x05\x5f\x90\x47\xc2\xbf\xf9\x9c\x4e\x41\x45\x01\xf8\xa1\x47\xbc\x69",
            "ALOT",
            18,
            "Dexalot Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x47\xef\xff\x67\x5e\x4a\x4e\xe1\xc2\xf9\x18\xd1\x81\xcd\xbd\x7a\x8e\x20\x8f",
            "ALPHA.e",
            18,
            "AlphaToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x02\x7d\xbc\xa0\x46\xca\x15\x6d\xe9\x62\x2c\xd1\xe2\xd9\x07\xd3\x75\xe5\x3a\xa7",
            "AMPL",
            9,
            "Ampleforth secured by Meter Pass",
        )
        yield (  # address, symbol, decimals, name
            b"\xb8\xd7\x71\x0f\x7d\x83\x49\xa5\x06\xb7\x5d\xd1\x84\xf0\x57\x77\xc8\x2d\xad\x0c",
            "ARENA",
            18,
            "ArenaToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x00\x00\x00\xef\xe3\x02\xbe\xaa\x2b\x3e\x6e\x1b\x18\xd0\x8d\x69\xa9\x01\x2a",
            "AUSD",
            6,
            "AUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\xa9\x88\x94\xff\xbf\xa9\x8c\x73\xa8\x18\xb5\xb0\x44\xe5\xb1\xc8\x66\x6f\x56",
            "AVIA",
            18,
            "Kepler",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\xde\x87\x71\xbc\x5d\xdb\x33\x62\xdb\x52\x9f\xc3\x35\x8f\x2d\xf3\xa0\xe3\x46",
            "avUSD",
            18,
            "avUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\xc7\x84\x26\x6c\xf0\x24\xa6\x0e\x8a\xcf\x24\x27\xb9\x85\x7a\xce\x19\x4c\x5d",
            "AXL",
            6,
            "Axelar",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\xb5\x50\x56\x8c\x68\x8d\x5d\x8a\x52\xc7\xd7\x94\xcb\x93\xed\xc2\x6e\xc0\xec",
            "axlUSDC",
            6,
            "Axelar Wrapped USDC",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\x5b\xcb\x9e\x0e\xa6\x9e\x6a\xb9\xfa\x08\x0c\x4c\x4a\x56\x32\x89\x62\x98\xc3",
            "BAL",
            18,
            "Balancer",
        )
        yield (  # address, symbol, decimals, name
            b"\x98\x44\x3b\x96\xea\x4b\x08\x58\xfd\xf3\x21\x9c\xd1\x3e\x98\xc7\xa4\x69\x05\x88",
            "BAT.e",
            18,
            "Basic Attention Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\x1d\x35\xc5\x2a\x3c\x24\x4e\x7b\x8b\xd9\x2c\x0c\x29\x77\x55\xfb\xd8\x92\x12",
            "BETA",
            18,
            "Beta Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\x0a\xfe\xd8\x9a\x6d\x6a\x10\x02\x73\xdb\x37\x7d\xba\x7a\x32\xc7\x39\xe3\x14",
            "BIG",
            18,
            "BIG",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\x66\x98\x08\xd8\x8b\x2b\x0b\x3d\x23\x21\x4d\xcd\x2a\x1c\xc6\xa8\xb1\xb5\xcd",
            "BLUB",
            18,
            "Blub",
        )
        yield (  # address, symbol, decimals, name
            b"\x08\x7c\x44\x0f\x25\x1f\xf6\xcf\xe6\x2b\x86\xdd\xe1\xbe\x55\x8b\x95\xb4\xbb\x9b",
            "Bold",
            18,
            "Bold Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x15\x2b\x9d\x0f\xdc\x40\xc0\x96\x75\x7f\x57\x0a\x51\xe4\x94\xbd\x4b\x94\x3e\x50",
            "BTC.b",
            8,
            "Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\x9e\x5f\xd8\xbb\xc2\x59\x84\xb1\x78\xfd\xce\x61\x17\xde\xfa\x39\xd2\xdb\x39",
            "BUSD",
            18,
            "BUSD Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\x34\x92\xa2\x50\x8d\xdf\x4f\xdc\x0c\xd7\x6f\x31\xf3\x40\xb3\x0d\x17\x93\xe6",
            "CLY",
            18,
            "Colony Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\xf2\x62\x4d\x5f\x99\xa9\x53\xae\x55\x74\xea\x57\xd3\x26\x87\x85\x94\x1d\xe4",
            "COLS",
            18,
            "Cointel",
        )
        yield (  # address, symbol, decimals, name
            b"\xc3\x04\x8e\x19\xe7\x6c\xb9\xa3\xaa\x9d\x77\xd8\xc0\x3c\x29\xfc\x90\x6e\x24\x37",
            "COMP.e",
            18,
            "Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\x63\x7a\xfe\xff\x75\xca\x66\x9f\xf9\x2e\x45\x70\xb1\x4d\x63\x99\xa6\x58\x90\x2f",
            "COOK",
            18,
            "Poly-Peg COOK",
        )
        yield (  # address, symbol, decimals, name
            b"\x42\x0f\xca\x01\x21\xdc\x28\x03\x91\x45\x00\x95\x70\x97\x57\x47\x29\x5f\x23\x29",
            "COQ",
            18,
            "COQINU",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\x28\x9c\xce\xaa\x86\x39\xe3\x83\x10\x95\xd7\x5a\x3e\x43\x52\x0f\xab\xf5\x52",
            "CTSI",
            18,
            "Cartesi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\x86\xe7\xf8\x44\xce\xa2\xf8\x7f\x50\x15\x26\x65\xbc\xbc\x2c\x27\x9d\x8d\x70",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\xe8\xd1\x18\xa1\x86\x4c\x7d\xc5\x3b\xf9\x1e\x00\x7a\xb7\xd9\x1f\x5a\x06\xfa",
            "DEXTF",
            18,
            "DEXTF Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf2\x0d\x96\x2a\x6c\x8f\x70\xc7\x31\xbd\x83\x8a\x3a\x38\x8d\x7d\x48\xfa\x6e\x15",
            "ETH",
            18,
            "Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xc8\x91\xeb\x4c\xbd\xef\xf6\xe0\x73\xe8\x59\xe9\x87\x81\x5e\xd1\x50\x5c\x2a\xcd",
            "EURC",
            6,
            "Euro Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x4f\x02\x0c\x54\xcc\x9d\x10\x4b\x6f\x4f\x69\x98\xc6\x3c\xe2\xa3\x1d\x18\x88",
            "FITFI",
            18,
            "STEP.APP",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\xb0\x6f\x17\xec\xcb\x22\x15\xa5\xdb\xf0\x42\xc6\x72\x10\x1f\xc2\x0d\xaf\x55",
            "FLUX",
            8,
            "Flux",
        )
        yield (  # address, symbol, decimals, name
            b"\xd2\x4c\x2a\xd0\x96\x40\x0b\x6f\xbc\xd2\xad\x8b\x24\xe7\xac\xbc\x21\xa1\xda\x64",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x4d\xb1\x07\x65\x4f\xf9\x87\xad\x85\x9f\x34\x12\x53\x07\x78\x3f\xc8\xe3\x87",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\xd6\x7d\xe0\xe0\xa0\xfd\x7b\x15\xdc\x83\x48\xbb\x9b\xe7\x42\xf3\xc5\x85\x04\x54",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\x8e\xbf\xb8\xe1\x46\x0a\xaa\xc7\xc2\x72\xcb\x90\x53\xb3\xd4\x24\x12\xaa\xc2",
            "GAU",
            18,
            "GAU Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x5e\xaf\x29\x06\xfa\x1a\x3a\x13\xed\xac\x9b\x96\x57\x10\x8a\xf7\xb7\x03\xe3",
            "ggAVAX",
            18,
            "GoGoPool Liquid Staking Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x26\x0b\x94\x83\xf9\x87\x1c\xa5\x7f\x81\xa9\x0d\x91\xe2\xf9\x6c\x2c\xd1\x1d",
            "GGP",
            18,
            "GoGoPool Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\x62\xed\xc0\x69\x2b\xd8\x97\xd2\x29\x58\x72\xa9\xff\xca\xc5\x42\x50\x11\xc6\x61",
            "GMX",
            18,
            "GMX",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x0c\xac\x13\xc7\xda\x96\x5a\x31\x2f\x08\xea\x42\x29\xc3\x78\x69\xe8\x5c\xb9",
            "GRT.e",
            18,
            "Graph Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x26\xde\xbd\x39\xd5\xed\x06\x97\x70\x40\x6f\xca\x10\xa0\xe4\xf8\xd2\xc7\x43\xeb",
            "GUN",
            18,
            "GUNZ",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\xaf\x13\x22\x7d\xcb\xfa\x2d\xc2\xb1\x92\x8a\xcf\xca\x03\xb8\x5e\x2d\x25\xdd",
            "IDIA",
            18,
            "Impossible Decentralized Incubat",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\x84\xa6\x21\x6e\xa6\xda\xcc\x71\xee\x8e\x6b\x0a\x5b\x73\x22\xee\xbc\x0f\xdd",
            "JOE",
            18,
            "JoeToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xc6\x54\x72\x1f\xbf\x1f\x37\x4f\xd9\xff\xa3\x38\x5b\xba\x2f\x49\x32\xa6\xaf\x55",
            "JUICY",
            18,
            "Juicy",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\xff\x00\x3a\x6b\xad\x9b\x74\x3d\x65\x80\x48\x74\x29\x35\xff\xfe\x2b\x6e\xd7",
            "KET",
            18,
            "yellow ket",
        )
        yield (  # address, symbol, decimals, name
            b"\x39\xfc\x9e\x94\xca\xea\xcb\x43\x58\x42\xfa\xde\xde\xcb\x78\x35\x89\xf5\x0f\x5f",
            "KNC",
            18,
            "Kyber Network Crystal v2",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\xf2\xbd\xf0\x0a\xcd\x06\x7c\x65\xa7\x9a\x0b\x6a\x39\x14\xc5\x55\x19\x6e\xa4",
            "KULA",
            18,
            "Kula",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\x43\xff\x77\xa9\xc0\xcf\x55\x2b\x5b\x65\x32\x68\xfb\xfe\x26\xa0\x52\x42\x9b",
            "LAMBO",
            18,
            "LAMBO",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x47\xbb\x27\x5c\x52\x10\x40\x05\x1d\x82\x39\x61\x92\x18\x1b\x41\x32\x27\xa3",
            "LINK.e",
            18,
            "Chainlink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x70\x92\x8e\x5b\x18\x8d\xef\x72\x81\x7b\x77\x75\xf0\xbf\x63\x25\x96\x8e\x56\x3b",
            "LUNA",
            6,
            "LUNA (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\x12\x8f\xd4\xb2\x59\x55\x2a\x9a\x1d\x45\x7f\x43\x5a\x65\x27\xaa\xb7\x2d\x42",
            "MKR.e",
            18,
            "Maker",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\x8a\x21\xdb\xca\xb4\x32\xfb\x5e\x46\x9d\x80\xf9\x76\xe0\x22\xc2\xf5\x6e\xa0",
            "MMUI",
            6,
            "MetaMUI Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\xc3\x4c\x53\xa0\x4b\x9a\xaa\x0b\xf0\x47\xe7\x29\x1f\xb4\xe8\xa4\x8f\x2a\x18",
            "NAI",
            18,
            "Nuklai Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xac\xfb\x89\x8c\xff\x26\x6e\x53\x27\x8c\xc0\x12\x4f\xc2\xc7\xc9\x4c\x8c\xb9\xa5",
            "NOCHILL",
            18,
            "AVAX HAS NO CHILL",
        )
        yield (  # address, symbol, decimals, name
            b"\x5e\x0e\x90\xe2\x68\xbc\x24\x7c\xc8\x50\xc7\x89\xa0\xdb\x0d\x5c\x76\x21\xfb\x59",
            "NXPC",
            18,
            "NXPC",
        )
        yield (  # address, symbol, decimals, name
            b"\x96\xe1\x05\x6a\x88\x14\xde\x39\xc8\xc3\xcd\x01\x76\x04\x2d\x6c\xec\xd8\x07\xd7",
            "OSAK",
            18,
            "Osaka Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x0d\xda\x87\x62\xc2\x4d\xa9\x48\x7f\x5f\xa0\x26\xa9\xb6\x4b\x69\x5a\x07\xea",
            "OX",
            18,
            "OX Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\xab\x9d\x12\xa3\x05\x04\x55\x9b\x0c\x5a\x9a\x59\x77\xfe\xe4\xa6\x08\x1c\x6b",
            "PHAR",
            18,
            "PHARAOH",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\x78\x1c\x25\x86\xd6\x82\x29\xfd\xe4\x75\x64\x54\x67\x84\xab\x3f\xac\xa9\x82",
            "PNG",
            18,
            "Pangolin",
        )
        yield (  # address, symbol, decimals, name
            b"\x87\x29\x43\x8e\xb1\x5e\x2c\x8b\x57\x6f\xcc\x6a\xec\xda\x6a\x14\x87\x76\xc0\xf5",
            "QI",
            18,
            "BENQI",
        )
        yield (  # address, symbol, decimals, name
            b"\x2b\x2c\x81\xe0\x8f\x1a\xf8\x83\x5a\x78\xbb\x2a\x90\xae\x92\x4a\xce\x0e\xa4\xbe",
            "sAVAX",
            18,
            "Staked AVAX",
        )
        yield (  # address, symbol, decimals, name
            b"\x06\xd4\x7f\x3f\xb3\x76\x64\x9c\x3a\x9d\xaf\xe0\x69\xb3\xd6\xe3\x55\x72\x21\x9e",
            "savUSD",
            18,
            "Staked avUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\x12\x32\xb9\x0d\x3f\x80\x9a\x54\xee\xae\xeb\xc6\x39\xc6\x2d\xf8\xa8\x94\x2f",
            "SB",
            9,
            "Snowbank",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x02\x29\x8a\x79\x39\x48\x69\x8b\x9a\x63\x31\x14\x04\xfb\xbe\xe9\x44\xea\xfd",
            "SHRAP",
            18,
            "SHRAPToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xbe\xc2\x43\xc9\x95\x40\x9e\x65\x20\xd7\xc4\x1e\x40\x4d\xa5\xde\xba\x4b\x20\x9b",
            "SNX.e",
            18,
            "Synthetix Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xfe\x6b\x19\x28\x68\x85\xa4\xf7\xf5\x5a\xda\xd0\x9c\x3c\xd1\xf9\x06\xd2\x47\x8f",
            "SOL",
            9,
            "Wrapped SOL (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\xbc\x78\xd8\x4b\xa0\xc4\x6d\xfe\x32\xcf\x28\x95\xa1\x99\x39\xc8\x6b\x81\xa7\x77",
            "SolvBTC",
            18,
            "Solv BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xce\x1b\xff\xbd\x53\x74\xda\xc8\x6a\x28\x93\x11\x96\x83\xf4\x91\x1a\x2f\x78\x14",
            "SPELL",
            18,
            "Spell Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x6f\x07\xcd\xcf\x35\x88\x94\x4b\xf4\xc4\x2a\xc7\x4f\xf2\x4b\xf5\x6e\x75\x90",
            "STG",
            18,
            "StargateToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\xfa\x58\x22\x8b\xb7\x91\xea\x35\x5c\x90\xda\x1e\x47\x83\x45\x2b\x9b\xd8\xc3",
            "SUPER",
            18,
            "SuperVerse",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\xc1\x74\x16\x92\x57\x89\xe0\x85\x2f\xbf\xcd\x81\xc4\x90\xca\x4a\xbc\x51\xf9",
            "SURE",
            18,
            "xDollar Interverse Money",
        )
        yield (  # address, symbol, decimals, name
            b"\x39\xcf\x1b\xd5\xf1\x5f\xb2\x2e\xc3\xd9\xff\x86\xb0\x72\x7a\xfc\x20\x34\x27\xcc",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x37\xb6\x08\x51\x9f\x91\xf7\x0f\x2e\xeb\x0e\x5e\xd9\xaf\x40\x61\x72\x2e\x4f\x76",
            "SUSHI.e",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\xb5\xd7\x2c\x83\x6e\x71\x8c\xda\x88\x88\xea\xf0\x37\x07\xfa\xef\x67\x50\x79",
            "SWAP.e",
            18,
            "TrustSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1e\x4c\x0e\x06\x0f\xba\x7d\x62\xfa\x9f\xbb\x1a\xa6\x24\xe5\x8f\x79\x6b\x4e\xfe",
            "SWEAT",
            18,
            "SWEAT",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x1e\x7c\x89\x38\x55\x52\x5b\x30\x3f\x99\xbd\xf5\xc3\xc0\x5b\xe0\x9c\xa2\x51",
            "SYN",
            18,
            "Synapse",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x66\x7d\xc8\x0a\x45\xb7\x73\x05\xcc\x39\x5d\xb5\x6d\x99\x75\x97\xdc\x6d\xdd",
            "TLOS",
            18,
            "TLOS",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\x4a\x34\x1d\xf6\xc1\x26\xf9\x0c\xf3\xec\xb9\x21\x20\xfd\x71\x90\xac\xb4\x01",
            "TRYB",
            6,
            "BiLira",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\x20\xe8\x91\xba\xb6\xb1\x72\x7d\x14\xda\x35\x8f\xae\x29\x84\xed\x9b\x59\xeb",
            "TUSD",
            18,
            "TrueUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x3b\xd2\xb1\xc7\xed\x8d\x39\x6d\xbb\x98\xde\xd3\xae\xbb\x41\x35\x0a\x5b\x23\x39",
            "UMA.e",
            18,
            "UMA Voting Token v1",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\xba\xf2\x2b\x6f\x05\x3d\xff\xea\xf4\x6f\x4d\xd9\xef\xa9\x5d\x89\xba\x85\x80",
            "UNI.e",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\x7e\xf9\xef\x87\x34\xc7\x19\x04\xd8\x00\x2f\x8b\x6b\xc6\x6d\xd9\xc4\x8a\x6e",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xa7\xd7\x07\x9b\x0f\xea\xd9\x1f\x3e\x65\xf8\x6e\x89\x15\xcb\x59\xc1\xa4\xc6\x64",
            "USDC.e",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\x14\xca\xbd\x09\xef\x5b\x16\x9e\xd3\xfe\x0f\xa8\xdb\xd5\x90\x74\x1e\x81\xc2",
            "USDD",
            18,
            "USDD",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\xd9\x35\x6e\x56\x5a\xb3\xa3\x6d\xd7\x77\x63\xfc\x0d\x87\xfe\xaf\x85\x50\x8c",
            "USDM",
            18,
            "Mountain Protocol USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\x02\x23\x0a\x8e\xa5\x36\x01\xf5\xcd\x2d\xc0\x0f\xdb\xc1\x3d\x4d\xf4\xa8\xc7",
            "USDt",
            6,
            "TetherToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x19\x84\x37\x98\x0c\x04\x1c\x80\x5a\x1e\xdc\xba\x50\xc1\xce\x5d\xb9\x51\x18",
            "USDT.e",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\x99\xc3\x59\x0f\x42\xf8\xf9\x95\xec\xfa\x0f\x85\xd2\x98\x0b\x76\x86\x2f\xc1",
            "UST",
            6,
            "UST (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x6e\xda\xc2\x63\x56\x1d\xa4\x1a\xde\x15\x5a\x99\x27\x59\x26\x0f\xaf\xb8\x7b\x43",
            "VERTAI",
            18,
            "Vertical AI",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\x1f\x66\xaa\x3c\x1e\x78\x53\x63\xf0\x87\x5a\x1b\x74\xe2\x7b\x85\xfd\x66\xc7",
            "WAVAX",
            18,
            "Wrapped AVAX",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\x2f\x7f\x22\xb1\xee\x2c\x84\x2b\xea\xff\x52\x88\x0d\x45\x73\xe9\x20\x11\x58",
            "WBNB",
            18,
            "Wrapped BNB (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x40\x8d\x4c\xd0\xad\xb7\xce\xbd\x1f\x1a\x1c\x33\xa0\xba\x20\x98\xe1\x29\x5b\xab",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\x82\xa2\x91\xf8\x3c\xa0\x7a\xf2\x21\x20\xab\xa2\x16\x32\x08\x8f\xc9\x29\x31",
            "WETH",
            18,
            "Wrapped Ether (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\xd5\xc2\xbd\xff\xac\x6c\xe2\xbf\xdb\x66\x40\xf4\xf8\x0f\x22\x6b\xc1\x0b\xab",
            "WETH.e",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x98\xa5\x31\x1d\xa1\x74\xa9\x52\x53\xce\x86\xc2\x1c\xa7\x27\x2b\x9b\x05\xf8",
            "WINK",
            18,
            "Wink",
        )
        yield (  # address, symbol, decimals, name
            b"\xf2\xf1\x3f\x0b\x70\x08\xab\x2f\xa4\xa2\x41\x8f\x4c\xcc\x36\x84\xe4\x9d\x20\xeb",
            "WMATIC",
            18,
            "Wrapped Matic (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\xab\xc9\x54\x7b\x53\x45\x19\xff\x73\x92\x1b\x1f\xba\x6e\x67\x2b\x5f\x58\xd0\x83",
            "WOO.e",
            18,
            "Wootrade Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x12\xaf\x5c\x1a\x23\x26\x75\xf6\x2f\x40\x5b\x58\x12\xa8\x0e\x7a\x6f\x75\xd7\x46",
            "wROSE",
            18,
            "Wrapped ROSE (Wormhole)",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\xde\x4a\x87\xb8\xb6\xfa\x58\x32\x6b\xb4\x62\x88\x2f\x17\x78\x15\x8b\x02\xf1",
            "WXT",
            18,
            "Wirex Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\xc3\xf9\x4d\xe7\xe5\xb4\x5f\xa4\xed\xbb\xa4\x72\x49\x1a\x9f\x4b\x16\x6f\xc4",
            "XAVA",
            18,
            "Avalaunch",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\xaa\xc1\xb2\x3d\x93\x53\x65\xbd\x7b\x54\x2f\xe2\x2c\xee\xe2\x92\x2f\x52\xdc",
            "YFI.e",
            18,
            "yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\x11\x11\x11\x11\x11\xed\x1d\x73\xf8\x60\xf5\x7b\x27\x98\xb6\x83\xf2\xd3\x25",
            "YUSD",
            18,
            "YUSD Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\x85\x88\x4c\x43\x92\xd3\x48\x58\x7b\x19\xcb\x9e\xaa\xf1\x57\xf1\x32\x71\xcd",
            "ZRO",
            18,
            "LayerZero",
        )
