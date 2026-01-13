//
//  JubiterSealer2100ImplTRX.cpp
//  JubSDK
//
//  Created by Pan Min on 2025/4/26.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#include "token/JubiterSealer2100Impl.h"

#include <Tron/Address.h>
#include <Tron/Transaction.h>
#include <Tron/Serialization.h>
#include <PublicKey.h>
#include <../PrivateKey.h>
#include <../HexCoding.h>
#include <TrezorCrypto/base58.h>

#include "common/protocpp/messages-tron.pb.h"
#include "common/protocpp/messages-bitcoin.pb.h"

using namespace hw::trezor::messages::tron ;
using namespace hw::trezor::messages::bitcoin;

namespace jub {

    enum TRXMessageType
    {
        JUB_ENUM_PROTOCOL_TRX_GET_PUBLICKEY = 11,//复用BTC id
        JUB_ENUM_PROTOCOL_TRX_PUBLICKEY = 12,
        JUB_ENUM_PROTOCOL_TRX_GET_ADDRESS = 10501,
        JUB_ENUM_PROTOCOL_TRX_ADDRESS = 10502,
        JUB_ENUM_PROTOCOL_TRX_SIGN_TX = 10503,
        JUB_ENUM_PROTOCOL_TRX_SIGNED_TX = 10504,    
    };

JUB_RV JubiterSealer2100Impl::GetAddressTRX(const std::string &path, const JUB_UINT16 tag, std::string &address)
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    try
    {
        
        Address address = parse_path(path);
        
        TronGetAddress msgToBeEncode;
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
    
    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_TRX_GET_ADDRESS, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_TRX_ADDRESS)
    {
        return JUBR_ERROR;
    }
    
    try
    {
        TronAddress msgToBeDecode;
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
JUB_RV JubiterSealer2100Impl::GetHDNodeTRX(const JUB_BYTE format, const std::string &path, std::string &pubkey)
{
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
    
    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_TRX_GET_PUBLICKEY, msgInPb, &recvType, msgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_TRX_PUBLICKEY)
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
                //uchar_vector vPubkey(pubkey, (unsigned int)ulRetDataLen);
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
    return JUBR_OK;        return JUBR_IMPL_NOT_SUPPORT;
}
JUB_RV JubiterSealer2100Impl::SetCoinTypeTRX()
{
    return JUBR_OK;
}
JUB_RV JubiterSealer2100Impl::SetTRC20Token(const std::string& tokenName,
                                            const JUB_UINT16 unitDP,
                                            const std::string& contractAddress) {
    
    return JUBR_OK;
}
JUB_RV JubiterSealer2100Impl::SignTXTRX(const std::vector<JUB_BYTE> &vPath,
                                        const std::vector<JUB_BYTE> &vRaw,
                                        std::vector<uchar_vector> &vSignatureRaw)
{
    std::string vMsgInPb;
    
    TW::Data TmpData;
    TmpData.clear();
    try
    {
        Address address = parse_path(std::string(vPath.begin(), vPath.end()));
        TW::Tron::Transaction tx;
        tx.raw_data.deserialize(vRaw);//deserialize方法会给raw_data各变量赋值。
        if (0 >= tx.raw_data.contracts.size()) {
            return JUBR_ARGUMENTS_BAD;
        }
        //使用raw_data解析数据，填充TronSignTx
        
        TronSignTx msgToBeEncode;
        msgToBeEncode.Clear();
        
        //repeated uint32 address_n = 1;
        for (size_t i = 0; i < address.path.size(); ++i)
        {
            msgToBeEncode.add_address_n(address.path[i]);
        }
        //required bytes ref_block_bytes = 2;
        TmpData.clear();
        TmpData = tx.raw_data.ref_block_bytes;
        if(0 < TmpData.size())
        {
            msgToBeEncode.set_ref_block_bytes(std::string(reinterpret_cast<const char*>(TmpData.data()), TmpData.size()));
        }
        //required bytes ref_block_hash = 3;
        TmpData.clear();
        TmpData = tx.raw_data.ref_block_hash;
        if(0 < TmpData.size())
        {
            msgToBeEncode.set_ref_block_hash(std::string(reinterpret_cast<const char*>(TmpData.data()), TmpData.size()));
        }
        //required uint64 expiration = 4;
        msgToBeEncode.set_expiration(tx.raw_data.expiration);
        //optional string data = 5;
        TmpData.clear();
        TmpData = tx.raw_data.data;
        if(0 < TmpData.size())
        {
            msgToBeEncode.set_data(std::string(reinterpret_cast<const char *>(TmpData.data()), TmpData.size()));
        }
        //required uint64 timestamp = 7;
        msgToBeEncode.set_timestamp(tx.raw_data.timestamp);
        //optional uint64 fee_limit = 8;
        msgToBeEncode.set_fee_limit(tx.raw_data.fee_limit);
        //required TronContract contract = 6; 下面处理contract相关，要处理多种
        TronSignTx_TronContract TronSignContract;//message TronContract
        TronSignContract.Clear();
        TronSignTx_TronContract_TronTransferContract TronSignTxTransferContract;
        TronSignTxTransferContract.Clear();
        TronSignTx_TronContract_TronTriggerSmartContract TronSignTxTriggerSmartContract;
        TronSignTxTriggerSmartContract.Clear();
        TronSignTx_TronContract_TronTransferAssetContract TronSignTxTransferAssetContract;
        TronSignTxTransferAssetContract.Clear();
        //auto* TronSignContract = new TronSignTx_TronContract;
        //TronSignContract->Clear();
        
        size_t contrIndex = tx.raw_data.contractOffset(0);
        if (0 == contrIndex) {
            return JUBR_ERROR_ARGS;
        }
        uint32_t permission_id = tx.raw_data.contracts[0].permission_id;
        if (permission_id > 0) {
            TronSignContract.set_permission_id(permission_id);
        }
        switch (tx.raw_data.contracts[0].type)
        {
                // TronTransferContract
            case ::protocol::Transaction_Contract_ContractType::Transaction_Contract_ContractType_TransferContract:
            {
                TW::Tron::TransferContract contract;
                if (!tx.raw_data.contracts[0].from_parameter<::protocol::TransferContract>(contract)) {
                    return JUBR_ERROR;
                }
                //optional string to_address = 2;
                TronSignTxTransferContract.set_to_address(contract.to_address);//此处可能有错误，要检查地址是否是base58
                //optional uint64 amount = 3;
                TronSignTxTransferContract.set_amount(contract.amount);
                auto* raw_ptr = new TronSignTx_TronContract_TronTransferContract(
                                                                                 std::move(TronSignTxTransferContract)
                                                                                 );
                TronSignContract.set_allocated_transfer_contract(raw_ptr);//设置TransferContract
                
            }   break;
                //TronTriggerSmartContract
            case ::protocol::Transaction_Contract_ContractType::Transaction_Contract_ContractType_TriggerSmartContract:
            {
                TW::Tron::TriggerSmartContract contract;
                if (!tx.raw_data.contracts[0].from_parameter<::protocol::TriggerSmartContract>(contract)) {
                    return JUBR_ERROR;
                }
                //optional string contract_address = 2;   // Contract address - base58
                TronSignTxTriggerSmartContract.set_contract_address(contract.contract_address);
                //optional uint64 call_value = 3;         // TRX amount if any
                TronSignTxTriggerSmartContract.set_call_value(contract.call_value);
                //optional bytes data = 4;                // Contract data
                TronSignTxTriggerSmartContract.set_data(std::string(reinterpret_cast<const char*>(contract.data.data()), contract.data.size()));
                //optional uint64 call_token_value = 5;   // Asset ID if any
                TronSignTxTriggerSmartContract.set_call_token_value(contract.call_token_value);
                //optional uint64 asset_id = 6;           // Asset signature
                TronSignTxTriggerSmartContract.set_asset_id(contract.token_id);
                auto* raw_ptr = new TronSignTx_TronContract_TronTriggerSmartContract(
                                                                                     std::move(TronSignTxTriggerSmartContract)
                                                                                     );
                TronSignContract.set_allocated_trigger_smart_contract(raw_ptr);//设置TriggerSmartContract
            }   break;
            case ::protocol::Transaction_Contract_ContractType::Transaction_Contract_ContractType_TransferAssetContract: {
                TW::Tron::TransferAssetContract contract;
                if (!tx.raw_data.contracts[0].from_parameter<::protocol::TransferAssetContract>(contract)) {
                    return JUBR_ERROR;
                }

                TronSignTxTransferAssetContract.set_asset_name(contract.asset_name);
                TronSignTxTransferAssetContract.set_to_address(contract.to_address);
                TronSignTxTransferAssetContract.set_amount(contract.amount);
                auto* raw_ptr = new TronSignTx_TronContract_TronTransferAssetContract(
                                                                                      std::move(TronSignTxTransferAssetContract)
                                                                                     );
                TronSignContract.set_allocated_transfer_asset_contract(raw_ptr);//设置TriggerSmartContract
                break;
            }
                //TronFreezeBalanceContract
                //TronUnfreezeBalanceContract
                //TronWithdrawBalanceContract
                //TronFreezeBalanceV2Contract
                //TronUnfreezeBalanceV2Contract
                //TronWithdrawExpireUnfreezeContract
                //TronDelegateResourceContract
                //TronUnDelegateResourceContract
                
            default:
                return JUBR_IMPL_NOT_SUPPORT;
                break;
        }
        //为TronSignTx设置TronContract
        auto* raw_contract_ptr = new TronSignTx_TronContract(
                                                             std::move(TronSignContract)
                                                             );
        msgToBeEncode.set_allocated_contract(raw_contract_ptr);
        
        if (!msgToBeEncode.SerializeToString(&vMsgInPb))
        {
            /*                if(TronSignContract.has_transfer_contract())
             TronSignContract.release_transfer_contract();
             if(TronSignContract.has_trigger_smart_contract())
             TronSignContract.release_trigger_smart_contract();
             if(msgToBeEncode.has_contract())
             msgToBeEncode.release_contract();*/
            return JUBR_ERROR;
        }
        /*            else
         {
         if(TronSignContract.has_transfer_contract())
         TronSignContract.release_transfer_contract();
         if(TronSignContract.has_trigger_smart_contract())
         TronSignContract.release_trigger_smart_contract();
         if(msgToBeEncode.has_contract())
         msgToBeEncode.release_contract();
         }*/
        
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_UINT16 recvType;
    std::string vMsgOutPb;
    JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_TRX_SIGN_TX, vMsgInPb, &recvType, vMsgOutPb);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvType != JUB_ENUM_PROTOCOL_TRX_SIGNED_TX)
    {
        return JUBR_ERROR;
    }
    TronSignedTx msgToBeDecode;
    try
    {
        if (!msgToBeDecode.ParseFromString(vMsgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        if(!msgToBeDecode.has_signature())
        {
            return JUBR_ARGUMENTS_BAD;
        }
    }
    catch (...)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    std::vector<JUB_BYTE> vSignature(
                                     msgToBeDecode.signature().begin(),
                                     msgToBeDecode.signature().end()
                                     );
    vSignatureRaw.push_back(vSignature);
    
    return JUBR_OK;
}

} // namespace jub end
