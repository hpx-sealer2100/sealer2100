
class UnsupportedQRType():
    type: str

class InvalidUR():
    pass

class SingleUR():
    pass

class MultiUR():
    expected: int
    processed: int

class ScanStart():
    ur: SingleUR|MultiUR

class Scanning():
    mur: MultiUR

class Done():
    pass

