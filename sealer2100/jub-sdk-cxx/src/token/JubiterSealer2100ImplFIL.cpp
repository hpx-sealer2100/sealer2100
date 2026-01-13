////
////  JubiterSealer2100ImplFIL.cpp
////  JubSDK
////
////  Created by Pan Min on 2025/4/26.
////  Copyright © 2025 ZianCube. All rights reserved.
////

#include "token/JubiterSealer2100Impl.h"
#include "common/protocpp/messages-filecoin.pb.h"
#include "common/protocpp/messages-bitcoin.pb.h"
#include "utils/logUtils.h"
#include "mSIGNA/stdutils/uchar_vector.h"
#include "BinaryCoding.h"
#include <TrezorCrypto/base58.h>
#include <Filecoin/Signer.h>

//#include "cbor.h"


#include <variant>

using namespace hw::trezor::messages::filecoin;
using namespace hw::trezor::messages::bitcoin;
namespace jub {

    enum FILMessageType
    {
        JUB_ENUM_PROTOCOL_FIL_GET_PUBLICKEY = 11,//复用BTC id
        JUB_ENUM_PROTOCOL_FIL_PUBLICKEY = 12,
        JUB_ENUM_PROTOCOL_FIL_GET_ADDRESS = 11200,
        JUB_ENUM_PROTOCOL_FIL_ADDRESS = 11201,
        JUB_ENUM_PROTOCOL_FIL_SignTx = 11202,
        JUB_ENUM_PROTOCOL_FIL_SignedTx = 11203,
    };

JUB_RV JubiterSealer2100Impl::GetAddressFIL(
    const std::string& path,
    const JUB_UINT16 tag,
    std::string& address
) {

    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    try
    {

        Address address = parse_path(path);

        FilecoinGetAddress msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            msgToBeEncode.add_address_n(address.path[i]);
        }
        if( 0 == tag)
            msgToBeEncode.set_show_display(false);
        else
            msgToBeEncode.set_show_display(true);

        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb))
        {
            return JUBR_ERROR;
        }
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_FIL_GET_ADDRESS, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_FIL_ADDRESS)
    {
        return JUBR_ERROR;
    }

    try
    {
        FilecoinAddress msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(msgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }

        if (msgToBeDecode.has_address())
        {
            address = msgToBeDecode.address();
        }
        else
        {
            rv = JUBR_ARGUMENTS_BAD;
        }
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }

    return rv;
}
JUB_RV JubiterSealer2100Impl::GetHDNodeFIL(
    const JUB_BYTE format,
    const std::string& path,
    std::string& pubkey
) {
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    InputScriptType ScriptType = SPENDADDRESS;//取xpub

    try
    {
        Address address = parse_path(path);

        GetPublicKey msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_show_display(false);
        msgToBeEncode.set_script_type(ScriptType);

        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb))
        {
            return JUBR_ERROR;
        }
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_FIL_GET_PUBLICKEY, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_FIL_PUBLICKEY)
    {
        return JUBR_ERROR;
    }

    try
    {
        PublicKey msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(msgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }

        if (msgToBeDecode.has_xpub())
        {
            if(JUB_ENUM_PUB_FORMAT::HEX  == format)
            {
                uint8_t decoded[112] = {0};  // 足够大的缓冲区
                int decoded_len = hyp_base58_decode_check(msgToBeDecode.xpub().c_str(), HASHER_SHA2D, decoded, sizeof(decoded));
                if (decoded_len != 78) {  // xpub 固定长度为 78 字节
                    return JUBR_ARGUMENTS_BAD;
                }
                const uint8_t* pubkey_bin = decoded + 45;  // 78 - 33 = 45
                pubkey = bin_to_hex(pubkey_bin, 33);
                //unsigned char byte = 0xFF;//为了保持与g2一致，临时补0XFF
                //pubkey.insert(0,1,static_cast<char>(byte));
            }
            else
            {
                pubkey = msgToBeDecode.xpub();
            }
        }
        else
        {
            rv = JUBR_ARGUMENTS_BAD;
        }
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    return JUBR_OK;
}
JUB_RV JubiterSealer2100Impl::SetCoinTypeFIL()
{
    return JUBR_OK;
}


JUB_RV JubiterSealer2100Impl::SignTXFIL(
    const uint64_t& nonce,
    const uint256_t& glimit,
    const uint256_t& gfeeCap,
    const uint256_t& gpremium,
    const std::string& to,
    const uint256_t& value,
    const std::string& input,
    const std::string& path,
    std::vector<uchar_vector>& vSignatureRaw
) {

    std::string vMsgInPb;
    try
    {
        //cbor编码
        std::string from;
        JUB_VERIFY_RV(GetAddressFIL(path, 0, from));
        uint64_t gaslimit = glimit;
        TW::Filecoin::Transaction tx(TW::Filecoin::Address(to),
                                     TW::Filecoin::Address(from),
                                     nonce,
                                     value,
                                     gaslimit,
                                     gfeeCap,
                                     gpremium);
        
        Address address = parse_path(path);

        FilecoinSignTx msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_raw_tx(tx.message().encoded().data(),tx.message().encoded().size());

        if (!msgToBeEncode.SerializeToString(&vMsgInPb))
        {
            return JUBR_ERROR;
        }
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_UINT16 recvType;
    std::string vMsgOutPb;
    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_FIL_SignTx, vMsgInPb, &recvType, vMsgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_FIL_SignedTx)
    {
        return JUBR_ERROR;
    }

    try
    {
        FilecoinSignedTx msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(vMsgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        uchar_vector signatureRaw(msgToBeDecode.signature().begin(), msgToBeDecode.signature().end());
        vSignatureRaw.push_back(signatureRaw);
        return JUBR_OK;
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
}
} // namespace jub end
