import  ur_parser
from trezor import log

origin= "HyperMate"

class ViewType():
    BtcNativeSegwitTx = 0
    BtcSegwitTx = 1
    BtcLegacyTx = 2
    BtcTx = 3
    BtcMsg = 4
    LtcTx = 5
    DogeTx = 6
    DashTx = 7
    BchTx = 8
    EthTx = 9
    EthBatchTx = 10
    EthPersonalMessage = 11
    EthTypedData = 12
    TronTx = 13
    SolanaTx = 14
    SolanaMessage = 15
    CardanoTx = 16
    CardanoSignData = 17
    CardanoSignCip8Data = 18
    CardanoCatalystVotingRegistration = 19
    CardanoSignTxHash = 20
    XRPTx = 21
    CosmosTx = 22
    CosmosEvmTx = 23
    SuiTx = 24
    SuiSignMessageHash = 25
    IotaTx = 26
    IotaSignMessageHash = 27
    ArweaveTx = 28
    ArweaveMessage = 29
    ArweaveDataItem = 30
    StellarTx = 31
    StellarHash = 32
    TonTx = 33
    TonSignProof = 34
    ZcashTx = 35
    AptosTx = 36
    AvaxTx = 37
    WebAuthResult = 38
    KeyDerivationRequest = 39
    BatchCall = 40
    ViewTypeUnKnown = 41


class QRCodeType():
    CryptoPSBT = 0
    CryptoPSBTExtend = 1
    CryptoMultiAccounts = 2
    CryptoAccount = 3
    Bytes = 4
    BtcSignRequest = 5
    SeedSignerMessage = 6
    KeystoneSignRequest = 7
    EthSignRequest = 8
    EthBatchSignRequest = 9
    SolSignRequest = 10
    CardanoSignRequest = 11
    CardanoSignTxHashRequest = 12
    CardanoSignDataRequest = 13
    CardanoCatalystVotingRegistrationRequest = 14
    CardanoSignCip8DataRequest = 15
    CosmosSignRequest = 16
    EvmSignRequest = 17
    SuiSignRequest = 18
    SuiSignHashRequest = 19
    IotaSignRequest = 20
    IotaSignHashRequest = 21
    AptosSignRequest = 22
    QRHardwareCall = 23
    ArweaveSignRequest = 24
    StellarSignRequest = 25
    TonSignRequest = 26
    AvaxSignRequest = 27
    ZcashPczt = 28
    HpxAppCallDevice = 29
    URTypeUnKnown = 30


class RustDecodedUR:


    def __init__(self) -> None:
        self.urParseResult: ur_parser.URParseResult|None = None
        self.urParseMultiResult: ur_parser.URParseMultiResult|None = None
        self.is_multi_part = False
        self.is_complete = False
        self.is_first_frame = True
        self.is_start_decoding = False
        self.process = 0

    def __del__(self):
        if self.urParseResult:
            self.urParseResult.close()
        if self.urParseMultiResult:
            self.urParseMultiResult.close()


    def decoding(self, data: str) -> bool:
        try:
            log.debug(__name__, f"rust_ur is_first_frame: {self.is_first_frame}")
            log.debug(__name__, f"rust_ur is_multi_part: {self.is_multi_part}")
            log.debug(__name__, f"rust_ur is_complete: {self.is_complete}")
            if self.is_first_frame:
                self.urParseResult = ur_parser.parse_ur(data)
                if self.urParseResult.error_code() ==0:
                    self.is_start_decoding = True
                    if self.urParseResult.is_multi_part() is not True:
                        self.is_multi_part = False
                        self.is_complete = True
                        self.is_first_frame = True
                        self.process = 100
                        return True
                    else:
                        self.is_first_frame = False
                        self.is_multi_part = True
                        self.is_complete = False
                        self.process = self.urParseResult.progress()
                        return True
                return False
            else:
                self.urParseMultiResult = ur_parser.receive( data,self.urParseResult.decoder())
                if self.urParseMultiResult.error_code() == 0:
                    self.is_start_decoding = False
                    self.process = self.urParseMultiResult.progress()
                    self.is_multi_part = True
                    self.is_complete = self.urParseMultiResult.is_complete()
                    self.is_first_frame = False
                    return True
                return False
        except Exception as e:
            log.debug(__name__, f"rust_ur decoding error: {e}")
            return False

    def data(self):
        if self.is_multi_part:
            return self.urParseMultiResult.data()
        else:
            return self.urParseResult.data()
    def ur_type(self):
        if self.is_multi_part:
            return self.urParseMultiResult.ur_type()
        else:
            return self.urParseResult.ur_type()

    def view_type(self):
        if self.is_multi_part:
            return self.urParseMultiResult.t()
        else:
            return self.urParseResult.t()

class RustEncodedUR:
    def __init__(self) -> None:
        self.urEncodeResult = None
    def __del__(self):
        if self.urEncodeResult:
            self.urEncodeResult.close()

    def encoding(self, encoded_ur:ur_parser.UREncodeResult) -> None:
        self.urEncodeResult = encoded_ur

    def is_multi_part(self) -> bool:
        if self.urEncodeResult is None:
            return False
        return self.urEncodeResult.is_multi_part()

    def get_next_data(self) -> str:
        if not self.is_multi_part():
            return None
        else:
            urEncodeMultiResult = ur_parser.get_next_part(self.urEncodeResult.encoder())
            if urEncodeMultiResult.error_code() == 0:
                data = urEncodeMultiResult.data()
                urEncodeMultiResult.close()
                return data
            else:
                return None
    def data(self) -> str:
        if self.urEncodeResult is None:
            return None
        return self.urEncodeResult.data()
