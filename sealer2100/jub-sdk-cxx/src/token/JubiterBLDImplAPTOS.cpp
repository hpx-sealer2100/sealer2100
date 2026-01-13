//
//  JubiterBLDImplAPTOS.cpp
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

JUB_RV JubiterBLDImpl::SelectAppletAPTOS() {

    return JUBR_IMPL_NOT_SUPPORT;
}


//MISC functions
JUB_RV JubiterBLDImpl::SetCoinTypeAPTOS() {

    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::GetAddressAPTOSEncodePb(
    const std::string& path,
    const JUB_UINT16 tag,
    std::string& msgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::GetAddressAPTOSDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::GetHDNodeAPTOSEncodePb(
    const JUB_BYTE format,
    const std::string& path,
    std::string& msgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::GetHDNodeAPTOSDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignTXAPTOSEncodePb(
    const std::vector<JUB_BYTE>& vPath,
    const std::vector<JUB_BYTE>& vRaw,
    std::vector<uchar_vector>& vMsgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignTXAPTOSDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}
} // namespace jub end
