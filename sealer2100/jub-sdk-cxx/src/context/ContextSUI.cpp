////
////  ContextSUI.cpp
////  JubSDK
////
////  Created by Pan Min on 2025/5/5.
////  Copyright © 2025 ZianCube. All rights reserved.
////
//
//
//#include "utility/util.h"
//
//#include "context/ContextSUI.h"
//#include "token/interface/TokenInterface.hpp"
//
//
//namespace jub {
//
//
//JUB_RV ContextSUI::ActiveSelf() {
//
//    auto token = dynamic_cast<SUITokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//    JUB_CHECK_NULL(ctoken);
//
//    JUB_VERIFY_RV(token->SelectAppletSUI());
//    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));
//    JUB_VERIFY_RV(token->SetCoinTypeSUI());
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSUI::GetMainHDNodeEncodePb(
//    const JUB_BYTE format,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<SUITokenInterfacePB*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeSUIEncodePb(format, _mainPath, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSUI::GetMainHDNodeDecodePb(
//    const std::string& msgInPb,
//    std::string& xpub
//) {
//    auto token = dynamic_cast<SUITokenInterfacePB*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeSUIDecodePb(msgInPb, xpub));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSUI::GetAddressEncodePb(
//    const BIP32_Path& path,
//    const JUB_UINT16 tag,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<SUITokenInterfacePB*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    std::string strPath = _FullBip32Path(path);
//    JUB_VERIFY_RV(token->GetAddressSUIEncodePb(strPath, tag, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSUI::GetAddressDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    auto token = dynamic_cast<SUITokenInterfacePB*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetAddressSUIDecodePb(msgInPb, address));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSUI::GetHDNodeEncodePb(
//    const JUB_BYTE format,
//    const BIP32_Path& path,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<SUITokenInterfacePB*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    std::string strPath = _FullBip32Path(path);
//    JUB_VERIFY_RV(token->GetHDNodeSUIEncodePb(format, strPath, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSUI::GetHDNodeDecodePb(
//    const std::string& msgInPb,
//    std::string& pubkey
//) {
//    auto token = dynamic_cast<SUITokenInterfacePB*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeSUIDecodePb(msgInPb, pubkey));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSUI::SignTransactionEncodePb(
//    const BIP32_Path& path,
//    const std::string& sender,
//    const std::string& recipient,
//    const std::string& amountInOcta,
//    JUB_OPTIONS_SUI_CPTR opts,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<SUITokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
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
//JUB_RV ContextSUI::SignTransactionDecodePb(
//    const std::string& msgInPb,
//    std::string& xpub
//) {
//    // TODO
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV ContextSUI::SignTransactionTokenEncodePb(
//    const BIP32_Path& path,
//    const std::string& sender,
//    const std::string& contract_addr,
//    const std::string& recipient,
//    const std::string& amount_str,
//    JUB_TOKEN_METADATA_CPTR metadata,
//    JUB_OPTIONS_SUI_CPTR opts,
//    std::string& msgInPb
//) {
//
//    auto token = dynamic_cast<SUITokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
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
//JUB_RV ContextSUI::SignTransactionTokenDecodePb(
//    const std::string& msgInPb,
//    std::string& xpub
//) {
//    // TODO
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//} // namespace jub end
