//
//  JubiterSealer2100Impl.cpp
//  JubSDK
//
//  Created by Pan Min on 2025/4/26.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#include "token/JubiterSealer2100Impl.h"
#include "device/JubiterBLENusDevice.hpp"
#include "trezorProtocol/ProtocolEncoder.h"
#include "trezorProtocol/ProtocolDecoder.h"

//Features start
#include <nlohmann/json.hpp>
using google::protobuf::Message;
using google::protobuf::Reflection;
using google::protobuf::FieldDescriptor;
using google::protobuf::EnumValueDescriptor;
using nlohmann::json;
//Features end


using namespace hw::trezor::messages::common;
using namespace hw::trezor::messages::management;
using namespace hw::trezor::messages::bootloader;
using namespace hw::trezor::messages;

// avoid name conflict. hw::trezor::message::fs::FsType::FILE vs stdio.h
namespace fs = hw::trezor::messages::fs;

namespace jub {

////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t HARDENED_FLAG = 0x80000000;

uint32_t H_(uint32_t index) {
    return index | HARDENED_FLAG;
}

Address parse_path(const std::string& nstr) {
    if (nstr.empty()) {
        return Address({});
    }

    std::vector<std::string> parts;
    std::stringstream ss(nstr);
    std::string token;

    while (std::getline(ss, token, '/')) {
        parts.push_back(token);
    }

    // Remove leading "m" if present
    if (!parts.empty() && parts[0] == "m") {
        parts.erase(parts.begin());
    }

    auto str_to_harden = [](const std::string& x) -> uint32_t {
        if (!x.empty() && x[0] == '-') {
            int val = std::stoi(x);
            return H_(static_cast<uint32_t>(std::abs(val)));
        } else if (!x.empty() && (x.back() == 'h' || x.back() == '\'')) {
            std::string numberPart = x.substr(0, x.size() - 1);
            return H_(static_cast<uint32_t>(std::stoul(numberPart)));
        } else {
            return static_cast<uint32_t>(std::stoul(x));
        }
    };

    try {
        std::vector<uint32_t> result;
        for (const auto& part : parts) {
            result.push_back(str_to_harden(part));
        }
        return Address(result);
    } catch (const std::exception& e) {
        throw std::invalid_argument("Invalid BIP32 path: " + nstr);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////

JubiterSealer2100Impl::JubiterSealer2100Impl(jub::JubiterBLEDevice* device)
    : JubiterBLDImpl(device) {
    _device = static_cast<const std::shared_ptr<JubiterBLENusDevice>>(dynamic_cast<jub::JubiterBLENusDevice *>(device));
        session_id.clear();
}

JubiterSealer2100Impl::~JubiterSealer2100Impl() {
    End_Session();
};
std::string JubiterSealer2100Impl::bytes_to_hex_fast(const std::string& input) {
    static const char hex_chars[] = "0123456789abcdef";
    std::string output;
    output.reserve(input.size() * 2); // 预分配内存

    for (unsigned char c : input) {
        output.push_back(hex_chars[c >> 4]);    // 高4位
        output.push_back(hex_chars[c & 0x0F]);  // 低4位
    }

    return output;
}
std::string JubiterSealer2100Impl::bin_to_hex(const uint8_t* data, size_t len)
{
    const char hex_chars[] = "0123456789abcdef";
    std::string hex;
    hex.reserve(len * 2);

    for (size_t i = 0; i < len; ++i) {
        hex.push_back(hex_chars[data[i] >> 4]);
        hex.push_back(hex_chars[data[i] & 0x0F]);
    }

    return hex;
}
JUB_RV JubiterSealer2100Impl::Get_Features(Features * msg_Features){
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    if(msg_Features == nullptr){
        return JUBR_ARGUMENTS_BAD;
    } else{
        msg_Features->Clear();
    }   
    

    try{
        GetFeatures msgToBeEncode;
        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }            
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }      
    JUB_RV rv = sendProtocolData(MessageType::MessageType_GetFeatures, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_Features) {
        return JUBR_ERROR;
    }
    try{        
        // Decode the message using standard protobuf
        if (!msg_Features->ParseFromString(msgOutPb)) {
            return JUBR_ERROR;
        }

        // cache features
        features = *msg_Features;
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    } 
    return rv;    
}
JUB_RV JubiterSealer2100Impl::Init_Device() {
    
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    try{
        Initialize msgToBeEncode;
        if(session_id.length() != 0)//
        {
            msgToBeEncode.set_session_id(session_id);
        }
      
        if (!msgToBeEncode.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(MessageType::MessageType_Initialize, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_Features) {
        return JUBR_ERROR;
    }
    Features msg_Features;
    msg_Features.Clear();
    if(!msg_Features.ParseFromString(msgOutPb))
        return JUBR_ERROR;
    if (msg_Features.has_session_id()) {
        session_id = msg_Features.session_id();
    }
    else{
        return JUBR_ERROR;
    }
    // cache features
    features = msg_Features;
    
    return rv;

//    //end session first
//    End_Session();
//    OP_Cancel();
//    Features msg_Features;
//    msg_Features.Clear();
//
//    return Get_Features(&msg_Features);
    
}
JUB_RV JubiterSealer2100Impl::OP_Cancel()
{
    std::string msgInPb;
    //std::string msgOutPb;
    //JUB_UINT16 recvType;

    try{
        Cancel msgToBeEncode;
        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = onlysendData(MessageType::MessageType_Cancel, msgInPb);
    return rv;
}
JUB_RV JubiterSealer2100Impl::End_Session()
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    try{
        EndSession msgToBeEncode;
        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(MessageType::MessageType_EndSession, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_Success) {
        return JUBR_ERROR;
    }
    session_id.clear();
    return rv;
}
// JUB_RV JubiterSealer2100Impl::SelectAppletETH() {

//     return Init_Device();

// }

JUB_RV JubiterSealer2100Impl::GetAppletVersionETH(stVersion& version) {
    (void)version; // Suppress unused variable warning
    return JUBR_OK;
}

JUB_RV JubiterSealer2100Impl::SetTimeout(const JUB_UINT16 timeout){
    (void)timeout; // Suppress unused variable warning
    return JUBR_OK;
}

JUB_RV JubiterSealer2100Impl::onlysendData(uint16_t sendType,const std::string& send_data){

    uchar_vector msgInTrezor = ProtocolEncoder::encodeProtocol(sendType,send_data);
    JUB_CHECK_NULL(_device);
    JUB_RV rv = _device->onlySendData(msgInTrezor.data(), (JUB_ULONG)msgInTrezor.size());
    return  rv;

}

JUB_RV JubiterSealer2100Impl::sendProtocolDataPlaint(uint16_t sendType, const std::string &send_data, uint16_t *recvType, std::string &recvData){
    uchar_vector msgInTrezor = ProtocolEncoder::encodeProtocol(sendType, send_data);
    JUB_CHECK_NULL(_device);
    uint8_t msgOutTrezor[4096] = {
        0,
    };
    JUB_ULONG msgOutLenTrezor = sizeof(msgOutTrezor) / sizeof(uint8_t);
    JUB_RV rv = _device->SendDataPlaint(msgInTrezor.data(), (JUB_ULONG)msgInTrezor.size(), msgOutTrezor, &msgOutLenTrezor);
    if (JUBR_OK != rv)
    {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }
    MessageResponse msgOutPb = ProtocolDecoder::decodeTV(msgOutTrezor, msgOutLenTrezor);
    *recvType = msgOutPb.messageType;
    recvData.clear();
    recvData.assign(msgOutPb.data.begin(), msgOutPb.data.end());
    return filterButtonRequest(*recvType, recvData, recvType, recvData);
}

JUB_RV JubiterSealer2100Impl::sendProtocolData(uint16_t sendType,const std::string& send_data, uint16_t* recvType, std::string& recvData){

    uchar_vector msgInTrezor = ProtocolEncoder::encodeProtocol(sendType,send_data);
    JUB_CHECK_NULL(_device);
    uint8_t msgOutTrezor[4096] = {0,};
    JUB_ULONG msgOutLenTrezor = sizeof(msgOutTrezor)/sizeof(uint8_t);
    JUB_RV rv = _device->SendData(msgInTrezor.data(), (JUB_ULONG)msgInTrezor.size(), msgOutTrezor, &msgOutLenTrezor);
    if (JUBR_OK != rv) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }
    MessageResponse msgOutPb = ProtocolDecoder::decodeTV(msgOutTrezor, msgOutLenTrezor);
    *recvType = msgOutPb.messageType;
    recvData.clear();
    recvData.assign(msgOutPb.data.begin(), msgOutPb.data.end());
    return filterButtonRequest(*recvType, recvData, recvType, recvData);
}

JUB_RV JubiterSealer2100Impl::filterButtonRequest(uint16_t recvType,const std::string& recvData, uint16_t* filteredType,std::string& filteredData){
    uint16_t _recvType = recvType;
    std::string _recvData = recvData;

    int requestCode = 0;

    if (_recvType == MessageType::MessageType_ButtonRequest) {
        try {
            ButtonRequest msgToBeDecode;
            if (!msgToBeDecode.ParseFromString(recvData)) {
                printf("Decode failed: could not parse ButtonRequest\n");
                return JUBR_ARGUMENTS_BAD;
            }
            // 暂时忽略Code
            requestCode = msgToBeDecode.code();
        }
        catch (...) {
            return JUBR_ERROR;
        }
        std::string strMsgInPb;

        // 通知应用层
        _device->notifyMessageType(requestCode);

        try {
            ButtonAck msgToBeEncode;

            if (!msgToBeEncode.SerializeToString(&strMsgInPb)) {
                printf("Encoding failed: could not serialize ButtonAck\n");
                return JUBR_ARGUMENTS_BAD;
            }
        }
        catch (...) {
            return JUBR_ERROR;
        }

        JUB_RV rv = sendProtocolData(MessageType::MessageType_ButtonAck, strMsgInPb, &_recvType, _recvData);
        if (JUBR_OK != rv) {
            return rv;
        }
    }
    else if (_recvType == MessageType::MessageType_PassphraseRequest) {
        try {
            PassphraseRequest msgToBeDecode;
            if (!msgToBeDecode.ParseFromString(recvData)) {
                printf("Decode failed: could not parse PassphraseRequest\n");
                return JUBR_ARGUMENTS_BAD;
            }
        }
        catch (...) {
            return JUBR_ERROR;
        }
        std::string strMsgInPb;

        // 通知应用层,返回用户输入的passphrase
        JUB_ULONG ulLen = 128;
        JUB_BYTE pTmpData[128] = {0x00};
        bool on_device = false;
        
        _device->notifyPassphrase(pTmpData,&ulLen, &on_device);
        
        std::string passphrase{(char*)pTmpData};
        // zero passphrase memory
        memset(pTmpData, 0x00, sizeof(pTmpData));
        try {
            PassphraseAck msgToBeEncode;
            if (on_device) {
                msgToBeEncode.set_on_device(true);
            } else {
                msgToBeEncode.set_passphrase(passphrase);
            }
            auto r = msgToBeEncode.SerializeToString(&strMsgInPb);
            if (!passphrase.empty()) {
                passphrase.assign(passphrase.size(), 0x00);
            }
            if (!r) {
                printf("Encoding failed: could not serialize PassphraseAck\n");
                return JUBR_ARGUMENTS_BAD;
            }
        }
        catch (...) {
            return JUBR_ERROR;
        }

        JUB_RV rv = sendProtocolData(MessageType::MessageType_PassphraseAck, strMsgInPb, &_recvType, _recvData);
        if (!strMsgInPb.empty()) {
            strMsgInPb.assign(strMsgInPb.size(), 0x00);
        }
        if (JUBR_OK != rv) {
            return rv;
        }
    }
    *filteredType = _recvType;
    filteredData = _recvData;
    return JUBR_OK;
}


// JUB_RV JubiterSealer2100Impl::EnumApplet(std::string& appletList) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }


// Base64 编码函数
std::string base64_encode(const std::string& input) {
    static const char base64_chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

    std::string encoded;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    size_t len = input.length();
    const char* bytes = input.data();

    while (len--) {
        char_array_3[i++] = *(bytes++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++) {
                encoded += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; j < i + 1; j++) {
            encoded += base64_chars[char_array_4[j]];
        }

        while (i++ < 3) {
            encoded += '=';
        }
    }

    return encoded;
}

// 处理特殊字段名的转换（下划线开头的字段）
std::string normalize_field_name(const std::string& name) {
    if (!name.empty() && name[0] == '_') {
        return name.substr(1); // 移除开头的下划线
    }
    return name;
}
bool isFieldNeeded(const std::string& FieldName)
{
    std::string tmp_string = FieldName;
    std::transform(tmp_string.begin(), tmp_string.end(), tmp_string.begin(), ::tolower);
    return (0 == std::memcmp(tmp_string.c_str(), "major_version",    std::string("major_version").size())
            || 0 == std::memcmp(tmp_string.c_str(), "minor_version",      std::string("minor_version").size())
            || 0 == std::memcmp(tmp_string.c_str(), "patch_version",   std::string("patch_version").size())
            || 0 == std::memcmp(tmp_string.c_str(), "bootloader_mode",     std::string("bootloader_mode").size())
            || 0 == std::memcmp(tmp_string.c_str(), "passphrase_protection",    std::string("passphrase_protection").size())
            || 0 == std::memcmp(tmp_string.c_str(), "language",    std::string("language").size())
            || 0 == std::memcmp(tmp_string.c_str(), "label",    std::string("label").size())
            || 0 == std::memcmp(tmp_string.c_str(), "initialized",    std::string("initialized").size())
            || 0 == std::memcmp(tmp_string.c_str(), "imported",    std::string("imported").size())
            || 0 == std::memcmp(tmp_string.c_str(), "unlocked",    std::string("unlocked").size())
            || 0 == std::memcmp(tmp_string.c_str(), "model",    std::string("model").size())
            || 0 == std::memcmp(tmp_string.c_str(), "passphrase_always_on_device",    std::string("passphrase_always_on_device").size())
            || 0 == std::memcmp(tmp_string.c_str(), "ble_name",    std::string("ble_name").size())
            || 0 == std::memcmp(tmp_string.c_str(), "ble_ver",    std::string("ble_ver").size())
            || 0 == std::memcmp(tmp_string.c_str(), "ble_enable",    std::string("ble_enable").size())
            || 0 == std::memcmp(tmp_string.c_str(), "se_enable",    std::string("se_enable").size())
            || 0 == std::memcmp(tmp_string.c_str(), "se_ver",    std::string("se_ver").size())
            || 0 == std::memcmp(tmp_string.c_str(), "serial_no",    std::string("serial_no").size())
            || 0 == std::memcmp(tmp_string.c_str(), "device_id",    std::string("device_id").size())
            || 0 == std::memcmp(tmp_string.c_str(), "bootloader_version",    std::string("bootloader_version").size())
            || 0 == std::memcmp(tmp_string.c_str(), "iris_version",    std::string("iris_version").size())
            || 0 == std::memcmp(tmp_string.c_str(), "battery", std::string("battery").size())
            || 0 == std::memcmp(tmp_string.c_str(), "hypermate_version", std::string("hypermate_version").size())
            );
}
// 递归转换 Protobuf 消息为 JSON
json protobuf_to_json(const Message& message) {
    const Reflection* reflection = message.GetReflection();
    const auto* descriptor = message.GetDescriptor();
    json j;

    std::vector<const FieldDescriptor*> fields;
    reflection->ListFields(message, &fields);

    for (const auto* field : fields) {
        // 跳过已弃用的字段
        if (field->options().deprecated()) {
            continue;
        }
        const std::string& field_name = field->name();
        //跳过不是所需字段
        if(!isFieldNeeded(field_name)){
            continue;
        }
        const std::string normalized_name = normalize_field_name(field_name);
        bool is_repeated = field->is_repeated();

        // 处理枚举字段
        if (field->cpp_type() == FieldDescriptor::CPPTYPE_ENUM) {
            if (is_repeated) {
                int count = reflection->FieldSize(message, field);
                for (int idx = 0; idx < count; ++idx) {
                    const EnumValueDescriptor* value =
                            reflection->GetRepeatedEnum(message, field, idx);
                    j[normalized_name].push_back(value->name());
                }
            } else {
                const EnumValueDescriptor* value = reflection->GetEnum(message, field);
                j[normalized_name] = value->name();
            }
            continue;
        }

        // 处理其他字段类型
        switch (field->cpp_type()) {
            case FieldDescriptor::CPPTYPE_INT32: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        j[normalized_name].push_back(reflection->GetRepeatedInt32(message, field, idx));
                    }
                } else {
                    j[normalized_name] = reflection->GetInt32(message, field);
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_UINT32: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        j[normalized_name].push_back(reflection->GetRepeatedUInt32(message, field, idx));
                    }
                } else {
                    j[normalized_name] = reflection->GetUInt32(message, field);
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_INT64: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        j[normalized_name].push_back(static_cast<int64_t>(
                                                             reflection->GetRepeatedInt64(message, field, idx)));
                    }
                } else {
                    j[normalized_name] = static_cast<int64_t>(reflection->GetInt64(message, field));
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_UINT64: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        j[normalized_name].push_back(static_cast<uint64_t>(
                                                             reflection->GetRepeatedUInt64(message, field, idx)));
                    }
                } else {
                    j[normalized_name] = static_cast<uint64_t>(reflection->GetUInt64(message, field));
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_DOUBLE: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        double val = reflection->GetRepeatedDouble(message, field, idx);
                        if (std::isnan(val)) j[normalized_name].push_back("NaN");
                        else if (std::isinf(val)) j[normalized_name].push_back(val < 0 ? "-Infinity" : "Infinity");
                        else j[normalized_name].push_back(val);
                    }
                } else {
                    double val = reflection->GetDouble(message, field);
                    if (std::isnan(val)) j[normalized_name] = "NaN";
                    else if (std::isinf(val)) j[normalized_name] = val < 0 ? "-Infinity" : "Infinity";
                    else j[normalized_name] = val;
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_FLOAT: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        float val = reflection->GetRepeatedFloat(message, field, idx);
                        if (std::isnan(val)) j[normalized_name].push_back("NaN");
                        else if (std::isinf(val)) j[normalized_name].push_back(val < 0 ? "-Infinity" : "Infinity");
                        else j[normalized_name].push_back(val);
                    }
                } else {
                    float val = reflection->GetFloat(message, field);
                    if (std::isnan(val)) j[normalized_name] = "NaN";
                    else if (std::isinf(val)) j[normalized_name] = val < 0 ? "-Infinity" : "Infinity";
                    else j[normalized_name] = val;
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_BOOL: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        j[normalized_name].push_back(reflection->GetRepeatedBool(message, field, idx));
                    }
                } else {
                    j[normalized_name] = reflection->GetBool(message, field);
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_STRING: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        std::string value = reflection->GetRepeatedString(message, field, idx);
                        if (field->type() == FieldDescriptor::TYPE_BYTES) {
                            j[normalized_name].push_back(base64_encode(value));
                        } else {
                            j[normalized_name].push_back(value);
                        }
                    }
                } else {
                    std::string value = reflection->GetString(message, field);
                    if (field->type() == FieldDescriptor::TYPE_BYTES) {
                        j[normalized_name] = base64_encode(value);
                    } else {
                        j[normalized_name] = value;
                    }
                }
                break;
            }
            case FieldDescriptor::CPPTYPE_MESSAGE: {
                if (is_repeated) {
                    int count = reflection->FieldSize(message, field);
                    for (int idx = 0; idx < count; ++idx) {
                        const Message& sub_msg =
                                reflection->GetRepeatedMessage(message, field, idx);
                        j[normalized_name].push_back(protobuf_to_json(sub_msg));
                    }
                } else {
                    const Message& sub_msg = reflection->GetMessage(message, field);
                    j[normalized_name] = protobuf_to_json(sub_msg);
                }
                break;
            }
            default:
                // 处理未支持的类型
                j[normalized_name] = nullptr;
                break;
        }
    }
    j["model"] = "Sealer2100";
    return j;
}
/*顶级键是支持的币种名称，min是开始支持的最小版本，max为最大版本，如果max为空，则表示一直支持
protocols是该币种支持的协议列表和开始支持的固件版本。*/
const nlohmann::json COINS_INFO = {
        {"BTC", {
                {"min", "1.0.0"},
                {"max", ""},
/*                {"protocols", {
                        {"Taproot", {
                                {"min", "2.1.0"},
                                {"max", ""}
                        }}
                }}*/
        }},
        {"ETH", {
                {"min", "1.0.0"},
                {"max", ""},
/*                {"protocols", {
                        {"TypedData", {
                                {"min", "2.0.0"},
                                {"max", ""}
                        }}
                }}*/
        }},
        {"EOS", {
                {"min", "1.0.0"},
                {"max", ""}
        }},
        {"LTC", {
                {"min", "1.0.0"},
                {"max", ""}
        }},
        {"BCH", {
                {"min", "1.0.0"},
                {"max", ""}
        }},
        {"DOGE", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"BNB", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"TRX", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"DOT", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"KSM", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"APTOS", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"SUI", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"SOLANA", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"XRP", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }},
        {"FIL", {
                        {"min", "1.0.0"},
                        {"max", ""}
                }}
};
 JUB_RV JubiterSealer2100Impl::EnumSupportCoins(std::string& coinList) {

     // TODO
     std::vector<std::string> keys;
     for (auto it = COINS_INFO.begin(); it != COINS_INFO.end(); ++it) {
         coinList += it.key();
         coinList += " ";
     }
     return JUBR_OK;
 }
 JUB_RV JubiterSealer2100Impl::IrisSetLable(const std::string& Lable)
 {
     std::string msgInPb;
     std::string msgOutPb;
     JUB_UINT16 recvType;

     try{
         ApplySettings msgToBeEncode;
         msgToBeEncode.Clear();
         msgToBeEncode.set_label(Lable);
         // Encode the message using standard protobuf
         if (!msgToBeEncode.SerializeToString(&msgInPb)) {
             return JUBR_ERROR;
         }
     }
     catch (...) {
         return JUBR_ARGUMENTS_BAD;
     }
     JUB_RV rv = sendProtocolData(MessageType::MessageType_ApplySettings, msgInPb, &recvType, msgOutPb);
     if( JUBR_OK != rv) {
         return rv;
     }
     if (recvType != MessageType::MessageType_Success) {
         return JUBR_ERROR;
     }
     return rv;
 }
JUB_RV JubiterSealer2100Impl::IrisUsePassphrase(const bool bUse)
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    try{
        ApplySettings msgToBeEncode;
        msgToBeEncode.Clear();
        msgToBeEncode.set_use_passphrase(bUse);
        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(MessageType::MessageType_ApplySettings, msgInPb, &recvType, msgOutPb);
    if (!msgInPb.empty()) {
        msgInPb.assign(msgInPb.size(), 0x00);
    }
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_Success) {
        return JUBR_ERROR;
    }
    return rv;
}
 JUB_RV JubiterSealer2100Impl::IrisGetFeatures(std::string& strFeatures)
 {
    //
     Features msg_Features;
     msg_Features.Clear();
     JUB_RV rv = Get_Features(&msg_Features);
     if (JUBR_OK != rv) {
         return rv;
     }
     json j = protobuf_to_json(msg_Features);
     strFeatures = j.dump(4);

     return JUBR_OK;
 }
 JUB_RV JubiterSealer2100Impl::IrisEndSession()
 {
     return  End_Session();
 }
JUB_RV JubiterSealer2100Impl::IrisOpCancel()
{
    return  OP_Cancel();
}
JUB_RV JubiterSealer2100Impl::IrisReboot()
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    try{

        RebootToBootloader msgToBeEncode;
        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(MessageType::MessageType_RebootToBootloader, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_Success) {
        return JUBR_ERROR;
    }
    return rv;
}
JUB_RV JubiterSealer2100Impl::IrisBL_Reboot()
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    try{

        Reboot msg_reboot;
        msg_reboot.set_reboot_type(RebootType::Normal);
        // Encode the message using standard protobuf
        if (!msg_reboot.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(MessageType::MessageType_Reboot, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_Success) {
        return JUBR_ERROR;
    }
    return rv;
}
//make dir
JUB_RV JubiterSealer2100Impl::Make_dir(const std::string& strDirPath)
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    JUB_RV rv = 0;
    try{
        fs::FsMkdir msgToBeEncode;
        msgToBeEncode.set_path(strDirPath);
        // Encode the message using standard protobuf
        if (!msgToBeEncode.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    rv = sendProtocolData(MessageType::MessageType_FsMkdir, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_Success) {
        return JUBR_ERROR;
    }
    return JUBR_OK;

}
size_t JubiterSealer2100Impl::emmc_Writefile(const std::string& strFilePath,
                                             const std::byte* chunk_data,
                                             size_t chunk_size,
                                             size_t offset,
                                             bool is_first_chunk,
                                             int ui_percentage)
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    JUB_RV rv = 0;

    fs::FsFileWrite msg;
    fs::FsFile* file = msg.mutable_file();
    file->set_path(strFilePath);
    file->set_len(chunk_size);
    file->set_offset(offset);
    file->set_data(chunk_data, chunk_size);

    msg.set_overwrite(is_first_chunk);
    msg.set_append(!is_first_chunk);
    msg.set_ui_percentage(ui_percentage);
    if (!msg.SerializeToString(&msgInPb)) {
        return JUBR_ERROR;
    }
    rv = sendProtocolDataPlaint(MessageType::MessageType_FsFileWrite, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return -1;
    }
    if (recvType == MessageType::MessageType_Failure) {//失败
        return -1;
    }
    fs::FsFile msg_file;
    msg_file.Clear();
    if (!msg_file.ParseFromString(msgOutPb))
    {
        return -1;
    }
    return msg_file.processed_byte();

}
JUB_RV JubiterSealer2100Impl::IrisUpdateFirmware(IN JUB_BYTE_PTR firmwareFilePayload,
                                                 IN JUB_UINT32 firmwareFileSize,
                                                 IN bool reboot_on_success)
{
    if (memcmp(firmwareFilePayload+1024, "HPTB", 4) == 0) {
        return updateBootloader(firmwareFilePayload, firmwareFileSize, reboot_on_success);
    }
    if (memcmp(firmwareFilePayload, "HPTB", 4) == 0) {
        return updateBootloader(firmwareFilePayload, firmwareFileSize, reboot_on_success);
    }
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    JUB_RV rv = 0;
    std::string strDirPath = "/updates";
    std::string strFilePath = strDirPath + "/update.bin";
    //make dir
    rv = Make_dir(strDirPath);
    if(rv != JUBR_OK)
        return rv;

    //写升级文件
    size_t local_file_size = firmwareFileSize;//总文件大小
    size_t processed_size = 0;//已处理文件大小
    size_t chunk_size_bytes = 16384;//每包大小。byte
    try{
        while (processed_size < local_file_size) {
            size_t next_process_size = std::min(chunk_size_bytes, local_file_size - processed_size);
            // 计算 UI 百分比
            int ui_percentage = static_cast<int>(100 * ((processed_size + next_process_size) / static_cast<double>(local_file_size)));
           //发送应用层回调进度
            _device->notifyPercentage(ui_percentage);
            size_t res_len = emmc_Writefile(strFilePath,reinterpret_cast<const std::byte*>(firmwareFilePayload + processed_size),
                                            next_process_size,processed_size,(processed_size == 0),ui_percentage);
            if(res_len == -1)
                return JUBR_ERROR;

            processed_size += res_len;//记录已处理的数据大小
        }
        //已发送完毕，执行安装
        msgInPb.clear();
        msgOutPb.clear();
        FirmwareUpdate msg_updateemmc;
        msg_updateemmc.Clear();
        msg_updateemmc.set_path(strFilePath);
        msg_updateemmc.set_reboot_on_success(reboot_on_success);
        if (!msg_updateemmc.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
        rv = sendProtocolData(MessageType::MessageType_FirmwareUpdate, msgInPb, &recvType, msgOutPb);
        if( JUBR_OK != rv) {
            return rv;
        }
        if (recvType != MessageType::MessageType_Success) {//失败
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    return JUBR_OK;
}

JUB_RV JubiterSealer2100Impl::updateBootloader(IN JUB_BYTE_PTR firmwareFilePayload,
                                               IN JUB_UINT32 firmwareFileSize,
                                               IN bool reboot_on_success)
{
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;
    JUB_RV rv = 0;
    std::string strDirPath = "/boot";
    std::string strFilePath = strDirPath + "/bootloader.bin";
    //make dir
    rv = Make_dir(strDirPath);
    if(rv != JUBR_OK)
        return rv;

    //写升级文件
    size_t local_file_size = firmwareFileSize;//总文件大小
    size_t processed_size = 0;//已处理文件大小
    size_t chunk_size_bytes = 16384;//每包大小。byte
    try{
        while (processed_size < local_file_size) {
            size_t next_process_size = std::min(chunk_size_bytes, local_file_size - processed_size);
            // 计算 UI 百分比
            int ui_percentage = static_cast<int>(100 * ((processed_size + next_process_size) / static_cast<double>(local_file_size)));
           //发送应用层回调进度
            _device->notifyPercentage(ui_percentage);
            size_t res_len = emmc_Writefile(strFilePath,reinterpret_cast<const std::byte*>(firmwareFilePayload + processed_size),
                                            next_process_size,processed_size,(processed_size == 0),ui_percentage);
            if(res_len == -1)
                return JUBR_ERROR;

            processed_size += res_len;//记录已处理的数据大小
        }
        //已发送完毕，执行安装
        msgInPb.clear();
        msgOutPb.clear();

        //是否重启？
        if(reboot_on_success)
        {
            Reboot msg_reboot;
            msg_reboot.set_reboot_type(RebootType::Normal);
            // Encode the message using standard protobuf
            if (!msg_reboot.SerializeToString(&msgInPb)) {
                return JUBR_ERROR;
            }
            rv = sendProtocolData(MessageType::MessageType_Reboot, msgInPb, &recvType, msgOutPb);
            if( JUBR_OK != rv) {
                return rv;
            }
            if (recvType != MessageType::MessageType_Success) {
                return JUBR_ERROR;
            }
            else
                return JUBR_OK;
            
        }
        else
            return JUBR_OK;
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    return JUBR_OK;
}
//#include "../../../../build/ios-dev/deps/libzip/zipconf.h"//"/Users/home/Work/HyperPay-Jub_SDK_CXX/build/ios-dev/deps/libzip/zipconf.h"
#include "lib/zip.h"

#define MAX_FILE_SIZE (64 * 1024 * 1024) // 64MB

JUB_RV JubiterSealer2100Impl::IrisUpdateResource(IN JUB_BYTE_PTR resourceZipPayload,
                                                 IN JUB_UINT32 resourceZipSize,
                                                 IN bool reboot_on_success) {
    JUB_RV ret = JUBR_OK;
    std::string strRootDirPath = "/res";

    // 创建错误指针
    zip_error_t error;

    // 从内存创建zip源
    zip_source_t *src = zip_source_buffer_create(resourceZipPayload, resourceZipSize, 0, &error);
    if (!src) {
        return JUBR_INVALID_MEMORY_PTR;
    }

    // 打开zip文件
    zip_t *za = zip_open_from_source(src, 0, &error);
    if (!za) {
        zip_source_free(src);
        return JUBR_INVALID_MEMORY_PTR;
    }

    // 计算总解压大小和文件数量
    int64_t total_uncompressed_size = 0;
    int file_count = zip_get_num_entries(za, 0);
    for (int i = 0; i < file_count; i++) {
        struct zip_stat st;
        if (zip_stat_index(za, i, 0, &st) == 0) {
            // 只计算文件（排除目录）
            if (st.name && st.name[strlen(st.name)-1] != '/' && st.size > 0) {
                total_uncompressed_size += st.size;
            }
        }
    }

    int64_t total_processed_bytes = 0;
    // 遍历zip中的所有文件
    for (int i = 0; i < file_count; i++) {
        struct zip_stat st;
        if (zip_stat_index(za, i, 0, &st) != 0) {
            ret = JUBR_INVALID_MEMORY_PTR;
            break;
        }

        if (!st.name) {
            continue; // 跳过无效文件名
        }

        std::string filename_inzip = st.name;
        std::string full_path = [&]() {
            // filter upgrader files
            if (filename_inzip.rfind("upgrader/") == 0) {
               return "/" + filename_inzip;
            }
            auto path = strRootDirPath;
            path.append("/");
            path.append(filename_inzip);
            return path;
        }();
        printf("IrisUpdateResource,full_path = %s \n", full_path.c_str());

        // 安全检查：防止路径遍历
        if (full_path.find("..") != std::string::npos) {
            ret = JUBR_INVALID_MEMORY_PTR;
            break;
        }

        // 处理目录
        if (st.name[strlen(st.name)-1] == '/') {
            ret = Make_dir(full_path);
            if (ret != JUBR_OK) {
                break;
            }
            continue;
        }

        // 确保父目录存在
        size_t last_slash = full_path.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            std::string parent_dir = full_path.substr(0, last_slash);
            if (!parent_dir.empty()) {
                ret = Make_dir(parent_dir);
                if (ret != JUBR_OK) {
                    break;
                }
            }
        }

        // 打开zip中的文件
        zip_file_t *file = zip_fopen_index(za, i, 0);
        if (!file) {
            ret = JUBR_INVALID_MEMORY_PTR;
            break;
        }

        // 检查文件大小限制
        if (st.size > MAX_FILE_SIZE) {
            zip_fclose(file);
            ret = JUBR_INVALID_MEMORY_PTR;
            break;
        }

        std::vector<JUB_BYTE> file_data(st.size);
        int64_t bytes_read = zip_fread(file, file_data.data(), st.size);
        zip_fclose(file);

        if (bytes_read != st.size) {
            ret = JUBR_INVALID_MEMORY_PTR;
            break;
        }

        // 分块写入文件并更新进度
        size_t processed_size = 0;
        size_t chunk_size_bytes = 16384;
        while (processed_size < file_data.size()) {
            size_t next_process_size = std::min(chunk_size_bytes, file_data.size() - processed_size);
            total_processed_bytes += next_process_size;
            int ui_percentage = static_cast<int>(100 * (total_processed_bytes / static_cast<double>(total_uncompressed_size)));

            _device->notifyPercentage(ui_percentage);

            size_t res_len = emmc_Writefile(full_path,
                                           reinterpret_cast<const std::byte*>(file_data.data() + processed_size),
                                           next_process_size,
                                           processed_size,
                                           (processed_size == 0),
                                           ui_percentage);
            if (res_len == -1) {
                ret = JUBR_INVALID_MEMORY_PTR;
                goto cleanup;
            }
            processed_size += res_len;
        }
    }

cleanup:
    zip_close(za); // 会自动释放src
    // 注意：zip_close后不需要单独释放src，因为zip_open_from_source转移了所有权

    if (reboot_on_success && ret == JUBR_OK) {
        std::string msgInPb;
        std::string msgOutPb;
        JUB_UINT16 recvType;
        Reboot msg_reboot;
        msg_reboot.set_reboot_type(RebootType::Normal);
        if (!msg_reboot.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
        ret = sendProtocolData(MessageType::MessageType_Reboot, msgInPb, &recvType, msgOutPb);
        if (JUBR_OK != ret) {
            return ret;
        }
        if (recvType != MessageType::MessageType_Success) {
            return JUBR_ERROR;
        }
    }
    return ret;
}

JUB_RV JubiterSealer2100Impl::GetDeviceCert(std::string& cert) {
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    try{

        ReadSEPublicCert msg_cert;
        // Encode the message using standard protobuf
        if (!msg_cert.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(MessageType::MessageType_ReadSEPublicCert, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_SEPublicCert) {
        return JUBR_ERROR;
    }
    try
    {
        SEPublicCert msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(msgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        if (msgToBeDecode.has_public_cert())
        {
            cert = msgToBeDecode.public_cert();
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
//    size_t certificate_size = sizeof(certificate_data);
//
//    cert = std::string(reinterpret_cast<const char*>(certificate_data), certificate_size);
//
// // TODO
//  return JUBR_OK;
}

JUB_RV JubiterSealer2100Impl::GetDeviceSignData(const std::string& hashData,std::string& SignData) {
    std::string msgInPb;
    std::string msgOutPb;
    JUB_UINT16 recvType;

    try{

        SESignMessage msg_signdata;
        msg_signdata.set_message(hashData);
        
        // Encode the message using standard protobuf
        if (!msg_signdata.SerializeToString(&msgInPb)) {
            return JUBR_ERROR;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = sendProtocolData(MessageType::MessageType_SESignMessage, msgInPb, &recvType, msgOutPb);
    if( JUBR_OK != rv) {
        return rv;
    }
    if (recvType != MessageType::MessageType_SEMessageSignature) {
        return JUBR_ERROR;
    }
    try
    {
        SEMessageSignature msgToBeDecode;
        if (!msgToBeDecode.ParseFromString(msgOutPb))
        {
            return JUBR_ARGUMENTS_BAD;
        }
        if (msgToBeDecode.has_signature())
        {
            uchar_vector vRaw(msgToBeDecode.signature().begin(),msgToBeDecode.signature().end());
            //SignData = msgToBeDecode.signature();
            SignData = vRaw.getHex();
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


//    SignData = "3045022100f4a1fb45a3fe394ba45b1d65820fd53a70567d5115b41d7d1eab9070c37bc8cf02207b465248571e81a4efc9bf10ecb53da65388e40357b02783a61ccf4609faabe5";
//
//    // TODO
//    return JUBR_OK;
}


// JUB_RV JubiterSealer2100Impl::SendOneApdu(const std::string& apdu, std::string& response) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }

//common functions
JUB_RV JubiterSealer2100Impl::QueryBattery(JUB_BYTE &percent) {

    // TODO
    (void)percent;
    return JUBR_OK;
}


JUB_RV JubiterSealer2100Impl::ShowVirtualPwd() {

    // TODO
    return JUBR_OK;
}


JUB_RV JubiterSealer2100Impl::CancelVirtualPwd() {

    // TODO
    return JUBR_OK;
}


JUB_RV JubiterSealer2100Impl::VerifyPIN(const std::string &pinMix, OUT JUB_ULONG &retry) {

    // TODO
    return JUBR_IMPL_NOT_SUPPORT;
}


bool JubiterSealer2100Impl::IsInitialize() {

    // TODO
    Features msg_Features;
    msg_Features.Clear();

    JUB_RV rv = Get_Features(&msg_Features);
    if (JUBR_OK != rv) {
        return false;
    }
    if(!msg_Features.has_initialized()){
        return false;
    }
    return msg_Features.initialized();
}


bool JubiterSealer2100Impl::IsBootLoader() {

    // TODO
    Features msg_Features;
    msg_Features.Clear();

    JUB_RV rv = Get_Features(&msg_Features);
    if (JUBR_OK != rv) {
        return false;
    }
    if(!msg_Features.has_bootloader_mode())
    {
        return false;
    }
    return msg_Features.bootloader_mode();
}


JUB_RV JubiterSealer2100Impl::GetBleVersion(JUB_BYTE bleVersion[4]) {

    // TODO
    Features msg_Features;
    msg_Features.Clear();

    JUB_RV rv = Get_Features(&msg_Features);
    if (JUBR_OK != rv) {
        return rv;
    }
    if(!msg_Features.has_ble_ver()){
        return JUBR_ARGUMENTS_BAD;
    }
    memset(bleVersion,0x00,4);
    std::string strVer = msg_Features.ble_ver();
    size_t iLength = (strVer.length() < 4) ? strVer.length() : 4;
    memcpy(bleVersion,strVer.data(),iLength);

    return rv;
}


JUB_RV JubiterSealer2100Impl::GetFwVersion(JUB_BYTE fwVersion[4]) {

    // TODO
    Features msg_Features;
    msg_Features.Clear();

    JUB_RV rv = Get_Features(&msg_Features);
    if (JUBR_OK != rv) {
        return rv;
    }
    if(!msg_Features.has_hypermate_version()){
        return JUBR_ARGUMENTS_BAD;
    }
    memset(fwVersion,0x00,4);
    std::string strVer = msg_Features.hypermate_version();
    size_t iLength = (strVer.length() < 4) ? strVer.length() : 4;
    memcpy(fwVersion,strVer.data(),iLength);
    return rv;
}


JUB_RV JubiterSealer2100Impl::GetSN(JUB_BYTE sn[24]) {

    // TODO
    Features msg_Features;
    msg_Features.Clear();

    JUB_RV rv = Get_Features(&msg_Features);
    if (JUBR_OK != rv) {
        return rv;
    }
    if(!msg_Features.has_serial_no()){
        return JUBR_ARGUMENTS_BAD;
    }
    memset(sn,0x00,24);
    std::string strVer = msg_Features.serial_no();
    size_t iLength = (strVer.length() < 24) ? strVer.length() : 24;
    memcpy(sn,strVer.data(),iLength);
    return rv;
}


JUB_RV JubiterSealer2100Impl::GetLabel(JUB_BYTE label[32]) {

    // TODO
    Features msg_Features;
    msg_Features.Clear();

    JUB_RV rv = Get_Features(&msg_Features);
    if (JUBR_OK != rv) {
        return rv;
    }
    if(!msg_Features.has_label()){
        return JUBR_ARGUMENTS_BAD;
    }
    memset(label,0x00,32);
    std::string strVer = msg_Features.label();
    size_t iLength = (strVer.length() < 32) ? strVer.length() : 32;
    memcpy(label,strVer.data(),iLength);
    return rv;
}


JUB_RV JubiterSealer2100Impl::GetPinRetry(JUB_BYTE& retry) {

    // TODO
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterSealer2100Impl::GetPinMaxRetry(JUB_BYTE& maxRetry) {

    // TODO
    return JUBR_IMPL_NOT_SUPPORT;
}


// JUB_RV JubiterSealer2100Impl::SetTimeout(const JUB_UINT16 timeout) {

//     return JUBR_OK;
// }


// // Iris
// JUB_RV JubiterSealer2100Impl::ConnectToken() {
//     return JUBR_IMPL_NOT_SUPPORT;
// }

// JUB_RV JubiterSealer2100Impl::SendOnePbMessage(const std::string& pbMessage, std::string& response) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }

// JUB_RV JubiterSealer2100Impl::DisconnectToken() {
//     return JUBR_IMPL_NOT_SUPPORT;
// }
// JUB_RV JubiterSealer2100Impl::SetERC20Token(const std::string& tokenName, const JUB_UINT16 unitDP, const std::string& contractAddress) {
//     return JUBR_IMPL_NOT_SUPPORT;
// }
// JUB_RV JubiterSealer2100Impl::SetERC20Tokens(const ERC20_TOKEN_INFO tokens[],
//                                       const JUB_UINT16 iCount) {
//     return JUBR_IMPL_NOT_SUPPORT;
// }
// JUB_RV JubiterSealer2100Impl::UIShowMain() {

//     return JUBR_IMPL_NOT_SUPPORT;
// }
// JUB_RV JubiterSealer2100Impl::IdentityVerify(IN JUB_BYTE mode, OUT JUB_ULONG &retry) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }
// JUB_RV JubiterSealer2100Impl::IdentityVerifyPIN(IN JUB_BYTE mode, IN const std::string &pinMix, OUT JUB_ULONG &retry) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }
// JUB_RV JubiterSealer2100Impl::IdentityNineGrids(IN bool bShow) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }
// JUB_RV JubiterSealer2100Impl::VerifyFingerprint(OUT JUB_ULONG &retry) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }


// JUB_RV JubiterSealer2100Impl::EnrollFingerprint(IN JUB_UINT16 fpTimeout,
//                                          INOUT JUB_BYTE_PTR fgptIndex, OUT JUB_ULONG_PTR ptimes,
//                                          OUT JUB_BYTE_PTR fgptID) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }


// JUB_RV JubiterSealer2100Impl::EnumFingerprint(std::string& fgptList) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }


// JUB_RV JubiterSealer2100Impl::EraseFingerprint(IN JUB_UINT16 fpTimeout) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }


// JUB_RV JubiterSealer2100Impl::DeleteFingerprint(IN JUB_UINT16 fpTimeout,
//                                          JUB_BYTE fgptID) {

//     return JUBR_IMPL_NOT_SUPPORT;
// }


}  // namespace jub end
