
class UnsupportedQRType():
    type: str

class InvalidUR():
    pass

class SingleUR():
    pass

class MultiUR():
    def __init__(self, expected: int, processed: int):
        self.expected = expected
        self.processed = processed

class ScanStart():
    def __init__(self, ur: SingleUR|MultiUR):
        self.ur = ur

class Scanning():
    def __init__(self, mur: MultiUR):
        self.mur = mur

class Done():
    pass

