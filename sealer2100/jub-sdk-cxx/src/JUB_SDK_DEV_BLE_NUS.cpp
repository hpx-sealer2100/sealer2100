//
//  JUB_SDK_DEV_BLE_NUS.cpp
//  JubSDK
//
//  Created by Pan Min on 2025/5/18.
//  Copyright © 2025 ZianCube. All rights reserved.
//

#include "JUB_SDK_DEV_BLE_NUS.h"

#include "utility/util.h"
#include "mSIGNA/stdutils/uchar_vector.h"

#include "common/protocpp/messages-common.pb.h"
#include "token/JubiterSealer2100Impl.h"

#ifdef __ANDROID__
#include "utils/logUtils.h"
#endif

using namespace hw::trezor::messages::common;


JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf);

// JUB_RV JUB_BleNusOpenSecureChannel(IN JUB_UINT16 deviceID){
//     auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
//     auto device = dynamic_cast<jub::JubiterBLENusDevice *>(token->getDevice());
//     if (device == nullptr)
//     {
//         return JUBR_ARGUMENTS_BAD;
//     }
//     return device->openSecureChannel();
// }

JUB_RV JUB_BleNusSetReadWriteCallback(
    IN JUB_UINT16 deviceID,
    IN JUB_BleNusWrite_PTR pWrite,
    IN JUB_BleNusRead_PTR pRead
){
    JUB_CHECK_NULL(pWrite);
    JUB_CHECK_NULL(pRead);

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    auto device = dynamic_cast<jub::JubiterBLENusDevice*>(token->getDevice());
    if(device == nullptr) {
        return JUBR_ARGUMENTS_BAD;
    }
    return device->SetReadWriteCallback(pWrite, pRead);
}

JUB_RV JUB_BleNusSetUpdatePercentageCallback(
        IN JUB_UINT16 deviceID,
        IN JUB_UpdatePercentage_PTR pUpdate
                                             ){
    JUB_CHECK_NULL(pUpdate);

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    auto device = dynamic_cast<jub::JubiterBLENusDevice*>(token->getDevice());
    if(device == nullptr) {
        return JUBR_ARGUMENTS_BAD;
    }
    return device->SetUpdatePercentageCallback(pUpdate);
}

JUB_RV JUB_BleNusSetNotifyMessageCallback(
        IN JUB_UINT16 deviceID,
        IN JUB_BleNusNotify_PTR pNotify
){
    JUB_CHECK_NULL(pNotify);

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    auto device = dynamic_cast<jub::JubiterBLENusDevice*>(token->getDevice());
    if(device == nullptr) {
        return JUBR_ARGUMENTS_BAD;
    }
    return device->SetNotifyCallback(pNotify);
}
JUB_RV JUB_BleNusSetPasephraseCallback(
        IN JUB_UINT16 deviceID,
        IN JUB_BleNusPassphrase_PTR pNotify
){
    JUB_CHECK_NULL(pNotify);

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    auto device = dynamic_cast<jub::JubiterBLENusDevice*>(token->getDevice());
    if(device == nullptr) {
        return JUBR_ARGUMENTS_BAD;
    }
    return device->SetPassPhraseNotifyCallback(pNotify);
}

JUB_RV JUB_RebootToBootLoader(IN JUB_UINT16 deviceID){
    
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);
    JUB_VERIFY_RV(token->IrisReboot());
    return JUBR_OK;
    
}
JUB_RV JUB_RebootToNormal(IN JUB_UINT16 deviceID){
    
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);
    JUB_VERIFY_RV(token->IrisBL_Reboot());
    return JUBR_OK;
    
}
JUB_RV JUB_UpdateFirmware(IN JUB_UINT16 deviceID,
                          IN JUB_BYTE_PTR firmwareFilePayload,
                          IN JUB_UINT32 firmwareFileSize,
                          IN bool reboot_on_success)
{
    
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);
    JUB_VERIFY_RV(token->IrisUpdateFirmware(firmwareFilePayload,firmwareFileSize,reboot_on_success));
    return JUBR_OK;
    
}

JUB_RV JUB_UpdateResource(IN JUB_UINT16 deviceID,
                          IN JUB_BYTE_PTR resourceZipPayload,
                          IN JUB_UINT32 resourceZipSize,
                          IN bool reboot_on_success){
    
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);
    JUB_VERIFY_RV(token->IrisUpdateResource(resourceZipPayload,resourceZipSize,reboot_on_success));
    return JUBR_OK;
}
