//
//  APTOSTokenInterface.hpp
//  JubSDK
//
//  Created by Pan Min on 2025/5/5.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#ifndef __APTOSTokenInterface__
#define __APTOSTokenInterface__

#include <vector>

namespace jub {


class APTOSTokenInterface {
public:
    virtual ~APTOSTokenInterface() = default;

    virtual JUB_RV SelectAppletAPTOS() = 0;
    virtual JUB_RV SetCoinTypeAPTOS() = 0;

    virtual JUB_RV GetAddressAPTOSEncodePb(
        const std::string& path,
        const JUB_UINT16 tag,
        std::string& msgInPb
    ) = 0;
    virtual JUB_RV GetAddressAPTOSDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;

    virtual JUB_RV GetHDNodeAPTOSEncodePb(
        const JUB_BYTE format,
        const std::string& path,
        std::string& msgInPb
    ) = 0;
    virtual JUB_RV GetHDNodeAPTOSDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;

    virtual JUB_RV SignTXAPTOSEncodePb(
        const std::vector<JUB_BYTE>& vPath,
        const std::vector<JUB_BYTE>& vRaw,
        std::vector<uchar_vector>& vMsgInPb
    ) = 0;
    virtual JUB_RV SignTXAPTOSDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;
}; // class APTOSTokenInterface end


} // namespace jub end

#endif
