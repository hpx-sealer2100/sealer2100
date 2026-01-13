from typing import *


# extmod/modthd89/modthd89.c
def get_version() -> str:
    """
    Read SE version
    """


# extmod/modthd89/modthd89.c
def get_public_key(
) -> bytes:
    """
    Read device public key.
    """


# extmod/modthd89/modthd89.c
def read_certificate(
) -> bytes:
    """
    Read device certificate.
    """


# extmod/modthd89/modthd89.c
def sign_message(msg: bytes) -> bytes:
    """
    Sign message.
    """


# extmod/modthd89/modthd89.c
def protect_user_storage() -> None:
    """
    make SE user storage protected.
    """
