from trezor import wire
from trezor.messages import EthereumStoreDefinition, Success
from . import helpers

async def store_definition(ctx: wire.Context, msg: EthereumStoreDefinition) -> Success:
    if not helpers.store_definition(msg.network, msg.token):
        raise wire.DataError("Invalid definition")

    return Success(message="Definition stored")
