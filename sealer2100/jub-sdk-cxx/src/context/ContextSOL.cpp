//
//  ContextSOL.cpp
//  JubSDK
//
//  Created by Pan Min on 2022/7/20.
//  Copyright © 2022 JuBiter. All rights reserved.
//

#include "utility/util.h"

#include "context/ContextSOL.h"
#include "token/interface/TokenInterface.hpp"

#include "Solana/Address.h"
#include "Solana/Signer.h"
#include "Solana/Transaction.h"
#include "token/JubiterSealer2100Impl.h"


namespace jub {

JUB_RV ContextSOL::ActiveSelf() {

    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);
    JUB_CHECK_NULL(ctoken);

    JUB_VERIFY_RV(token->SelectAppletSOL());
    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));
    JUB_VERIFY_RV(token->SetCoinTypeSOL());

    return JUBR_OK;
}
std::string ContextSOL::_FullBip32Path(const BIP32_Path& path) {
    // 将_mainPath的倒数第二位替换为 path 中的 index 并加上硬化
    // 拆分 _mainPath
    std::vector<std::string> parts;
    {
        std::stringstream ss(_mainPath);
        std::string item;
        while (std::getline(ss, item, '/')) {
            if (!item.empty()) parts.push_back(item);
        }
    }

    // 计算 index，去掉可能的硬化标志位并强制加硬化
    uint32_t rawIndex = static_cast<uint32_t>(path.addressIndex) & 0x7FFFFFFFu;
    std::string indexStr = std::to_string(rawIndex) + "'";

    // 替换倒数第二位；若不足两段则尽量合理处理（保留 "m" 前缀）
    if (parts.size() >= 2) {
        parts[parts.size() - 2] = indexStr;
    } else if (parts.size() == 1) {
        // 仅有 "m" 时，追加 index'
        if (parts[0] == "m") {
            parts.push_back(indexStr);
        } else {
            // 单段非 "m" 时，追加 index'
            parts.push_back(indexStr);
        }
    } else {
        // 空路径时，构造 "m/index'"
        parts.push_back("m");
        parts.push_back(indexStr);
    }

    // 重新拼接路径
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i) oss << "/";
        oss << parts[i];
    }
    return oss.str();
}
JUB_RV ContextSOL::GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address) {

    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressSOL(strPath, tag, address));

    return JUBR_OK;
}
JUB_RV ContextSOL::GetMainHDNode(const JUB_BYTE format, std::string& xpub) {

    //0x00 for hex, 0x01 for xpub
    if (   JUB_ENUM_PUB_FORMAT::HEX  != format
        && JUB_ENUM_PUB_FORMAT::XPUB != format
        ) {
        return JUBR_ERROR_ARGS;
    }

    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->GetHDNodeSOL(format, _mainPath, xpub));

    return JUBR_OK;
}

JUB_RV ContextSOL::GetHDNode(const JUB_BYTE& format, const BIP32_Path& path, std::string& pubkey) {

    //0x00 for hex, 0x01 for xpub
    if (   JUB_ENUM_PUB_FORMAT::HEX  != format
        && JUB_ENUM_PUB_FORMAT::XPUB != format
        ) {
        return JUBR_ERROR_ARGS;
    }

    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetHDNodeSOL(format, strPath, pubkey));

    return JUBR_OK;
}
JUB_RV ContextSOL::SignTransferTx(BIP32_Path& path,
                                   JUB_CHAR_CPTR recentHash,
                                   JUB_CHAR_CPTR dest,
                                   JUB_UINT64 amount,
                                  std::string &txRaw){


    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    if(dynamic_cast<JubiterSealer2100Impl*>(token)  != nullptr){
        JUB_VERIFY_RV(token->SignTransferTxSOl_2100(strPath,recentHash,dest,amount,txRaw));
        
        return JUBR_OK;
    } else{
        return JUBR_IMPL_NOT_SUPPORT;
    }

}

JUB_RV ContextSOL::SignTokenTransferTx(BIP32_Path& path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR tokenMint,
                                       JUB_CHAR_CPTR source, JUB_CHAR_CPTR dest, JUB_UINT64 amount, JUB_UINT8 decimal,
                                       std::string &txRaw) {

    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    if(dynamic_cast<JubiterSealer2100Impl*>(token)  != nullptr){
        JUB_VERIFY_RV(token->SignTokenTransferTxSOL_2100(strPath,recentHash,tokenMint,source,dest,amount,decimal,txRaw));
        
        return JUBR_OK;
    } else{
        return JUBR_IMPL_NOT_SUPPORT;
    }
}
JUB_RV ContextSOL::SignTokenCreateAndTransferTx(BIP32_Path& path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR tokenMint,JUB_CHAR_CPTR destMainAddress,
                                       JUB_CHAR_CPTR source, JUB_CHAR_CPTR dest, JUB_UINT64 amount, JUB_UINT8 decimal,
                                       std::string &txRaw) {

    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    if(dynamic_cast<JubiterSealer2100Impl*>(token)  != nullptr){
        JUB_VERIFY_RV(token->SignTokenCreateAndTransferTxSOL_2100(strPath,recentHash,tokenMint,destMainAddress,source,dest,amount,decimal,txRaw));
        
        return JUBR_OK;
    } else{
        return JUBR_IMPL_NOT_SUPPORT;
    }
}

//
//
//namespace {
//inline TW::Data a2d(JUB_CHAR_CPTR a) { return TW::Solana::Address{a}.vector(); }
//} // namespace
//
//
//JUB_RV ContextSOL::SetToken(JUB_CHAR_CPTR name, JUB_UINT8 decimal, JUB_CHAR_CPTR tokenMint) {
//
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->SetTokenInfo(name, decimal, tokenMint));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::SignTransferTx(JUB_CHAR_CPTR path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR dest, JUB_UINT64 amount,
//                                  std::string &txRaw) {
//
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    auto raw = TW::Data{};
//
//    JUB_VERIFY_RV(token->SignTransferTx(path, a2d(recentHash), a2d(dest), amount, raw));
////#if defined(DEBUG)
//    // verify
//    std::string pubkey;
//    JUB_VERIFY_RV(token->GetHDNodeSOL((JUB_BYTE)JUB_ENUM_PUB_FORMAT::HEX, path, pubkey));
//    auto keys = std::vector<TW::PublicKey>{TW::PublicKey{uchar_vector{pubkey}, TWPublicKeyTypeED25519}};
//
//    auto signer = TW::Solana::Signer{};
//    auto tx     = TW::Solana::Transaction::decode(raw);
//    JUB_VERIFY_RV(signer.verify(keys, tx));
////#endif
//    txRaw = TW::Base58::bitcoin.encode(raw);
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::SignTokenTransferTx(JUB_CHAR_CPTR path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR tokenMint,
//                                       JUB_CHAR_CPTR source, JUB_CHAR_CPTR dest, JUB_UINT64 amount, JUB_UINT8 decimal,
//                                       std::string &txRaw) {
//
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    auto raw = TW::Data{};
//
//    JUB_VERIFY_RV(token->SignTokenTransferTx(path, a2d(recentHash), a2d(tokenMint), a2d(source), a2d(dest), amount,
//                                             decimal, raw));
//#if defined(DEBUG)
//    // verify
//    std::string pubkey;
//    JUB_VERIFY_RV(token->GetHDNodeSOL((JUB_BYTE)JUB_ENUM_PUB_FORMAT::HEX, path, pubkey));
//    auto keys = std::vector<TW::PublicKey>{TW::PublicKey{uchar_vector{pubkey}, TWPublicKeyTypeED25519}};
//
//    auto signer = TW::Solana::Signer{};
//    auto tx     = TW::Solana::Transaction::decode(raw);
//    JUB_VERIFY_RV(signer.verify(keys, tx));
//#endif
//    txRaw = TW::Base58::bitcoin.encode(raw);
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::SignCreateTokenAccountTx(JUB_CHAR_CPTR path, JUB_CHAR_CPTR recentHash, JUB_CHAR_CPTR owner,
//                                            JUB_CHAR_CPTR tokenMint, std::string &txRaw) {
//
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    auto raw = TW::Data{};
//
//    JUB_VERIFY_RV(token->SignCreateTokenAccountTx(path, a2d(recentHash), a2d(owner), a2d(tokenMint), raw));
//#if defined(DEBUG)
//    // verify
//    std::string pubkey;
//    JUB_VERIFY_RV(token->GetHDNodeSOL((JUB_BYTE)JUB_ENUM_PUB_FORMAT::HEX, path, pubkey));
//    auto keys = std::vector<TW::PublicKey>{TW::PublicKey{uchar_vector{pubkey}, TWPublicKeyTypeED25519}};
//
//    auto signer = TW::Solana::Signer{};
//    auto tx     = TW::Solana::Transaction::decode(raw);
//    JUB_VERIFY_RV(signer.verify(keys, tx));
//#endif
//    txRaw = TW::Base58::bitcoin.encode(raw);
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::GetAddressEncodePb(
//    const std::string& path,
//    const JUB_UINT16 tag,
//    std::string& msgInPb
//) {
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetAddressSOLEncodePb(path, tag, msgInPb));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::GetAddressDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetAddressSOLDecodePb(msgInPb, address));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::GetHDNodeEncodePb(
//    const JUB_BYTE format,
//    const std::string &path,
//    std::string &msgInPb
//) {
//    //0x00 for hex
//    if (JUB_ENUM_PUB_FORMAT::HEX  != format) {
//        return JUBR_ERROR_ARGS;
//    }
//
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeSOLEncodePb(format, path, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::GetHDNodeDecodePb(
//    const std::string& msgInPb,
//    std::string& pubkey
//) {
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeSOLDecodePb(msgInPb, pubkey));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::GetMainHDNodeEncodePb(
//    const JUB_BYTE format,
//    std::string &msgInPb
//) {
//
//    //0x00 for hex
//    if (JUB_ENUM_PUB_FORMAT::HEX  != format) {
//        return JUBR_ERROR_ARGS;
//    }
//
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeSOLEncodePb(format, _mainPath, msgInPb));
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::GetMainHDNodeDecodePb(
//    const std::string& msgInPb,
//    std::string& xpub
//) {
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->GetHDNodeSOLDecodePb(msgInPb, xpub));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::SignTransferTxEncodePb(
//    JUB_CHAR_CPTR path,
//    JUB_CHAR_CPTR recentHash,
//    JUB_CHAR_CPTR dest,
//    JUB_UINT64 amount,
//    std::string &msgInPb
//) {
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    auto raw = TW::Data{};
//
//    JUB_VERIFY_RV(token->SignTransferTxEncodePb(path, a2d(recentHash), a2d(dest), amount, raw));
////#if defined(DEBUG)
////    // verify
////    std::string pubkey;
////    JUB_VERIFY_RV(token->GetHDNodeSOL((JUB_BYTE)JUB_ENUM_PUB_FORMAT::HEX, path, pubkey));
////    auto keys = std::vector<TW::PublicKey>{TW::PublicKey{uchar_vector{pubkey}, TWPublicKeyTypeED25519}};
////
////    auto signer = TW::Solana::Signer{};
////    auto tx     = TW::Solana::Transaction::decode(raw);
////    JUB_VERIFY_RV(signer.verify(keys, tx));
////#endif
////    txRaw = TW::Base58::bitcoin.encode(raw);
//
//    uchar_vector vMsgInPb(raw);
//    msgInPb = vMsgInPb.getHex();
//
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::SignTransferTxDecodePb(
//    const std::string& msgInPb,
//    std::string& txRaw
//) {
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->SignTransferTxDecodePb(msgInPb, txRaw));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::SignTokenTransferTxEncodePb(
//    JUB_CHAR_CPTR path,
//    JUB_CHAR_CPTR recentHash,
//    JUB_CHAR_CPTR tokenMint,
//    JUB_CHAR_CPTR source,
//    JUB_CHAR_CPTR dest,
//    JUB_UINT64 amount,
//    JUB_UINT8 decimal,
//    std::string &msgInPb
//) {
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV ContextSOL::SignTokenTransferTxDecodePb(
//    const std::string& msgInPb,
//    std::string& txRaw
//) {
//    auto token = dynamic_cast<SOLTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
//    JUB_CHECK_NULL(token);
//
//    JUB_VERIFY_RV(token->SignTransferTxDecodePb(msgInPb, txRaw));
//    return JUBR_OK;
//}
//
//
//JUB_RV ContextSOL::SignCreateTokenAccountTxEncodePb(
//    JUB_CHAR_CPTR path,
//    JUB_CHAR_CPTR recentHash,
//    JUB_CHAR_CPTR owner,
//    JUB_CHAR_CPTR tokenMint,
//    std::string &msgInPb
//) {
//    // TODO
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV ContextSOL::SignCreateTokenAccountTxDecodePb(
//    const std::string& msgInPb,
//    std::string& txRaw
//) {
//    // TODO
//    return JUBR_IMPL_NOT_SUPPORT;
//}
} // namespace jub
