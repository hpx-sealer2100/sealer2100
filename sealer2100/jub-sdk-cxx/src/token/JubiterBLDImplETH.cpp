#include "token/JubiterBLDImpl.h"
#include "libETH/ERC20Abi.h"
#include "libETH/libETH.hpp"
#include <Data.h>
#include <PublicKey.h>
#include <Ethereum/RLP.h>
#include <Ethereum/Transaction.h>
#include <Ethereum/EIP712.h>

namespace jub {


#define SWITCH_TO_ETH_APP                       \
do {				                            \
    JUB_VERIFY_RV(_SelectApp(kPKIAID_ETH, sizeof(kPKIAID_ETH)/sizeof(JUB_BYTE))); \
} while (0);                                    \


JUB_RV JubiterBLDImpl::SelectAppletETH() {
    SWITCH_TO_ETH_APP;
    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::GetAppletVersionETH(stVersion& version) {

    uchar_vector appID(kPKIAID_ETH, 16);
    JUB_VERIFY_RV(GetAppletVersion(CharPtr2HexStr(appID), version));

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::GetAddressETH(const std::string& path, uint64_t chainId, const JUB_UINT16 tag, std::string& address) {
    // not used
    (void)chainId;

    uchar_vector data(path.begin(), path.end());

    APDU apdu(0x00, 0xf6, 0x00, (JUB_BYTE)tag, (JUB_ULONG)data.size(), data.data(), 0x14);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    uchar_vector vAddress(retData, (unsigned int)ulRetDataLen);
    address = std::string(ETH_PRDFIX) + vAddress.getHex();

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::GetHDNodeETH(const JUB_BYTE format, const std::string& path, std::string& pubkey) {

    //path = "m/44'/60'/0'";
    uchar_vector vPath;
    vPath << path;
    uchar_vector apduData = ToTlv(0x08, vPath);

    //0x00 for hex, 0x01 for xpub
    if (   JUB_ENUM_PUB_FORMAT::HEX  != format
        && JUB_ENUM_PUB_FORMAT::XPUB != format
        ) {
        return JUBR_ERROR_ARGS;
    }

    APDU apdu(0x00, 0xe6, 0x00, format, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    if (JUB_ENUM_PUB_FORMAT::HEX == format) {
        uchar_vector vPubkey(retData, (unsigned int)ulRetDataLen);
        pubkey = std::string(ETH_PRDFIX) + vPubkey.getHex();
    }
    else if (JUB_ENUM_PUB_FORMAT::XPUB == format) {
        pubkey = (JUB_CHAR_PTR)retData;
    }

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::SignTXETH(const int erc,
                                 const std::vector<JUB_BYTE>& vNonce,
                                 const std::vector<JUB_BYTE>& vGasPrice,
                                 const std::vector<JUB_BYTE>& vGasLimit,
                                 const std::vector<JUB_BYTE>& vTo,
                                 const std::vector<JUB_BYTE>& vValue,
                                 const std::vector<JUB_BYTE>& vInput,
                                 const std::vector<JUB_BYTE>& vPath,
                                 const std::vector<JUB_BYTE>& vChainID,
                                 std::vector<JUB_BYTE>& vRaw) {

    // ETH token extension apdu
    if (_appletVersion >= stVersionExp::FromString(ETH_APPLET_VERSION_SUPPORT_EXT_TOKENS)) {
        return _SignTXUpgradeETH(erc,
                                 vNonce,
                                 vGasPrice,
                                 vGasLimit,
                                 vTo,
                                 vValue,
                                 vInput,
                                 vPath,
                                 vChainID,
                                 vRaw);
    }
    else {
        return _SignTXETH(erc,
                          vNonce,
                          vGasPrice,
                          vGasLimit,
                          vTo,
                          vValue,
                          vInput,
                          vPath,
                          vChainID,
                          vRaw);
    }
}


JUB_RV JubiterBLDImpl::_SignTXETH(const int erc,
                                  const std::vector<JUB_BYTE>& vNonce,
                                  const std::vector<JUB_BYTE>& vGasPrice,
                                  const std::vector<JUB_BYTE>& vGasLimit,
                                  const std::vector<JUB_BYTE>& vTo,
                                  const std::vector<JUB_BYTE>& vValue,
                                  const std::vector<JUB_BYTE>& vInput,
                                  const std::vector<JUB_BYTE>& vPath,
                                  const std::vector<JUB_BYTE>& vChainID,
                                  std::vector<JUB_BYTE>& vRaw) {

    uchar_vector apduData;

    if (0x00 == vNonce[0]) {
        apduData << (JUB_BYTE)0x41;
        apduData << (JUB_BYTE)0x00;
    }
    else {
        apduData << ToTlv(0x41, vNonce);
    }

    apduData << ToTlv(0x42, vGasPrice);
    apduData << ToTlv(0x43, vGasLimit);
    apduData << ToTlv(0x44, vTo);

    // If value=0, when sending apdu,
    // it is clear that this part is empty
    uchar_vector vValueInWei(vValue);
    if (   1 == vValueInWei.size()
        && 0 == vValueInWei[0]
        ) {
        vValueInWei.clear();
    }
    apduData << ToTlv(0x45, vValueInWei);

    apduData << ToTlv(0x46, vInput);
    apduData << ToTlv(0x47, vPath);
    apduData << ToTlv(0x48, vChainID);

    JUB_BYTE ins = 0x2A;
    JUB_BYTE p1  = JUB_ENUM_APDU_ERC_P1::ERC20;
    switch (erc) {
    case JUB_ENUM_APDU_ERC_ETH::ERC_20:
        ins = 0xC8;
        break;
    case JUB_ENUM_APDU_ERC_ETH::ERC_INVALID:
    default:
        break;
    }

    //one pack can do it
    APDU apdu(0x00, ins, p1, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    vRaw.clear();
    vRaw.insert(vRaw.end(), retData, retData + ulRetDataLen);

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::_SignTXUpgradeETH(const int erc,
                                         const std::vector<JUB_BYTE>& vNonce,
                                         const std::vector<JUB_BYTE>& vGasPrice,
                                         const std::vector<JUB_BYTE>& vGasLimit,
                                         const std::vector<JUB_BYTE>& vTo,
                                         const std::vector<JUB_BYTE>& vValue,
                                         const std::vector<JUB_BYTE>& vInput,
                                         const std::vector<JUB_BYTE>& vPath,
                                         const std::vector<JUB_BYTE>& vChainID,
                                         std::vector<JUB_BYTE>& vRaw) {

    // COS subpackage size 512 Byte
    uchar_vector apduData;

    if (0x00 == vNonce[0]) {
        apduData << (JUB_BYTE)0x41;
        apduData << (JUB_BYTE)0x00;
    }
    else {
        apduData << ToTlv(0x41, vNonce);
    }

    apduData << ToTlv(0x42, vGasPrice);
    apduData << ToTlv(0x43, vGasLimit);
    apduData << ToTlv(0x44, vTo);

    // If value=0, when sending apdu,
    // it is clear that this part is empty
    uchar_vector vValueInWei(vValue);
    if (   1 == vValueInWei.size()
        && 0 == vValueInWei[0]
        ) {
        vValueInWei.clear();
    }
    apduData << ToTlv(0x45, vValueInWei);

    apduData << ToTlv(0x46, vInput);
    apduData << ToTlv(0x47, vPath);
    apduData << ToTlv(0x48, vChainID);

    JUB_BYTE ins = 0x2a;
    JUB_BYTE p1  = JUB_ENUM_APDU_ERC_P1::ERC20;
    switch (erc) {
    case JUB_ENUM_APDU_ERC_ETH::ERC_20:
        p1  = JUB_ENUM_APDU_ERC_P1::ERC20;
        break;
    case JUB_ENUM_APDU_ERC_ETH::ERC_INVALID:
    default:
        break;
    }

    // subpackage
    {
        constexpr JUB_UINT32 kSendOnceLen = 230;
        JUB_ULONG offset = 23;

        //  first pack
        APDU apdu(0x00, 0xF8, 0x01, 0x00, offset, apduData.data());
        JUB_UINT16 ret = 0;
        JUB_VERIFY_RV(_SendApdu(&apdu, ret));
        if (0x9000 != ret) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }

        unsigned long iCnt       = (apduData.size()-offset)/kSendOnceLen;
        JUB_UINT32    iRemainder = (apduData.size()-offset)%kSendOnceLen;
        if (iCnt) {
            int bOnce = false;
            for (unsigned long i=0; i<iCnt; ++i) {
                if (   (i+1) == iCnt
                    &&    0  == iRemainder
                    ) {
                    bOnce = true;
                }
                uchar_vector apduDataPart(&apduData[offset+i*kSendOnceLen], kSendOnceLen);
                JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, bOnce));  // last data or not.
            }
        }
        if (iRemainder) {
            uchar_vector apduDataPart(&apduData[offset+iCnt*kSendOnceLen], iRemainder);
            JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, true));  // last data.
        }

        apduData.clear();
    }

    //one pack can do it
    APDU apdu(0x00, ins, p1, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    uchar_vector vSignature;
    vSignature.insert(vSignature.end(), retData, retData + ulRetDataLen);

    vRaw.clear();
    JUB_VERIFY_RV(jub::eth::serializeTx(vNonce,
                                        vGasPrice,
                                        vGasLimit,
                                        vTo,
                                        vValue,
                                        vInput,
                                        vSignature,
                                        vRaw));

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::SetERC20ETHToken(const std::string& tokenName, const JUB_UINT16 unitDP, const std::string& contractAddress) {

    // ETH token extension apdu
    if (JubiterBLDImpl::_appletVersion < stVersionExp::FromString(JubiterBLDImpl::ETH_APPLET_VERSION_SUPPORT_EXT_TOKEN)) {
        return JUBR_OK;
    }

    return SetERC20Token(tokenName, unitDP, contractAddress);
}


JUB_RV JubiterBLDImpl::SetERC20ETHTokens(const ERC20_TOKEN_INFO tokens[],
                                         const JUB_UINT16 iCount) {

    // ETH token extension apdu
    if (JubiterBLDImpl::_appletVersion >= stVersionExp::FromString(JubiterBLDImpl::ETH_APPLET_VERSION_SUPPORT_EXT_TOKENS)) {
        return SetERC20Tokens(tokens, iCount);
    }
    else if (1 == iCount) {
        return SetERC20ETHToken(tokens[0].tokenName, tokens[0].unitDP, tokens[0].contractAddress);
    }

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::SignContractETH(const JUB_BYTE inputType,
                                       const std::vector<JUB_BYTE>& vNonce,
                                       const std::vector<JUB_BYTE>& vGasPrice,
                                       const std::vector<JUB_BYTE>& vGasLimit,
                                       const std::vector<JUB_BYTE>& vTo,
                                       const std::vector<JUB_BYTE>& vValue,
                                       const std::vector<JUB_BYTE>& vInput,
                                       const std::vector<JUB_BYTE>& vPath,
                                       const std::vector<JUB_BYTE>& vChainID,
                                       std::vector<JUB_BYTE>& signatureRaw) {

    constexpr JUB_UINT32 kSendOnceLen = 230;

    uchar_vector apduData;

    if (0x00 == vNonce[0]) {
        apduData << (JUB_BYTE)0x41;
        apduData << (JUB_BYTE)0x00;
    }
    else {
        apduData << ToTlv(0x41, vNonce);
    }

    apduData << ToTlv(0x42, vGasPrice);
    apduData << ToTlv(0x43, vGasLimit);
    apduData << ToTlv(0x44, vTo);
    apduData << ToTlv(0x45, vValue);
    // Too length to send at here
//    apduData << ToTlv(0x46, vInput);
    apduData << ToTlv(0x47, vPath);
    apduData << ToTlv(0x48, vChainID);

    //  first pack
    APDU apdu(0x00, 0xF8, 0x01, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }
    apduData.clear();

    uchar_vector tlvInput;
    tlvInput << ToTlv(inputType, vInput);
    apduData << ToTlv(0x46, tlvInput);
    unsigned long iCnt = apduData.size()/kSendOnceLen;
    JUB_UINT32 iRemainder = apduData.size()%kSendOnceLen;
    if (iCnt) {
        int bOnce = false;
        for (unsigned long i=0; i<iCnt; ++i) {
            if (   (i+1) == iCnt
                &&    0  == iRemainder
                ) {
                bOnce = true;
            }
            uchar_vector apduDataPart(&apduData[i*kSendOnceLen], kSendOnceLen);
            JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, bOnce));  // last data or not.
        }
    }
    if (iRemainder) {
        uchar_vector apduDataPart(&apduData[iCnt*kSendOnceLen], iRemainder);
        JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, true));  // last data.
    }
    apduData.clear();

    //  sign transactions
    JUB_BYTE ins = 0xc9;
    apdu.SetApdu(0x00, ins, 0x00, 0x00, 0);
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x6f09 == ret) {
        return JUBR_USER_CANCEL;
    }
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    signatureRaw.clear();
    signatureRaw.insert(signatureRaw.end(), retData, retData + ulRetDataLen);

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::SignContractHashETH(const JUB_BYTE inputType,
                                           const std::vector<JUB_BYTE>& vGasLimit,
                                           const std::vector<JUB_BYTE>& vInputHash,
                                           const std::vector<JUB_BYTE>& vUnsignedTxHash,
                                           const std::vector<JUB_BYTE>& vPath,
                                           const std::vector<JUB_BYTE>& vChainID,
                                           std::vector<JUB_BYTE>& signatureRaw) {

    uchar_vector apduData;

    apduData << ToTlv(0x43, vGasLimit);
    apduData << ToTlv(0x07, vUnsignedTxHash);
    // Too length to send at here
//    data << ToTlv(0x46, vInput);
    apduData << ToTlv(0x47, vPath);
    apduData << ToTlv(0x48, vChainID);
    uchar_vector tlvInput;
    tlvInput << ToTlv(inputType, vInputHash);
    apduData << ToTlv(0x46, tlvInput);

    JUB_BYTE ins = 0xca;

    //one pack can do it
    APDU apdu(0x00, ins, 0x01, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    signatureRaw.clear();
    signatureRaw.insert(signatureRaw.end(), retData, retData + ulRetDataLen);

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::SignBytestring(const std::vector<JUB_BYTE> &vData, const std::vector<JUB_BYTE> &vPath,
                                      const std::vector<JUB_BYTE> &vChainID, std::vector<JUB_BYTE> &signatureRaw) {

    constexpr JUB_UINT32 kSendOnceLen = 230;

    uint16_t total = 0;

    uchar_vector pathTLV = ToTlv(0x47, vPath);
    total += pathTLV.size();
    uchar_vector chainIdTLV = ToTlv(0x48, vChainID);
    total += chainIdTLV.size();
    uchar_vector dataTLV = ToTlv(0x49, vData);
    total += dataTLV.size();

    uchar_vector apduData;
    apduData << total;
    apduData.reverse();
    apduData << pathTLV;
    apduData << chainIdTLV;

    //  first pack
    APDU apdu(0x00, 0xF8, 0x01, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }
    apduData.clear();

    apduData << dataTLV;
    unsigned long iCnt = apduData.size() / kSendOnceLen;
    JUB_UINT32 iRemainder = apduData.size() % kSendOnceLen;
    if (iCnt) {
        int bOnce = false;
        for (unsigned long i = 0; i < iCnt; ++i) {
            if ((i + 1) == iCnt && 0 == iRemainder) {
                bOnce = true;
            }
            uchar_vector apduDataPart(&apduData[i * kSendOnceLen], kSendOnceLen);
            JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, bOnce)); // last data or not.
        }
    }
    if (iRemainder) {
        uchar_vector apduDataPart(&apduData[iCnt * kSendOnceLen], iRemainder);
        JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, true)); // last data.
    }
    apduData.clear();

    //  sign transactions
    apdu.SetApdu(0x00, 0xCB, 0x00, 0x00, 0);
    JUB_BYTE retData[2048] = {
        0,
    };
    JUB_ULONG ulRetDataLen = sizeof(retData) / sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x6f09 == ret) {
        return JUBR_USER_CANCEL;
    }
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    signatureRaw.clear();
    signatureRaw.insert(signatureRaw.end(), retData, retData + ulRetDataLen);

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::VerifyBytestring(const uchar_vector& vData,
                                        const uchar_vector& vSignature,
                                        const std::vector<JUB_BYTE>& publicKey) {
    return (
        true == TW::PublicKey(publicKey,
                              TWPublicKeyType::TWPublicKeyTypeSECP256k1
                ).verify(vSignature,
                             TW::Ethereum::TransactionPersonal(vData).preHash()
                )
    ) ? JUBR_OK : JUBR_ERROR;
}


JUB_RV JubiterBLDImpl::SignTypedData(const bool &bMetamaskV4Compat, const std::string &typedDataInJSON,
                                     const std::vector<JUB_BYTE> &vPath, const std::vector<JUB_BYTE> &vChainID,
                                     std::vector<JUB_BYTE> &signatureRaw) {
    nlohmann::json typedData = nlohmann::json::parse(typedDataInJSON);
    if (nlohmann::detail::value_t::object != typedData.type()) {
        return JUBR_DATA_INVALID;
    }

    if (!jub::eth::EIP712::parseJSON(typedData)) {
        jub::eth::EIP712::clearJSON();
        return JUBR_DATA_INVALID;
    }

    uchar_vector domainSeparator = jub::eth::EIP712::typed_data_envelope(
                        jub::eth::EIP712::EIP712Domain(),
                        typedData[jub::eth::EIP712::domainEnter()],
                        bMetamaskV4Compat);
    if (domainSeparator.empty()) {
        jub::eth::EIP712::clearJSON();
        return JUBR_DATA_INVALID;
    }

    uchar_vector hashStructMessage = jub::eth::EIP712::typed_data_envelope(
                        typedData[jub::eth::EIP712::primaryTypeEnter()].get<std::string>().c_str(),
                        typedData[jub::eth::EIP712::messageEnter()],
                        bMetamaskV4Compat);
    if (hashStructMessage.empty()) {
        jub::eth::EIP712::clearJSON();
        return JUBR_DATA_INVALID;
    }

    auto domainName = typedData.at(jub::eth::EIP712::domainEnter()).at("name").get<std::string>();
    auto name = std::vector<unsigned char>(domainName.size());
    std::copy(domainName.begin(), domainName.end(), name.begin());

    uint16_t total = 0;
    auto pathTLV = ToTlv(0x47, vPath);
    total += pathTLV.size();
    auto chainIdTLV = ToTlv(0x48, vChainID);
    total += chainIdTLV.size();
    auto nameTLV = ToTlv(0x50, name);
    total += nameTLV.size();
    auto domainTLV = ToTlv(0x51, domainSeparator);
    total += domainTLV.size();
    auto msgHashTLV = ToTlv(0x52, hashStructMessage);
    total += msgHashTLV.size();

    uchar_vector apduData;
    // little ending
    apduData << total;
    // big ending
    apduData.reverse();

    for (auto &tlv : {pathTLV, chainIdTLV, nameTLV, domainTLV, msgHashTLV}) {
        apduData << tlv;
    }

    // can send by one apdu
    APDU apdu(0x00, 0xF8, 0x01, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    //  sign transactions
    apdu.SetApdu(0x00, 0xCC, 0x00, 0x00, 0);
    JUB_BYTE retData[2048] = {
        0,
    };
    JUB_ULONG ulRetDataLen = sizeof(retData) / sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x6f09 == ret) {
        jub::eth::EIP712::clearJSON();
        return JUBR_USER_CANCEL;
    }
    if (0x9000 != ret) {
        jub::eth::EIP712::clearJSON();
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    TW::Ethereum::TransactionTypedData tx(typedDataInJSON);
    auto sig = TW::Ethereum::Signature{
        {retData, retData + 32},      // r
        {retData + 32, retData + 64}, // s
        {retData + 64, retData + 65}, // v
    };
    signatureRaw = tx.encoded(sig);

    jub::eth::EIP712::clearJSON();

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::VerifyTypedData(const bool& bMetamaskV4Compat,
                                       const std::string& typedDataInJSON,
                                       const std::vector<JUB_BYTE>& vSignature,
                                       const std::vector<JUB_BYTE>& publicKey) {
    return (
        true == TW::PublicKey(publicKey,
                              TWPublicKeyType::TWPublicKeyTypeSECP256k1
                ).verify(vSignature,
                         TW::Ethereum::TransactionTypedData(typedDataInJSON, bMetamaskV4Compat).preHash()
                )
    ) ? JUBR_OK : JUBR_ERROR;
}
} // namespace jub end
