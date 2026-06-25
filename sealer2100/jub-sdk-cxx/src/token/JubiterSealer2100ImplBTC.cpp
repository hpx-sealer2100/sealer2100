////
////  JubiterSealer2100ImplBTC.cpp
////  JubSDK
////
////  Created by Pan Min on 2025/4/26.
////  Copyright © 2025 ZianCube. All rights reserved.
////
//
#include "token/JubiterSealer2100Impl.h"
#include "common/protocpp/messages-bitcoin.pb.h"
#include "common/protocpp/messages-common.pb.h"
#include "utils/logUtils.h"
#include "mSIGNA/stdutils/uchar_vector.h"
#include "libETH/libETH.hpp"
#include "BinaryCoding.h"
#include <TrezorCrypto/base58.h>
#include "TrezorCrypto/bip32.h"
#include "HDKey/constant.hpp"

#include <variant>

using namespace hw::trezor::messages::bitcoin;
using namespace hw::trezor::messages::common;


namespace jub {

//constexpr JUB_BYTE kMainnetP2PKH = 0x00;
////constexpr JUB_BYTE kMainnetP2SH = 0x01;
//constexpr JUB_BYTE kMainnetP2WPKH = 0x02;
////constexpr JUB_BYTE kMainnetP2WSH = 0x03;
//constexpr JUB_BYTE kMainnetP2SH_P2WPKH = 0x04;
////constexpr JUB_BYTE kMainnetP2SH_P2WSH = 0x05;
//
//constexpr JUB_BYTE kMainnetP2SH_Multisig = 0x11;

enum BTCMessageType
{
    JUB_ENUM_PROTOCOL_BTC_GET_PUBLICKEY = 11,
    JUB_ENUM_PROTOCOL_BTC_PUBLICKEY = 12,
    JUB_ENUM_PROTOCOL_BTC_GET_ADDRESS = 29,
    JUB_ENUM_PROTOCOL_BTC_ADDRESS = 30,
    JUB_ENUM_PROTOCOL_BTC_SignTx = 15,
    JUB_ENUM_PROTOCOL_BTC_TxRequest = 21,
    JUB_ENUM_PROTOCOL_BTC_TxAck = 22,
};


JUB_RV JubiterSealer2100Impl::GetHDNodeBTC_2100(
    const JUB_ENUM_COINTYPE_BTC coinType,
    const JUB_BTC_TRANS_TYPE& type,
    const std::string& path,
    std::string& xpub
) {
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    std::string strCoinName;
    switch (coinType) {
        case COINBTC:  strCoinName = "Bitcoin";
            break;
        case COINBCH:  strCoinName = "Bcash";
            break;
        case COINLTC:  strCoinName = "Litecoin";
            break;
        //case COINUSDT: strCoinName = "USDT";break;
        case COINDASH: strCoinName = "Dash";
            break;
        case COINQTUM: strCoinName =  "Qtum";
            break;
        case COINDOGE: strCoinName =  "Dogecoin";
            break;
        default:       strCoinName = "Bitcoin";
            break;
    }
    InputScriptType ScriptType;
    //InputScriptType
    switch (type) {
        case p2pkh:
        {
            ScriptType = SPENDADDRESS;//xpub
            break;
        }
        case p2sh_multisig:
        case p2wsh_multisig:
        {
            ScriptType = SPENDMULTISIG;   //xpub
            break;
        } // case p2pkh/p2sh_multisig end
        case p2sh_p2wpkh:
        {
            ScriptType = SPENDP2SHWITNESS;   //ypub  format
            break;
        } // case p2sh_p2wpkh end
        case p2wpkh:
        {
            ScriptType = SPENDWITNESS;   //zpub format, BIP84
            break;
        } // case p2wpkh end
        case SPEND_APROOT:
        {
            ScriptType = SPENDTAPROOT;   //tpub format
            break;
        }
        default:
            break;
    } // switch (type) end

    try
    {
        Address address = parse_path(path);

        GetPublicKey msgToBeEncode;
        msgToBeEncode.Clear();

        for (size_t i = 0; i < address.path.size(); ++i)
        {
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_coin_name(strCoinName);
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

    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_BTC_GET_PUBLICKEY, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_BTC_PUBLICKEY)
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
            xpub = msgToBeDecode.xpub();
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


JUB_RV JubiterSealer2100Impl::GetAddressBTC_2100(
    const JUB_ENUM_COINTYPE_BTC coinType,
    const JUB_BYTE addrFmt,
    const JUB_BTC_TRANS_TYPE& type,
    const std::string& path,
    const JUB_UINT16 tag,
    std::string& address
) {
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    std::string strCoinName;
    switch (coinType) {
        case COINBTC:  strCoinName = "Bitcoin";
            break;
        case COINBCH:  strCoinName = "Bcash";
            break;
        case COINLTC:  strCoinName = "Litecoin";
            break;
        //case COINUSDT: strCoinName = "USDT";break;
        case COINDASH: strCoinName = "Dash";
            break;
        case COINQTUM: strCoinName =  "Qtum";
            break;
        case COINDOGE: strCoinName =  "Dogecoin";
            break;
        default:       strCoinName = "Bitcoin";
            break;
    }

    bool bShow = (bool)tag;
    InputScriptType ScriptType;
    //InputScriptType
    switch (type) {
        case p2pkh:
        {
            ScriptType = SPENDADDRESS;
            break;
        }
        case p2sh_multisig:
        case p2wsh_multisig:
        {
            //不支持multisig。
            return JUBR_IMPL_NOT_SUPPORT;
           // ScriptType = SPENDMULTISIG;   //xpub
            //break;
        } // case p2pkh/p2sh_multisig end
        case p2sh_p2wpkh:
        {
            ScriptType = SPENDP2SHWITNESS;   //ypub  format
            break;
        } // case p2sh_p2wpkh end
        case p2wpkh:
        {
            ScriptType = SPENDWITNESS;   //zpub format, BIP84
            break;
        } // case p2wpkh end
        case SPEND_APROOT:
        {
            ScriptType = SPENDTAPROOT;   //tpub format
            break;
        }
        default:
            break;
    } // switch (type) end


    try {
        Address address = parse_path(path);
        GetAddress msgToBeEncode;
        msgToBeEncode.Clear();
        for (size_t i = 0; i < address.path.size(); ++i)
        {
            msgToBeEncode.add_address_n(address.path[i]);
        }
        msgToBeEncode.set_coin_name(strCoinName);
        msgToBeEncode.set_show_display(bShow);
        msgToBeEncode.set_script_type(ScriptType);

        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb))
        {
            return JUBR_ERROR;
        }

    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_BTC_GET_ADDRESS, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_BTC_ADDRESS)
    {
        return JUBR_ERROR;
    }

    try
    {
        ::hw::trezor::messages::bitcoin::Address msgToBeDecode;
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
    return  JUBR_OK;
}
//JUB_RV JubiterSealer2100Impl::GetAddressMultisigBTC_2100(const JUB_BYTE addrFmt,
//                                          const JUB_BTC_TRANS_TYPE& type,
//                                          const std::string& path,
//                                          const std::vector<std::string>& CosignerMainxPub,
//                                          const unsigned long m,
//                                          const unsigned long n,
//                                          const JUB_UINT16 tag,
//                                          std::string& address)
//{
//    std::string msgInPb;
//    std::string msgOutPb;
//    JUB_UINT16 recvType;
//    bool bShow = (bool)tag;
//    InputScriptType ScriptType;
//    //InputScriptType
//    switch (type) {
//        case p2sh_multisig:
//        case p2wsh_multisig:
//        {
//            ScriptType = SPENDMULTISIG;   //xpub
//            break;
//        } // case p2pkh/p2sh_multisig end
//        case p2sh_p2wpkh:
//        case p2pkh:
//        case p2wpkh:
//        case SPEND_APROOT:
//        {
//            return JUBR_IMPL_NOT_SUPPORT;
//        }
//        default:
//            break;
//    } // switch (type) end
//
//    //解析redeemScriptTlv   hdnode_deserialize
//
//    Address address_n = parse_path(path);
//
//    MultisigRedeemScriptType multisigType;
//    multisigType.set_m(m);
//    for (size_t i = 0; i < address_n.path.size(); ++i)
//    {
//        multisigType.add_address_n(address_n.path[i]);
//    }
//    for (unsigned long i=0; i<CosignerMainxPub.size(); ++i) {
//        HDNode_HPY hdkey;
//        uint32_t parentFingerprint;
//        if (0 != hdnode_deserialize(CosignerMainxPub[i].c_str(),
//                                    bitcoinXPUB, bitcoinXPRV,
//                                    "secp256k1",
//                                    &hdkey, &parentFingerprint)) {
//            return JUBR_ARGUMENTS_BAD;
//        }
//        auto* node = multisigType.add_nodes();
//        node->set_depth(hdkey.depth);
//        node->set_fingerprint(parentFingerprint);
//        node->set_child_num(hdkey.child_num);
//        node->set_chain_code(std::string(hdkey.chain_code, hdkey.chain_code + 32));
//        node->set_public_key(std::string(hdkey.public_key, hdkey.public_key + 32));
//    }
//
//
//
//    try {
//
//        GetAddress msgToBeEncode;
//        msgToBeEncode.Clear();
//        for (size_t i = 0; i < address_n.path.size(); ++i)
//        {
//            msgToBeEncode.add_address_n(address_n.path[i]);
//        }
//        msgToBeEncode.set_show_display(bShow);
//        msgToBeEncode.set_script_type(ScriptType);
//
//        // Encode the message using standard protobuf
//        if (!msgToBeEncode.SerializeToString(&msgInPb))
//        {
//            return JUBR_ERROR;
//        }
//        *msgToBeEncode.mutable_multisig() = multisigType;
//
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_BTC_GET_ADDRESS, msgInPb, &recvType, msgOutPb);
//    if (JUBR_OK != rv)
//    {
//        return rv;
//    }
//    if (recvType != JUB_ENUM_PROTOCOL_BTC_ADDRESS)
//    {
//        return JUBR_ERROR;
//    }
//
//    try
//    {
//        ::hw::trezor::messages::bitcoin::Address msgToBeDecode;
//        if (!msgToBeDecode.ParseFromString(msgOutPb))
//        {
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        if (msgToBeDecode.has_address())
//        {
//            address = msgToBeDecode.address();
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
//    return  JUBR_OK;
//}

//JUB_RV JubiterSealer2100Impl::GetAddressMultisigBTC(
//    const JUB_BYTE addrFmt,
//    const JUB_BTC_TRANS_TYPE& type,
//    const std::string& path,
//    const JUB_UINT16 tag,
//    const uchar_vector& vRedeemScriptTlv,
//    std::string& address
//) {
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//新设备比特币签名函数
    std::vector<unsigned char> hex_to_little_endian_direct(const std::string& hex_big_endian) {
        // 验证输入
        if (hex_big_endian.empty() || hex_big_endian.length() % 2 != 0) {
            throw std::invalid_argument("Invalid hex string");
        }

        // 将大端序十六进制直接转换为字节向量
        std::vector<unsigned char> bytes;
        for (size_t i = 0; i < hex_big_endian.length(); i += 2) {
            std::string byte_string = hex_big_endian.substr(i, 2);
            bytes.push_back(static_cast<unsigned char>(
                                    std::stoul(byte_string, nullptr, 16))
            );
        }

        // 反转字节顺序
        //std::reverse(bytes.begin(), bytes.end());

        return bytes;
    }
TxAck_TransactionType_TxInputType input_btcToTxInputType(const std::string mainPath, const INPUT_BTC& input_btc) {
    TxAck_TransactionType_TxInputType input;

    std::string strPath = mainPath + "/" + std::to_string(input_btc.path.change)+ "/" + std::to_string(input_btc.path.addressIndex);
    Address address = parse_path(strPath);

    for(size_t i = 0; i < address.path.size(); ++i)
    {
        input.add_address_n(address.path[i]);
    }
    input.set_amount(input_btc.amount);
    input.set_prev_index(input_btc.preIndex);
    //input.set_prev_hash(input_btc.preHash);
    auto prev_hash = hex_to_little_endian_direct(input_btc.preHash);
    input.set_prev_hash(prev_hash.data(),prev_hash.size());

    input.set_sequence(input_btc.nSequence);
    InputScriptType inputst = InputScriptType::SPENDADDRESS;
    switch (input_btc.type) {
        case SCRIPT_BTC_TYPE::P2PKH:
            inputst = InputScriptType::SPENDADDRESS;
            break;
        case SCRIPT_BTC_TYPE::P2SH_MULTISIG:
            inputst = InputScriptType::SPENDMULTISIG;
            break;
        case SCRIPT_BTC_TYPE::P2WSH_MULTISIG:
            inputst = InputScriptType::SPENDP2SHWITNESS;
            break;
        case SCRIPT_BTC_TYPE::P2SH_P2WPKH:
            inputst = InputScriptType::SPENDP2SHWITNESS;
            break;
        case SCRIPT_BTC_TYPE::P2WPKH:
            inputst = InputScriptType::SPENDWITNESS;
            break;
        case SCRIPT_BTC_TYPE::TAPROOT:
            inputst = InputScriptType::SPENDTAPROOT;
            break;
        default:
            inputst = InputScriptType::SPENDADDRESS;//其他对不上，默认SPENDADDRESS
            break;
    }
    input.set_script_type(inputst);
    return input;
}
TxAck_TransactionType_TxOutputType output_btcToTxOutputType(const std::string mainPath, const OUTPUT_BTC& output_btc) {
    TxAck_TransactionType_TxOutputType output;
    if(output_btc.stdOutput.changeAddress == JUB_ENUM_BOOL::BOOL_FALSE){
        output.set_address(output_btc.stdOutput.address);
    }else{
        std::string strPath = mainPath + "/" + std::to_string(output_btc.stdOutput.path.change)+ "/" + std::to_string(output_btc.stdOutput.path.addressIndex);
        Address address = parse_path(strPath);
        for(size_t i = 0; i < address.path.size(); ++i)
        {
            output.add_address_n(address.path[i]);
        }
    }
    output.set_amount(output_btc.stdOutput.amount);
    OutputScriptType outputst = OutputScriptType::PAYTOADDRESS;
    switch (output_btc.type) {
        case SCRIPT_BTC_TYPE::P2PKH:
            outputst = OutputScriptType::PAYTOADDRESS;
            break;
        case SCRIPT_BTC_TYPE::P2SH_MULTISIG:
            outputst = OutputScriptType::PAYTOMULTISIG;
            break;
        case SCRIPT_BTC_TYPE::P2WSH_MULTISIG:
            outputst = OutputScriptType::PAYTOP2SHWITNESS;
            break;
        case SCRIPT_BTC_TYPE::RETURN0:
            outputst = OutputScriptType::PAYTOOPRETURN;
            break;
        case SCRIPT_BTC_TYPE::P2SH_P2WPKH:
            outputst = OutputScriptType::PAYTOP2SHWITNESS;
            break;
        case SCRIPT_BTC_TYPE::P2WPKH:
            outputst = OutputScriptType::PAYTOWITNESS;
            break;
        case SCRIPT_BTC_TYPE::TAPROOT:
            outputst = OutputScriptType::PAYTOTAPROOT;
            break;
        default:
            outputst = OutputScriptType::PAYTOADDRESS;//其他对不上，默认PAYTOADDRESS
            break;
    }
    output.set_script_type(outputst);
    return output;

}

JUB_RV JubiterSealer2100Impl::SignTXBTC_2100(const JUB_ENUM_COINTYPE_BTC coinType,
                                             const std::string mainPath,
                                             const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt,
                                             const std::vector<INPUT_BTC>& vInputs,
                                             const std::vector<OUTPUT_BTC>& vOutputs,
                                             const JUB_UINT32 lockTime,
                                             std::vector<JUB_BYTE>& vRaw)
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    JUB_RV rv = JUBR_OK;
    std::string strCoinName;
    switch (coinType) {
        case COINBTC:  strCoinName = "Bitcoin";
            break;
        case COINBCH:  strCoinName = "Bcash";
            break;
        case COINLTC:  strCoinName = "Litecoin";
            break;
        //case COINUSDT: strCoinName = "USDT";break;
        case COINDASH: strCoinName = "Dash";
            break;
        case COINQTUM: strCoinName =  "Qtum";
            break;
        case COINDOGE: strCoinName =  "Dogecoin";
            break;
        default:       strCoinName = "Bitcoin";
            break;
    }
    //将vInputs转为TxInputType
    std::vector<TxAck_TransactionType_TxInputType> inputs;
    for (const auto& input : vInputs) {
        inputs.push_back(input_btcToTxInputType(mainPath,input));
    }
    //将vOutputs转为TxOutputType
    std::vector<TxAck_TransactionType_TxOutputType> outputs;
    for (const auto& output : vOutputs) {
        outputs.push_back(output_btcToTxOutputType(mainPath,output));
    }
    std::vector<std::optional<std::string>> signatures(inputs.size(), std::nullopt);//接收签名
    std::string serialized_tx;//接收序列化交易
    //获取details（暂没有，后续对应）
    //获取input对应的prev_txes  （暂没有，后续对应）这个也应转为TransactionType
    try
    {
        //Sending MessageType_SignTx
        SignTx msgSignTx;
        msgSignTx.Clear();
        msgSignTx.set_coin_name(strCoinName);
        msgSignTx.set_inputs_count(vInputs.size());
        msgSignTx.set_outputs_count(vOutputs.size());
        msgSignTx.set_lock_time(lockTime);
        msgSignTx.set_version(2);
        if (!msgSignTx.SerializeToString(&msgInPb))
        {
            return JUBR_ERROR;
        }
        rv = sendProtocolData(JUB_ENUM_PROTOCOL_BTC_SignTx, msgInPb, &recvType, msgOutPb);
        if (JUBR_OK != rv)
        {
            return rv;
        }
        //硬件请求数据
        //组织message TxAck------message TransactionType
        TxAck_TransactionType this_tx;
        for (const auto& input : inputs) {
            *this_tx.add_inputs() = input;
        }
        for (const auto& output : outputs) {
            *this_tx.add_outputs() = output;
        }
        this_tx.set_inputs_cnt(inputs.size());
        this_tx.set_outputs_cnt(outputs.size());
        this_tx.set_version(msgSignTx.version());//


        //TxRequest_RequestType R;
        while (JUB_ENUM_PROTOCOL_BTC_TxRequest == recvType)
        {
            //请求什么数据
            TxRequest msgRequest;
            msgRequest.Clear();
            if (!msgRequest.ParseFromString(msgOutPb))
            {
                return JUBR_ARGUMENTS_BAD;
            }
            //如果请求了prev_txes，直接报错。
            if(msgRequest.details().has_tx_hash())
            {
                throw std::runtime_error("The current firmware version should not request previous transaction");
            }
            //如果已经回来了某个input签名
            if (msgRequest.has_serialized()) {
                if (msgRequest.serialized().has_serialized_tx()) {
                    serialized_tx += msgRequest.serialized().serialized_tx();
                }
                if (msgRequest.serialized().has_signature_index()) {
                    auto idx = msgRequest.serialized().signature_index();
                    auto sig = msgRequest.serialized().signature();
                    if (signatures[idx]) {
                        throw std::runtime_error("Signature for index already filled");
                    }
                    signatures[idx] = sig;
                }
            }
            //处理下个请求
            TxAck tx_ack;
            auto* tx = tx_ack.mutable_tx();
            //TxAck_TransactionType msgTxAck_TxType;
            switch (msgRequest.request_type()) {
                case TxRequest::TXINPUT:
                case TxRequest::TXORIGINPUT:{//请求input
                    uint32_t index = msgRequest.details().request_index();
                    auto* pinput = tx->add_inputs();
                    pinput->CopyFrom(this_tx.inputs(index));
                }
                    break;
                case TxRequest::TXOUTPUT: {//请求output
                    uint32_t index = msgRequest.details().request_index();
                    auto* poutput = tx->add_outputs();
                    poutput->CopyFrom(this_tx.outputs(index));
                }
                    break;
                case TxRequest::TXFINISHED: {//请求结束
                }
                    break;
                case TxRequest::TXMETA:{//trezorctl给了input和output个数
                    tx->set_inputs_cnt(this_tx.inputs_size());
                    tx->set_outputs_cnt(this_tx.outputs_size());
                }
                    break;
                default:{//不支持的request type
                    throw std::runtime_error("Unsupported request type");
                }
            }

            // request_type 为 TXFINISHED 时，跳出 while 循环
            if(msgRequest.has_request_type() &&
               msgRequest.request_type() == TxRequest::TXFINISHED) {
                break;
            }

            msgInPb.clear();
            msgOutPb.clear();
            recvType = 0;
            if (!tx_ack.SerializeToString(&msgInPb))
            {
                return JUBR_ERROR;
            }
            rv = sendProtocolData(JUB_ENUM_PROTOCOL_BTC_TxAck, msgInPb, &recvType, msgOutPb);
            if(recvType == 3)
            {
                return JUBR_ERROR;
            }
            if (JUBR_OK != rv)
            {
                return rv;
            }
        }

        vRaw.clear();

        vRaw.insert(vRaw.end(), serialized_tx.begin(), serialized_tx.end());
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }

    return rv;
}
JUB_RV JubiterSealer2100Impl::SignTXBTC(
    const JUB_BYTE addrFmt,
    const JUB_BTC_TRANS_TYPE& type,
    const JUB_UINT16 inputCount,
    const std::vector<JUB_UINT64>& vInputAmount,
    const std::vector<std::string>& vInputPath,
    const std::vector<JUB_UINT16>& vChangeIndex,
    const std::vector<std::string>& vChangePath,
    const std::vector<JUB_BYTE>& vUnsigedTrans,
    std::vector<JUB_BYTE>& vRaw
) {
    // TODO
    try {
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }

    return JUBR_IMPL_NOT_SUPPORT;
}
//
//
//JUB_RV JubiterSealer2100Impl::SignTXMultisigBTC(
//    const JUB_BYTE addrFmt,
//    const JUB_BTC_TRANS_TYPE& type,
//    const JUB_UINT16 inputCount,
//    const std::vector<JUB_UINT64>& vInputAmount,
//    const std::vector<std::string>& vInputPath,
//    const std::vector<uchar_vector>& vRedeemScriptTlv,
//    const std::vector<JUB_UINT16>& vChangeIndex,
//    const std::vector<std::string>& vChangePath,
//    const std::vector<uchar_vector>& vChangeRedeemScriptTlv,
//    const std::vector<JUB_BYTE>& vUnsigedTrans,
//    std::vector<uchar_vector>& vSignatureRaw
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


JUB_RV JubiterSealer2100Impl::SetUnitBTC(
    const JUB_BTC_UNIT_TYPE& unit
) {
    return JUBR_OK;
}


JUB_RV JubiterSealer2100Impl::SetCoinTypeBTC(
    const JUB_ENUM_COINTYPE_BTC& type
) {
    return JUBR_OK;
}
} // namespace jub end
