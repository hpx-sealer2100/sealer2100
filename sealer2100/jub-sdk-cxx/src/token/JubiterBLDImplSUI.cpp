//
//  JubiterBLDImplSUI.cpp
//  JubSDK
//
//  Created by Pan Min on 2025/5/5.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#include "token/JubiterBLDImpl.h"

#include <bigint/BigIntegerUtils.hh>
#include <TrezorCrypto/bip32.h>
#include "utility/util.h"


namespace jub {

JUB_RV JubiterBLDImpl::SelectAppletSUI() {

    return JUBR_IMPL_NOT_SUPPORT;
}


//MISC functions
JUB_RV JubiterBLDImpl::SetCoinTypeSUI() {

    return JUBR_IMPL_NOT_SUPPORT;
}
} // namespace jub end
