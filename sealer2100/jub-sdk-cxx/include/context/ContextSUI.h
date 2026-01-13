//
//  ContextSUI.h
//  JubSDK
//
//  Created by Pan Min on 2025/5/5.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#pragma once
#ifndef __ContextSUI__
#define __ContextSUI__

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "context/Context.h"


#define JUB_CHECK_CONTEXT_SUI(x)                                    \
do {                                                                \
    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
    JUB_CHECK_NULL(context);                                        \
    const std::type_info& tCtx = typeid(*context);                  \
    const std::type_info& tCtxSUI = typeid(jub::ContextSUI);        \
    if (tCtx.hash_code() != tCtxSUI.hash_code()) {                  \
        return JUBR_ARGUMENTS_BAD;                                  \
    }                                                               \
} while(0)


namespace jub {

class ContextSUI : public Context {
public:
    ContextSUI(CONTEXT_CONFIG_SUI cfg, JUB_UINT16 deviceID) {
        _mainPath = cfg.mainPath;
        _deviceID = deviceID;
         _timeout = 120 * 2;
    }
    ~ContextSUI() {}

    virtual JUB_RV ActiveSelf() override;
}; // class ContextSUI end

} // namespace jub end

#endif // #pragma once
