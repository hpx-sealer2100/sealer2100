//
//  ContextSOL.h
//  JubSDK
//
//  Created by Pan Min on 2022/07/12.
//  Copyright © 2022 JuBiter. All rights reserved.
//

#pragma once
#ifndef __ContextSOL__
#define __ContextSOL__

#include "JUB_SDK_SOL.h"

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "context/Context.h"


#define JUB_CHECK_CONTEXT_SOL(x)                                    \
do {                                                                \
    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
    JUB_CHECK_NULL(context);                                        \
    const std::type_info& tCtx = typeid(*context);                  \
    const std::type_info& tCtxSOL = typeid(jub::ContextSOL);        \
    if (tCtx.hash_code() != tCtxSOL.hash_code()) {                  \
        return JUBR_ARGUMENTS_BAD;                                  \
    }                                                               \
} while(0)

namespace jub {
class ContextSOL : public Context {
  public:
    ContextSOL(CONTEXT_CONFIG_SOL cfg, JUB_UINT16 deviceID) {
        _mainPath = cfg.mainPath;
        _deviceID = deviceID;
        _timeout = 120 * 2;
    }
    ~ContextSOL() {}

    virtual JUB_RV GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
//    virtual JUB_RV SetMyAddress(const BIP32_Path& path, std::string& address);
    virtual JUB_RV GetHDNode(const JUB_BYTE& format, const BIP32_Path& path, std::string& pubkey);
    virtual JUB_RV GetMainHDNode(const JUB_BYTE format, std::string& xpub);
    virtual JUB_RV SignTransferTx(BIP32_Path& path,
                                  JUB_CHAR_CPTR recentHash,
                                  JUB_CHAR_CPTR dest,
                                  JUB_UINT64 amount,
                                  std::string &txRaw);

//
//    // Trasfer `Token`
    virtual JUB_RV SignTokenTransferTx(BIP32_Path& path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR tokenMint,
                                       JUB_CHAR_CPTR source, JUB_CHAR_CPTR dest, JUB_UINT64 amount, JUB_UINT8 decimal,
                                       std::string &txRaw);
    virtual JUB_RV SignTokenCreateAndTransferTx(BIP32_Path& path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR tokenMint,JUB_CHAR_CPTR destMainAddress,
                                       JUB_CHAR_CPTR source, JUB_CHAR_CPTR dest, JUB_UINT64 amount, JUB_UINT8 decimal,
                                       std::string &txRaw);
//
//    // create token address
//    virtual JUB_RV SignCreateTokenAccountTx(JUB_CHAR_CPTR path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR owner,
//                                            JUB_CHAR_CPTR tokenMint, std::string &txRaw);

    virtual JUB_RV ActiveSelf() override;
    virtual std::string _FullBip32Path(const BIP32_Path& path);

}; // class SOLContext end

} // namespace jub

#endif // #pragma once
