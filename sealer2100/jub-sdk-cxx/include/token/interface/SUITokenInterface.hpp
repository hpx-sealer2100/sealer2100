//
//  SUITokenInterface.hpp
//  JubSDK
//
//  Created by Pan Min on 2025/5/5.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#ifndef __SUITokenInterface__
#define __SUITokenInterface__
#include <vector>

namespace jub {


class SUITokenInterface {
public:
    virtual ~SUITokenInterface() = default;

    virtual JUB_RV SelectAppletSUI() = 0;
    virtual JUB_RV SetCoinTypeSUI() = 0;

}; // class SUITokenInterface end


} // namespace jub end

#endif
