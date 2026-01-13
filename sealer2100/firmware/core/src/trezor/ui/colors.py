import lvgl as lv

#### Standard Colors
class STD:
    WHITE = lv.color_hex(0xFFFFFF)
    SILVER = lv.color_hex(0xC0C0C0)
    GRAY = lv.color_hex(0x808080)
    BLACK = lv.color_hex(0x000000)
    RED = lv.color_hex(0xFF0000)
    MAROON = lv.color_hex(0x800000)
    YELLOW = lv.color_hex(0xFFFF00)
    OLIVE = lv.color_hex(0x808000)
    LIME = lv.color_hex(0x00FF00)
    GREEN = lv.color_hex(0x008000)
    CYAN = lv.color_hex(0x00FFFF)
    AQUA = CYAN
    TEAL = lv.color_hex(0x008080)
    BLUE = lv.color_hex(0x0000FF)
    NAVY = lv.color_hex(0x000080)
    MAGENTA = lv.color_hex(0xFF00FF)
    PURPLE = lv.color_hex(0x800080)
    ORANGE = lv.color_hex(0xFFA500)

#### user defined colors
class USER:

    DEEP_TAUPE = lv.color_hex(0x494A45)
    TAUPE = lv.color_hex(0x959490)

    SILVER = lv.color_hex(0xC4C4C4)

    BLACK = lv.color_hex(0x1D1D1D)
    WHITE = lv.color_hex(0xFFFFFF)
    
    GRAY  = lv.color_hex(0xF5F5F5)
    LIGHT_GRAY = lv.color_hex(0xBFBFBF)
    DARK_GRAY = lv.color_hex(0x282828)

    RED = lv.color_hex(0xFF4D4F)
    DARK_RED = lv.color_hex(0x4E181A)

    GREEN = lv.color_hex(0x00FE33)
    DARK_GREEN = lv.color_hex(0x0B3D15)
    BLUE = lv.color_hex(0x003DF4)
    YELLOW = lv.color_hex(0xFED500)

    PURPLE = lv.color_hex(0x5234EA)
    ORANGE = lv.color_hex(0xF7EBE3)

    SHADOW = lv.color_hex(0x092A61)
    SHADOW_OPA = lv.OPA._20

    PRIMARY = DARK_GREEN
    SECONDARY = WHITE

    BORDER = lv.color_hex(0xB5B5B5)

    SUCCESS = GREEN
    WARNING = YELLOW
    ERROR = RED

    PLEASURE = GREEN
    PAIN = RED

    DANGER = RED
    CRITICAL = RED

    TEXT_DEFAULT = BLACK
    TEXT_DISABLED = GRAY

    BUTTON_DEFAULT = PRIMARY # primary
    BUTTON_TEXT = WHITE

    PRESSED_FILTER_COLOR = STD.BLACK
    PRESSED_FILTER_OPA = lv.OPA._80

    CHECKED_FILTER_COLOR = BLUE
    CHECKED_FILTER_OPA = lv.OPA._30

    DISABLED_FILTER_COLOR = BLACK
    DISABLED_FILTER_OPA = lv.OPA._60

    KEYBOARD_BUTTON = WHITE
    KEYBOARD_PRESSED = PRIMARY
    KEYBOARD_PRESSED_TEXT_COLOR = GREEN

    SUBTITLE = LIGHT_GRAY
    SWITCH = lv.color_hex(0xACACAC)

    QRCODE_BG = BLACK
    QRCODE_FG = WHITE