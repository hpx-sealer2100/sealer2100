//
//  JubiterSealer2100ImplETH.cpp
//  JubSDK
//
//  Created by Pan Min on 2025/4/26.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#include "token/JubiterSealer2100Impl.h"
#include "common/protocpp/messages-ethereum.pb.h"
#include "common/protocpp/messages-ethereum-eip712.pb.h"
#include "utils/logUtils.h"
#include "mSIGNA/stdutils/uchar_vector.h"
#include "libETH/libETH.hpp"
#include "BinaryCoding.h"
#include <TrezorCrypto/base58.h>
#include <Ethereum/EIP712.h>

#include <variant>

using namespace hw::trezor::messages::ethereum;
using namespace hw::trezor::messages::ethereum_eip712;
using namespace hw::trezor::messages::ethereum_definitions;

namespace jub
{

    enum ETHMessageType
    {
        JUB_ENUM_PROTOCOL_ETH_GET_ADDRESS = 56,
        JUB_ENUM_PROTOCOL_ETH_ADDRESS = 57,
        JUB_ENUM_PROTOCOL_ETH_SIGN_TX = 58,
        JUB_ENUM_PROTOCOL_ETH_TX_REQUEST = 59,
        JUB_ENUM_PROTOCOL_ETH_GET_PUBLICKEY = 450,
        JUB_ENUM_PROTOCOL_ETH_PUBLICKEY = 451,
        JUB_ENUM_PROTOCOL_ETH_SIGN_MESSAGE = 64,
        JUB_ENUM_PROTOCOL_ETH_MESSAGE_SIGNATURE = 66,
        JUB_ENUM_PROTOCOL_ETH_VERIFY_MESSAGE = 65,
        JUB_ENUM_PROTOCOL_MessageType_Success = 2,
        JUB_ENUM_PROTOCOL_MessageType_Failure = 3,
        JUB_ENUM_PROTOCOL_ETH_SIGN_TYPEDDATA = 464,
        JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_StructRequest = 465,
        JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_StructAck = 466,
        JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_ValueRequest = 467,
        JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_ValueAck = 468,
        JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_Signature = 469,
        JUB_ENUM_PROTOCOL_ETH_StoreDefinition = 471,

    };


    JUB_RV JubiterSealer2100Impl::GetHDNodeETH(const JUB_BYTE format, const std::string &path, std::string &pubkey)
    {
        std::string msgInPb;
        std::string msgOutPb;
        JUB_UINT16 recvType;
        if(JUB_ENUM_PUB_FORMAT::HEX  != format && JUB_ENUM_PUB_FORMAT::XPUB != format)
            return JUBR_ARGUMENTS_BAD;
        try
        {

            Address address = parse_path(path);

            EthereumGetPublicKey msgToBeEncode;
            msgToBeEncode.Clear();

            for (size_t i = 0; i < address.path.size(); ++i)
            {
                msgToBeEncode.add_address_n(address.path[i]);
            }
            msgToBeEncode.set_show_display(false);

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

        JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_GET_PUBLICKEY, msgInPb, &recvType, msgOutPb);
        if (JUBR_OK != rv)
        {
            return rv;
        }
        if (recvType != JUB_ENUM_PROTOCOL_ETH_PUBLICKEY)
        {
            return JUBR_ERROR;
        }

        try
        {
            EthereumPublicKey msgToBeDecode;
            if (!msgToBeDecode.ParseFromString(msgOutPb))
            {
                return JUBR_ARGUMENTS_BAD;
            }

            if (msgToBeDecode.has_xpub())
            {
                if(JUB_ENUM_PUB_FORMAT::HEX  == format)
                {
                    uint8_t decoded[112] = {0};  // 足够大的缓冲区
                    int decoded_len = hyp_base58_decode_check(msgToBeDecode.xpub().c_str(), HasherType::HASHER_SHA2D, decoded, sizeof(decoded));
                    if (decoded_len != 78) {  // xpub 固定长度为 78 字节
                        return JUBR_ARGUMENTS_BAD;
                    }
                    const uint8_t* pubkey_bin = decoded + 45;  // 78 - 33 = 45
                    pubkey = std::string(ETH_PRDFIX) + bin_to_hex(pubkey_bin, 33);
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

    JUB_RV JubiterSealer2100Impl::GetAddressETH(const std::string &path, uint64_t chainId, const JUB_UINT16 tag, std::string &address)
    {
        std::string msgInPb;
        std::string msgOutPb;
        JUB_UINT16 recvType;
        try
        {

            Address address = parse_path(path);

            EthereumGetAddress msgToBeEncode;
            msgToBeEncode.Clear();

            for (size_t i = 0; i < address.path.size(); ++i)
            {
                msgToBeEncode.add_address_n(address.path[i]);
            }
            if( 0 == tag)
                msgToBeEncode.set_show_display(false);
            else
                msgToBeEncode.set_show_display(true);
            msgToBeEncode.set_chain_id(chainId);

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

        JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_GET_ADDRESS, msgInPb, &recvType, msgOutPb);
        if (JUBR_OK != rv)
        {
            return rv;
        }
        if (recvType != JUB_ENUM_PROTOCOL_ETH_ADDRESS)
        {
            return JUBR_ERROR;
        }

        try
        {
            EthereumAddress msgToBeDecode;
            if (!msgToBeDecode.ParseFromString(msgOutPb))
            {
                return JUBR_ARGUMENTS_BAD;
            }

            if (msgToBeDecode.has_address())
            {
                address = msgToBeDecode.address();
            }
            else if (msgToBeDecode.has__old_address())
            {
                // _old_address is bytes, convert to hex string
                const std::string &oldAddr = msgToBeDecode._old_address();
                uchar_vector vOldAddress((const unsigned char *)oldAddr.data(), (unsigned int)oldAddr.size());
                address = std::string(vOldAddress.getHex());
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
    JUB_RV JubiterSealer2100Impl::SetERC20ETHToken(const std::string& tokenName,
                                    const JUB_UINT16 unitDP,
                                    const std::string& contractAddress)
    {
        return JUBR_OK;
    }
    JUB_RV JubiterSealer2100Impl::SetERC20ETHTokens(const ERC20_TOKEN_INFO tokens[],
                                     const JUB_UINT16 iCount)
    {
        return JUBR_OK;
    }
    JUB_RV JubiterSealer2100Impl::StoreETHDefinition(JUB_ETH_NETWORK_INFO networkInfo,
                                                     std::optional<JUB_ERC20_TOKEN_INFO> tokenInfo) {
        // check version
        if (features) {
            // 1.2.35
            const static uint32_t kAvailableVersion = 0x01022300;
            uint32_t version = 0;
            version |= features->major_version() << 24;
            version |= features->minor_version() << 16;
            version |= features->patch_version() << 8;
            if (version < kAvailableVersion) {
                return JUBR_OK;
            }
        }
        EthereumStoreDefinition msgToBeEncode = {};

        // network
        auto* network = new EthereumNetworkInfo();
        network->set_name(networkInfo.name);
        network->set_symbol(networkInfo.symbol);
        network->set_chain_id(networkInfo.chainID);
        network->set_slip44(networkInfo.slip44ID);

        msgToBeEncode.set_allocated_network(network);

        // token
        if (tokenInfo) {
            auto token = new EthereumTokenInfo();
            token->set_chain_id(tokenInfo->chainID);
            token->set_name(tokenInfo->name);
            token->set_symbol(tokenInfo->symbol);
            token->set_decimals(tokenInfo->decimals);

            auto address = jub::ETHHexStr2CharPtr(tokenInfo->address);
            token->set_address(address.data(), address.size());
            msgToBeEncode.set_allocated_token(token);
        }

        auto vMsgInPb = msgToBeEncode.SerializeAsString();

        JUB_UINT16 recvType;
        std::string vMsgOutPb;
        JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_StoreDefinition, vMsgInPb, &recvType, vMsgOutPb);
        if( JUBR_OK != rv) {
            return rv;
        }
        if (recvType != JUB_ENUM_PROTOCOL_MessageType_Success) {
            return JUBR_ERROR;
        }
        return JUBR_OK;
    }

    JUB_RV JubiterSealer2100Impl::SignTXETH(const int erc,
                                            const std::vector<JUB_BYTE> &vNonce,
                                            const std::vector<JUB_BYTE> &vGasPrice,
                                            const std::vector<JUB_BYTE> &vGasLimit,
                                            const std::vector<JUB_BYTE> &vTo,
                                            const std::vector<JUB_BYTE> &vValue,
                                            const std::vector<JUB_BYTE> &vData,
                                            const std::vector<JUB_BYTE> &vPath,
                                            const std::vector<JUB_BYTE> &vChainID,
                                            std::vector<JUB_BYTE> &vRaw)
    {

        std::string vMsgInPb;

        try
        {

            Address address = parse_path(std::string(vPath.begin(), vPath.end()));

            EthereumSignTx msgToBeEncode;
            msgToBeEncode.Clear();

            for (size_t i = 0; i < address.path.size(); ++i)
            {
                msgToBeEncode.add_address_n(address.path[i]);
            }

//            if (!vNonce.empty())
//            {
                msgToBeEncode.set_nonce(std::string(reinterpret_cast<const char *>(vNonce.data()), vNonce.size()));
//            }
           if (!vGasPrice.empty())
            {
                msgToBeEncode.set_gas_price(std::string(reinterpret_cast<const char *>(vGasPrice.data()), vGasPrice.size()));
            }
            if (!vGasLimit.empty())
            {
                msgToBeEncode.set_gas_limit(std::string(reinterpret_cast<const char *>(vGasLimit.data()), vGasLimit.size()));
            }
            if (!vValue.empty())
            {
                    msgToBeEncode.set_value(std::string(reinterpret_cast<const char *>(vValue.data()), vValue.size()));
            }
            if (!vData.empty())
            {
                msgToBeEncode.set_data_initial_chunk(std::string(reinterpret_cast<const char *>(vData.data()), vData.size()));
                msgToBeEncode.set_data_length(static_cast<uint32_t>(vData.size()));
            }
            if (!vChainID.empty())
            {
                uint64_t chainId = TW::decode64BE(&vChainID[0],vChainID.size());
                msgToBeEncode.set_chain_id(chainId);
            }
            if (!vTo.empty())
            {
                msgToBeEncode.set_to(std::string(CharPtr2HexStr(vTo)));
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
        JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_SIGN_TX, vMsgInPb, &recvType, vMsgOutPb);
        if (JUBR_OK != rv)
        {
            return rv;
        }
        if (recvType != JUB_ENUM_PROTOCOL_ETH_TX_REQUEST)
        {
            return JUBR_ERROR;
        }
        JUB_BBOOL bHasSignatureV = FALSE;
        JUB_BBOOL bHasSignatureR = FALSE;
        JUB_BBOOL bHasSignatureS = FALSE;
        JUB_UINT32 signatureV = 0;
        std::string signatureR;
        std::string signatureS;

        try
        {
            EthereumTxRequest msgToBeDecode;
            if (!msgToBeDecode.ParseFromString(vMsgOutPb))
            {
                return JUBR_ARGUMENTS_BAD;
            }
            bHasSignatureV = msgToBeDecode.has_signature_v();
            if (msgToBeDecode.has_signature_v())
            {
                signatureV = msgToBeDecode.signature_v();
            }
            bHasSignatureR = msgToBeDecode.has_signature_r();
            if (msgToBeDecode.has_signature_r())
            {
                signatureR = msgToBeDecode.signature_r();
            }
            bHasSignatureS = msgToBeDecode.has_signature_s();
            if (msgToBeDecode.has_signature_s())
            {
                signatureS = msgToBeDecode.signature_s();
            }
        }
        catch (...)
        {
            return JUBR_ARGUMENTS_BAD;
        }

        if (bHasSignatureV && bHasSignatureR && bHasSignatureS)
        {
            std::vector<JUB_BYTE> vSignatureV = jub::HexStr2CharPtr(jub::numberToHexString(signatureV));
            std::vector<JUB_BYTE> vSignatureR = std::vector<JUB_BYTE>(signatureR.begin(), signatureR.end());
            std::vector<JUB_BYTE> vSignatureS = std::vector<JUB_BYTE>(signatureS.begin(), signatureS.end());

            std::vector<JUB_BYTE> vSig = vSignatureR;
            vSig.insert(vSig.end(), vSignatureS.begin(), vSignatureS.end());
            vSig.insert(vSig.end(), vSignatureV.begin(), vSignatureV.end());
            if(10010 == erc)//signcontracteth,返回签名结果
            {
                vRaw = vSig;
                return JUBR_OK;
            }
            else {

                return jub::eth::serializeTx(vNonce,
                                             vGasPrice,
                                             vGasLimit,
                                             vTo,
                                             vValue,
                                             vData,
                                             vSig,
                                             vRaw);
            }
        }
        else
        {
            return JUBR_VERIFY_SIGN_FAILED;
        }
        return JUBR_VERIFY_SIGN_FAILED;
    }
    JUB_RV JubiterSealer2100Impl::SignContractETH( const JUB_BYTE inputType,
                                                   const std::vector<JUB_BYTE>& vNonce,
                                                   const std::vector<JUB_BYTE>& vGasPrice,
                                                   const std::vector<JUB_BYTE>& vGasLimit,
                                                   const std::vector<JUB_BYTE>& vTo,
                                                   const std::vector<JUB_BYTE>& vValue,
                                                   const std::vector<JUB_BYTE>& vInput,
                                                   const std::vector<JUB_BYTE>& vPath,
                                                   const std::vector<JUB_BYTE>& vChainID,
                                                   std::vector<JUB_BYTE>& signatureRaw)
    {
        //inputType为contract ABI类型，新硬件不用判断
        JUB_RV rv = SignTXETH(10010,vNonce,vGasPrice,vGasLimit,vTo,vValue,vInput,vPath,vChainID,signatureRaw);
        return rv;

    }
    JUB_RV JubiterSealer2100Impl::SignBytestring(const std::vector<JUB_BYTE>& vData,
                                  const std::vector<JUB_BYTE>& vPath,
                                  const std::vector<JUB_BYTE>& vChainID,
                                  std::vector<JUB_BYTE>& signatureRaw)
    {
        std::string msgInPb;
        std::string msgOutPb;
        JUB_UINT16 recvType;
        try
        {

            Address address = parse_path(std::string(vPath.begin(), vPath.end()));

            EthereumSignMessage msgToBeEncode;
            msgToBeEncode.Clear();

            for (size_t i = 0; i < address.path.size(); ++i)
            {
                msgToBeEncode.add_address_n(address.path[i]);
            }
            if (!vData.empty())
            {
                msgToBeEncode.set_message(std::string(reinterpret_cast<const char *>(vData.data()), vData.size()));
            }
            if (!vChainID.empty())
            {
                msgToBeEncode.set_encoded_network(std::string(reinterpret_cast<const char *>(vData.data()), vData.size()));
            }

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

        JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_SIGN_MESSAGE, msgInPb, &recvType, msgOutPb);
        if (JUBR_OK != rv)
        {
            return rv;
        }
        if (recvType != JUB_ENUM_PROTOCOL_ETH_MESSAGE_SIGNATURE)
        {
            return JUBR_ERROR;
        }

        try
        {
            EthereumMessageSignature msgToBeDecode;
            if (!msgToBeDecode.ParseFromString(msgOutPb))
            {
                return JUBR_ARGUMENTS_BAD;
            }

            if (msgToBeDecode.has_signature())
            {
                signatureRaw.reserve(msgToBeDecode.signature().size());
                signatureRaw.insert(signatureRaw.end(),reinterpret_cast<const JUB_BYTE*>(msgToBeDecode.signature().data()),
                        reinterpret_cast<const JUB_BYTE*>(msgToBeDecode.signature().data() + msgToBeDecode.signature().size()));
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
    JUB_RV JubiterSealer2100Impl::VerifyBytestring(const uchar_vector& vData,
                                    const uchar_vector& vSignature,
                                    const std::vector<JUB_BYTE>& publicKey)
    {
        std::string msgInPb;
        std::string msgOutPb;
        JUB_UINT16 recvType;
        try
        {

            //Address address = parse_path(std::string(vPath.begin(), vPath.end()));

            EthereumVerifyMessage msgToBeEncode;
            msgToBeEncode.Clear();

            if (!vData.empty())
            {
                msgToBeEncode.set_message(std::string(reinterpret_cast<const char *>(vData.data()), vData.size()));
            }
            if (!vSignature.empty())
            {
                msgToBeEncode.set_signature(std::string(reinterpret_cast<const char *>(vSignature.data()), vSignature.size()));
            }
            if (!publicKey.empty())
            {
                msgToBeEncode.set_address(std::string(reinterpret_cast<const char *>(publicKey.data()), publicKey.size()));
            }

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

        JUB_RV rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_VERIFY_MESSAGE, msgInPb, &recvType, msgOutPb);
        if (JUBR_OK != rv)
        {
            return rv;
        }
        if (recvType  == JUB_ENUM_PROTOCOL_MessageType_Success)
        {
            return JUBR_OK;
        }
        else if(recvType  == JUB_ENUM_PROTOCOL_MessageType_Failure)
        {
            return JUBR_ERROR;
        }
        else
            return JUBR_ARGUMENTS_BAD;

    }


    EthereumTypedDataStructAck_EthereumFieldType get_field_type(const std::string& type_name,const nlohmann::json& types)
    {
        EthereumTypedDataStructAck_EthereumDataType data_type;
        std::optional<uint32_t> size;
        //EthereumTypedDataStructAck_EthereumFieldType entry_type;
        //entry_type.Clear();
        auto* raw_ptr = (EthereumTypedDataStructAck_EthereumFieldType*)nullptr;
        //std::shared_ptr<EthereumTypedDataStructAck_EthereumFieldType> entry_type = nullptr;
        std::string struct_name;
        if (jub::eth::EIP712::is_array(type_name)) {
            data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_ARRAY;
            size = jub::eth::EIP712::parse_array_n(type_name);
            std::string member_typename = jub::eth::EIP712::typeof_array(type_name);

            // 递归处理数组元素类型
            //entry_type = get_field_type(member_typename, types);
            EthereumTypedDataStructAck_EthereumFieldType member_type = get_field_type(member_typename, types);
            if (member_type.data_type() == EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_ARRAY) {
                //if (entry_type.data_type() == EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_ARRAY) {
                throw std::runtime_error("Nested arrays are not supported");
            }
            raw_ptr = new EthereumTypedDataStructAck_EthereumFieldType(
                    std::move(member_type)
            );
            //entry_type = std::make_shared<EthereumTypedDataStructAck_EthereumFieldType>(std::move(member_type));//创建智能指针对象

            // 检查嵌套数组
/*            if (member_type->data_type() == EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_ARRAY) {
                //if (entry_type.data_type() == EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_ARRAY) {
                throw std::runtime_error("Nested arrays are not supported");
            }*/

           // throw std::runtime_error("arrays are not supported");//暂不支持数组
        }
        else if (type_name.find("uint") == 0) {
            data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_UINT;
            size = jub::eth::EIP712::get_byte_size_for_int_type(type_name);
        }
        else if (type_name.find("int") == 0) {
            data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_INT;
            size = jub::eth::EIP712::get_byte_size_for_int_type(type_name);
        }
        else if (type_name.find("bytes") == 0) {
            data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_BYTES;
            size = jub::eth::EIP712::parse_bytes_n(type_name);
        }
        else if (type_name == "string") {
            data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_STRING;
        }
        else if (type_name == "bool") {
            data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_BOOL;
        }
        else if (type_name == "address") {
            data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_ADDRESS;
        }
        else {
            // 检查是否是自定义结构体
            if (types.contains(type_name)) {
                data_type = EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_STRUCT;
                size = types[type_name].size(); // 结构体成员数量
                struct_name = type_name;
            } else {
                throw std::invalid_argument("Unsupported type name: " + type_name);
            }
        }
        EthereumTypedDataStructAck_EthereumFieldType ret;
        ret.Clear();
        if(EthereumTypedDataStructAck_EthereumDataType::EthereumTypedDataStructAck_EthereumDataType_STRUCT == data_type)
            ret.set_struct_name(type_name);
        if(size.has_value())
            ret.set_size(size.value());
        ret.set_data_type(data_type);
        if(nullptr != raw_ptr && raw_ptr->has_data_type())//具有required字段。
        {
            ret.set_allocated_entry_type(raw_ptr);//放弃智能指针所有权,交给protobuf管理。
            //entry_type.release_entry_type();
        }
        return ret;
    }

    JUB_RV JubiterSealer2100Impl::SignTypedData(const bool& bMetamaskV4Compat,
                                 const std::string& typedDataInJSON,
                                 const std::vector<JUB_BYTE>& vPath,
                                 const std::vector<JUB_BYTE>& vChainID,
                                 std::vector<JUB_BYTE>& signatureRaw)
    {
        //unconplete
        std::string msgInPb;
        std::string msgOutPb;
        JUB_UINT16 recvType;
        JUB_RV rv = JUBR_OK;
        nlohmann::json typedData = nlohmann::json::parse(typedDataInJSON);
        if (nlohmann::detail::value_t::object != typedData.type()) {
            return JUBR_DATA_INVALID;
        }
        if (!jub::eth::EIP712::parseJSON(typedData)) {
            jub::eth::EIP712::clearJSON();
            return JUBR_DATA_INVALID;
        }
        nlohmann::json types = typedData["types"];

        try
        {
            //Sending EthereumSignTypedData
            //vChainID和bMetamaskV4Compat不做设置，用默认值
            EthereumSignTypedData msgToBeEncode;
            msgToBeEncode.Clear();
            Address address = parse_path(std::string(vPath.begin(), vPath.end()));
            for (size_t i = 0; i < address.path.size(); ++i)
            {
                msgToBeEncode.add_address_n(address.path[i]);
            }
            if(nlohmann::detail::value_t::string == typedData["primaryType"].type())
                msgToBeEncode.set_primary_type(typedData["primaryType"]);
            else{
                jub::eth::EIP712::clearJSON();
                return JUBR_DATA_INVALID;
            }
            if (!msgToBeEncode.SerializeToString(&msgInPb))
            {
                jub::eth::EIP712::clearJSON();
                return JUBR_ERROR;
            }
            rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_SIGN_TYPEDDATA, msgInPb, &recvType, msgOutPb);
            if (JUBR_OK != rv)
            {
                jub::eth::EIP712::clearJSON();
                return rv;
            }
            EthereumTypedDataStructRequest StructRequest;
            StructRequest.Clear();
            if (!StructRequest.ParseFromString(msgOutPb))
            {
                jub::eth::EIP712::clearJSON();
                return JUBR_ARGUMENTS_BAD;
            }
            EthereumTypedDataValueRequest ValueRequest;
            ValueRequest.Clear();
            //Sending all the types

            while (JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_StructRequest == recvType)
            {
                std::string struct_name = StructRequest.name();
                std::vector<EthereumTypedDataStructAck_EthereumStructMember> members;
                for (auto& item : types[struct_name]) {
                    EthereumTypedDataStructAck_EthereumFieldType FieldType = get_field_type(item["type"],types);
                    auto* raw_ptr = new EthereumTypedDataStructAck_EthereumFieldType(
                            std::move(FieldType)
                    );
                    // 创建智能指针
                    //auto auto_ptr = std::make_shared<EthereumTypedDataStructAck_EthereumFieldType>(
                    //        std::move(FieldType)
                    //);
                    EthereumTypedDataStructAck_EthereumStructMember member;
                    member.set_name(item["name"]);
                    member.set_allocated_type(raw_ptr);
                    //member.set_allocated_type(&FieldType);
                    //member.set_allocated_type(auto_ptr.get());
                    //auto_ptr.reset();
                    members.push_back(member);
                    //member.release_type();
                }
                EthereumTypedDataStructAck StructAck;
                for (const auto& member : members) {
                    // 添加新元素并拷贝数据
                    *StructAck.add_members() = member;
                }
                //google::protobuf::RepeatedPtrField<EthereumTypedDataStructAck_EthereumStructMember> tmp(
                //        members.begin(), members.end()
                //);
                //StructAck.mutable_members()->CopyFrom(tmp);
/*                auto* pb_members = StructAck.mutable_members();
                pb_members->Add();
                pb_members->Swap(
                        reinterpret_cast<google::protobuf::RepeatedPtrField<
                                EthereumTypedDataStructAck_EthereumStructMember>*>(&members)
                );*/

                msgInPb.clear();
                msgOutPb.clear();
                recvType = 0;
                if (!StructAck.SerializeToString(&msgInPb))
                {
                    jub::eth::EIP712::clearJSON();
                    return JUBR_ERROR;
                }
                rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_StructAck, msgInPb, &recvType, msgOutPb);
                if (JUBR_OK != rv)
                {
                    jub::eth::EIP712::clearJSON();
                    return rv;
                }
                if(JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_StructRequest != recvType)//请求完了types
                    break;
                if (!StructRequest.ParseFromString(msgOutPb))
                {
                    jub::eth::EIP712::clearJSON();
                    return JUBR_ARGUMENTS_BAD;
                }

            }
            //Sending the whole message that should be signed
            //here
            if(JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_ValueRequest != recvType)
            {
                jub::eth::EIP712::clearJSON();
                return JUBR_ARGUMENTS_BAD;
            }
            if (!ValueRequest.ParseFromString(msgOutPb))
            {
                jub::eth::EIP712::clearJSON();
                return JUBR_ARGUMENTS_BAD;
            }
            std::string member_typename;
            nlohmann::json member_data;
            while (JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_ValueRequest == recvType)
            {
                //# Index 0 is for the domain data, 1 is for the actual message
                uint32_t root_index = ValueRequest.member_path()[0];//Root index can only be 0 or 1
                if(root_index == 0)
                {
                    member_typename = "EIP712Domain";
                    member_data = typedData["domain"];
                }
                else if(root_index == 1)
                {
                    member_typename = typedData["primaryType"];
                    member_data = typedData["message"];
                }
                else
                {
                    jub::eth::EIP712::clearJSON();
                    return JUBR_ARGUMENTS_BAD;
                }
                //遍历剩余路径
                for (int i = 1; i < ValueRequest.member_path_size(); ++i) {
                    const uint32_t idx = ValueRequest.member_path(i);
                    if (member_data.is_object())//dict
                    {
                        nlohmann::json member_def = types[member_typename][idx];
                        member_typename = member_def["type"];
                        std::string tmp_name = member_def["name"];
                        member_data = member_data[tmp_name];
                    } else if (member_data.is_array())//list
                    {
                        member_typename = jub::eth::EIP712::typeof_array(member_typename);
                        member_data = member_data[idx];
                    } else {
                        jub::eth::EIP712::clearJSON();
                        return JUBR_ARGUMENTS_BAD;
                    }
                }
                std::vector<uint8_t> encoded;
                if(member_data.is_array())//dict
                {
                    // 数组类型：发送长度 (2字节大端序)
                    const size_t length = member_data.size();
                    encoded = {
                            static_cast<uint8_t>((length >> 8) & 0xFF),
                            static_cast<uint8_t>(length & 0xFF)
                    };
                }
                else
                {
                    encoded = jub::eth::EIP712::encode_data(member_typename,member_data);
                    //encoded = jub::eth::EIP712::atomic_typed_data_envelope(member_typename,member_data);
                }
                //发送EthereumTypedDataValueAck
                EthereumTypedDataValueAck ValueAck;
                ValueAck.Clear();
                ValueAck.set_value(encoded.data(), encoded.size());

                msgInPb.clear();
                msgOutPb.clear();
                recvType = 0;
                if (!ValueAck.SerializeToString(&msgInPb))
                {
                    jub::eth::EIP712::clearJSON();
                    return JUBR_ERROR;
                }
                rv = sendProtocolData(JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_ValueAck, msgInPb, &recvType, msgOutPb);
                if (JUBR_OK != rv)
                {
                    jub::eth::EIP712::clearJSON();
                    return rv;
                }
                if(JUB_ENUM_PROTOCOL_ETH_TYPEDDATA_ValueRequest != recvType)//请求完了types
                    break;
                if (!ValueRequest.ParseFromString(msgOutPb))
                {
                    jub::eth::EIP712::clearJSON();
                    return JUBR_ARGUMENTS_BAD;
                }
            }
            //到这里，msgOutPb就是签名结果了。EthereumTypedDataSignature
            EthereumTypedDataSignature TypedDataSignature;

            if (!TypedDataSignature.ParseFromString(msgOutPb))
            {
                return JUBR_ARGUMENTS_BAD;
            }

            if (TypedDataSignature.has_signature())
            {
                signatureRaw.reserve(TypedDataSignature.signature().size());
                signatureRaw.insert(signatureRaw.end(),reinterpret_cast<const JUB_BYTE*>(TypedDataSignature.signature().data()),
                                    reinterpret_cast<const JUB_BYTE*>(TypedDataSignature.signature().data() + TypedDataSignature.signature().size()));
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
    JUB_RV JubiterSealer2100Impl::VerifyTypedData(const bool& bMetamaskV4Compat,
                                   const std::string& typedDataInJSON,
                                   const std::vector<JUB_BYTE>& vSignature,
                                   const std::vector<JUB_BYTE>& publicKey)
    {
        return JUBR_OK;
    }

} // namespace jub end
