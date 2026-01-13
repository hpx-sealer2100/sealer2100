from typing import TYPE_CHECKING

from trezor import loop, ui, wire

if TYPE_CHECKING:
    from typing import Callable, Any, Awaitable, TypeVar

    T = TypeVar("T")

CONFIRMED = object()
CANCELLED = object()
INFO = object()
GO_BACK = object()
SHOW_PAGINATED = object()


def is_confirmed(x: Any) -> bool:
    return x is CONFIRMED


async def raise_if_cancelled(a: Awaitable[T], exc: Any = wire.ActionCancelled) -> T:
    result = await a
    if result is CANCELLED:
        raise exc
    return result


async def is_confirmed_info(
    ctx: wire.GenericContext,
    dialog: ui.Layout,
    info_func: Callable,
) -> bool:
    while True:
        result = await ctx.wait(dialog)

        if result is INFO:
            await info_func(ctx)
        else:
            return is_confirmed(result)
