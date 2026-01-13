//
//  JUB_SDK_DEV_BLE_NUS.h
//  JubSDK
//
//  Created by Pan Min on 2025/5/18.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#ifndef JUB_SDK_DEV_BLE_NUS_h
#define JUB_SDK_DEV_BLE_NUS_h

#include "JUB_SDK_DEV_BLE.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef JUB_RV (*JUB_BleNusWrite_PTR)(
    IN JUB_CHAR_PTR pData,
    IN JUB_UINT16 dataLen
);
typedef JUB_RV (*JUB_BleNusRead_PTR)(
    OUT JUB_CHAR_PTR_PTR pData,
    OUT JUB_UINT16_PTR pDataLen
);
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BleNusSetReadWriteCallback(
    IN JUB_UINT16 deviceID,
    IN JUB_BleNusWrite_PTR pWrite,
    IN JUB_BleNusRead_PTR pRead
);

typedef void (*JUB_BleNusPassphrase_PTR)(
    IN JUB_BYTE_PTR pData,
    INOUT JUB_ULONG_PTR pDataLen,
    INOUT bool *on_device
);
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BleNusSetPasephraseCallback(
        IN JUB_UINT16 deviceID,
        IN JUB_BleNusPassphrase_PTR pNotify
);

typedef void (*JUB_BleNusNotify_PTR)(
        IN JUB_UINT16 messageType
);


JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BleNusSetNotifyMessageCallback(
        IN JUB_UINT16 deviceID,
        IN JUB_BleNusNotify_PTR pNotify
);

typedef void (*JUB_UpdatePercentage_PTR)(
        IN JUB_UINT16 percentage
);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BleNusSetUpdatePercentageCallback(
        IN JUB_UINT16 deviceID,
        IN JUB_UpdatePercentage_PTR pUpdate
);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BleNusOpenSecureChannel(
    IN JUB_UINT16 deviceID
);

/*****************************************************************************
 * @function name : JUB_RebootToNormal
 * @in  param : deviceID - device ID *
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_RebootToNormal(IN JUB_UINT16 deviceID);

/*****************************************************************************
 * @function name : JUB_RebootToBootLoader
 * @in  param : deviceID - device ID *
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_RebootToBootLoader(IN JUB_UINT16 deviceID);


/*****************************************************************************
 * @function name : JUB_COINCORE_DLL_EXPORT
 * @in  param : deviceID - device ID *
 * @in  param : firmwareFilePayload - firmware file payload
 *            : firmwareFileSize - firmware file size
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_UpdateFirmware(IN JUB_UINT16 deviceID,
                          IN JUB_BYTE_PTR firmwareFilePayload,
                          IN JUB_UINT32 firmwareFileSize,
                          IN bool reboot_on_success);


/*****************************************************************************
 * @function name : JUB_UpdateResource      
 *  @in  param : deviceID - device ID *
 * @in  param : resourceZipPayload - resource zip file payload
 *            : resourceZipSize - resource zip file size
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_UpdateResource(IN JUB_UINT16 deviceID,
                          IN JUB_BYTE_PTR resourceZipPayload,
                          IN JUB_UINT32 resourceZipSize,
                          IN bool reboot_on_success);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_DEV_BLE_NUS_h */
