from ubinascii import hexlify

from trezor.crypto.hashlib import sha3_256

TRANSACTION_PREFIX = b"APTOS::RawTransaction"
