import lvgl as lv

from trezor.ui import Style
from trezor.ui.theme import Styles

from trezor.ui.types import *

if TYPE_CHECKING:
    Direction = Literal['VStack', 'HStack', 'Column', 'Row']
    Alignment = Literal['']

class Container(lv.obj):
    def __init__(self, parent, direction: Direction):
        super().__init__(parent)
        self.remove_style_all()

        # the property set by set_style_xxx function can't cover by add_style
        # as container, not a visible object, no border
        self.add_style(Styles.container, lv.PART.MAIN)
        
        # as a container, we don't process events
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.set_direction(direction)

    def set_direction(self, direction: Direction):
        self.direction: Direction = direction
        if self.direction in ('Column', 'VStack'):
            self.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        elif self.direction in ('Row', 'HStack'):
            self.set_flex_flow(lv.FLEX_FLOW.ROW)

    def spacing(self, v: int):
        self.set_style_pad_gap(v, lv.PART.MAIN)

    def reverse(self):
        if self.direction in ('Column', 'VStack'):
            self.set_flex_flow(lv.FLEX_FLOW.COLUMN_REVERSE)
        elif self.direction in ('Row', 'HStack'):
            self.set_flex_flow(lv.FLEX_FLOW.ROW_REVERSE)

    def items_center(self):
        self.set_flex_align(
            lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER
        )

    def add(self, cls: Type[Widget]) -> Widget:
        return cls(self)

class VStack(Container):
    """VStack container layout it's children in vertical direction
    item0
    item1
    ...
    itemN
    """

    def __init__(self, parent) -> None:
        super().__init__(parent, 'VStack')

class HStack(Container):
    """
    HStack container layout it's children in horizontal direction
    item0 item1 ... itemN
    """

    def __init__(self, parent) -> None:
        super().__init__(parent, 'HStack')

# type alise
Column = VStack
Row = HStack