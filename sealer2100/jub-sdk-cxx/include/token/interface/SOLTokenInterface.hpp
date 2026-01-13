//
//  SOLTokenInterface.hpp
//  JubSDK
//
//  Created by Pan Min on 2022/8/25.
//  Copyright © 2022 JuBiter. All rights reserved.
//

#ifndef __SOLTokenInterface__
#define __SOLTokenInterface__

#include <vector>

#include "mSIGNA/stdutils/uchar_vector.h"
#include "utility/util.h"
#include <uint256_t/uint256_t.h>

namespace jub {

class SOLTokenInterface {

public:
    virtual JUB_RV SelectAppletSOL() = 0;
    virtual JUB_RV SetCoinTypeSOL() = 0;

    virtual JUB_RV GetAddressSOL(const std::string& path, const JUB_UINT16 tag, std::string& address) = 0;
    virtual JUB_RV GetAddressSOLEncodePb(
        const std::string& path,
        const JUB_UINT16 tag,
        std::string& msgInPb
    ) = 0;
    virtual JUB_RV GetAddressSOLDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;

    virtual JUB_RV GetHDNodeSOL(const JUB_BYTE format, const std::string& path, std::string& pubkey) = 0;
    virtual JUB_RV GetHDNodeSOLEncodePb(
        const JUB_BYTE format,
        const std::string& path,
        std::string& msgInPb
    ) = 0;
    virtual JUB_RV GetHDNodeSOLDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;

    virtual JUB_RV SetTokenInfo(const std::string &name, JUB_UINT8 decimal, const std::string &tokenMint) = 0;

    // Trasfer `SOL`
    virtual JUB_RV SignTransferTxSOl_2100(const std::string &path,const JUB_CHAR_CPTR recentHash,const JUB_CHAR_CPTR dest,JUB_UINT64 amount,std::string &raw) = 0;
    virtual JUB_RV SignTransferTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
                                  const std::vector<JUB_BYTE> &dest, JUB_UINT64 amount, std::vector<JUB_BYTE> &raw) = 0;

    virtual JUB_RV SignTransferTxEncodePb(
        const std::string &path,
        const std::vector<JUB_BYTE> &recentHash,
        const std::vector<JUB_BYTE> &dest,
        JUB_UINT64 amount,
        std::vector<JUB_BYTE> &vMsgInPb
    ) = 0;
    virtual JUB_RV SignTransferTxDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;

    // Trasfer `Token`
    virtual JUB_RV SignTokenTransferTxSOL_2100(const std::string &path,
                                               const JUB_CHAR_CPTR recentHash,
                                               const JUB_CHAR_CPTR token,
                                               const JUB_CHAR_CPTR &from,
                                               const JUB_CHAR_CPTR &dest,
                                               JUB_UINT64 amount,
                                               JUB_UINT8 decimal,
                                               std::string &raw) = 0;
    virtual JUB_RV SignTokenCreateAndTransferTxSOL_2100(const std::string &path,
                                                        const JUB_CHAR_CPTR recentHash,
                                                        const JUB_CHAR_CPTR token,
                                                        const JUB_CHAR_CPTR destMainAddress,
                                                        const JUB_CHAR_CPTR &from,
                                                        const JUB_CHAR_CPTR &dest,
                                                        JUB_UINT64 amount,
                                                        JUB_UINT8 decimal,
                                                        std::string &raw) = 0;
    virtual JUB_RV SignTokenTransferTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
                                       const std::vector<JUB_BYTE> token, const std::vector<JUB_BYTE> &from,
                                       const std::vector<JUB_BYTE> &dest, JUB_UINT64 amount, JUB_UINT8 decimal,
                                       std::vector<JUB_BYTE> &raw) = 0;
    virtual JUB_RV SignTokenTransferTxEncodePb(
        const std::string &path,
        const std::vector<JUB_BYTE> &recentHash,
        const std::vector<JUB_BYTE> token,
        const std::vector<JUB_BYTE> &from,
        const std::vector<JUB_BYTE> &dest,
        JUB_UINT64 amount,
        JUB_UINT8 decimal,
        std::vector<JUB_BYTE> &vMsgInPb
    ) = 0;
    virtual JUB_RV SignTokenTransferTxDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;

    // create token address
    virtual JUB_RV SignCreateTokenAccountTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
                                            const std::vector<JUB_BYTE> &owner, const std::vector<JUB_BYTE> &token,
                                            std::vector<JUB_BYTE> &raw) = 0;
    virtual JUB_RV SignCreateTokenAccountTxEncodePb(
        const std::string &path,
        const std::vector<JUB_BYTE> &recentHash,
        const std::vector<JUB_BYTE> &owner,
        const std::vector<JUB_BYTE> &token,
        std::vector<JUB_BYTE> &vMsgInPb
    ) = 0;
    virtual JUB_RV SignCreateTokenAccountTxDecodePb(
        const std::string& msgInPb,
        std::string& address
    ) = 0;
}; // class SOLTokenInterface end

} // namespace jub end

#endif
