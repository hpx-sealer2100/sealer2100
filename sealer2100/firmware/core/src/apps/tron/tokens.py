# generated from tokens.py.mako
# do not edit manually!
# flake8: noqa
# fmt: off

class TokenInfo:
    def __init__(self, symbol: str, decimals: int) -> None:
        self.symbol = symbol
        self.decimals = decimals


UNKNOWN_TOKEN = TokenInfo("UNKN", 0)


def token_by_address(token_type, address) -> TokenInfo:
    if token_type == "trc10":
        if address == "1002000":
            return TokenInfo("BTTOLD", 6)
        if address == "1002361":
            return TokenInfo("DEC", 0)
        if address == "1002357":
            return TokenInfo("GMCOIN", 2)
        if address == "1003026":
            return TokenInfo("GPCX", 4)
        if address == "1003887":
            return TokenInfo("PALG", 6)
    if token_type == "trc20":
        if address == "TAoA331n3iKDkR62kAZ4H2n3vNL7y3d8x9":
            return TokenInfo("BCN", 6)
        if address == "TR95WcjR78Y4puyv8BkbFuxaf4WcvmmPpd":
            return TokenInfo("BIAO", 18)
        if address == "TF7ixydn7nfCgj9wQj3fRdKRAvsZ8egHcx":
            return TokenInfo("BICOIN", 18)
        if address == "TXtu3Ke9mJQtPtAQxmoTbzu3N4Qx4XP9Y2":
            return TokenInfo("BLAKE", 18)
        if address == "TXBcx59eDVndV5upFQnTR2xdvqFd5reXET":
            return TokenInfo("BPX", 18)
        if address == "TN3W4H6rK2ce4vX9YnFQHwKENnHjoxb3m9":
            return TokenInfo("BTCT", 8)
        if address == "TAFjULxiVgT4qWk6UZwjqwZXTSaGaqnVp4":
            return TokenInfo("BTT", 18)
        if address == "TAt4ufXFaHZAEV44ev7onThjTnF61SEaEM":
            return TokenInfo("BULL", 18)
        if address == "TMz2SWatiAtZVVcH2ebpsbVtYwUPT9EdjH":
            return TokenInfo("BUSD", 18)
        if address == "TTLVdtBYipLVqVbPaaQb2Zbcubbpddtxu7":
            return TokenInfo("CAMT", 18)
        if address == "TRv9ipj4kKAZqQggQ7ceJpe5ERD1ZShpgs":
            return TokenInfo("CCC", 18)
        if address == "TVXmroHbJsJ6rVm3wGn2G9723yz3Kbqp9x":
            return TokenInfo("CDOG", 18)
        if address == "TKg1nGjtWYDcf1HNTSNQtwGwNAuTxd1X2A":
            return TokenInfo("CMCX", 18)
        if address == "TL5BvThAMg9QBCvbgXu7HwHh8HqGdAq4DD":
            return TokenInfo("CNC", 10)
        if address == "TKRYQndqTdnU4Bg17gY39rooyK6CzM3ush":
            return TokenInfo("CNF", 18)
        if address == "TRwptGFfX3fuffAMbWDDLJZAZFmP6bGfqL":
            return TokenInfo("DCT", 18)
        if address == "TRNneCXQMeGfZ7ma4Vo8UvqHaAk9L29BAA":
            return TokenInfo("DECKO", 18)
        if address == "TV5yB8f4AdoAfVVUdkytyZnX5e7SeGAZr2":
            return TokenInfo("DRGN", 18)
        if address == "TLWH5YFXkd9UgU6gspkfy7bUEJuYmtDq5i":
            return TokenInfo("ECG", 6)
        if address == "THb4CqiFdwNHsWsQCs4JhzwjMWys4aqCbF":
            return TokenInfo("ETH", 18)
        if address == "TWr6yzukRwZ53HDe3bzcC8RCTbiKa4Zzb6":
            return TokenInfo("FLUX", 8)
        if address == "TF8EjxT89LmcUNKV3fq55oeD5V44V7tjo2":
            return TokenInfo("FMC", 8)
        if address == "TUFonyWZ4Tza5MzgDj6g2u5rfdGoRVYG7g":
            return TokenInfo("FOFAR", 18)
        if address == "TFenNvccFr9zvkh9xhQspcAxY4xxttNkWg":
            return TokenInfo("FRED", 18)
        if address == "TDXyBerEqKHrGLEQFH9S3prXHPQiqaDsTA":
            return TokenInfo("GOLC", 18)
        if address == "TEBdrpWiPVSU3cmR96wCVYgsxb9fwwJSZi":
            return TokenInfo("HAMMY", 18)
        if address == "TUPM7K8REVzD2UdV4R5fe5M8XbnR2DdoJ6":
            return TokenInfo("HTX", 18)
        if address == "TSig7sWzEL2K83mkJMQtbyPpiVSbR6pZnb":
            return TokenInfo("IVfun", 18)
        if address == "TN7zQd2oCCguSQykZ437tZzLEaGJ7EGyha":
            return TokenInfo("JFI", 18)
        if address == "TCFLL5dx5ZJdKnWuesXxi1VPwjLVmWZZy9":
            return TokenInfo("JST", 18)
        if address == "TQVkTUQPKWdr2JtTy9X8ZxYUMuwBQap3Y7":
            return TokenInfo("KIT", 18)
        if address == "TVj7RNVHy6thbM7BWdSe9G6gXwKhjhdNZS":
            return TokenInfo("KLV", 6)
        if address == "THS2ZuQowumzFPD1z3wchB1PijWMggUgmA":
            return TokenInfo("KRRX", 4)
        if address == "TC8QDMS2nLdWMT7iJwvtG5YH6XiGEJT6om":
            return TokenInfo("LLD", 18)
        if address == "TR1iXU8Sp7oUYHXWiddsBervJzSbdibMFZ":
            return TokenInfo("MEOW", 18)
        if address == "TE2T2vLnEQT1XW647EAQAHWqd6NZL1hweR":
            return TokenInfo("MUNCAT", 18)
        if address == "TEfg1LnM3yApCjAgax35wDg6SRpmZFuQS3":
            return TokenInfo("MWD", 18)
        if address == "TBBThLgHtUhGuT9DJ1rrzXDQ6j2XSP2Dq5":
            return TokenInfo("N", 18)
        if address == "TCGPc27oyS2x7S5pex7ssyZxZ2edPWonk2":
            return TokenInfo("Neiro", 18)
        if address == "TFczxzPhnThNSqr5by8tvxsdCFRRz6cPNq":
            return TokenInfo("NFT", 6)
        if address == "TQYuR8FpmhMJK3ZpfoTgERjaZRywqMnFAH":
            return TokenInfo("PAYN", 8)
        if address == "TMacq4TDUw5q8NFBwmbY4RLXvzvG5JTkvi":
            return TokenInfo("PEPEONTRON", 18)
        if address == "TAbkLGngV1k8bNb5hnqW7oX46FG4Dbs5e2":
            return TokenInfo("Pino", 18)
        if address == "TFZkrMkKawhgH2gLUhL8Q7N5sFY7bxJJyn":
            return TokenInfo("POLL", 8)
        if address == "TFf1aBoNFqxN32V2NQdvNrXVyYCy9qY8p1":
            return TokenInfo("PROS", 18)
        if address == "TX5eXdf8458bZ77fk8xdvUgiQmC3L93iv7":
            return TokenInfo("PUSS", 18)
        if address == "TCxDsTm4nHDwNmY5MUQ4GFmSsbUqHnee2W":
            return TokenInfo("PXAI", 18)
        if address == "TRgnuKcAx4pNPS4hgLMNugKKtSrAqhGXM2":
            return TokenInfo("QUBY", 18)
        if address == "TGBfBt6Y2Dm3RHdNpZAdqywBsvfdysf834":
            return TokenInfo("REAL", 6)
        if address == "TJvwMR3RjHc8jA9QwwjWGANrR3Y4scLSZm":
            return TokenInfo("ROCK", 18)
        if address == "TSjUeAj5y2p1QBX7353CKDAqHdb6gcpJhR":
            return TokenInfo("SQUID", 18)
        if address == "TBLQs7LqUYAgzYirNtaiX3ixnCKnhrVVCe":
            return TokenInfo("SST", 8)
        if address == "TU3kjFuhtEo42tsCBtfYUAZxoqQ4yuSLQ5":
            return TokenInfo("sTRX", 18)
        if address == "TQKQRxEDH2vhtxGB4obGeHwu7AqGDryU3C":
            return TokenInfo("SUGAR", 18)
        if address == "TSSMHYeV2uE9qYH95DqyoCuNCzEL1NvU3S":
            return TokenInfo("SUN", 18)
        if address == "TAwAg9wtQzTMFsijnSFotJrpxhMm3AqW1d":
            return TokenInfo("SUNCAT", 18)
        if address == "TXL6rJbvmjD46zeN1JssfgxvSo99qC8MRT":
            return TokenInfo("SUNDOG", 18)
        if address == "TBk4dXtfo6j3rxPc1VJDhA7BsVi2XHK1bi":
            return TokenInfo("SUNLION", 18)
        if address == "TNFsYemyso755pjPs19bfdEuDnchbDzqua":
            return TokenInfo("SUNNEIRO", 18)
        if address == "TAzpJHxxgJd8f5AKjzndEndH3S9pzNUM5W":
            return TokenInfo("SUNPEPE", 18)
        if address == "TX4xgxs6ctAz8RFbpk4rPzBrULWHE7Siiq":
            return TokenInfo("SUNPIG", 18)
        if address == "TMwk13CthNktLiNpo8WDRuUMpyrzZTVZYH":
            return TokenInfo("SUNPUMP", 18)
        if address == "TPP9Pq2LQwtrwfyUDLSyFsKJwJjFouf45B":
            return TokenInfo("SUNTRON", 18)
        if address == "TP3prcvQknVthrVnn281cKST56eWiLgJJM":
            return TokenInfo("SUNWUKONG", 18)
        if address == "TJUHBYfCSBLKC1fSGNprnKjppdsXm9dj6x":
            return TokenInfo("SUYA", 18)
        if address == "TPeoxx1VhUMnAUyjwWfximDYFDQaxNQQ45":
            return TokenInfo("TBULL", 18)
        if address == "TCMwzYUUCxLkTNpXjkYSBgXgqXwt7KJ82y":
            return TokenInfo("TC", 4)
        if address == "TVgHqeP41s3qMDH3oKBsScEUzvyXw6bKAm":
            return TokenInfo("Tcat", 18)
        if address == "TD6xZDStjpd5FjgpNGihSYnm46nQUA6Dsj":
            return TokenInfo("TOPCAT", 18)
        if address == "TEKWczHwjSiS75N37YSz1fgraZLuarks86":
            return TokenInfo("TronDOG", 18)
        if address == "TUpMhErZL2fhh4sVNULAbNKLokS4GjC1F4":
            return TokenInfo("TUSD", 18)
        if address == "TTFreuJ4pYDaCeEMEtiR1GQDwPPrS4jKFk":
            return TokenInfo("TWX", 18)
        if address == "TK4zHRAU1WrRPivksia3BMs2JvSLT63opb":
            return TokenInfo("UAHG", 6)
        if address == "TPFqcBAaaUMCSVRCqPaQ9QnzKhmuoLR6Rc":
            return TokenInfo("USD1", 18)
        if address == "TEkxiTehnzSmSe2XqrBj4w32RUN966rdz8":
            return TokenInfo("USDC", 6)
        if address == "TPYmHEhy5n8TCEfYGqW2rPxsghSfzghPDn":
            return TokenInfo("USDD", 18)
        if address == "TXDk8mbtRbXeYuMNS83CfKPaYYT8XWv9Hz":
            return TokenInfo("USDD", 18)
        if address == "TMwFHYXLJaRUPeW6421aqXL4ZEzPRFGkGT":
            return TokenInfo("USDJ", 18)
        if address == "TR7NHqjeKQxGTCi8q8ZY4pL8otSzgjLj6t":
            return TokenInfo("USDT", 6)
        if address == "TE9f8MGouEdh2jPqDaUgUVaJYLKkV6vhv6":
            return TokenInfo("USHARK", 18)
        if address == "TYX2iy3i3793YgKU5vqKxDnLpiBMSa5EdV":
            return TokenInfo("USTX", 6)
        if address == "TP7r1pDoS1snMjEJE1kE17GRt3Df4mYuZz":
            return TokenInfo("VIKITA", 18)
        if address == "TFptbWaARrWTX5Yvy3gNG5Lm8BmhPx82Bt":
            return TokenInfo("WBT", 8)
        if address == "TLa2f6VPqDgRE67v1736s7bJ8Ray5wYjU7":
            return TokenInfo("WIN", 6)
        if address == "TGkxzkDKyMeq2T7edKnyjZoFypyzjkkssq":
            return TokenInfo("WSTUSDT", 18)
        if address == "TNUC9Qb1rRpS5CbWLmNMxXBjyFoydXjWFR":
            return TokenInfo("WTRX", 6)
        if address == "TQzUXP5eXHwrRMou9KYQQq7wEmu8KQF7mX":
            return TokenInfo("XLG", 18)
        if address == "TBAbgFHHpuLERRcUFvjnojtBjkBr39NFB7":
            return TokenInfo("YSU", 18)
        if address == "TPUZseUsV89tM157aLfkuKYEaRKzBirmRb":
            return TokenInfo("ZAPO", 18)
    return UNKNOWN_TOKEN
