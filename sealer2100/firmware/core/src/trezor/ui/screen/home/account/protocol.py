from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import List, Protocol, Tuple
    from typing import Callable
    from trezor.ui.screen import Navigation

    class CoinProtocol(Protocol):
        def get_name() -> str:
            ...

        def get_family_icon() -> str:
            ...

        def get_icon() -> str:
            ...

        def get_path() -> str:
            ...

        async def get_address() -> str:
            ...

    class CoinDetailViewProtocol(Protocol, Navigation):
        coin: CoinProtocol
        pass

    CoinConsturctor = Callable[[], CoinProtocol]
    CoinDetailViewConstructor = Callable[[CoinProtocol], CoinDetailViewProtocol]