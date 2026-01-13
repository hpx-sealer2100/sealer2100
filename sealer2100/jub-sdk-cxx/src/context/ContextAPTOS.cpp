////
////  ContextAPTOS.cpp
////  JubSDK
////
////  Created by Pan Min on 2025/5/5.
////  Copyright © 2025 ZianCube. All rights reserved.
////
//
//
//
//#include "utility/util.h"
//
//#include "context/ContextAPTOS.h"
//#include "token/interface/TokenInterface.hpp"
//
//
//namespace jub {
//
//
//JUB_RV ContextAPTOS::ActiveSelf() {
//
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//    JUB_CHECK_NULL(ctoken);
//
//    JUB_VERIFY_RV(token->SelectAppletAPTOS());
//    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));
//    JUB_VERIFY_RV(token->SetCoinTypeAPTOS());
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::GetAddressEncodePb(
//    const BIP32_Path& path,
//    const JUB_UINT16 tag,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    std::string strPath = _FullBip32Path(path);
//    JUB_VERIFY_RV(token->GetAddressAPTOSEncodePb(strPath, tag, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::GetAddressDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetAddressAPTOSDecodePb(msgInPb, address));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::GetHDNodeEncodePb(
//    const JUB_BYTE format,
//    const BIP32_Path& path,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    std::string strPath = _FullBip32Path(path);
//    JUB_VERIFY_RV(token->GetHDNodeAPTOSEncodePb(format, strPath, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::GetHDNodeDecodePb(
//    const std::string& msgInPb,
//    std::string& pubkey
//) {
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeAPTOSDecodePb(msgInPb, pubkey));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::GetMainHDNodeEncodePb(
//    const JUB_BYTE format,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeAPTOSEncodePb(format, _mainPath, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::GetMainHDNodeDecodePb(
//    const std::string& msgInPb,
//    std::string& xpub
//) {
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeAPTOSDecodePb(msgInPb, xpub));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::SignTransactionEncodePb(
//    const BIP32_Path& path,
//    const std::string& sender,
//    const std::string& recipient,
//    const std::string& amountInOcta,
//    JUB_OPTIONS_APTOS_CPTR opts,
//    std::string& msgInPb
//) {
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    std::string strPath = _FullBip32Path(path);
//    std::vector<JUB_BYTE> vPath(strPath.begin(), strPath.end());
//
//    // finish transaction
//    try {
//        // TODO
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::SignTransactionDecodePb(
//    const std::string& msgInPb,
//    std::string& raw
//) {
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->SignTXAPTOSDecodePb(msgInPb, raw));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::SignTransactionTokenEncodePb(
//    const BIP32_Path& path,
//    const std::string& sender,
//    const std::string& recipient,
//    const std::string& token_type,
//    const std::string& amount_str,
//    JUB_OPTIONS_APTOS_CPTR opts,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    std::string strPath = _FullBip32Path(path);
//    std::vector<JUB_BYTE> vPath(strPath.begin(), strPath.end());
//
//    // finish transaction
//    try {
//        // TODO
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::SignTransactionTokenDecodePb(
//    const std::string& msgInPb,
//    std::string& raw
//) {
//    // TODO
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV ContextAPTOS::SignMessageEncodePb(
//    const BIP32_Path& path,
//    const std::string& message,
//    const JUB_BBOOL add_prefix,
//    JUB_OPTIONS_APTOS_CPTR opts,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<APTOSTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    std::string strPath = _FullBip32Path(path);
//    std::vector<JUB_BYTE> vPath(strPath.begin(), strPath.end());
//
//    // finish transaction
//    try {
//        // TODO
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextAPTOS::SignMessageDecodePb(
//    const std::string& msgInPb,
//    std::string& signature
//) {
//    // TODO
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//} // namespace jub end
