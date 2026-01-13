//
//  JubSDKCore+DEV_BLE.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+DEV_BLE.h"
#import "JUB_SDK_DEV_BLE.h"
#import "JUB_SDK_DEV_BLE_NUS.h"
#import "JubSDKCore+DEV.h"

//typedef int(*DEV_ReadCallBack)(JUB_ULONG devHandle, JUB_BYTE_PTR data, JUB_UINT32 dataLen);
//typedef void(*DEV_ScanCallBack)(JUB_BYTE_PTR devName, JUB_BYTE_PTR uuid, JUB_UINT32 type);
//typedef void(*DEV_DiscCallBack)(JUB_BYTE_PTR uuid);
//typedef struct _DEVICE_INIT_PARAM_ {
//    void* param;
//    DEV_ReadCallBack callBack;
//    DEV_ScanCallBack scanCallBack;
//    DEV_DiscCallBack discCallBack;
//} DEVICE_INIT_PARAM;
@implementation DeviceInitParam
@synthesize param;
@synthesize readCallBack;
@synthesize scanCallBack;
@synthesize discCallBack;
@end
//typedef struct {
//    JUB_CHAR label[32];
//    JUB_CHAR sn[24];
//    JUB_UINT16 pinRetry;
//    JUB_UINT16 pinMaxRetry;
//    JUB_CHAR bleVersion[4];
//    JUB_CHAR firmwareVersion[4];
//} JUB_DEVICE_INFO;
@implementation DeviceInfo
@synthesize label;
@synthesize sn;
@synthesize pinRetry;
@synthesize pinMaxRetry;
@synthesize bleVersion;
@synthesize firmwareVersion;
@end


@implementation DeviceTypeInfo
@synthesize commode;
@synthesize category;
@end


extern JUB_ENUM_BOOL    (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_NS_ENUM_BOOL (^inlineNSBool)(JUB_ENUM_BOOL);


JUB_NS_ENUM_COMMODE (^inlineNSCOMMODE)(JUB_ENUM_COMMODE) = ^(JUB_ENUM_COMMODE argument) {
    JUB_NS_ENUM_COMMODE commode;
    switch (argument) {
        case JUB_ENUM_COMMODE::HID:
            commode = JUB_NS_ENUM_COMMODE::NS_HID;
            break;
        case JUB_ENUM_COMMODE::BLE:
            commode = JUB_NS_ENUM_COMMODE::NS_BLE;
            break;
        case JUB_ENUM_COMMODE::COMMODE_NS_ITEM:
            commode = JUB_NS_ENUM_COMMODE::NS_COMMODE_NS_ITEM;
            break;
    }
    return commode;
};


JUB_NS_ENUM_DEVICE (^inlineNSDEVICE)(JUB_ENUM_DEVICE) = ^(JUB_ENUM_DEVICE argument) {
    JUB_NS_ENUM_DEVICE category;
    switch (argument) {
        case JUB_ENUM_DEVICE::BLADE:
            category = JUB_NS_ENUM_DEVICE::NS_BLADE;
            break;
        case JUB_ENUM_DEVICE::BIO:
            category = JUB_NS_ENUM_DEVICE::NS_BIO;
            break;
        case JUB_ENUM_DEVICE::Sealer2100:
            category = JUB_NS_ENUM_DEVICE::NS_Sealer2100;
            break;
        case JUB_ENUM_DEVICE::DEVICE_NS_ITEM:
            category = JUB_NS_ENUM_DEVICE::NS_DEVICE_NS_ITEM;
            break;
    }
    return category;
};
@implementation JubSDKCore (DEV_BLE)

//JUB_RV JUB_GetDeviceType(IN JUB_UINT16 deviceID,
//                         OUT JUB_ENUM_COMMODE_PTR commode, OUT JUB_ENUM_DEVICE_PTR deviceClass);
- (DeviceTypeInfo*)JUB_GetDeviceType:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    DeviceTypeInfo *info = [[DeviceTypeInfo alloc] init];
    
    JUB_ENUM_COMMODE commode;
    JUB_ENUM_DEVICE  category;
    JUB_RV rv = JUB_GetDeviceType(deviceID, &commode, &category);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
    
    info.commode  = inlineNSCOMMODE(commode);
    info.category = inlineNSDEVICE(category);
    
    return info;
}


//JUB_RV JUB_GetDeviceInfo(IN JUB_UINT16 deviceID,
//                         OUT JUB_DEVICE_INFO_PTR info);
- (DeviceInfo*)JUB_GetDeviceInfo:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_DEVICE_INFO deviceInfo;
    JUB_RV rv = JUB_GetDeviceInfo(deviceID,
                                  &deviceInfo);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return (DeviceInfo*)[NSNull null];
    }
    
    DeviceInfo* info = [[DeviceInfo alloc] init];
    info.label = [NSString stringWithCString:deviceInfo.label
                                    encoding:NSUTF8StringEncoding];
    info.sn    = [NSString stringWithCString:deviceInfo.sn
                                    encoding:NSUTF8StringEncoding];
    info.pinRetry    = deviceInfo.pinRetry;
    info.pinMaxRetry = deviceInfo.pinMaxRetry;
    info.bleVersion  = [NSString stringWithCString:deviceInfo.bleVersion
                                          encoding:NSUTF8StringEncoding];
    info.firmwareVersion = [NSString stringWithCString:deviceInfo.firmwareVersion
                                              encoding:NSUTF8StringEncoding];
    
    return info;
}
//JUB_RV JUB_SendOneApdu(IN JUB_UINT16 deviceID,
//                       IN JUB_CHAR_PTR apdu ,
//                       OUT JUB_CHAR_PTR_PTR response);
- (NSString*)JUB_SendOneApdu:(NSUInteger)deviceID
                                  apdu:(NSString*)apdu
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pApdu = nullptr;
    if (nil != apdu) {
        pApdu = (JUB_CHAR_PTR)[apdu UTF8String];
    }
    
    JUB_CHAR_PTR response = nullptr;
    JUB_RV rv = JUB_SendOneApdu(deviceID,
                                pApdu,
                                &response);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString *strResp = [NSString stringWithCString:response
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(response);
    
    return strResp;
}

//JUB_ENUM_BOOL JUB_IsInitialize(IN JUB_UINT16 deviceID);
- (JUB_NS_ENUM_BOOL)JUB_IsInitialize:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_BOOL bRv = JUB_IsInitialize(deviceID);
    
    return inlineNSBool(bRv);
}

//JUB_ENUM_BOOL JUB_IsBootLoader(IN JUB_UINT16 deviceID);
- (JUB_NS_ENUM_BOOL)JUB_IsBootLoader:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_BOOL bRv = JUB_IsBootLoader(deviceID);
    
    return inlineNSBool(bRv);
}

//JUB_RV JUB_SetTimeOut(IN JUB_UINT16 contextID, IN JUB_UINT16 timeout);
- (void)JUB_SetTimeOut:(NSUInteger)contextID
               timeout:(NSUInteger)timeout
{
    self.lastError = JUB_SetTimeOut(contextID,
                                    timeout);
}

////JUB_RV JUB_FreeMemory(IN JUB_CHAR_CPTR memPtr);
//- (void) JUB_FreeMemory :(NSString*)memPtr;

//JUB_RV JUB_EnumApplets(IN JUB_UINT16 deviceID,
//                       OUT JUB_CHAR_PTR_PTR appList);
- (NSString*)JUB_EnumApplets:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR appList = nullptr;
    JUB_RV rv = JUB_EnumApplets(deviceID,
                                &appList);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAppList = [NSString stringWithCString:appList
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(appList);
    
    return strAppList;
}



//JUB_RV JUB_GetAppletVersion(IN JUB_UINT16 deviceID,
//                            IN JUB_CHAR_PTR appID,
//                            OUT JUB_CHAR_PTR_PTR version);
- (NSString*)JUB_GetAppletVersion:(NSUInteger)deviceID
                            appID:(NSString*)appID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pAppID = nullptr;
    if (nil != appID) {
        pAppID = (JUB_CHAR_PTR)[appID UTF8String];
    }
    
    JUB_CHAR_PTR version = nullptr;
    JUB_RV rv = JUB_GetAppletVersion(deviceID,
                                     pAppID,
                                     &version);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strVersion = [NSString stringWithCString:version
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(version);
    
    return strVersion;
}

//JUB_RV JUB_initDevice(IN DEVICE_INIT_PARAM param);
- (void) JUB_initDevice :(DeviceInitParam*)param
{
    DEVICE_INIT_PARAM parameter;
    parameter.param = param.param;
    parameter.callBack     = param.readCallBack;
    parameter.scanCallBack = param.scanCallBack;
    parameter.discCallBack = param.discCallBack;
    
    self.lastError = JUB_initDevice(parameter);
}

//JUB_RV JUB_enumDevices(void);
- (void)JUB_enumDevices
{
    self.lastError = JUB_enumDevices();
}

//JUB_RV JUB_stopEnumDevices(void);
- (void)JUB_stopEnumDevices
{
    self.lastError = JUB_stopEnumDevices();
}

//JUB_RV JUB_connectDevice(JUB_BYTE_PTR devName,   /**< ble device name */
//                         JUB_BYTE_PTR bBLEUUID,  /**< ble device UUID */
//                         JUB_UINT32 connectType, /**< ble device connect type */
//                         JUB_UINT16* pDeviceID,  /**< output ble device connect handle */
//                         JUB_UINT32 timeout);
- (NSUInteger)JUB_connectDevice:(NSString*)deviceName
                           uuid:(NSString*)bBLEUUID
                    connectType:(NSUInteger)connectType
                        timeout:(NSUInteger)timeout
{
    self.lastError = JUBR_OK;
    
    JUB_BYTE_PTR pDeviceName = nullptr;
    if (nil != deviceName) {
        pDeviceName = (JUB_BYTE_PTR)[deviceName UTF8String];
    }
    JUB_BYTE_PTR pBLEUUID = nullptr;
    if (nil != bBLEUUID) {
        pBLEUUID = (JUB_BYTE_PTR)[bBLEUUID UTF8String];
    }
    
    JUB_UINT16 deviceID = 0;
    JUB_RV rv = JUB_connectDevice(pDeviceName,
                                  pBLEUUID,
                                  (JUB_UINT32)connectType,
                                  &deviceID,
                                  (JUB_UINT32)timeout);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return deviceID;
    }
    
    return NSUInteger(deviceID);
}

//JUB_RV JUB_cancelConnect(JUB_BYTE_PTR devName, JUB_BYTE_PTR bBLEUUID);
- (void)JUB_cancelConnect:(NSString*)deviceName
                     uuid:(NSString*)bBLEUUID
{
    JUB_BYTE_PTR pDeviceName = nullptr;
    if (nil != deviceName) {
        pDeviceName = (JUB_BYTE_PTR)[deviceName UTF8String];
    }
    JUB_BYTE_PTR pBLEUUID = nullptr;
    if (nil != bBLEUUID) {
        pBLEUUID = (JUB_BYTE_PTR)[bBLEUUID UTF8String];
    }
    
    self.lastError = JUB_cancelConnect(pDeviceName, pBLEUUID);
}

//JUB_RV JUB_disconnectDevice(JUB_UINT16 deviceID);
- (void)JUB_disconnectDevice:(NSUInteger)deviceID
{
    self.lastError = JUB_disconnectDevice(deviceID);
}

//JUB_RV JUB_isDeviceConnect(JUB_UINT16 deviceID);
- (void)JUB_isDeviceConnect:(NSUInteger)deviceID
{
    self.lastError = JUB_isDeviceConnect(deviceID);
}

//JUB_RV JUB_QueryBattery(IN JUB_UINT16 deviceID,
//                        OUT JUB_BYTE_PTR percent);
- (NSString*)JUB_QueryBattery:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_BYTE percent = 0x00;
    JUB_RV rv = JUB_QueryBattery(deviceID,
                                 &percent);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    return [NSString stringWithFormat:@"%c", percent];
}

- (void)JUB_BleNusSetPassphraseCallback:(NSUInteger)deviceID
                               callback:(GetPassphrase)getPassphrase
{
    self.lastError = JUBR_OK;
    
    // 静态字典保存回调block
    static NSMutableDictionary<NSNumber *,GetPassphrase> *callbackDict;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        callbackDict = [NSMutableDictionary dictionary];
    });
    
    // 更新回调字典
    if (getPassphrase) {
        callbackDict[@(deviceID)] = [getPassphrase copy];
    } else {
        [callbackDict removeObjectForKey:@(deviceID)];
        JUB_BleNusSetPasephraseCallback(deviceID, nullptr);
        return;
    }
    
    
    // C回调桥接
    JUB_BleNusPassphrase_PTR cCallback = nullptr;
    cCallback = [](JUB_BYTE_PTR pData, JUB_ULONG_PTR pDataLen, bool *on_device) {
        // the caller not provide buffer
        NSNumber* key = callbackDict.allKeys.firstObject;
        GetPassphrase block = callbackDict[key];
        assert(block != nil);

        // call block with user provide passphrase
        InputPassphrase input;
        block(&input);
        *on_device = input.on_device;
        // buffer not enough
        if (!input.passphrase || input.passphrase.length > *pDataLen) return;
        const char* str = [input.passphrase UTF8String];
        strcpy((char*)pData, str);
    };

    // 注册C回调
    JUB_RV rv = JUB_BleNusSetPasephraseCallback(deviceID, cCallback);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
}
- (void)JUB_BleNusSetUpdatePercentageCallback:(NSUInteger)deviceID
                                   percentage:(void(^)(NSUInteger percentage))percentageCallback
{
    self.lastError = JUBR_OK;
    static NSMutableDictionary<NSNumber *, void(^)(NSUInteger)> *callbackDict;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        callbackDict = [NSMutableDictionary dictionary];
    });
    if (percentageCallback) {
        callbackDict[@(deviceID)] = [percentageCallback copy];
    } else {
        [callbackDict removeObjectForKey:@(deviceID)];
    }

    // C callback bridge
    void (^blockCopy)(NSUInteger) = callbackDict[@(deviceID)];
    JUB_UpdatePercentage_PTR cCallback = nullptr;
    if (blockCopy) {
        cCallback = [](JUB_UINT16 percentage) {
            NSNumber *key = @(percentage); // deviceID is not passed, only messageType
            // Find the first callback (since only messageType is available)
            for (NSNumber *devID in callbackDict) {
                void (^cb)(NSUInteger) = callbackDict[devID];
                if (cb) {
                    cb(percentage);
                }
            }
        };
    }

    // Register the C callback
    JUB_RV rv = JUB_BleNusSetUpdatePercentageCallback(deviceID, cCallback);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
}
- (void)JUB_BleNusSetNotifyMessageCallback:(NSUInteger)deviceID
                                    notify:(void(^)(NSUInteger messageType))notifyCallback
{
    self.lastError = JUBR_OK;
    static NSMutableDictionary<NSNumber *, void(^)(NSUInteger)> *callbackDict;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        callbackDict = [NSMutableDictionary dictionary];
    });
    if (notifyCallback) {
        callbackDict[@(deviceID)] = [notifyCallback copy];
    } else {
        [callbackDict removeObjectForKey:@(deviceID)];
    }

    // C callback bridge
    void (^blockCopy)(NSUInteger) = callbackDict[@(deviceID)];
    JUB_BleNusNotify_PTR cCallback = nullptr;
    if (blockCopy) {
        cCallback = [](JUB_UINT16 messageType) {
            NSNumber *key = @(messageType); // deviceID is not passed, only messageType
            // Find the first callback (since only messageType is available)
            for (NSNumber *devID in callbackDict) {
                void (^cb)(NSUInteger) = callbackDict[devID];
                if (cb) {
                    cb(messageType);
                }
            }
        };
    }

    // Register the C callback
    JUB_RV rv = JUB_BleNusSetNotifyMessageCallback(deviceID, cCallback);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
}
//JUB_RV JUB_GetFeatures(IN JUB_UINT16 deviceID,
//                       OUT JUB_CHAR_PTR_PTR Features)
- (NSString*)JUB_GetFeatures:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR strTmp = nullptr;
    JUB_RV rv = JUB_GetFeatures(deviceID,&strTmp);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strFeatures = [NSString stringWithCString:strTmp
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(strTmp);
    
    return strFeatures;
}

//JUB_RV JUB_SetLable(IN JUB_UINT16 deviceID, IN JUB_CHAR_PTR pLable)
- (void)JUB_SetLable:(NSUInteger)deviceID
              pLable:(NSString*)pLable
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR strTmp = nullptr;
    if (nil != pLable) {
        strTmp = (JUB_CHAR_PTR)[pLable UTF8String];
    }
    
    self.lastError = JUB_SetLable(deviceID,strTmp);
}
//JUB_RV JUB_UsePassphrase(IN JUB_UINT16 deviceID, IN bool bUse)
- (void)JUB_UsePassphrase:(NSUInteger)deviceID
                bUse:(BOOL)bUse
{
    self.lastError = JUBR_OK;
    
    bool cppBool = bUse ? true : false;
    
    self.lastError = JUB_UsePassphrase(deviceID,cppBool);
}
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_EndSession(IN JUB_UINT16 deviceID);
- (void)JUB_EndSession:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    self.lastError = JUB_EndSession(deviceID);
}
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_OpCancel(IN JUB_UINT16 deviceID);
- (void)JUB_OpCancel:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    self.lastError = JUB_OpCancel(deviceID);
}
//JUB_RV Jub_EnumSupportCoins(IN JUB_UINT16 deviceID,
//                            OUT JUB_CHAR_PTR_PTR coinList);
- (NSString*)Jub_EnumSupportCoins:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR coinList = nullptr;
    JUB_RV rv = Jub_EnumSupportCoins(deviceID,
                                     &coinList);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strCoinList = [NSString stringWithCString:coinList
                                               encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(coinList);
    
    return strCoinList;
}
//JUB_RV JUB_GetDeviceCert(IN JUB_UINT16 deviceID,
//                         OUT JUB_CHAR_PTR_PTR cert);
- (NSString*)JUB_GetDeviceCert:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    char *cert = nullptr;
    JUB_RV rv = JUB_GetDeviceCert(deviceID,
                                  &cert);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString *strCert = [NSString stringWithCString:cert
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(cert);
    
    return strCert;
}
//JUB_RV JUB_GetDeviceSignData(IN JUB_UINT16 deviceID,
//                             IN JUB_CHAR_PTR hashData,
//                             OUT JUB_CHAR_PTR_PTR SignData)

- (NSString*)JUB_GetDeviceSignData:(NSUInteger)deviceID
                          hashData:(NSString*)hashData
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR strTmp = nullptr;
    if (nil != hashData) {
        strTmp = (JUB_CHAR_PTR)[hashData UTF8String];
    }
    
    JUB_CHAR_PTR SignData = nullptr;
    JUB_RV rv = JUB_GetDeviceSignData(deviceID, strTmp, &SignData);
//    JUB_RV rv = JUB_GetDeviceSignData(deviceID,
//                                      hashData,
//                                     &SignData);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return nil;
    }
    
    
    NSString *strData = [NSString stringWithCString:SignData
                                           encoding:NSUTF8StringEncoding];

    JUB_FreeMemory(SignData);
    
    return strData;
}
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_RebootToBootLoader(IN JUB_UINT16 deviceID);
- (void)JUB_RebootToBootLoader:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    self.lastError = JUB_RebootToBootLoader(deviceID);
    
}
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_RebootToNormal(IN JUB_UINT16 deviceID);
- (void)JUB_RebootToNormal:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    self.lastError = JUB_RebootToNormal(deviceID);
    
}

//JUB_RV JUB_UpdateFirmware(IN JUB_UINT16 deviceID,
//                          IN JUB_BYTE_PTR firmwareFilePayload,
//                          IN JUB_UINT32 firmwareFileSize,
//                          IN bool reboot_on_success);
- (void)JUB_UpdateFirmware:(NSUInteger)deviceID
       firmwareFilePayload:(NSData*)firmwareFilePayload
          firmwareFileSize:(NSUInteger)firmwareFileSize
         reboot_on_success:(BOOL)reboot_on_success
{
    self.lastError = JUBR_OK;
    
//    JUB_BYTE_PTR strTmp = nullptr;
//    if (nil != firmwareFilePayload) {
//        strTmp = (JUB_BYTE_PTR)[firmwareFilePayload UTF8String];
//    }
    const JUB_BYTE_PTR firmwareBytes = (const JUB_BYTE_PTR)[firmwareFilePayload bytes];
    JUB_UINT32 firmwareSize = (JUB_UINT32)firmwareFilePayload.length;
    bool cppBool = reboot_on_success ? true : false;
    
    self.lastError = JUB_UpdateFirmware(deviceID,firmwareBytes,(JUB_UINT32)firmwareSize,cppBool);
}

//JUB_RV JUB_UpdateResource(IN JUB_UINT16 deviceID,
//                          IN JUB_BYTE_PTR resourceZipPayload,
//                          IN JUB_UINT32 resourceZipSize,
//                          IN bool reboot_on_success);
- (void)JUB_UpdateResource:(NSUInteger)deviceID
        resourceZipPayload:(NSData*)resourceZipPayload
           resourceZipSize:(NSUInteger)resourceZipSize
         reboot_on_success:(BOOL)reboot_on_success
{
    self.lastError = JUBR_OK;
    
//    JUB_BYTE_PTR strTmp = nullptr;
//    if (nil != resourceZipPayload) {
//        strTmp = (JUB_BYTE_PTR)[resourceZipPayload UTF8String];
//    }
    const JUB_BYTE_PTR resourceBytes = (const JUB_BYTE_PTR)[resourceZipPayload bytes];
    JUB_UINT32 resourceSize = (JUB_UINT32)resourceZipPayload.length;
    bool cppBool = reboot_on_success ? true : false;
    
    self.lastError = JUB_UpdateResource(deviceID,resourceBytes,(JUB_UINT32)resourceSize,cppBool);
}

//for test
#import "JUB_SDK_SOL.h"
extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT);
typedef ContextConfig ContextConfigSOL;
- (NSUInteger)JUB_CreateContextSOL:(NSUInteger)deviceID
                               cfg:(ContextConfigSOL*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_SOL cfgSOL;
    if (nil != cfg.mainPath) {
        cfgSOL.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextSOL(cfgSOL,
                                     deviceID,
                                     &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}
//JUB_RV JUB_GetAddressSOL(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressSOL:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;

    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_GetAddressSOL(contextID,
                                  bip32Path,
                                  isShow,
                                  &address);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAddress = [NSString stringWithCString:address
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(address);
    
    return strAddress;
}


//JUB_RV JUB_GetHDNodeSOL(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeSOL:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    
    JUB_CHAR_PTR pubkey = nullptr;
    JUB_RV rv = JUB_GetHDNodeSOL(contextID,
                                 fmt,
                                 bip32Path,
                                 &pubkey);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strPubkey = [NSString stringWithCString:pubkey
                                             encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(pubkey);
    
    return strPubkey;
}


//JUB_RV JUB_GetMainHDNodeSOL(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeSOL:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetMainHDNodeSOL(contextID,
                                     fmt,
                                     &xpub);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strXpub = [NSString stringWithCString:xpub
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(xpub);
    
    return strXpub;
}


//JUB_RV JUB_SignTransactionSOL(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_CHAR_CPTR recentHash,
//                              IN JUB_CHAR_CPTR dest,
//                              IN JUB_UINT64 amount,
//                              OUT JUB_CHAR_PTR_PTR txRaw);
- (NSString*)JUB_SignTransactionSOL:(NSUInteger)contextID
                               path:(BIP32Path*)path
                         recentHash:(NSString*)recentHash
                               dest:(NSString*)dest
                             amount:(NSNumber*)amount
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pRecentHash = nullptr;
    if (nil != recentHash) {
        pRecentHash = (JUB_CHAR_PTR)[recentHash UTF8String];
    }
    JUB_CHAR_PTR pDest = nullptr;
    if (nil != dest) {
        pDest = (JUB_CHAR_PTR)[dest UTF8String];
    }
    JUB_UINT64 uAmount = [amount unsignedLongLongValue];
    
    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTransactionSOL(contextID,
                                       bip32Path,
                                       pRecentHash,
                                       pDest,
                                       uAmount,
                                       &raw);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strRaw = [NSString stringWithCString:raw
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(raw);
    
    return strRaw;
}
//JUB_RV JUB_SignTransactionTokenSOL(IN JUB_UINT16 contextID,
//                                   IN BIP32_Path path,
//                                   IN JUB_CHAR_CPTR recentHash,
//                                   IN JUB_CHAR_CPTR tokenMint,
//                                   IN JUB_CHAR_CPTR source,
//                                   IN JUB_CHAR_CPTR dest,
//                                   IN JUB_UINT64 amount,
//                                   IN JUB_UINT8 decimal,
//                                   OUT JUB_CHAR_PTR_PTR txRaw);
- (NSString*)JUB_SignTransactionTokenSOL:(NSUInteger)contextID
                                    path:(BIP32Path*)path
                              recentHash:(NSString*)recentHash
                               tokenMint:(NSString*)tokenMint
                                  source:(NSString*)source
                                    dest:(NSString*)dest
                                  amount:(NSNumber*)amount
                                 decimal:(NSInteger)decimal
{
    self.lastError = JUBR_OK;

    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pRecentHash = nullptr;
    if (nil != recentHash) {
        pRecentHash = (JUB_CHAR_PTR)[recentHash UTF8String];
    }
    JUB_CHAR_PTR pTokenMint = nullptr;
    if (nil != tokenMint) {
        pTokenMint = (JUB_CHAR_PTR)[tokenMint UTF8String];
    }
    JUB_CHAR_PTR pSource = nullptr;
    if (nil != source) {
        pSource = (JUB_CHAR_PTR)[source UTF8String];
    }
    JUB_CHAR_PTR pDest = nullptr;
    if (nil != dest) {
        pDest = (JUB_CHAR_PTR)[dest UTF8String];
    }
    JUB_UINT64 uAmount = [amount unsignedLongLongValue];

    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTransactionTokenSOL(contextID,
                                            bip32Path,
                                            pRecentHash,
                                            pTokenMint,
                                            pSource,
                                            pDest,
                                            uAmount,
                                            decimal,
                                            &raw);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strRaw = [NSString stringWithCString:raw
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(raw);
    
    return strRaw;
}
//JUB_RV JUB_SignTxTokenCreateAndTransferSOL(IN JUB_UINT16 contextID,
//                                   IN BIP32_Path path,
//                                   IN JUB_CHAR_CPTR recentHash,
//                                   IN JUB_CHAR_CPTR tokenMint,
//                                   IN JUB_CHAR_CPTR destMainAddress,
//                                   IN JUB_CHAR_CPTR source,
//                                   IN JUB_CHAR_CPTR dest,
//                                   IN JUB_UINT64 amount,
//                                   IN JUB_UINT8 decimal,
//                                   OUT JUB_CHAR_PTR_PTR txRaw);
- (NSString*)JUB_SignTxTokenCreateAndTransferSOL:(NSUInteger)contextID
                                            path:(BIP32Path*)path
                                      recentHash:(NSString*)recentHash
                                       tokenMint:(NSString*)tokenMint
                                 destMainAddress:(NSString*)destMainAddress
                                          source:(NSString*)source
                                            dest:(NSString*)dest
                                          amount:(NSNumber*)amount
                                         decimal:(NSInteger)decimal
{
    self.lastError = JUBR_OK;

    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pRecentHash = nullptr;
    if (nil != recentHash) {
        pRecentHash = (JUB_CHAR_PTR)[recentHash UTF8String];
    }
    JUB_CHAR_PTR pTokenMint = nullptr;
    if (nil != tokenMint) {
        pTokenMint = (JUB_CHAR_PTR)[tokenMint UTF8String];
    }
    JUB_CHAR_PTR pdestMainAddress = nullptr;
    if (nil != destMainAddress) {
        pdestMainAddress = (JUB_CHAR_PTR)[destMainAddress UTF8String];
    }
    JUB_CHAR_PTR pSource = nullptr;
    if (nil != source) {
        pSource = (JUB_CHAR_PTR)[source UTF8String];
    }
    JUB_CHAR_PTR pDest = nullptr;
    if (nil != dest) {
        pDest = (JUB_CHAR_PTR)[dest UTF8String];
    }
    JUB_UINT64 uAmount = [amount unsignedLongLongValue];

    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTxTokenCreateAndTransferSOL(contextID,
                                                    bip32Path,
                                                    pRecentHash,
                                                    pTokenMint,
                                                    pdestMainAddress,
                                                    pSource,
                                                    pDest,
                                                    uAmount,
                                                    decimal,
                                                    &raw);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strRaw = [NSString stringWithCString:raw
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(raw);
    
    return strRaw;
}
//for test xrp

//#import "JUB_SDK_XRP.h"
// JUB_NS_XRP_TX_TYPE -> JUB_ENUM_XRP_TX_TYPE
JUB_ENUM_XRP_TX_TYPE (^inlineXRPTxType)(JUB_NS_XRP_TX_TYPE) = ^(JUB_NS_XRP_TX_TYPE argument) {
    JUB_ENUM_XRP_TX_TYPE u;
    switch (argument) {
        case NS_PYMT:
            u = JUB_ENUM_XRP_TX_TYPE::PYMT;
            break;
        default:
            u = JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE;
            break;
    }
    
    return u;
};

// JUB_NS_XRP_TX_TYPE <- JUB_ENUM_XRP_TX_TYPE
JUB_NS_XRP_TX_TYPE (^inlineNSXRPTxType)(JUB_ENUM_XRP_TX_TYPE) = ^(JUB_ENUM_XRP_TX_TYPE argument) {
    JUB_NS_XRP_TX_TYPE u;
    switch (argument) {
        case JUB_ENUM_XRP_TX_TYPE::PYMT:
            u = NS_PYMT;
            break;
        default:
            u = NS_TX_XRP_TYPE_NS;
            break;
    }
    
    return u;
};

// JUB_NS_XRP_PYMT_TYPE -> JUB_ENUM_XRP_PYMT_TYPE
JUB_ENUM_XRP_PYMT_TYPE (^inlineXRPPymtType)(JUB_NS_XRP_PYMT_TYPE) = ^(JUB_NS_XRP_PYMT_TYPE argument) {
    JUB_ENUM_XRP_PYMT_TYPE u;
    switch (argument) {
        case NS_DXRP:
            u = JUB_ENUM_XRP_PYMT_TYPE::DXRP;
            break;
        default:
            u = JUB_ENUM_XRP_PYMT_TYPE::NS_ITEM_PYMT_TYPE;
            break;
    }
    
    return u;
};

// JUB_NS_XRP_PYMT_TYPE <- JUB_ENUM_XRP_PYMT_TYPE
JUB_NS_XRP_PYMT_TYPE (^inlineNSXRPPymtType)(JUB_ENUM_XRP_PYMT_TYPE) = ^(JUB_ENUM_XRP_PYMT_TYPE argument) {
    JUB_NS_XRP_PYMT_TYPE u;
    switch (argument) {
        case JUB_ENUM_XRP_PYMT_TYPE::DXRP:
            u = NS_DXRP;
            break;
        default:
            u = NS_DXRP_XRP_TYPE_NS;
            break;
    }
    
    return u;
};

// XRPMemo* -> JUB_XRP_MEMO
void (^inlineXRPMemo)(XRPMemo*, JUB_XRP_MEMO&) = ^(XRPMemo* xrpMemo, JUB_XRP_MEMO& memo) {
    if (nil != xrpMemo.type) {
        memo.type   = (JUB_CHAR_PTR)[xrpMemo.type UTF8String];
    }
    if (nil != xrpMemo.data) {
        memo.data   = (JUB_CHAR_PTR)[xrpMemo.data UTF8String];
    }
    if (nil != xrpMemo.format) {
        memo.format = (JUB_CHAR_PTR)[xrpMemo.format UTF8String];
    }
};

// XRPMemo* <- JUB_XRP_MEMO
XRPMemo* (^inlineNSXRPMemo)(JUB_XRP_MEMO&) = ^(JUB_XRP_MEMO& xrpMemo) {
    XRPMemo* memo = [[XRPMemo alloc] init];
    
    if (nullptr != memo.type) {
        memo.type = [[NSString alloc] init];
        memo.type = [[NSString stringWithCString:xrpMemo.type
                                        encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpMemo.type);
    }
    
    if (nullptr != memo.data) {
        memo.data = [[NSString alloc] init];
        memo.data = [[NSString stringWithCString:xrpMemo.data
                                        encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpMemo.data);
    }
    
    if (nullptr != memo.format) {
        memo.format = [[NSString alloc] init];
        memo.format = [[NSString stringWithCString:xrpMemo.format
                                          encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpMemo.format);
    }
    
    return memo;
};

// PymtAmount* -> JUB_PYMT_AMOUNT
void (^inlinePymtAmount)(PymtAmount*, JUB_PYMT_AMOUNT&) = ^(PymtAmount* pymtAmount, JUB_PYMT_AMOUNT& amount) {
    if (nil != pymtAmount.currency) {
        amount.currency = (JUB_CHAR_PTR)[pymtAmount.currency UTF8String];
    }
    if (nil != pymtAmount.value) {
        amount.value    = (JUB_CHAR_PTR)[pymtAmount.value UTF8String];
    }
    if (nil != pymtAmount.issuer) {
        amount.issuer   = (JUB_CHAR_PTR)[pymtAmount.issuer UTF8String];
    }
};

// PymtAmount* <- JUB_PYMT_AMOUNT
PymtAmount* (^inlineNSPymtAmount)(JUB_PYMT_AMOUNT&) = ^(JUB_PYMT_AMOUNT& pymtAmount) {
    PymtAmount* amount = [[PymtAmount alloc] init];
    
    if (nullptr != pymtAmount.currency) {
        amount.currency = [[NSString alloc] init];
        amount.currency = [[NSString stringWithCString:pymtAmount.currency
                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(pymtAmount.currency);
    }
    
    if (nullptr != pymtAmount.value) {
        amount.value = [[NSString alloc] init];
        amount.value = [[NSString stringWithCString:pymtAmount.value
                                           encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(pymtAmount.value);
    }
    
    if (nullptr != pymtAmount.issuer) {
        amount.issuer = [[NSString alloc] init];
        amount.issuer = [[NSString stringWithCString:pymtAmount.issuer
                                            encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(pymtAmount.issuer);
    }
    
    return amount;
};

// Payment* -> JUB_PYMT_XRP
void (^inlinePayment)(PaymentXRP*, JUB_PYMT_XRP&) = ^(PaymentXRP* pymt, JUB_PYMT_XRP& payment) {
    payment.type           = inlineXRPPymtType(pymt.type);
    inlinePymtAmount(pymt.amount, payment.amount);
    if (nil != pymt.destination) {
        payment.destination    = (JUB_CHAR_PTR)[pymt.destination UTF8String];
    }
    if (nil != pymt.destinationTag) {
        payment.destinationTag = (JUB_CHAR_PTR)[pymt.destinationTag UTF8String];
    }
    if (nil != pymt.invoiceID) {
        payment.invoiceID      = (JUB_CHAR_PTR)[pymt.invoiceID UTF8String];
    }
    inlinePymtAmount(pymt.sendMax, payment.sendMax);
    inlinePymtAmount(pymt.deliverMin, payment.deliverMin);
};

// Payment* <- JUB_PYMT_XRP
PaymentXRP* (^inlineNSPayment)(JUB_PYMT_XRP&) = ^(JUB_PYMT_XRP& pymt) {
    PaymentXRP* payment = [[PaymentXRP alloc] init];
    
    payment.type   = inlineNSXRPPymtType(pymt.type);
    payment.amount = inlineNSPymtAmount(pymt.amount);
    
    if (nullptr != pymt.destination) {
        payment.destination = [[NSString alloc] init];
        payment.destination = [[NSString stringWithCString:pymt.destination
                                                  encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(pymt.destination);
    }
    
    if (nullptr != pymt.destinationTag) {
        payment.destinationTag = [[NSString alloc] init];
        payment.destinationTag = [[NSString stringWithCString:pymt.destinationTag
                                                     encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(pymt.destinationTag);
    }
    
    if (nullptr != pymt.invoiceID) {
        payment.invoiceID = [[NSString alloc] init];
        payment.invoiceID = [[NSString stringWithCString:pymt.invoiceID
                                                encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(pymt.invoiceID);
    }
    
    payment.sendMax    = inlineNSPymtAmount(pymt.sendMax);
    payment.deliverMin = inlineNSPymtAmount(pymt.deliverMin);
    
    return payment;
};

// TxXRP* -> JUB_TX_XRP
void (^inlineTxXRP)(TxXRP*, JUB_TX_XRP&) = ^(TxXRP* xrpTx, JUB_TX_XRP& tx) {
    if (nil != xrpTx.account) {
        tx.account   = (JUB_CHAR_PTR)[xrpTx.account UTF8String];
    }
    tx.type      = inlineXRPTxType(xrpTx.type);
    if (nil != xrpTx.fee) {
        tx.fee       = (JUB_CHAR_PTR)[xrpTx.fee UTF8String];
    }
    if (nil != xrpTx.sequence) {
        tx.sequence  = (JUB_CHAR_PTR)[xrpTx.sequence UTF8String];
    }
    if (nil != xrpTx.accountTxnID) {
        tx.accountTxnID = (JUB_CHAR_PTR)[xrpTx.accountTxnID UTF8String];
    }
    if (nil != xrpTx.flags) {
        tx.flags     = (JUB_CHAR_PTR)[xrpTx.flags UTF8String];
    }
    if (nil != xrpTx.lastLedgerSequence) {
        tx.lastLedgerSequence = (JUB_CHAR_PTR)[xrpTx.lastLedgerSequence UTF8String];
    }
    if (nil != xrpTx.memo) {
        inlineXRPMemo(xrpTx.memo, tx.memo);
    }
    if (nil != xrpTx.sourceTag) {
        tx.sourceTag = (JUB_CHAR_PTR)[xrpTx.sourceTag UTF8String];
    }
    
    switch (tx.type) {
        case JUB_ENUM_XRP_TX_TYPE::PYMT:
            inlinePayment(xrpTx.pymt, tx.pymt);
            break;
        case JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE:
        default:
            break;
    }
};

// TxXRP* <- JUB_TX_XRP
TxXRP* (^inlineNSTxXRP)(JUB_TX_XRP) = ^(JUB_TX_XRP xrpTx) {
    TxXRP* tx = [[TxXRP alloc] init];
    
    if(nullptr != xrpTx.account) {
        tx.account = [[NSString alloc] init];
        tx.account = [[NSString stringWithCString:xrpTx.account
                                         encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpTx.account);
    }
    
    tx.type = inlineNSXRPTxType(xrpTx.type);
    
    if(nullptr != xrpTx.fee) {
        tx.fee = [[NSString alloc] init];
        tx.fee = [[NSString stringWithCString:xrpTx.fee
                                     encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpTx.fee);
    }
    
    if(nullptr != xrpTx.sequence) {
        tx.sequence = [[NSString alloc] init];
        tx.sequence = [[NSString stringWithCString:xrpTx.sequence
                                          encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpTx.sequence);
    }
    
    if(nullptr != xrpTx.accountTxnID) {
        tx.accountTxnID = [[NSString alloc] init];
        tx.accountTxnID = [[NSString stringWithCString:xrpTx.accountTxnID
                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpTx.accountTxnID);
    }
    
    if(nullptr != xrpTx.flags) {
        tx.flags = [[NSString alloc] init];
        tx.flags = [[NSString stringWithCString:xrpTx.flags
                                       encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpTx.flags);
    }
    
    if(nullptr != xrpTx.lastLedgerSequence) {
        tx.lastLedgerSequence = [[NSString alloc] init];
        tx.lastLedgerSequence = [[NSString stringWithCString:xrpTx.lastLedgerSequence
                                                    encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpTx.lastLedgerSequence);
    }
    
    tx.memo = inlineNSXRPMemo(xrpTx.memo);
    
    if(nullptr != xrpTx.sourceTag) {
        tx.sourceTag = [[NSString alloc] init];
        tx.sourceTag = [[NSString stringWithCString:xrpTx.sourceTag
                                           encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xrpTx.sourceTag);
    }
    
    switch (xrpTx.type) {
        case JUB_ENUM_XRP_TX_TYPE::PYMT:
            tx.pymt = inlineNSPayment(xrpTx.pymt);
            break;
        case JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE:
        default:
            break;
    }
    
    return tx;
};
typedef ContextConfig ContextConfigXRP;
//JUB_RV JUB_CreateContextXRP(IN CONTEXT_CONFIG_XRP cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextXRP:(NSUInteger)deviceID
                               cfg:(ContextConfigXRP*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_XRP cfgXRP;
    if (nil != cfg.mainPath) {
        cfgXRP.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextXRP(cfgXRP,
                                     deviceID,
                                     &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}

//JUB_RV JUB_GetAddressXRP(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressXRP:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_GetAddressXRP(contextID,
                                  bip32Path,
                                  isShow,
                                  &address);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAddress = [NSString stringWithCString:address
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(address);
    
    return strAddress;
}

//JUB_RV JUB_GetHDNodeXRP(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeXRP:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pubkey = nullptr;
    JUB_RV rv = JUB_GetHDNodeXRP(contextID,
                                 fmt,
                                 bip32Path,
                                 &pubkey);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strPubkey = [NSString stringWithCString:pubkey
                                             encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(pubkey);
    
    return strPubkey;
}

//JUB_RV JUB_GetMainHDNodeXRP(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeXRP:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetMainHDNodeXRP(contextID,
                                     fmt,
                                     &xpub);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strXpub = [NSString stringWithCString:xpub
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(xpub);
    
    return strXpub;
}

//JUB_RV JUB_SetMyAddressXRP(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressXRP:(NSUInteger)contextID
                            path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_SetMyAddressXRP(contextID,
                                    bip32Path,
                                    &address);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAddress = [NSString stringWithCString:address
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(address);
    
    return strAddress;
}

//JUB_RV JUB_SignTransactionXRP(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_TX_XRP tx,
//                              OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionXRP:(NSUInteger)contextID
                               path:(BIP32Path*)path
                                 tx:(TxXRP*)tx
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_TX_XRP xrpTx;
    inlineTxXRP(tx, xrpTx);
    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTransactionXRP(contextID,
                                       bip32Path,
                                       xrpTx,
                                       &raw);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strRaw = [NSString stringWithCString:raw
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(raw);
    
    return strRaw;
}
@end
