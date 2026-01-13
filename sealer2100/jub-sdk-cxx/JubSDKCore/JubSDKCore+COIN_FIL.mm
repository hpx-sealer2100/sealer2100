//
//  JubSDKCore+COIN_FIL.mm
//  JubSDKCore
//
//  Created by Pan Min on 2021/4/27.
//  Copyright © 2021 JuBiter. All rights reserved.
//

#import "JubSDKCore+COIN_FIL.h"
#import "JUB_SDK_FIL.h"

extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT);

@implementation JubSDKCore (COIN_FIL)

//JUB_RV JUB_CreateContextFIL(IN CONTEXT_CONFIG_FIL cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextFIL:(NSUInteger)deviceID
                               cfg:(ContextConfigFIL*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_FIL cfgFIL;
    if (nil != cfg.mainPath) {
        cfgFIL.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextFIL(cfgFIL,
                                     deviceID,
                                     &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}

//JUB_RV JUB_GetAddressFIL(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressFIL:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_GetAddressFIL(contextID,
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

//JUB_RV JUB_GetHDNodeFIL(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeFIL:(NSUInteger)contextID
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
    JUB_RV rv = JUB_GetHDNodeFIL(contextID,
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

//JUB_RV JUB_GetMainHDNodeFIL(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeFIL:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetMainHDNodeFIL(contextID,
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

//JUB_RV JUB_SignTransactionFIL(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_UINT32 nonce,
//                              IN JUB_UINT32 gasLimit,
//                              IN JUB_CHAR_PTR gasFeeCapInAtto,
//                              IN JUB_CHAR_PTR gasPremiumInAtto,
//                              IN JUB_CHAR_PTR to,
//                              IN JUB_CHAR_PTR valueInAtto,
//                              IN JUB_CHAR_PTR input,
//                              OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionFIL:(NSUInteger)contextID
                               path:(BIP32Path*)path
                              nonce:(NSUInteger)nonce
                           gasLimit:(NSUInteger)gasLimit
                    gasFeeCapInAtto:(NSString*)gasFeeCapInAtto
                   gasPremiumInAtto:(NSString*)gasPremiumInAtto
                                 to:(NSString*)to
                        valueInAtto:(NSString*)valueInAtto
                              input:(NSString*)input
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR vInAtto = nullptr;
    if (NSOrderedSame != [valueInAtto compare:@""]
        &&        nil !=  valueInAtto
        ) {
        vInAtto = (JUB_CHAR_PTR)[valueInAtto UTF8String];
    }
    JUB_CHAR_PTR pGasFeeCapInAtto = nullptr;
    if (nil != gasFeeCapInAtto) {
        pGasFeeCapInAtto = (JUB_CHAR_PTR)[gasFeeCapInAtto UTF8String];
    }
    JUB_CHAR_PTR pGasPremiumInAtto = nullptr;
    if (nil != gasPremiumInAtto) {
        pGasPremiumInAtto = (JUB_CHAR_PTR)[gasPremiumInAtto UTF8String];
    }
    JUB_CHAR_PTR pTo = nullptr;
    if (nil != to) {
        pTo = (JUB_CHAR_PTR)[to UTF8String];
    }
    JUB_CHAR_PTR pInput = nullptr;
    if (nil != input) {
        pInput = (JUB_CHAR_PTR)[input UTF8String];
    }
    
    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTransactionFIL(contextID,
                                       bip32Path,
                                       (JUB_UINT32)nonce,
                                       (JUB_UINT32)gasLimit,
                                       pGasFeeCapInAtto,
                                       pGasPremiumInAtto,
                                       pTo,
                                       vInAtto,
                                       pInput,
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
