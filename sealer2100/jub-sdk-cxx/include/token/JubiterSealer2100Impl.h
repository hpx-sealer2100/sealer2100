#ifndef __JubiterSealer2100Impl__
#define __JubiterSealer2100Impl__
#ifndef HID_MODE
//JubiterSealer2100Impl not support USB

#include "JUB_SDK_DEV_BLE.h"
#include <memory>

#include "utility/util.h"
#include "device/JubiterBLENusDevice.hpp"
#include "token/JubiterBLDImpl.h"
#include "common/protocpp/messages-common.pb.h"
#include "common/protocpp/messages-management.pb.h"
#include "common/protocpp/FirmwareUpdate.pb.h"

using namespace hw::trezor::messages::common;
using namespace hw::trezor::messages::management;
using namespace hw::trezor::messages::FirmwareUpdate;
namespace jub {

////////////////////////////////////////////////////////////////////////////////////////////////
struct Address {
    std::vector<uint32_t> path;
    Address(const std::vector<uint32_t>& p) : path(p) {}
};

Address parse_path(const std::string& nstr);
////////////////////////////////////////////////////////////////////////////////////////////////

class JubiterSealer2100Impl : public JubiterBLDImpl
{
public:
    JubiterSealer2100Impl(jub::JubiterBLEDevice* device);
    ~JubiterSealer2100Impl();

    std::string bin_to_hex(const uint8_t* data, size_t len);
    std::string bytes_to_hex_fast(const std::string& input);

public:
    /* functions */
    static JUB_ENUM_BOOL ConfirmType(JUB_BYTE_PTR devName,
                                     JUB_BYTE_PTR bBLEUUID
                                     ) {

        JUB_ENUM_BOOL b = JUB_ENUM_BOOL::BOOL_FALSE;

//        if (   0 == std::string((char*)devName).find(FAKE_BLE_NAME_MAGIC)
//            && 0 == std::string((char*)bBLEUUID).find(FAKE_BLE_UUID_MAGIC)
//        ) {
//            b = JUB_ENUM_BOOL::BOOL_TRUE;
//        }
       
        if (0 == std::string((char*)devName).find(FAKE_BLE_NAME_MAGIC) || 0 == std::string((char*)devName).find(FAKE_BLE_NAME_MAGIC_1) || 0 == std::string((char*)devName).find(FAKE_BLE_NAME_MAGIC_2) || 0 == std::string((char*)devName).find(FAKE_BLE_NAME_MAGIC_3))
        {
            b = JUB_ENUM_BOOL::BOOL_TRUE;
        }
        
        return b;
    }

    // virtual DeviceTypeBase* getDevice() override;

    // virtual JUB_RV ConnectToken() override;
    // virtual JUB_RV DisconnectToken() override;
    
    // virtual JUB_RV SetERC20Token(const std::string& tokenName,
    //                              const JUB_UINT16 unitDP,
    //                              const std::string& contractAddress) override;
    // virtual JUB_RV SetERC20Tokens(const ERC20_TOKEN_INFO tokens[],
    //                               const JUB_UINT16 iCount) override;

	// //HC functions
	// virtual JUB_RV selectApplet_HC() override;

    // //BTC functions
    virtual JUB_RV SelectAppletBTC() override {
        return Init_Device();
    }
//    virtual JUB_RV GetHDNodeBTC(const JUB_BTC_TRANS_TYPE& type, const std::string& path, std::string& xpub) override;
//    virtual JUB_RV GetAddressBTC(const JUB_BYTE addrFmt, const JUB_BTC_TRANS_TYPE& type, const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    JUB_RV GetHDNodeBTC_2100(const JUB_ENUM_COINTYPE_BTC coinType,const JUB_BTC_TRANS_TYPE& type, const std::string& path, std::string& xpub);
    JUB_RV GetAddressBTC_2100(const JUB_ENUM_COINTYPE_BTC coinType,
                              const JUB_BYTE addrFmt,
                              const JUB_BTC_TRANS_TYPE& type,
                              const std::string& path,
                              const JUB_UINT16 tag,
                              std::string& address);
    // virtual JUB_RV GetAddressMultisigBTC(const JUB_BYTE addrFmt,
    //                                      const JUB_BTC_TRANS_TYPE& type,
    //                                      const std::string& path,
    //                                      const JUB_UINT16 tag,
    //                                      const uchar_vector& vRedeemScriptTlv,
    //                                      std::string& address) override;
    virtual JUB_RV SetUnitBTC(const JUB_BTC_UNIT_TYPE& unit) override;
    virtual JUB_RV SetCoinTypeBTC(const JUB_ENUM_COINTYPE_BTC& type) override;
    JUB_RV SignTXBTC_2100(const JUB_ENUM_COINTYPE_BTC coinType,
                          const std::string mainPath,
                          const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt,
                          const std::vector<INPUT_BTC>& vInputs,
                          const std::vector<OUTPUT_BTC>& vOutputs,
                          const JUB_UINT32 lockTime,
                          std::vector<JUB_BYTE>& vRaw);
     virtual JUB_RV SignTXBTC(const JUB_BYTE addrFmt,
                              const JUB_BTC_TRANS_TYPE& type,
                              const JUB_UINT16 inputCount,
                              const std::vector<JUB_UINT64>& vInputAmount,
                              const std::vector<std::string>& vInputPath,
                              const std::vector<JUB_UINT16>& vChangeIndex,
                              const std::vector<std::string>& vChangePath,
                              const std::vector<JUB_BYTE>& vUnsigedTrans,
                              std::vector<JUB_BYTE>& vRaw) override;
    // virtual JUB_RV SignTXMultisigBTC(const JUB_BYTE addrFmt,
    //                                  const JUB_BTC_TRANS_TYPE& type,
    //                                  const JUB_UINT16 inputCount,
    //                                  const std::vector<JUB_UINT64>& vInputAmount,
    //                                  const std::vector<std::string>& vInputPath,
    //                                  const std::vector<uchar_vector>& vRedeemScriptTlv,
    //                                  const std::vector<JUB_UINT16>& vChangeIndex,
    //                                  const std::vector<std::string>& vChangePath,
    //                                  const std::vector<uchar_vector>& vChangeRedeemScriptTlv,
    //                                  const std::vector<JUB_BYTE>& vUnsigedTrans,
    //                                  std::vector<uchar_vector>& vSignatureRaw) override;

    // //ETH functions
    virtual JUB_RV SelectAppletETH() override {
        return Init_Device();
    }
    virtual JUB_RV GetAppletVersionETH(stVersion& version) override;

    virtual JUB_RV GetAddressETH(const std::string& path, const JUB_UINT16 tag, std::string& address) override;

    virtual JUB_RV GetHDNodeETH(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;

    virtual JUB_RV SignTXETH(const int erc,
                             const std::vector<JUB_BYTE>& vNonce,
                             const std::vector<JUB_BYTE>& vGasPrice,
                             const std::vector<JUB_BYTE>& vGasLimit,
                             const std::vector<JUB_BYTE>& vTo,
                             const std::vector<JUB_BYTE>& vValue,
                             const std::vector<JUB_BYTE>& vData,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainID,
                             std::vector<JUB_BYTE>& vRaw) override;
    // virtual JUB_RV _SignTXETH(const int erc,
    //                           const std::vector<JUB_BYTE>& vNonce,
    //                           const std::vector<JUB_BYTE>& vGasPrice,
    //                           const std::vector<JUB_BYTE>& vGasLimit,
    //                           const std::vector<JUB_BYTE>& vTo,
    //                           const std::vector<JUB_BYTE>& vValue,
    //                           const std::vector<JUB_BYTE>& vData,
    //                           const std::vector<JUB_BYTE>& vPath,
    //                           const std::vector<JUB_BYTE>& vChainID,
    //                           std::vector<JUB_BYTE>& vRaw);
    // virtual JUB_RV _SignTXUpgradeETH(const int erc,
    //                                  const std::vector<JUB_BYTE>& vNonce,
    //                                  const std::vector<JUB_BYTE>& vGasPrice,
    //                                  const std::vector<JUB_BYTE>& vGasLimit,
    //                                  const std::vector<JUB_BYTE>& vTo,
    //                                  const std::vector<JUB_BYTE>& vValue,
    //                                  const std::vector<JUB_BYTE>& vData,
    //                                  const std::vector<JUB_BYTE>& vPath,
    //                                  const std::vector<JUB_BYTE>& vChainID,
    //                                  std::vector<JUB_BYTE>& vRaw);
                                     
     virtual JUB_RV SetERC20ETHToken(const std::string& tokenName,
                                     const JUB_UINT16 unitDP,
                                     const std::string& contractAddress) override;
     virtual JUB_RV SetERC20ETHTokens(const ERC20_TOKEN_INFO tokens[],
                                      const JUB_UINT16 iCount) override;

    virtual JUB_RV SignContractETH(const JUB_BYTE inputType,
                                    const std::vector<JUB_BYTE>& vNonce,
                                    const std::vector<JUB_BYTE>& vGasPrice,
                                    const std::vector<JUB_BYTE>& vGasLimit,
                                    const std::vector<JUB_BYTE>& vTo,
                                    const std::vector<JUB_BYTE>& vValue,
                                    const std::vector<JUB_BYTE>& vInput,
                                    const std::vector<JUB_BYTE>& vPath,
                                    const std::vector<JUB_BYTE>& vChainID,
                                    std::vector<JUB_BYTE>& signatureRaw) override;

    // virtual JUB_RV SignContractHashETH(const JUB_BYTE inputType,
    //                                    const std::vector<JUB_BYTE>& vGasLimit,
    //                                    const std::vector<JUB_BYTE>& vInputHash,
    //                                    const std::vector<JUB_BYTE>& vUnsignedTxHash,
    //                                    const std::vector<JUB_BYTE>& vPath,
    //                                    const std::vector<JUB_BYTE>& vChainID,
    //                                    std::vector<JUB_BYTE>& signatureRaw) override;

    virtual JUB_RV SignBytestring(const std::vector<JUB_BYTE>& vData,
                                  const std::vector<JUB_BYTE>& vPath,
                                  const std::vector<JUB_BYTE>& vChainID,
                                  std::vector<JUB_BYTE>& signatureRaw) override;
     virtual JUB_RV VerifyBytestring(const uchar_vector& vData,
                                     const uchar_vector& vSignature,
                                     const std::vector<JUB_BYTE>& publicKey) override;

    virtual JUB_RV SignTypedData(const bool& bMetamaskV4Compat,
                                 const std::string& typedDataInJSON,
                                 const std::vector<JUB_BYTE>& vPath,
                                 const std::vector<JUB_BYTE>& vChainID,
                                 std::vector<JUB_BYTE>& signatureRaw) override;

/*     virtual JUB_RV SignTypedDataMAX(const std::string& typedDataInJSON,
                                     const std::vector<JUB_BYTE>& vPath,
                                     const std::vector<JUB_BYTE>& vChainID,
                                     std::vector<JUB_BYTE>& signatureRaw) override;*/

     virtual JUB_RV VerifyTypedData(const bool& bMetamaskV4Compat,
                                    const std::string& typedDataInJSON,
                                    const std::vector<JUB_BYTE>& vSignature,
                                    const std::vector<JUB_BYTE>& publicKey) override;

    // //EOS functions
    virtual JUB_RV SelectAppletEOS() override {
        return Init_Device();
    }
    // virtual JUB_RV SetCoinTypeEOS() override;
    // virtual JUB_RV GetAddressEOS(const TW::EOS::Type& type, const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    // virtual JUB_RV GetHDNodeEOS(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    // virtual JUB_RV SignTXEOS(const TW::EOS::Type& type,
    //                          const std::vector<JUB_BYTE>& vPath,
    //                          const std::vector<JUB_BYTE>& vChainId,
    //                          const std::vector<JUB_BYTE>& vRaw,
    //                          std::vector<uchar_vector>& vSignatureRaw,
    //                          const bool bWithType=false) override;

    // //XRP functions
    virtual JUB_RV SelectAppletXRP() override {
        return Init_Device();
    }
    virtual JUB_RV SetCoinTypeXRP() override;
    virtual JUB_RV GetAddressXRP(const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeXRP(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    virtual JUB_RV SignTXXRP(const std::vector<JUB_BYTE>& vPath,
                            std::vector<JUB_BYTE>& vUnsignedRaw,
                            std::vector<uchar_vector>& vSignatureRaw) override;
    virtual JUB_RV SignTXXRP_2100(const std::vector<JUB_BYTE>& vPath,
                                  const JUB_TX_XRP& tx,
                                  std::vector<JUB_BYTE>& vUnsignedRaw,
                                  std::string& signedRaw,
                                  std::vector<uchar_vector>& vSignatureRaw);

    // //TRX functions
    virtual JUB_RV SelectAppletTRX() override {
        return Init_Device();
    }
    virtual JUB_RV SetCoinTypeTRX() override;
    virtual JUB_RV GetAddressTRX(const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeTRX(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    virtual JUB_RV SignTXTRX(const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vRaw,
                             std::vector<uchar_vector>& vSignatureRaw) override;
//    JUB_RV SignTXTRX_2100(const std::vector<JUB_BYTE>& vPath,
//                          const JUB_TX_XRP& tx,
//                          std::vector<uchar_vector>& vSignatureRaw);
     virtual JUB_RV SetTRC20Token(const std::string& tokenName,
                                  const JUB_UINT16 unitDP,
                                  const std::string& contractAddress) override;

    // //FIL functions
    virtual JUB_RV SelectAppletFIL() override {
        return Init_Device();
    }
    virtual JUB_RV SetCoinTypeFIL() override;
    // virtual JUB_RV GetAppletVersionFIL(stVersion& version) override;
    virtual JUB_RV GetAddressFIL(const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeFIL(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
     virtual JUB_RV SignTXFIL(const uint64_t& nonce,
                              const uint256_t& glimit,
                              const uint256_t& gfeeCap,
                              const uint256_t& gpremium,
                              const std::string& to,
                              const uint256_t& value,
                              const std::string& input,
                              const std::string& path,
                              std::vector<uchar_vector>& vSignatureRaw) override;

    // //SOL functions
    virtual JUB_RV SelectAppletSOL() override {
        return Init_Device();
    }
    virtual JUB_RV SetCoinTypeSOL() override;

     virtual JUB_RV GetAddressSOL(const std::string& path, const JUB_UINT16 tag, std::string& address) override;

     virtual JUB_RV GetHDNodeSOL(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;

     virtual JUB_RV SignTransferTxSOl_2100(const std::string &path,const JUB_CHAR_CPTR recentHash,const JUB_CHAR_CPTR dest,JUB_UINT64 amount,std::string &raw) override;
    virtual JUB_RV SignTokenTransferTxSOL_2100(const std::string &path,
                                               const JUB_CHAR_CPTR recentHash,
                                               const JUB_CHAR_CPTR token,
                                               const JUB_CHAR_CPTR &from,
                                               const JUB_CHAR_CPTR &dest,
                                               JUB_UINT64 amount,
                                               JUB_UINT8 decimal,
                                               std::string &raw) override;
    virtual JUB_RV SignTokenCreateAndTransferTxSOL_2100(const std::string &path,
                                                        const JUB_CHAR_CPTR recentHash,
                                                        const JUB_CHAR_CPTR token,
                                                        const JUB_CHAR_CPTR destMainAddress,
                                                        const JUB_CHAR_CPTR &from,
                                                        const JUB_CHAR_CPTR &dest,
                                                        JUB_UINT64 amount,
                                                        JUB_UINT8 decimal,
                                                        std::string &raw) override;

     // virtual JUB_RV SetTokenInfo(const std::string &name, JUB_UINT8 decimal, const std::string &tokenMint) override;

    // // Trasfer `SOL`
    // virtual JUB_RV SignTransferTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
    //                               const std::vector<JUB_BYTE> &dest, JUB_UINT64 amount, std::vector<JUB_BYTE> &raw) override;

    // // Trasfer `Token`
    // virtual JUB_RV SignTokenTransferTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
    //                                    const std::vector<JUB_BYTE> token, const std::vector<JUB_BYTE> &from,
    //                                    const std::vector<JUB_BYTE> &dest, JUB_UINT64 amount, JUB_UINT8 decimal,
    //                                    std::vector<JUB_BYTE> &raw) override;

    // // create token address
    // virtual JUB_RV SignCreateTokenAccountTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
    //                                         const std::vector<JUB_BYTE> &owner, const std::vector<JUB_BYTE> &token,
    //                                         std::vector<JUB_BYTE> &raw) override;

    // //APTOS functions
    virtual JUB_RV SelectAppletAPTOS() override {
        return Init_Device();
    }
    // virtual JUB_RV SetCoinTypeAPTOS() override;

    // //SUI functions
    virtual JUB_RV SelectAppletSUI() override {
        return Init_Device();
    }
    // virtual JUB_RV SetCoinTypeSUI() override;

    // //common functions
    virtual JUB_RV QueryBattery(JUB_BYTE &percent) override;
    virtual JUB_RV ShowVirtualPwd() override;
    virtual JUB_RV CancelVirtualPwd() override;
    virtual bool   IsInitialize() override;
    virtual bool   IsBootLoader() override;
    virtual JUB_RV GetSN(JUB_BYTE sn[24]) override;
    virtual JUB_RV GetLabel(JUB_BYTE label[32]) override;
    virtual JUB_RV GetPinRetry(JUB_BYTE& retry) override;
    virtual JUB_RV GetPinMaxRetry(JUB_BYTE& maxRetry) override;
    virtual JUB_RV GetBleVersion(JUB_BYTE bleVersion[4]) override;
    virtual JUB_RV GetFwVersion(JUB_BYTE fwVersion[4]) override;

    // virtual JUB_RV EnumApplet(std::string& appletList) override;
    // virtual JUB_RV GetAppletVersion(const std::string& appID, stVersion& version) override;
    virtual JUB_RV EnumSupportCoins(std::string& coinList) override;
    virtual JUB_RV GetDeviceCert(std::string& cert) override;
    virtual JUB_RV GetDeviceSignData(const std::string& hashData,std::string& SignData) override;
    // virtual JUB_RV SendOneApdu(const std::string& apdu, std::string& response) override;
    // virtual JUB_RV SendOnePbMessage(const std::string& pbMessage, std::string& response) override;

     virtual JUB_RV VerifyPIN(const std::string &pinMix, OUT JUB_ULONG &retry) override;

    virtual JUB_RV SetTimeout(const JUB_UINT16 timeout) override;

    // // BIO
    // virtual JUB_RV UIShowMain() override;

    // virtual JUB_RV IdentityVerify(IN JUB_BYTE mode, OUT JUB_ULONG &retry) override;
    // virtual JUB_RV IdentityVerifyPIN(IN JUB_BYTE mode, IN const std::string &pinMix, OUT JUB_ULONG &retry) override;
    // virtual JUB_RV IdentityNineGrids(IN bool bShow) override;

    // virtual JUB_RV VerifyFingerprint(OUT JUB_ULONG &retry) override;
    // virtual JUB_RV EnrollFingerprint(IN JUB_UINT16 fpTimeout,
    //                                  INOUT JUB_BYTE_PTR fgptIndex, OUT JUB_ULONG_PTR ptimes,
    //                                  OUT JUB_BYTE_PTR fgptID) override;
    // virtual JUB_RV EnumFingerprint(std::string& fgptList) override;
    // virtual JUB_RV EraseFingerprint(IN JUB_UINT16 fpTimeout) override;
    // virtual JUB_RV DeleteFingerprint(IN JUB_UINT16 fpTimeout,
    //                                  JUB_BYTE fgptID) override;
    // Iris
    virtual JUB_RV IrisSetLable(const std::string& Lable) override;
    virtual JUB_RV IrisUsePassphrase(const bool bUse) override;
    virtual JUB_RV IrisGetFeatures(std::string& strFeatures) override;
    virtual JUB_RV IrisEndSession() override;
    virtual JUB_RV IrisOpCancel() override;
    virtual JUB_RV IrisReboot() override;
    virtual JUB_RV IrisBL_Reboot() override;
    virtual JUB_RV IrisUpdateFirmware(IN JUB_BYTE_PTR firmwareFilePayload,
                                      IN JUB_UINT32 firmwareFileSize,
                                      IN bool reboot_on_success) override;
    virtual JUB_RV IrisUpdateResource(IN JUB_BYTE_PTR resourceZipPayload,
                                      IN JUB_UINT32 resourceZipSize,
                                      IN bool reboot_on_success) override;

protected:
    std::shared_ptr<jub::JubiterBLENusDevice> _device;
    std::string _path;
    std::string session_id;//当前session_id
private:
    JUB_RV sendProtocolData(uint16_t sendType,const std::string& send_data, uint16_t* recvType, std::string& recvData);
    JUB_RV sendProtocolDataPlaint(uint16_t sendType, const std::string &send_data, uint16_t *recvType, std::string &recvData);
    JUB_RV onlysendData(uint16_t sendType,const std::string& send_data);
    JUB_RV filterButtonRequest(uint16_t recvType, const std::string& recvData, uint16_t* filteredType, std::string& filteredData);
    JUB_RV updateBootloader(IN JUB_BYTE_PTR firmwareFilePayload,
                            IN JUB_UINT32 firmwareFileSize,
                            IN bool reboot_on_success);
private:
    JUB_RV Get_Features(Features * msg_Features);
    JUB_RV Init_Device();
    JUB_RV End_Session();
    JUB_RV OP_Cancel();
    JUB_RV Make_dir(const std::string& strDirPath);
    size_t emmc_Writefile(const std::string& strFilePath,
                                                 const std::byte* chunk_data,
                                                 size_t chunk_size,
                                                 size_t offset,
                                                 bool is_first_chunk,
                                                 int ui_percentage);

}; // class JubiterSealer2100Impl end

}  // namespace jub end

#endif //HID_MODE

#endif  // __JubiterSealer2100Impl__
