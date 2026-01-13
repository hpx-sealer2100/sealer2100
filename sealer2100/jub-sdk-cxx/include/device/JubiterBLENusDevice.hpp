#ifndef __JubiterBLENusDevice__
#define __JubiterBLENusDevice__

#include "device/JubiterBLEDevice.hpp"
#include "JUB_SDK_DEV.h"
#include "secure-channel/secure_channel.h"

#ifdef BLE_MODE
#include <memory>

#include "bleNusTransmit/gdbleTransmit.h"

//#include "bleTransmit/bleTransmit.h"

#define FAKE_DEVICE_HANDLE 0x00ABCDEF

#define FAKE_BLE_UUID_MAGIC "00000000-0000-0000-0000-000000000000" /**< fake uuid */
//#define FAKE_BLE_UUID_MAGIC "5D1BAB5D-B7E3-A105-79CB-F1025D81ACDA"
#define FAKE_BLE_NAME_MAGIC_1 "Sealer2100" /**< fake name */
#define FAKE_BLE_NAME_MAGIC_2 "Sealer" /**< fake name */
#define FAKE_BLE_NAME_MAGIC_3 "sansec" /**< fake name */
//#define FAKE_BLE_NAME_MAGIC "Digitshield B955" /**< fake name */
#define FAKE_BLE_NAME_MAGIC "HyperMateMax" /**< fake name */

namespace jub {

class JubiterBLENusDevice : public JubiterBLEDevice {

public:
     JubiterBLENusDevice();
    ~JubiterBLENusDevice();

public:
//   virtual unsigned int Connect(unsigned char* devName,    /**< ble device name */
//                                unsigned char* bBLEUUID,   /**< ble device UUID */
//                                unsigned int connectType,  /**< ble device connect type */
//                                unsigned long* pdevHandle, /**< output ble device connect handle */
//                                unsigned int timeout) override;
//   virtual unsigned int Disconnect(unsigned long handle) override;

    // for common device
//    virtual JUB_RV SendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
//                            OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
//                            IN JUB_ULONG ulMiliSecondTimeout = 1200000) override;
    
    
//    virtual JUB_RV Connect(const std::string path = "") override;
//    virtual JUB_RV Disconnect() override;
//
    virtual JUB_RV SendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                            OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                            IN JUB_ULONG ulMiliSecondTimeout = 1200000) override;
    
    virtual JUB_RV onlySendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen) override;

    JUB_RV SendDataPlaint(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                            OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                            IN JUB_ULONG ulMiliSecondTimeout = 1200000);

    JUB_RV SetReadWriteCallback(
        IN JUB_BleNusWrite_PTR pWrite,
        IN JUB_BleNusRead_PTR pRead);

    JUB_RV SetNotifyCallback(
        IN JUB_BleNusNotify_PTR pNotify);
    
    JUB_RV SetPassPhraseNotifyCallback(
        IN JUB_BleNusPassphrase_PTR pNotify);
    
    JUB_RV SetUpdatePercentageCallback(
        IN JUB_UpdatePercentage_PTR pNotify);
    
    JUB_RV notifyPassphrase(IN JUB_BYTE_PTR pData, INOUT JUB_ULONG_PTR pulDataLen, INOUT bool *on_device);

    JUB_RV notifyMessageType(
        IN JUB_UINT16 messageType);
    
    JUB_RV notifyPercentage(
        IN JUB_UINT16 percentage);

    JUB_RV openSecureChannel(void);
    public :
    // for ble device
    virtual unsigned int Initialize(const GDBLE_INIT_PARAM &params);
    virtual unsigned int Finalize();
    virtual unsigned int Scan();
    virtual unsigned int StopScan();
    virtual unsigned int Connect(unsigned char* devName,    /**< ble device name */
                                 unsigned char* bBLEUUID,   /**< ble device UUID */
                                 unsigned int connectType,  /**< ble device connect type */
                                 unsigned long* pdevHandle, /**< output ble device connect handle */
                                 unsigned int timeout);
    virtual unsigned int CancelConnect(unsigned char* devName, unsigned char* bBLEUUID);
    virtual unsigned int Disconnect(unsigned long handle);
    virtual unsigned int IsConnect(unsigned long handle);
    virtual unsigned long GetHandle();
    virtual void SetHandle(unsigned long handle);
    virtual void SetConnectStatuteFalse();

    GDBLE_INIT_PARAM outerParams;
protected:
    static int GDBLE_ReadCallBack(unsigned long devHandle,
                                unsigned char* data, unsigned int uiDataLen);

    static void GDBLE_ScanCallBack(unsigned char* devName,
                                 unsigned char* uuid,
                                 unsigned int type);

    static void GDBLE_DiscCallBack(unsigned char* uuid);

    JUB_RV GDMatchErrorCode(int error);

    //static std::shared_ptr<jub::JubiterBLEDevice> getThis();

    // check ble version, and set ble library reconnect flag
//    void ExtraSetting();
    
protected:
    /* data */
    GDBLE_INIT_PARAM _param1;
    unsigned long _handle;
    bool _bConnected;
private:
    JUB_BleNusWrite_PTR m_writeCallback = nullptr;
    JUB_BleNusRead_PTR m_readCallback = nullptr;
    JUB_BleNusNotify_PTR m_notifyCallback = nullptr;
    JUB_BleNusPassphrase_PTR m_PassphraseCallback = nullptr;
    JUB_UpdatePercentage_PTR m_PercentageCallback = nullptr;

    secure_channel_context m_sc_context = {0};

}; // class JubiterBLENusDevice end



}  // namespace jub end

#endif  // USE_BLE_DEVICE
#endif  // __JubiterBLENusDevice__
