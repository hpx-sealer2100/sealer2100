////
////  ContextAPTOS.h
////  JubSDK
////
////  Created by Pan Min on 2025/5/5.
////  Copyright © 2025 ZianCube. All rights reserved.
////
//
//#pragma once
//#ifndef __ContextAPTOS__
//#define __ContextAPTOS__
//
//
//#include <string>
//#include <vector>
//
//#include "utility/Singleton.h"
//#include "utility/xManager.hpp"
//
//#include "context/Context.h"
//
//
//#define JUB_CHECK_CONTEXT_APTOS(x)                                  \
//do {                                                                \
//    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
//    JUB_CHECK_NULL(context);                                        \
//    const std::type_info& tCtx = typeid(*context);                  \
//    const std::type_info& tCtxAPTOS = typeid(jub::ContextAPTOS);    \
//    if (tCtx.hash_code() != tCtxAPTOS.hash_code()) {                \
//        return JUBR_ARGUMENTS_BAD;                                  \
//    }                                                               \
//} while(0)
//
//
//namespace jub {
//
//class ContextAPTOS : public Context {
//public:
//    ContextAPTOS(CONTEXT_CONFIG_APTOS cfg, JUB_UINT16 deviceID) {
//        _mainPath = cfg.mainPath;
//        _deviceID = deviceID;
//         _timeout = 120 * 2;
//    }
//    ~ContextAPTOS() {}
//
//    virtual JUB_RV GetAddressEncodePb(
//        const BIP32_Path& path,
//        const JUB_UINT16 tag,
//        std::string& msgInPb
//    );
//    virtual JUB_RV GetAddressDecodePb(
//        const std::string& msgInPb,
//        std::string& address
//    );
//
//    virtual JUB_RV GetHDNodeEncodePb(
//        const JUB_BYTE format,
//        const BIP32_Path& path,
//        std::string& msgInPb
//    );
//    virtual JUB_RV GetHDNodeDecodePb(
//        const std::string& msgInPb,
//        std::string& pubkey
//    );
//
//    virtual JUB_RV GetMainHDNodeEncodePb(
//        const JUB_BYTE format,
//        std::string& msgInPb
//    );
//    virtual JUB_RV GetMainHDNodeDecodePb(
//        const std::string& msgInPb,
//        std::string& xpub
//    );
//
//    virtual JUB_RV SignTransactionEncodePb(
//        const BIP32_Path& path,
//        const std::string& sender,
//        const std::string& recipient,
//        const std::string& amountInOcta,
//        JUB_OPTIONS_APTOS_CPTR opts,
//        std::string& msgInPb
//    );
//    virtual JUB_RV SignTransactionDecodePb(
//        const std::string& msgInPb,
//        std::string& raw
//    );
//
//    virtual JUB_RV SignTransactionTokenEncodePb(
//        const BIP32_Path& path,
//        const std::string& sender,
//        const std::string& recipient,
//        const std::string& token_type,
//        const std::string& amount_str,
//        JUB_OPTIONS_APTOS_CPTR opts,
//        std::string& msgInPb
//    );
//    virtual JUB_RV SignTransactionTokenDecodePb(
//        const std::string& msgInPb,
//        std::string& raw
//    );
//
//    virtual JUB_RV SignMessageEncodePb(
//        const BIP32_Path& path,
//        const std::string& message,
//        const JUB_BBOOL add_prefix,
//        JUB_OPTIONS_APTOS_CPTR opts,
//        std::string& msgInPb
//    );
//    virtual JUB_RV SignMessageDecodePb(
//        const std::string& msgInPb,
//        std::string& signature
//    );
//
//    virtual JUB_RV ActiveSelf() override;
//}; // class ContextAPTOS end
//
//} // namespace jub end
//
//#endif // #pragma once
