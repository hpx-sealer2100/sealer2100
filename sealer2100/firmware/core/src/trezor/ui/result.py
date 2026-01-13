from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Generic, TypeVar

    T = TypeVar("T")
else:
    # typechecker cheat: Generic[T] will be `object` which is a valid parent type
    Generic = [object]
    T = 0
class Result(Generic[T]):
    def __init__(self, value: T):
        self.value = value

class NavigationBack:
    """"
    Navigation screen navigate back
    """
    def __bool__(self):
        return False

class Redo:
    """
    Some operation need redo
    """

class AutoClose:
    """"
    Modeled screen auto close
    """

    def __bool__(self):
        return False

class Done:
    def __bool__(self):
        return True

class Confirm:
    def __bool__(self):
        return True

class Reject:
    def __bool__(self):
        return False

class Cancel:
    def __bool__(self):
        return False

class Continue:
    def __bool__(self):
        return True

class More:
    def __bool__(self):
        return True

class Detail:
    def __bool__(self):
        return True
