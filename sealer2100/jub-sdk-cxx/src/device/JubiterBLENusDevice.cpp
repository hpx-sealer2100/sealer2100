#include "device/DeviceIOLogHelper.hpp"
#include <thread>
#include <future>
#include <string>
#include <random>
#include <thread>
#include <chrono>


#ifdef BLE_MODE

#include <unistd.h>
#include <sys/time.h>
#include "utility/util.h"
#include "utility/Singleton.h"
#include "mSIGNA/stdutils/uchar_vector.h"

#include "device/JubiterBLENusDevice.hpp"

#ifdef __ANDROID__
#include "log_utils.h"
#include "bleTransmit/android/BTManager.h"
#else
#include "ErrorCodesAndMacros.h"
#include "BLEInterface.h"
#endif

namespace jub {

//  JubiterBLENusDevice::JubiterBLENusDevice():
//  _param1{nullptr, GDBLE_ReadCallBack, GDBLE_ScanCallBack, GDBLE_DiscCallBack},
//  _handle(0),
// _bConnected(false) {
//
//  }
//

//template <uint8_t N>
//struct sized_payload_t{
//    static constexpr  uint8_t size = N;
//    uint8_t payload[N];
//};

template <uint8_t N, class T>
struct sized_payload_t{
    uint8_t size;
    T payload[N];
}__attribute__((packed));

JubiterBLENusDevice::JubiterBLENusDevice():
    _param1{nullptr, GDBLE_ReadCallBack, GDBLE_ScanCallBack, GDBLE_DiscCallBack},
    _handle(0),
    _bConnected(false),
    m_readCallback(nullptr),
    m_writeCallback(nullptr){

}

JubiterBLENusDevice::~JubiterBLENusDevice() {
//    Disconnect();
}
typedef enum secure_channel_cammand
{
    SC_OPEN = 1,
    SC_SUCCESS = 2,
    SC_FAILED = 3,
    SC_CLOSE = 4,
    SC_TRANS = 5,
} secure_channel_cammand;

#define SECURE_CHANNEL_MAGIC_0 '#'
#define SECURE_CHANNEL_MAGIC_1 '?'

#define info_prefix "Sealer2100-AESKey-v1"
#define pre_set_info "aFHDfju2h!@#Sxv~"

static int rng_function(void* ctx, unsigned char* buf, size_t len) {
    if (!buf || len == 0) {
        return -1;
    }
    try {
        std::random_device rd;
        for (size_t i = 0; i < len; ++i) {
            buf[i] = static_cast<unsigned char>(rd());
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

static std::string get_random_device_id(){
    char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "_-@#";
    std::string id;
    id.resize(16);
    std::random_device rd;
    std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);
    for (auto& c : id) {
        c = charset[dist(rd)];
    }
    return id;
}

static bool secure_channel_decode_encrypted_message(const uint8_t* data, size_t data_len,
                                                    uint8_t iv[IV_SIZE],
                                                    uint8_t mac_tag[MAC_TAG_SIZE],
                                                    uchar_vector& encrypted_message)
{
    // #??<iv_len><iv><mac_tag_len><mac_tag><encrypted_message_len><encrypted_message>
    if (data_len < 7 + IV_SIZE + MAC_TAG_SIZE) {
        return false;
    }
    size_t offset = 4;
    uint8_t iv_len = data[offset];
    if (iv_len != IV_SIZE) {
        return false;
    }
    offset += 1;
    memcpy(iv, data + offset, IV_SIZE);
    offset += IV_SIZE;
    uint8_t mac_tag_len = data[offset];
    if (mac_tag_len != MAC_TAG_SIZE) {
        return false;
    }
    offset += 1;
    memcpy(mac_tag, data + offset, MAC_TAG_SIZE);
    offset += MAC_TAG_SIZE;
    if (data_len < offset + 2) {
        return false;
    }
    uint16_t encrypted_message_len = (data[offset] << 8) | data[offset + 1];
    offset += 2;
    if (data_len < offset + encrypted_message_len) {
        return false;
    }
    encrypted_message.assign(data + offset, data + offset + encrypted_message_len);
    return true;
}

static uchar_vector secure_channel_encode_encrypted_message(const uint8_t* encrypted_message, size_t encrypted_message_len,uint8_t iv[IV_SIZE],uint8_t mac_tag[MAC_TAG_SIZE])
{
    uchar_vector data_send_buf;
    // ?##<SC_TRANS><iv_len><iv><mac_tag_len><mac_tag><encrypted_message_len><encrypted_message>
    data_send_buf.reserve(7 + IV_SIZE + MAC_TAG_SIZE + encrypted_message_len);
    data_send_buf.push_back('#');
    data_send_buf.push_back('?');
    data_send_buf.push_back('?');
    data_send_buf.push_back(SC_TRANS);
    // iv length + iv
    data_send_buf.push_back(static_cast<unsigned char>(IV_SIZE));
    data_send_buf.insert(data_send_buf.end(),
                             iv,
                             iv + IV_SIZE);
    // mac_tag length + mac_tag
    data_send_buf.push_back(static_cast<unsigned char>(MAC_TAG_SIZE));
    data_send_buf.insert(data_send_buf.end(),
                             mac_tag,
                             mac_tag + MAC_TAG_SIZE);  
    // encrypted_message length + encrypted_message
    uint16_t len16 = static_cast<uint16_t>(encrypted_message_len);
    data_send_buf.push_back(static_cast<unsigned char>((len16 >> 8) & 0xFF));
    data_send_buf.push_back(static_cast<unsigned char>(len16 & 0xFF));
    data_send_buf.insert(data_send_buf.end(), encrypted_message, encrypted_message + encrypted_message_len);
    return data_send_buf;
}

static uchar_vector secure_channel_encode_open_response(secure_channel_context *ctx,const std::string& self_device_id)
{
    uchar_vector data_received_buf;
    // ?##<SC_OPEN><pubkey_len><pubkey><nonce_len><nonce><a_deviceid_len><a_deviceid>

    data_received_buf.reserve(7 + PUBLIC_KEY_SIZE + NONCE_SIZE + self_device_id.size());

    data_received_buf.push_back('#');
    data_received_buf.push_back('?');
    data_received_buf.push_back('?');
    data_received_buf.push_back(SC_OPEN);

    // pubkey length + pubkey
    data_received_buf.push_back(static_cast<unsigned char>(PUBLIC_KEY_SIZE));
    data_received_buf.insert(data_received_buf.end(),
                             ctx->self_ecdh_public_key,
                             ctx->self_ecdh_public_key + PUBLIC_KEY_SIZE);

    // nonce length + nonce
    data_received_buf.push_back(static_cast<unsigned char>(NONCE_SIZE));
    data_received_buf.insert(data_received_buf.end(),
                             ctx->a_nonce,
                             ctx->a_nonce + NONCE_SIZE);

    // self id
    size_t self_id_len = self_device_id.size();
    data_received_buf.push_back(static_cast<unsigned char>(self_id_len));
    data_received_buf.insert(data_received_buf.end(), self_device_id.begin(), self_device_id.end());

    return data_received_buf;
}
 

unsigned int JubiterBLENusDevice::Connect(unsigned char* devName,    /**< ble device name */
                                            unsigned char* bBLEUUID,   /**< ble device UUID */
                                            unsigned int connectType,  /**< ble device connect type */
                                            unsigned long* pdevHandle, /**< output ble device connect handle */
                                            unsigned int timeout) {
    secure_channel_destroy(&m_sc_context);
#ifdef __ANDROID__
        _handle = FAKE_DEVICE_HANDLE;
    *pdevHandle = _handle;
    _bConnected = true;
    return JUBR_OK;
#else
        std::string uuid = reinterpret_cast<char *>(bBLEUUID);
#if defined(__APPLE__)
//    uuid = reinterpret_cast<char*>(devName);
#endif // #if defined(__APPLE__)

    unsigned int ret = GD_BLE_ConnDev((unsigned char*)uuid.c_str(), connectType, pdevHandle, timeout);
    
    if (IFD_SUCCESS == ret) {
        _handle = *pdevHandle;
        _bConnected = true;
    }
    return (unsigned int)MatchErrorCode(ret);
#endif
    
}

JUB_RV JubiterBLENusDevice::openSecureChannel(void){
    std::string get_random_device_id_str = get_random_device_id();
    sc_error_code code = secure_channel_init_a(&m_sc_context, rng_function,
                                               get_random_device_id_str.c_str(),
                                               "");
    if (code != SUCCESS)
    {
        return JUBR_HOST_MEMORY;
    }

    uchar_vector open_response = secure_channel_encode_open_response(&m_sc_context, get_random_device_id_str);

    JUB_ULONG ulSendLen = open_response.size();
    JUB_BYTE_PTR sendData = open_response.data();
    JUB_BYTE retData[512] = {0};
    JUB_ULONG pulRetDataLen = sizeof(retData);
    JUB_RV rv = SendDataPlaint(sendData, ulSendLen, retData, &pulRetDataLen, 5000);
    if (JUBR_OK != rv)
    {
        return rv;
    }

    if (pulRetDataLen < 2) {
        return JUBR_ERROR;
    }

    JUB_ULONG newLen = pulRetDataLen - 2;
    memmove(retData, retData + 2, newLen);
    memset(retData + newLen, 0, 2);
    pulRetDataLen = newLen;

    struct ble_sec_open_resp_t{
        uint8_t magic[3];
        uint8_t cmd;
        sized_payload_t<PUBLIC_KEY_SIZE, uint8_t> ecdh_pk;
        sized_payload_t<NONCE_SIZE, uint8_t> nonce;
        sized_payload_t<DEVICE_ID_SIZE, char> device_id;
    }__attribute__((packed));
    if (pulRetDataLen != sizeof(ble_sec_open_resp_t)) {
        JUBR_ERROR;
    }
    auto* resp = static_cast<ble_sec_open_resp_t*>((void*)retData);

    // check magic and cmd
    if (resp->magic[0] != SECURE_CHANNEL_MAGIC_0 ||
        resp->magic[1] != SECURE_CHANNEL_MAGIC_1 ||
        resp->magic[2] != SECURE_CHANNEL_MAGIC_1 ||
        resp->cmd != SC_OPEN) {
        return JUBR_ERROR;
    }
    // check `pk` `nonce` `device_id` size
    if (resp->ecdh_pk.size != PUBLIC_KEY_SIZE ||
        resp->nonce.size != NONCE_SIZE ||
        resp->device_id.size != DEVICE_ID_SIZE) {
        return JUBR_ERROR;
    }

    // cache peer deviceid
    strncpy(m_sc_context.b_deviceid, resp->device_id.payload, DEVICE_ID_SIZE);
    code = secure_channel_open(
            &m_sc_context,
            resp->ecdh_pk.payload,
            resp->nonce.payload,
            (const char*) info_prefix,
            (uint8_t*)pre_set_info,
            strlen(pre_set_info)
            );
    if (code != SUCCESS) {
        return JUBR_HOST_MEMORY;
    }
    return JUBR_OK;
}

unsigned int JubiterBLENusDevice::Disconnect(unsigned long handle) {
    secure_channel_destroy(&m_sc_context);
#ifdef __ANDROID__
    _handle = 0;
    _bConnected = false;
    return JUBR_OK;
#else
    if (  0 != _handle
        && IFD_SUCCESS == GD_BLE_DisConn(_handle)
        ) {

        _handle = 0;
    }
    _handle = 0;
    _bConnected = false;
    return JUBR_OK;
#endif
}

JUB_RV JubiterBLENusDevice::SendDataPlaint(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                                     OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                                     IN JUB_ULONG ulMiliSecondTimeout)
{
#ifdef __ANDROID__
    if (nullptr == m_writeCallback || nullptr == m_readCallback)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = m_writeCallback((JUB_CHAR_PTR)sendData, (JUB_UINT16)ulSendLen);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    JUB_UINT16 dataLen = 0;
    JUB_CHAR_PTR pData = nullptr;
    // Use std::thread and std::future to implement timeout for blocking m_readCallback
    //    std::promise<JUB_RV> prom;
    //    std::future<JUB_RV> fut = prom.get_future();
    //
    //    std::thread readThread([&prom, this, &pData, &dataLen]() {
    //        JUB_RV readRv = m_readCallback(&pData, &dataLen);
    //        prom.set_value(readRv);
    //    });
    //
    //    if (fut.wait_for(std::chrono::milliseconds(ulMiliSecondTimeout)) == std::future_status::ready) {
    //        rv = fut.get();
    //    } else {
    //        rv = JUBR_ERROR;
    //    }
    //
    //    if (readThread.joinable()) {
    //        readThread.join();
    //    }
    rv = m_readCallback(&pData, &dataLen);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (nullptr == pData || 0 == dataLen)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    if (dataLen > *pulRetDataLen)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    *pulRetDataLen = dataLen;
    memcpy(retData, pData, dataLen);
    if (pData)
    {
        delete[] pData; // Assuming pData was allocated with new[]
        pData = nullptr;
    }
    return JUBR_OK;
#else
    // Xufeng TODO

    unsigned int ret = 0;

    unsigned int recvlen = 0;

    ret = GD_BLE_SendAPDU_Sync(_handle, const_cast<unsigned char *>(sendData), ulSendLen, retData, &recvlen, ulMiliSecondTimeout);

    // printf("recvlen------->%d------ret---%d",recvlen,ret);
    *pulRetDataLen = recvlen;

    return JUBR_OK;

#endif
}

static void dump_hex(const char* title, const uint8_t* data, size_t len) {
    printf("%s: ", title);
    for (size_t i = 0; i < len; ++i) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

JUB_RV JubiterBLENusDevice::SendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                                      OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                                      IN JUB_ULONG ulMiliSecondTimeout) {
    if(!m_sc_context.is_open){
        openSecureChannel();
    }

#ifdef __ANDROID__
        if (nullptr == m_writeCallback || nullptr == m_readCallback) {
        return JUBR_ARGUMENTS_BAD;
    }
    if (ulSendLen % 64 != 0) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = JUBR_OK;

    for (int pos = 0; pos < ulSendLen; pos += 64)
    {
        uint8_t iv[IV_SIZE] = {0};
        uint8_t mac_tag[MAC_TAG_SIZE] = {0};
        uint8_t ciphertext[128] = {0};
        size_t ciphertext_len = 128;
        sc_error_code code = secure_channel_encrypt(&m_sc_context,
                                                   (const uint8_t*)(sendData + pos),
                                                   64,
                                                   ciphertext,
                                                   &ciphertext_len,
                                                   iv,
                                                   mac_tag);
        if (code != SUCCESS) {
            return JUBR_ERROR;
        }
        uchar_vector send_buf =  secure_channel_encode_encrypted_message(ciphertext, ciphertext_len, iv, mac_tag);

        rv = m_writeCallback((JUB_CHAR_PTR)send_buf.data(), send_buf.size());
        if (JUBR_OK != rv) {
            return rv;
        }
    }
    JUB_UINT16 dataLen = 0;
    JUB_CHAR_PTR pData = nullptr;
    // Use std::thread and std::future to implement timeout for blocking m_readCallback
//    std::promise<JUB_RV> prom;
//    std::future<JUB_RV> fut = prom.get_future();
//
//    std::thread readThread([&prom, this, &pData, &dataLen]() {
//        JUB_RV readRv = m_readCallback(&pData, &dataLen);
//        prom.set_value(readRv);
//    });
//
//    if (fut.wait_for(std::chrono::milliseconds(ulMiliSecondTimeout)) == std::future_status::ready) {
//        rv = fut.get();
//    } else {
//        rv = JUBR_ERROR;
//    }
//
//    if (readThread.joinable()) {
//        readThread.join();
//    }
    rv = m_readCallback(&pData, &dataLen);
    if (JUBR_OK != rv) {
        return rv;
    }
    if (nullptr == pData || 0 == dataLen) {
        return JUBR_ARGUMENTS_BAD;
    }
    if (dataLen > *pulRetDataLen) {
        return JUBR_ARGUMENTS_BAD;
    }

    //1.decode 密文协议 00 00 #??<iv_len><iv><mac_tag_len><mac_tag><encrypted_message_len><encrypted_message>
    uint8_t recv_iv[IV_SIZE] = {0};
    uint8_t recv_mac_tag[MAC_TAG_SIZE] = {0};
    uchar_vector encrypted_message;
    bool decode_result = secure_channel_decode_encrypted_message((const uint8_t*)pData+2, dataLen - 2,
                                                                recv_iv,
                                                                recv_mac_tag,
                                                                encrypted_message);
    if (!decode_result) {
        return JUBR_ERROR;
    }
    //2.decrypt 密文
    uint8_t decrypted_message[encrypted_message.size()];
    size_t decrypted_message_len = encrypted_message.size();
    sc_error_code code = secure_channel_decrypt(&m_sc_context,
                                                encrypted_message.data(),
                                                encrypted_message.size(),
                                                (uint8_t *)decrypted_message,
                                                &decrypted_message_len,
                                                recv_iv,
                                                recv_mac_tag);
    if (code != SUCCESS){
        return JUBR_ERROR;
    }

    //3.把明文协议 ?##<msg_id 2bytes><data_len><data> encode 成 <msg_id 2bytes><data> 返出去
    if (decrypted_message_len < 5) {
        return JUBR_ERROR;
    }
    if (decrypted_message[0] != '?' || decrypted_message[1] != '#' || decrypted_message[2] != '#') {
        return JUBR_ERROR;
    }
    memcpy(retData, decrypted_message + 3, 2);
    memcpy(retData + 2, decrypted_message + 9, decrypted_message_len - 9);

    LogBinary("Decrypted Message", decrypted_message, decrypted_message_len);

    *pulRetDataLen = decrypted_message_len - 7;
    if (pData) {
        delete[] pData;  // Assuming pData was allocated with new[]
        pData = nullptr;
    }
    return JUBR_OK;
#else
        // Xufeng TODO

    //     unsigned int ret = 0;
    

    // unsigned int recvlen = 0;
    
    // ret = GD_BLE_SendAPDU_Sync(_handle, const_cast<unsigned char *>(sendData), ulSendLen, retData,&recvlen,ulMiliSecondTimeout);
    
    
    // // printf("recvlen------->%d------ret---%d",recvlen,ret);
    // *pulRetDataLen = recvlen;
    
    // return JUBR_OK;
    if (ulSendLen % 64 != 0)
    {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = JUBR_OK;

    for (int pos = 0; pos < ulSendLen; pos += 64)
    {
        uint8_t iv[IV_SIZE] = {0};
        uint8_t mac_tag[MAC_TAG_SIZE] = {0};
        uint8_t ciphertext[128] = {0};
        size_t ciphertext_len = 128;
        sc_error_code code = secure_channel_encrypt(&m_sc_context,
                                                    (const uint8_t *)(sendData + pos),
                                                    64,
                                                    ciphertext,
                                                    &ciphertext_len,
                                                    iv,
                                                    mac_tag);
        if (code != SUCCESS)
        {
            return JUBR_ERROR;
        }
        uchar_vector send_buf = secure_channel_encode_encrypted_message(ciphertext, ciphertext_len, iv, mac_tag);

        rv = GD_BLE_SendAPDU_only(_handle, send_buf.data(), send_buf.size());
        if (JUBR_OK != rv)
        {
            return rv;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
    }

    uint8_t recvData[4096] = {0};
    unsigned int recvlen = 4096;
    rv = GD_BLE_ReceiveAPDU_only(_handle, recvData, &recvlen, 500000);
    if (JUBR_OK != rv)
    {
        return rv;
    }
    if (recvlen > *pulRetDataLen)
    {
        return JUBR_ARGUMENTS_BAD;
    }

    // 1.decode 密文协议 00 00 #??<iv_len><iv><mac_tag_len><mac_tag><encrypted_message_len><encrypted_message>
    uint8_t recv_iv[IV_SIZE] = {0};
    uint8_t recv_mac_tag[MAC_TAG_SIZE] = {0};
    uchar_vector encrypted_message;
    bool decode_result = secure_channel_decode_encrypted_message((const uint8_t *)recvData + 2, recvlen - 2,
                                                                 recv_iv,
                                                                 recv_mac_tag,
                                                                 encrypted_message);
    if (!decode_result)
    {
        return JUBR_ERROR;
    }
    // 2.decrypt 密文
    uint8_t decrypted_message[encrypted_message.size()];
    size_t decrypted_message_len = encrypted_message.size();
    sc_error_code code = secure_channel_decrypt(&m_sc_context,
                                                encrypted_message.data(),
                                                encrypted_message.size(),
                                                (uint8_t *)decrypted_message,
                                                &decrypted_message_len,
                                                recv_iv,
                                                recv_mac_tag);
    if (code != SUCCESS)
    {
        return JUBR_ERROR;
    }

    // 3.把明文协议 ?##<msg_id 2bytes><data_len><data> encode 成 <msg_id 2bytes><data> 返出去
    if (decrypted_message_len < 5)
    {
        return JUBR_ERROR;
    }
    if (decrypted_message[0] != '?' || decrypted_message[1] != '#' || decrypted_message[2] != '#')
    {
        return JUBR_ERROR;
    }
    memcpy(retData, decrypted_message + 3, 2);
    memcpy(retData + 2, decrypted_message + 9, decrypted_message_len - 9);

    *pulRetDataLen = decrypted_message_len - 7;
    return JUBR_OK;

#endif
}
JUB_RV JubiterBLENusDevice::onlySendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen) {
#ifdef __ANDROID__
    if (nullptr == m_writeCallback || nullptr == m_readCallback) {
        return JUBR_ARGUMENTS_BAD;
    }
    JUB_RV rv = m_writeCallback((JUB_CHAR_PTR)sendData, (JUB_UINT16)ulSendLen);
    return  rv;
    
#else
    // Xufeng TODO
    
    unsigned int ret = 0;
    

    unsigned int recvlen = 0;
    
    ret = GD_BLE_SendAPDU_only(_handle, const_cast<unsigned char *>(sendData), ulSendLen);
 
    
    return ret;

#endif
}


unsigned int JubiterBLENusDevice::Initialize(const GDBLE_INIT_PARAM& params) {
#ifdef __ANDROID__
    return JUBR_IMPL_NOT_SUPPORT;
#else
        // init with inner _param
    _param1.param = params.param;
    unsigned int ret = GD_BLE_Initialize(_param1);
    if (IFD_SUCCESS == ret) {
        outerParams = params;
    }

    return (unsigned int)MatchErrorCode(ret);
#endif
}

unsigned int JubiterBLENusDevice::Finalize() {
#ifdef __ANDROID__
    return JUBR_IMPL_NOT_SUPPORT;
#else
    outerParams = {0, 0, 0, 0};
    return GD_BLE_Finalize();
#endif



}

unsigned int JubiterBLENusDevice::Scan() {

    #ifdef __ANDROID__
        return JUBR_IMPL_NOT_SUPPORT;
    #else 
        return (unsigned int)MatchErrorCode(GD_BLE_Scan());
    #endif
}

unsigned int JubiterBLENusDevice::StopScan() {

    #ifdef __ANDROID__
        return JUBR_IMPL_NOT_SUPPORT;
    #else
        return (unsigned int)MatchErrorCode(GD_BLE_StopScan());
    #endif
}




unsigned int JubiterBLENusDevice::CancelConnect(unsigned char* devName, unsigned char* bBLEUUID) {

    #ifdef __ANDROID__
        return JUBR_IMPL_NOT_SUPPORT;
    #else
        std::string uuid = reinterpret_cast<char*>(bBLEUUID);
    #if defined(__APPLE__)
        //uuid = reinterpret_cast<char*>(devName);
    #endif // #if defined(__APPLE__)

        unsigned int ret = GD_BLE_CancelConnDev((unsigned char*)uuid.c_str());
        _handle = 0;
        _bConnected = false;

        return (unsigned int)MatchErrorCode(ret);
    #endif
}



unsigned int JubiterBLENusDevice::IsConnect(unsigned long handle) {

    return _bConnected ? JUBR_OK : JUBR_ERROR;
}

unsigned long JubiterBLENusDevice::GetHandle() {

    return _handle;
}

void JubiterBLENusDevice::SetHandle(unsigned long handle) {

    _handle = handle;
}

void JubiterBLENusDevice::SetConnectStatuteFalse() {

    _bConnected = false;
}



int JubiterBLENusDevice::GDBLE_ReadCallBack(unsigned long devHandle,
                                       unsigned char* data, unsigned int uiDataLen)

{


//    auto bleDevice = Singleton<jub::JubiterBLENusDevice>::GetInstance();
//    if (bleDevice) {
//        Fido::RecvCallBack(devHandle, data, uiDataLen);
//    }
    
    
    
    

    return IFD_SUCCESS;
}

void JubiterBLENusDevice::GDBLE_ScanCallBack(unsigned char* devName,
                                        unsigned char* uuid,
                                        unsigned int type) {


    auto bleDevice = Singleton<jub::JubiterBLENusDevice>::GetInstance();
    if (bleDevice) {
        std::this_thread::sleep_for(std::chrono::seconds(2));        // 睡眠2秒
        if (bleDevice->outerParams.scanCallBack) {
            bleDevice->outerParams.scanCallBack(devName, uuid, type);
        }
    }

    return;
}

void JubiterBLENusDevice::GDBLE_DiscCallBack(unsigned char* uuid) {

    

    auto bleDevice = Singleton<jub::JubiterBLENusDevice>::GetInstance();
    if (bleDevice) {
        bleDevice->SetHandle(0);
        bleDevice->SetConnectStatuteFalse();
        if (bleDevice->outerParams.discCallBack) {
            bleDevice->outerParams.discCallBack(uuid);
        }
    }

    
    
    return;
}


JUB_RV JubiterBLENusDevice::SetReadWriteCallback(
        IN JUB_BleNusWrite_PTR pWrite,
        IN JUB_BleNusRead_PTR pRead){
#ifdef __ANDROID__
    m_writeCallback = pWrite;
    m_readCallback = pRead;
    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif
}


JUB_RV JubiterBLENusDevice::SetNotifyCallback(
        IN JUB_BleNusNotify_PTR pNotify){
    m_notifyCallback = pNotify;
    return JUBR_OK;
}

JUB_RV JubiterBLENusDevice::SetPassPhraseNotifyCallback(
        IN JUB_BleNusPassphrase_PTR pNotify){
    m_PassphraseCallback = pNotify;
    return JUBR_OK;
}

JUB_RV JubiterBLENusDevice::SetUpdatePercentageCallback(
        IN JUB_UpdatePercentage_PTR pNotify){
    m_PercentageCallback = pNotify;
    return JUBR_OK;
}
JUB_RV JubiterBLENusDevice::notifyPassphrase(IN JUB_BYTE_PTR pData, INOUT JUB_ULONG_PTR pulDataLen, INOUT bool *on_device) {
    if (m_PassphraseCallback) {
        m_PassphraseCallback(pData,pulDataLen, on_device);
        return JUBR_OK;
    }
    return JUBR_ARGUMENTS_BAD;
}

JUB_RV JubiterBLENusDevice::notifyMessageType(IN JUB_UINT16 messageType) {
    if (m_notifyCallback) {
        m_notifyCallback(messageType);
        return JUBR_OK;
    }
    return JUBR_ARGUMENTS_BAD;
}
JUB_RV JubiterBLENusDevice::notifyPercentage(IN JUB_UINT16 percentage) {
    if (m_PercentageCallback) {
        m_PercentageCallback(percentage);
        return JUBR_OK;
    }
    return JUBR_ARGUMENTS_BAD;
}


}  // namespace jub end

#endif // USE_BLE_DEVICE
