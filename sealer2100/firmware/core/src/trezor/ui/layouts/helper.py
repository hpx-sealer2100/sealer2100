from trezor import log, wire, workflow, loop
from trezor.enums import ButtonRequestType
from trezor.messages import ButtonAck, ButtonRequest

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import Any, Awaitable, TypeVar
    from trezor.ui.screen import Screen

    T = TypeVar("T")
    LayoutType = Awaitable[Any]

# If multiple contexts need to be supported simultaneously, such as allowing USB, Bluetooth, or on-screen operations,
# then it makes sense to keep the ctx parameter in the functions.
# If not, you might consider removing the ctx parameter and using a global ctx instead.

async def button_request(
    ctx: wire.GenericContext,
    code: ButtonRequestType = ButtonRequestType.Other,
) -> None:
    if not isinstance(ctx, wire.DummyContext):
        workflow.close_others()
    await ctx.call(ButtonRequest(code=code), ButtonAck)


async def raise_if_cancelled(a: Awaitable[T], exc: Any = wire.ActionCancelled) -> T:
    result = await a
    if not result:
        raise exc
    return result


async def interact(
    ctx: wire.GenericContext,
    screen: Screen,
    br_code: ButtonRequestType = ButtonRequestType.Other,
) -> Any:
    await button_request(ctx, br_code)
    return await ctx.wait(screen)

# user create many template function, such as
# async confirm_do_something(ctx, ...):
#     from trezor.ui.screen.xxx import XXXX
#     screen = XXXX(...)
#     await screen.show()
#     await raise_if_cancelled(interact(ctx, screen))
# we can create a decorator make code clean
