//
//  JubSDKCore+DEV_BLE.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"
#import "JubSDKCore+COIN_SOL.h"
#import "JubSDKCore+COIN_XRP.h"

NS_ASSUME_NONNULL_BEGIN

typedef ContextConfig ContextConfigSOL;
typedef ContextConfig ContextConfigXRP;
//typedef int(*DEV_ReadCallBack)(JUB_ULONG devHandle, JUB_BYTE_PTR data, JUB_UINT32 dataLen);
//typedef void(*DEV_ScanCallBack)(JUB_BYTE_PTR devName, JUB_BYTE_PTR uuid, JUB_UINT32 type);
//typedef void(*DEV_DiscCallBack)(JUB_BYTE_PTR uuid);
//typedef struct _DEVICE_INIT_PARAM_ {
//    void* param;
//    DEV_ReadCallBack callBack;
//    DEV_ScanCallBack scanCallBack;
//    DEV_DiscCallBack discCallBack;
//} DEVICE_INIT_PARAM;
typedef  int(*ReadCallBack)(unsigned long int devHandle, unsigned char* data, unsigned int uiDataLen);
typedef void(*ScanCallBack)(unsigned char* devName, unsigned char* uuid, unsigned int type);
typedef void(*DiscCallBack)(unsigned char* uuid);

typedef struct {
    NSString* passphrase;
    BOOL on_device;
} InputPassphrase;
typedef void (^GetPassphrase)(InputPassphrase* passphrase);

@interface DeviceInitParam : NSObject {
    void* param;
    ReadCallBack readCallBack;
    ScanCallBack scanCallBack;
    DiscCallBack discCallBack;
}
@property (atomic, assign) void* param;
@property (atomic, assign) ReadCallBack readCallBack;
@property (atomic, assign) ScanCallBack scanCallBack;
@property (atomic, assign) DiscCallBack discCallBack;
@end

@interface JubSDKCore (DEV_BLE)

//JUB_RV JUB_initDevice(IN DEVICE_INIT_PARAM param);
- (void) JUB_initDevice :(DeviceInitParam*)param;

//JUB_RV JUB_enumDevices(void);
- (void)JUB_enumDevices;

//JUB_RV JUB_stopEnumDevices(void);
- (void)JUB_stopEnumDevices;

//JUB_RV JUB_connectDevice(JUB_BYTE_PTR devName,   /**< ble device name */
//                         JUB_BYTE_PTR bBLEUUID,  /**< ble device UUID */
//                         JUB_UINT32 connectType, /**< ble device connect type */
//                         JUB_UINT16* pDeviceID,  /**< output ble device connect handle */
//                         JUB_UINT32 timeout);
- (NSUInteger)JUB_connectDevice:(NSString*)deviceName
                           uuid:(NSString*)bBLEUUID
                    connectType:(NSUInteger)connectType
                        timeout:(NSUInteger)timeout;

//JUB_RV JUB_cancelConnect(JUB_BYTE_PTR devName, JUB_BYTE_PTR bBLEUUID);
- (void)JUB_cancelConnect:(NSString*)deviceName
                     uuid:(NSString*)bBLEUUID;

//JUB_RV JUB_disconnectDevice(JUB_UINT16 deviceID);
- (void)JUB_disconnectDevice:(NSUInteger)deviceID;

//JUB_RV JUB_isDeviceConnect(JUB_UINT16 deviceID);
- (void)JUB_isDeviceConnect:(NSUInteger)deviceID;

//JUB_RV JUB_QueryBattery(IN JUB_UINT16 deviceID,
//                        OUT JUB_BYTE_PTR percent);
- (NSString*)JUB_QueryBattery:(NSUInteger)deviceID;

//JUB_RV JUB_BleNusSetNotifyMessageCallback(
//    IN JUB_UINT16 deviceID,
//    IN JUB_BleNusNotify_PTR pNotify
//);
- (void)JUB_BleNusSetNotifyMessageCallback:(NSUInteger)deviceID
                                    notify:(void(^)(NSUInteger messageType))notifyCallback;

//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_BleNusSetUpdatePercentageCallback(
//        IN JUB_UINT16 deviceID,
//        IN JUB_UpdatePercentage_PTR pUpdate
//);
- (void)JUB_BleNusSetUpdatePercentageCallback:(NSUInteger)deviceID
                                   percentage:(void(^)(NSUInteger percentage))percentageCallback;

//JUB_RV JUB_BleNusSetPasephraseCallback(
//        IN JUB_UINT16 deviceID,
//        IN JUB_BleNusPassphrase_PTR pNotify
//);

- (void)JUB_BleNusSetPassphraseCallback:(NSUInteger)deviceID
                               callback:(GetPassphrase) getPassphrase;

//JUB_RV JUB_GetFeatures(IN JUB_UINT16 deviceID,
//                       OUT JUB_CHAR_PTR_PTR Features)
- (NSString*)JUB_GetFeatures:(NSUInteger)deviceID;

//JUB_RV JUB_SetLable(IN JUB_UINT16 deviceID, IN JUB_CHAR_PTR pLable)
- (void)JUB_SetLable:(NSUInteger)deviceID
              pLable:(NSString*)pLable;

//JUB_RV JUB_UsePassphrase(IN JUB_UINT16 deviceID, IN bool bUse)
- (void)JUB_UsePassphrase:(NSUInteger)deviceID
                bUse:(BOOL)bUse;

//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_EndSession(IN JUB_UINT16 deviceID);
- (void)JUB_EndSession:(NSUInteger)deviceID;

//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_OpCancel(IN JUB_UINT16 deviceID);
- (void)JUB_OpCancel:(NSUInteger)deviceID;

//JUB_RV Jub_EnumSupportCoins(IN JUB_UINT16 deviceID,
//                            OUT JUB_CHAR_PTR_PTR coinList);
- (NSString*)Jub_EnumSupportCoins:(NSUInteger)deviceID;

//JUB_RV JUB_GetDeviceCert(IN JUB_UINT16 deviceID,
//                         OUT JUB_CHAR_PTR_PTR cert);
- (NSString*)JUB_GetDeviceCert:(NSUInteger)deviceID;

//JUB_RV JUB_GetDeviceSignData(IN JUB_UINT16 deviceID,
//                             IN JUB_CHAR_PTR hashData,
//                             OUT JUB_CHAR_PTR_PTR SignData)

- (NSString*)JUB_GetDeviceSignData:(NSUInteger)deviceID
                          hashData:(NSString*)hashData;

//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_RebootToBootLoader(IN JUB_UINT16 deviceID);
- (void)JUB_RebootToBootLoader:(NSUInteger)deviceID;

//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_RebootToNormal(IN JUB_UINT16 deviceID);
- (void)JUB_RebootToNormal:(NSUInteger)deviceID;

//JUB_RV JUB_UpdateFirmware(IN JUB_UINT16 deviceID,
//                          IN JUB_BYTE_PTR firmwareFilePayload,
//                          IN JUB_UINT32 firmwareFileSize,
//                          IN bool reboot_on_success);
- (void)JUB_UpdateFirmware:(NSUInteger)deviceID
       firmwareFilePayload:(NSData*)firmwareFilePayload
          firmwareFileSize:(NSUInteger)firmwareFileSize
         reboot_on_success:(BOOL)reboot_on_success;

//JUB_RV JUB_UpdateResource(IN JUB_UINT16 deviceID,
//                          IN JUB_BYTE_PTR resourceZipPayload,
//                          IN JUB_UINT32 resourceZipSize,
//                          IN bool reboot_on_success);
- (void)JUB_UpdateResource:(NSUInteger)deviceID
        resourceZipPayload:(NSData*)resourceZipPayload
           resourceZipSize:(NSUInteger)resourceZipSize
         reboot_on_success:(BOOL)reboot_on_success;




//JUB_RV JUB_CreateContextSOL(IN CONTEXT_CONFIG_SOL cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextSOL:(NSUInteger)deviceID
                               cfg:(ContextConfigSOL*)cfg;

//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_GetAddressSOL(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressSOL:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow;

//JUB_RV JUB_GetHDNodeSOL(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeSOL:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path;

////JUB_RV JUB_GetMainHDNodeFIL(IN JUB_UINT16 contextID,
//JUB_RV JUB_GetMainHDNodeSOL(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeSOL:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format;


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
                             amount:(NSNumber*)amount;

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
                                 decimal:(NSInteger)decimal;

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
                                         decimal:(NSInteger)decimal;

@end

NS_ASSUME_NONNULL_END
