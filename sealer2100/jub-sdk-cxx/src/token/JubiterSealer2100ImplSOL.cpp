//
//  JubiterSealer2100ImplSOL.cpp
//  JubSDK
//
//  Created by Pan Min on 2025/4/26.
//  Copyright © 2025 ZianCube. All rights reserved.
//
#include "token/JubiterSealer2100Impl.h"
#include "common/protocpp/messages-solana.pb.h"
#include "common/protocpp/messages-bitcoin.pb.h"
#include "utils/logUtils.h"
#include "mSIGNA/stdutils/uchar_vector.h"
#include "BinaryCoding.h"
#include <TrezorCrypto/base58.h>
#include "Solana/Address.h"
#include "Solana/Signer.h"
#include "Solana/Transaction.h"
#include <cstdio>
using namespace hw::trezor::messages::solana;
using namespace hw::trezor::messages::bitcoin;

namespace jub {
enum FILMessageType
{
    JUB_ENUM_PROTOCOL_SOL_GET_PUBLICKEY = 11,//复用BTC id
    JUB_ENUM_PROTOCOL_SOL_PUBLICKEY = 12,
    JUB_ENUM_PROTOCOL_SOL_GET_ADDRESS = 10100,
    JUB_ENUM_PROTOCOL_SOL_ADDRESS = 10101,
    JUB_ENUM_PROTOCOL_SOL_SignTx = 10102,
    JUB_ENUM_PROTOCOL_SOL_SignedTx = 10103,
};

JUB_RV JubiterSealer2100Impl::GetAddressSOL(
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

        SolanaGetAddress msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            //sol要求都是硬化path
            if(address.path[i] < 0x80000000){
                return JUBR_ARGUMENTS_BAD;
            }
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

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_SOL_GET_ADDRESS, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_SOL_ADDRESS)
    {
        return JUBR_ERROR;
    }

    try
    {
        SolanaAddress msgToBeDecode;
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
JUB_RV JubiterSealer2100Impl::SignTransferTxSOl_2100(
        const std::string &path,
        const JUB_CHAR_CPTR recentHash,
        const JUB_CHAR_CPTR dest,
        JUB_UINT64 amount,
        std::string &raw)
{
    //获取from
    std::string str_from;
    JUB_VERIFY_RV(GetAddressSOL(path,false,str_from));
    TW::Solana::Address from(str_from);
    TW::Solana::Address to(dest);

    auto message = TW::Solana::Message::createTransfer(from, to, amount, TW::Solana::Hash::fromBase58(recentHash));
    TW::Solana::Transaction unsignedTx(message);
    TW::Data twraw =  unsignedTx.messageData();

    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    try
    {

        Address address = parse_path(path);

        SolanaSignTx msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            //sol要求都是硬化path
            if(address.path[i] < 0x80000000){
                return JUBR_ARGUMENTS_BAD;
            }
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_raw_tx(twraw.data(),twraw.size());

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

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_SOL_SignTx, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_SOL_SignedTx)
    {
        return JUBR_ERROR;
    }

    try
    {
        SolanaSignedTx msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(msgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        const std::string& signatureData = msgToBeDecode.signature();
        TW::Data signatureBytes(signatureData.begin(), signatureData.end());
        TW::Solana::Signature signature(signatureBytes);
        TW::Solana::Transaction signedTx = unsignedTx;
        if (!signedTx.signatures.empty()) {
            signedTx.signatures[0] = signature;
        } else {
            throw std::runtime_error("No signature slot available in transaction");
        }
        //TW::Data serializedData = signedTx.serialize();

        uchar_vector vTx(signedTx.serialize());
        std::string  hexString = vTx.getHex();
        raw.assign(hexString.begin(), hexString.end());
        //raw.assign(serializedData.begin(), serializedData.end());
        //return JUBR_OK;

    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    return rv;

}
JUB_RV JubiterSealer2100Impl::SignTokenCreateAndTransferTxSOL_2100(const std::string &path,
                                                            const JUB_CHAR_CPTR recentHash,
                                                            const JUB_CHAR_CPTR token,
                                                            const JUB_CHAR_CPTR destMainAddress,
                                                            const JUB_CHAR_CPTR &from,
                                                            const JUB_CHAR_CPTR &dest,
                                                            JUB_UINT64 amount,
                                                            JUB_UINT8 decimal,
                                                            std::string &raw) {
    //获取from
    std::string str_from;
    JUB_VERIFY_RV(GetAddressSOL(path,false,str_from));
    TW::Solana::Address signer(str_from);
    TW::Solana::Address tokenMint(token);
    TW::Solana::Address sender(from);
    TW::Solana::Address to(dest);
    TW::Solana::Address toaccount(destMainAddress);

    auto message = TW::Solana::Message::createTokenCreateAndTransfer(signer, toaccount,tokenMint, to, sender, amount, decimal, TW::Solana::Hash::fromBase58(recentHash));

    TW::Solana::Transaction unsignedTx(message);
    TW::Data twraw =  unsignedTx.messageData();
    std::cout << bin_to_hex(twraw.data(), twraw.size()) << std::endl;

    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    try
    {

        Address address = parse_path(path);

        SolanaSignTx msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            //sol要求都是硬化path
            if(address.path[i] < 0x80000000){
                return JUBR_ARGUMENTS_BAD;
            }
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_raw_tx(twraw.data(),twraw.size());

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

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_SOL_SignTx, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_SOL_SignedTx)
    {
        return JUBR_ERROR;
    }

    try
    {
        SolanaSignedTx msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(msgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        const std::string& signatureData = msgToBeDecode.signature();
        TW::Data signatureBytes(signatureData.begin(), signatureData.end());
        TW::Solana::Signature signature(signatureBytes);
        TW::Solana::Transaction signedTx = unsignedTx;
        if (!signedTx.signatures.empty()) {
            signedTx.signatures[0] = signature;
        } else {
            throw std::runtime_error("No signature slot available in transaction");
        }
        //TW::Data serializedData = signedTx.serialize();

        uchar_vector vTx(signedTx.serialize());
        std::string  hexString = vTx.getHex();
        raw.assign(hexString.begin(), hexString.end());
        //raw.assign(serializedData.begin(), serializedData.end());
        //return JUBR_OK;

    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    return rv;
}
JUB_RV JubiterSealer2100Impl::SignTokenTransferTxSOL_2100(const std::string &path,
                                                          const JUB_CHAR_CPTR recentHash,
                                                          const JUB_CHAR_CPTR token,
                                                          const JUB_CHAR_CPTR &from,
                                                          const JUB_CHAR_CPTR &dest,
                                                          JUB_UINT64 amount,
                                                          JUB_UINT8 decimal,
                                                          std::string &raw){
    //获取from
    std::string str_from;
    JUB_VERIFY_RV(GetAddressSOL(path,false,str_from));
    TW::Solana::Address signer(str_from);
    TW::Solana::Address tokenMint(token);
    TW::Solana::Address sender(from);
    TW::Solana::Address to(dest);
    
    auto message = TW::Solana::Message::createTokenTransfer(signer, tokenMint, sender, to, amount, decimal, TW::Solana::Hash::fromBase58(recentHash));

    TW::Solana::Transaction unsignedTx(message);
    TW::Data twraw =  unsignedTx.messageData();
    std::cout << bin_to_hex(twraw.data(), twraw.size()) << std::endl;

    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    try
    {

        Address address = parse_path(path);

        SolanaSignTx msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            //sol要求都是硬化path
            if(address.path[i] < 0x80000000){
                return JUBR_ARGUMENTS_BAD;
            }
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_raw_tx(twraw.data(),twraw.size());

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

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_SOL_SignTx, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_SOL_SignedTx)
    {
        return JUBR_ERROR;
    }

    try
    {
        SolanaSignedTx msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(msgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        const std::string& signatureData = msgToBeDecode.signature();
        TW::Data signatureBytes(signatureData.begin(), signatureData.end());
        TW::Solana::Signature signature(signatureBytes);
        TW::Solana::Transaction signedTx = unsignedTx;
        if (!signedTx.signatures.empty()) {
            signedTx.signatures[0] = signature;
        } else {
            throw std::runtime_error("No signature slot available in transaction");
        }
        //TW::Data serializedData = signedTx.serialize();

        uchar_vector vTx(signedTx.serialize());
        std::string  hexString = vTx.getHex();
        raw.assign(hexString.begin(), hexString.end());
        //raw.assign(serializedData.begin(), serializedData.end());
        //return JUBR_OK;

    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    return rv;
    
}

JUB_RV JubiterSealer2100Impl::GetHDNodeSOL(
    const JUB_BYTE format,
    const std::string& path,
    std::string& pubkey
) {
    return JUBR_IMPL_NOT_SUPPORT;//sol不支持取hdnode
//    std::string msgInPb;
//    std::string msgOutPb;
//    JUB_UINT16 recvType;
//    InputScriptType ScriptType = SPENDADDRESS;//取xpub
//
//    try
//    {
//        Address address = parse_path(path);
//
//        GetPublicKey msgToBeEncode;
//        msgToBeEncode.Clear();
//
//        for (size_t i = 0; i < address.path.size(); ++i)
//        {
//            //sol要求都是硬化path
//            if(address.path[i] < 0x80000000){
//                return JUBR_ARGUMENTS_BAD;
//            }
//            msgToBeEncode.add_address_n(address.path[i]);
//        }
//        msgToBeEncode.set_show_display(false);
//        msgToBeEncode.set_script_type(ScriptType);
//
//        // Encode the message using standard protobuf
//        if (!msgToBeEncode.SerializeToString(&msgInPb))
//        {
//            return JUBR_ERROR;
//        }
//    }
//    catch (...)
//    {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_SOL_GET_PUBLICKEY, msgInPb, &recvType, msgOutPb);
//    if (JUBR_OK != rv)
//    {
//        return rv;
//    }
//    if (recvType != JUB_ENUM_PROTOCOL_SOL_PUBLICKEY)
//    {
//        return JUBR_ERROR;
//    }
//
//    try
//    {
//        PublicKey msgToBeDecode;
//        if (!msgToBeDecode.ParseFromString(msgOutPb))
//        {
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        if (msgToBeDecode.has_xpub())
//        {
//            if(JUB_ENUM_PUB_FORMAT::HEX  == format)
//            {
//                uint8_t decoded[112] = {0};  // 足够大的缓冲区
//                int decoded_len = hyp_base58_decode_check(msgToBeDecode.xpub().c_str(), HASHER_SHA2D, decoded, sizeof(decoded));
//                if (decoded_len != 78) {  // xpub 固定长度为 78 字节
//                    return JUBR_ARGUMENTS_BAD;
//                }
//                const uint8_t* pubkey_bin = decoded + 45;  // 78 - 33 = 45
//                pubkey = bin_to_hex(pubkey_bin, 33);
//                //unsigned char byte = 0xFF;//为了保持与g2一致，临时补0XFF
//                //pubkey.insert(0,1,static_cast<char>(byte));
//            }
//            else
//            {
//                pubkey = msgToBeDecode.xpub();
//            }
//        }
//        else
//        {
//            rv = JUBR_ARGUMENTS_BAD;
//        }
//    }
//    catch (...)
//    {
//        return JUBR_ARGUMENTS_BAD;
//    }
//    return JUBR_OK;
}
JUB_RV JubiterSealer2100Impl::SetCoinTypeSOL()
{
    return JUBR_OK;
}
//
//
//JUB_RV JubiterSealer2100Impl::SetTokenInfo(
//    const std::string &name,
//    JUB_UINT8 decimal,
//    const std::string &tokenMint
//) {
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//// Trasfer `SOL`
//JUB_RV JubiterSealer2100Impl::SignTransferTx(
//    const std::string &path,
//    const std::vector<JUB_BYTE> &recentHash,
//    const std::vector<JUB_BYTE> &dest,
//    JUB_UINT64 amount,
//    std::vector<JUB_BYTE> &raw
//) {
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//// Trasfer `Token`
//JUB_RV JubiterSealer2100Impl::SignTokenTransferTx(
//    const std::string &path,
//    const std::vector<JUB_BYTE> &recentHash,
//    const std::vector<JUB_BYTE> token, const std::vector<JUB_BYTE> &from,
//    const std::vector<JUB_BYTE> &dest, JUB_UINT64 amount, JUB_UINT8 decimal,
//    std::vector<JUB_BYTE> &raw
//) {
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//// create token address
//JUB_RV JubiterSealer2100Impl::SignCreateTokenAccountTx(
//    const std::string &path, const std::vector<JUB_BYTE> &recentHash,
//    const std::vector<JUB_BYTE> &owner, const std::vector<JUB_BYTE> &token,
//    std::vector<JUB_BYTE> &raw
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//JUB_RV JubiterSealer2100Impl::GetAddressSOLEncodePb(
//    const std::string& path,
//    const JUB_UINT16 tag,
//    std::string& addressInPb
//) {
//    try {
//        uchar_vector path_n(path.begin(), path.end());
//
//        SolanaGetAddress getAddressMsg = SolanaGetAddress_init_zero;
//        if (path_n.size() > sizeof(getAddressMsg.address_n)
//        ) {
//            return JUBR_HOST_MEMORY;
//        }
//
//        memcpy(getAddressMsg.address_n, path_n.data(), path_n.size());
//        getAddressMsg.address_n_count = path_n.size();
//        getAddressMsg.has_show_display = false;
//        getAddressMsg.show_display = false;
//
//        // Encode the message
//        size_t encoded_size = 0;
//        if (!pb_get_encoded_size(&encoded_size, SolanaGetAddress_fields, &getAddressMsg)) {
//            return JUBR_ERROR;
//        }
//
//        uint8_t *encoded = new uint8_t[encoded_size];
//        if (nullptr == encoded) {
//            return JUBR_MEMORY_NULL_PTR;
//        }
//        pb_ostream_t stream = pb_ostream_from_buffer(encoded, encoded_size);
//
//        bool status = pb_encode(&stream, SolanaGetAddress_fields, &getAddressMsg);
//        if (!status) {
//            // Handle encoding error
//            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//            delete [] encoded; encoded = nullptr;
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        addressInPb.assign(reinterpret_cast<const char*>(encoded), stream.bytes_written);
//        delete [] encoded; encoded = nullptr;
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_OK;
//}
//
//
//JUB_RV JubiterSealer2100Impl::GetAddressSOLDecodePb(
//    const std::string& addressInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::GetHDNodeSOLEncodePb(
//    const JUB_BYTE format,
//    const std::string& path,
//    std::string& msgInPb
//) {
//    try {
//        uchar_vector path_n(path.begin(), path.end());
//
//        SolanaGetPublicKey getPublicKey = SolanaGetPublicKey_init_zero;
//        if (path_n.size() > sizeof(getPublicKey.address_n)
//        ) {
//            return JUBR_HOST_MEMORY;
//        }
//
//        memcpy(getPublicKey.address_n, path_n.data(), path_n.size());
//        getPublicKey.address_n_count = path_n.size();
//        getPublicKey.has_show_display = false;
//        getPublicKey.show_display = false;
//
//        // Encode the message
//        size_t encoded_size = 0;
//        if (!pb_get_encoded_size(&encoded_size, SolanaGetPublicKey_fields, &getPublicKey)) {
//            return JUBR_ERROR;
//        }
//
//        uint8_t *encoded = new uint8_t[encoded_size];
//        if (nullptr == encoded) {
//            return JUBR_MEMORY_NULL_PTR;
//        }
//        pb_ostream_t stream = pb_ostream_from_buffer(encoded, encoded_size);
//
//        bool status = pb_encode(&stream, SolanaGetPublicKey_fields, &getPublicKey);
//        if (!status) {
//            // Handle encoding error
//            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//            delete [] encoded; encoded = nullptr;
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        msgInPb.assign(reinterpret_cast<const char*>(encoded), stream.bytes_written);
//        delete [] encoded; encoded = nullptr;
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_OK;
//}
//
//
//JUB_RV JubiterSealer2100Impl::GetHDNodeSOLDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignTransferTxEncodePb(
//    const std::string &path,
//    const std::vector<JUB_BYTE> &recentHash,
//    const std::vector<JUB_BYTE> &dest,
//    JUB_UINT64 amount,
//    std::vector<JUB_BYTE> &vMsgInPb
//) {
//    try {
//        uchar_vector path_n(path.begin(), path.end());
//
//        SolanaSignTx signTx = SolanaSignTx_init_zero;
//        if ((path_n.size() > sizeof(signTx.address_n))
//        ) {
//            return JUBR_HOST_MEMORY;
//        }
//
//        memcpy(signTx.address_n, path_n.data(), path_n.size());
//        signTx.address_n_count = path_n.size();
//
//        // TODO
//
//        // Encode the message
//        size_t encoded_size = 0;
//        if (!pb_get_encoded_size(&encoded_size, SolanaSignTx_fields, &signTx)) {
//            return JUBR_ERROR;
//        }
//
//        uint8_t *encoded = new uint8_t[encoded_size];
//        if (nullptr == encoded) {
//            return JUBR_MEMORY_NULL_PTR;
//        }
//        pb_ostream_t stream = pb_ostream_from_buffer(encoded, encoded_size);
//
//        bool status = pb_encode(&stream, SolanaSignTx_fields, &signTx);
//        if (!status) {
//            // Handle encoding error
//            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//            delete [] encoded; encoded = nullptr;
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        vMsgInPb.insert(vMsgInPb.end(), encoded, encoded+encoded_size);
//        delete [] encoded; encoded = nullptr;
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignTransferTxDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignTokenTransferTxEncodePb(
//    const std::string &path,
//    const std::vector<JUB_BYTE> &recentHash,
//    const std::vector<JUB_BYTE> token,
//    const std::vector<JUB_BYTE> &from,
//    const std::vector<JUB_BYTE> &dest,
//    JUB_UINT64 amount,
//    JUB_UINT8 decimal,
//    std::vector<JUB_BYTE> &vMsgInPb
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignTokenTransferTxDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignCreateTokenAccountTxEncodePb(
//    const std::string &path,
//    const std::vector<JUB_BYTE> &recentHash,
//    const std::vector<JUB_BYTE> &owner,
//    const std::vector<JUB_BYTE> &token,
//    std::vector<JUB_BYTE> &vMsgInPb
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignCreateTokenAccountTxDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
} // namespace jub end
