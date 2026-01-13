//
//  JubiterSealer2100ImplTRX.cpp
//  JubSDK
//
//  Created by Pan Min on 2025/4/26.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#include "token/JubiterSealer2100Impl.h"
#include "common/protocpp/messages-ripple.pb.h"
#include "common/protocpp/messages-bitcoin.pb.h"
#include "Ripple/Address.h"
#include "Ripple/Transaction.h"
#include "utils/logUtils.h"
#include "mSIGNA/stdutils/uchar_vector.h"
#include "BinaryCoding.h"
#include <TrezorCrypto/base58.h>
#include <bigint/BigIntegerUtils.hh>
#include "JUB_SDK_XRP.h"


#include <variant>

using namespace hw::trezor::messages::ripple;
using namespace hw::trezor::messages::bitcoin;
namespace jub {
    enum FILMessageType
    {
        JUB_ENUM_PROTOCOL_XRP_GET_PUBLICKEY = 11,//复用BTC id
        JUB_ENUM_PROTOCOL_XRP_PUBLICKEY = 12,
        JUB_ENUM_PROTOCOL_XRP_GET_ADDRESS = 400,
        JUB_ENUM_PROTOCOL_XRP_ADDRESS = 401,
        JUB_ENUM_PROTOCOL_XRP_SignTx = 402,
        JUB_ENUM_PROTOCOL_XRP_SignedTx = 403,
    };

JUB_RV JubiterSealer2100Impl::GetAddressXRP(
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

        RippleGetAddress msgToBeEncode;
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

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_XRP_GET_ADDRESS, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_XRP_ADDRESS)
    {
        return JUBR_ERROR;
    }

    try
    {
        RippleAddress msgToBeDecode;
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


JUB_RV JubiterSealer2100Impl::GetHDNodeXRP(
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

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_XRP_GET_PUBLICKEY, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_XRP_PUBLICKEY)
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

JUB_RV JubiterSealer2100Impl::SetCoinTypeXRP()
{
    return JUBR_OK;
}
JUB_RV JubiterSealer2100Impl::SignTXXRP_2100(const std::vector<JUB_BYTE>& vPath,
                                             const JUB_TX_XRP& tx,
                                             std::vector<JUB_BYTE>& vUnsignedRaw,
                                             std::string& signedRaw,
                      std::vector<uchar_vector>& vSignatureRaw)
{
    if (!tx.isValid()) {
        return JUBR_ARGUMENTS_BAD;
    }
    std::string vMsgInPb;
    try
    {
        TW::Ripple::Transaction _tx;
        _tx.setPreImage(vUnsignedRaw);

        std::string path(vPath.begin(), vPath.end());
        std::string pubkey;
        JUB_VERIFY_RV(GetHDNodeXRP((JUB_BYTE)JUB_ENUM_PUB_FORMAT::HEX, path, pubkey));
        uchar_vector vPubkey(pubkey);
        _tx.pub_key.insert(_tx.pub_key.end(), vPubkey.begin(), vPubkey.end());
        _tx.serialize();
        vUnsignedRaw.clear();
        vUnsignedRaw = _tx.getPreImage();

        Address address = parse_path(std::string(vPath.begin(), vPath.end()));

        RippleSignTx msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_fee(stringToBigInteger(tx.fee).toLong());
        msgToBeEncode.set_sequence(stringToBigInteger(tx.sequence).toInt());
        msgToBeEncode.set_flags(stringToBigInteger(tx.flags).toLong());
        JUB_UINT32 lastledgerseq = stringToBigInteger(tx.lastLedgerSequence).toInt();
        if(lastledgerseq > 0) {
            msgToBeEncode.set_last_ledger_sequence(
                    stringToBigInteger(tx.lastLedgerSequence).toInt());
        }
        // 填充 RipplePayment
        RippleSignTx_RipplePayment* payment = msgToBeEncode.mutable_payment();
        payment->set_amount(stringToBigInteger(tx.pymt.amount.value).toLong());
        payment->set_destination(tx.pymt.destination);
        JUB_UINT32 desttag = stringToBigInteger(tx.pymt.destinationTag).toLong();
        if(desttag > 0) {
            payment->set_destination_tag(stringToBigInteger(tx.pymt.destinationTag).toLong());
        }

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
    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_XRP_SignTx, vMsgInPb, &recvType, vMsgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_XRP_SignedTx)
    {
        return JUBR_ERROR;
    }

    try
    {
        RippleSignedTx msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(vMsgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        signedRaw = bytes_to_hex_fast(msgToBeDecode.serialized_tx());
        uchar_vector signatureRaw(msgToBeDecode.signature().begin(), msgToBeDecode.signature().end());
        vSignatureRaw.push_back(signatureRaw);
        return JUBR_OK;
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
}

JUB_RV JubiterSealer2100Impl::SignTXXRP(
    const std::vector<JUB_BYTE>& vPath,
    std::vector<JUB_BYTE>& vUnsignedRaw,
    std::vector<uchar_vector>& vSignatureRaw
) {
    return JUBR_IMPL_NOT_SUPPORT;
}
} // namespace jub end
