//
//  JubSDKCore+COIN_SOL.mm
//  JubSDKCore
//
//  Created by Pan Min on 2022/8/25.
//  Copyright © 2022 JuBiter. All rights reserved.
//

#import "JubSDKCore+COIN_SOL.h"
#import "JUB_SDK_SOL.h"
#import "JubSDKCore+DEV_BLE.h"

extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT);

@implementation JubSDKCore (COIN_SOL)

//JUB_RV JUB_CreateContextSOL(IN CONTEXT_CONFIG_SOL cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
//- (NSUInteger)JUB_CreateContextSOL:(NSUInteger)deviceID
//                               cfg:(ContextConfigSOL*)cfg
//{
//    self.lastError = JUBR_OK;
//    
//    CONTEXT_CONFIG_SOL cfgSOL;
//    if (nil != cfg.mainPath) {
//        cfgSOL.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
//    }
//    JUB_UINT16 contextID = 0;
//    JUB_RV rv = JUB_CreateContextSOL(cfgSOL,
//                                     deviceID,
//                                     &contextID);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return contextID;
//    }
//    
//    return contextID;
//}


////JUB_RV JUB_GetAddressSOL(IN JUB_UINT16 contextID,
////                         IN BIP32_Path path,
////                         IN JUB_ENUM_BOOL bShow,
////                         OUT JUB_CHAR_PTR_PTR address);
//- (NSString*)JUB_GetAddressSOL:(NSUInteger)contextID
//                          path:(BIP32Path*)path
//                         bShow:(JUB_NS_ENUM_BOOL)bShow
//{
//    self.lastError = JUBR_OK;
//
//    
//    BIP32_Path bip32Path;
//    bip32Path.change = inlineBool(path.change);
//    bip32Path.addressIndex = path.addressIndex;
//    JUB_ENUM_BOOL isShow = inlineBool(bShow);
//    JUB_CHAR_PTR address;
//    JUB_RV rv = JUB_GetAddressSOL(contextID,
//                                  bip32Path,
//                                  isShow,
//                                  &address);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strAddress = [NSString stringWithCString:address
//                                              encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(address);
//    
//    return strAddress;
//}
//
//
////JUB_RV JUB_GetHDNodeSOL(IN JUB_UINT16 contextID,
////                        IN JUB_ENUM_PUB_FORMAT format,
////                        IN BIP32_Path path,
////                        OUT JUB_CHAR_PTR_PTR pubkey);
//- (NSString*)JUB_GetHDNodeSOL:(NSUInteger)contextID
//                       format:(JUB_NS_ENUM_PUB_FORMAT)format
//                         path:(BIP32Path*)path
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
//    if (PUB_FORMAT_NS_ITEM == fmt) {
//        self.lastError = JUBR_ARGUMENTS_BAD;
//        return @"";
//    }
//    BIP32_Path bip32Path;
//    bip32Path.change = inlineBool(path.change);
//    bip32Path.addressIndex = path.addressIndex;
//    
//    JUB_CHAR_PTR pubkey = nullptr;
//    JUB_RV rv = JUB_GetHDNodeSOL(contextID,
//                                 fmt,
//                                 bip32Path,
//                                 &pubkey);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strPubkey = [NSString stringWithCString:pubkey
//                                             encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(pubkey);
//    
//    return strPubkey;
//}
//
//
////JUB_RV JUB_GetMainHDNodeSOL(IN JUB_UINT16 contextID,
////                            IN JUB_ENUM_PUB_FORMAT format,
////                            OUT JUB_CHAR_PTR_PTR xpub);
//- (NSString*)JUB_GetMainHDNodeSOL:(NSUInteger)contextID
//                           format:(JUB_NS_ENUM_PUB_FORMAT)format
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
//    if (PUB_FORMAT_NS_ITEM == fmt) {
//        self.lastError = JUBR_ARGUMENTS_BAD;
//        return @"";
//    }
//    JUB_CHAR_PTR xpub = nullptr;
//    JUB_RV rv = JUB_GetMainHDNodeSOL(contextID,
//                                     fmt,
//                                     &xpub);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strXpub = [NSString stringWithCString:xpub
//                                           encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(xpub);
//    
//    return strXpub;
//}
//
//
////JUB_RV JUB_SignTransactionSOL(IN JUB_UINT16 contextID,
////                              IN BIP32_Path path,
////                              IN JUB_CHAR_CPTR recentHash,
////                              IN JUB_CHAR_CPTR dest,
////                              IN JUB_UINT64 amount,
////                              OUT JUB_CHAR_PTR_PTR txRaw);
//- (NSString*)JUB_SignTransactionSOL:(NSUInteger)contextID
//                               path:(BIP32Path*)path
//                         recentHash:(NSString*)recentHash
//                               dest:(NSString*)dest
//                             amount:(NSNumber*)amount
//{
//    self.lastError = JUBR_OK;
//    
//    BIP32_Path bip32Path;
//    bip32Path.change = inlineBool(path.change);
//    bip32Path.addressIndex = path.addressIndex;
//    JUB_CHAR_PTR pRecentHash = nullptr;
//    if (nil != recentHash) {
//        pRecentHash = (JUB_CHAR_PTR)[recentHash UTF8String];
//    }
//    JUB_CHAR_PTR pDest = nullptr;
//    if (nil != dest) {
//        pDest = (JUB_CHAR_PTR)[dest UTF8String];
//    }
//    JUB_UINT64 uAmount = [amount unsignedLongLongValue];
//    
//    JUB_CHAR_PTR raw = nullptr;
//    JUB_RV rv = JUB_SignTransactionSOL(contextID,
//                                       bip32Path,
//                                       pRecentHash,
//                                       pDest,
//                                       uAmount,
//                                       &raw);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strRaw = [NSString stringWithCString:raw
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(raw);
//    
//    return strRaw;
//}


////JUB_RV JUB_SignTransactionTokenSOL(IN JUB_UINT16 contextID,
////                                   IN JUB_CHAR_CPTR path,
////                                   IN JUB_CHAR_CPTR recentHash,
////                                   IN JUB_CHAR_CPTR tokenMint,
////                                   IN JUB_CHAR_CPTR source,
////                                   IN JUB_CHAR_CPTR dest,
////                                   IN JUB_UINT64 amount,
////                                   IN JUB_UINT8 decimal,
////                                   OUT JUB_CHAR_PTR_PTR txRaw);
//- (NSString*)JUB_SignTransactionTokenSOL:(NSUInteger)contextID
//                                    path:(NSString*)path
//                              recentHash:(NSString*)recentHash
//                               tokenMint:(NSString*)tokenMint
//                                  source:(NSString*)source
//                                    dest:(NSString*)dest
//                                  amount:(NSNumber*)amount
//                                 decimal:(NSInteger)decimal
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pPath = nullptr;
//    if (nil != path) {
//        pPath = (JUB_CHAR_PTR)[path UTF8String];
//    }
//    JUB_CHAR_PTR pRecentHash = nullptr;
//    if (nil != recentHash) {
//        pRecentHash = (JUB_CHAR_PTR)[recentHash UTF8String];
//    }
//    JUB_CHAR_PTR pTokenMint = nullptr;
//    if (nil != tokenMint) {
//        pTokenMint = (JUB_CHAR_PTR)[tokenMint UTF8String];
//    }
//    JUB_CHAR_PTR pSource = nullptr;
//    if (nil != source) {
//        pSource = (JUB_CHAR_PTR)[source UTF8String];
//    }
//    JUB_CHAR_PTR pDest = nullptr;
//    if (nil != dest) {
//        pDest = (JUB_CHAR_PTR)[dest UTF8String];
//    }
//    JUB_UINT64 uAmount = [amount unsignedLongLongValue];
//    
//    JUB_CHAR_PTR raw = nullptr;
////    JUB_RV rv = JUB_SignTransactionTokenSOL(contextID,
////                                            pPath,
////                                            pRecentHash,
////                                            pTokenMint,
////                                            pSource,
////                                            pDest,
////                                            uAmount,
////                                            decimal,
////                                            &raw);
////    if (JUBR_OK != rv) {
////        self.lastError = rv;
////        return @"";
////    }
//    
//    NSString* strRaw = [NSString stringWithCString:raw
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(raw);
//    
//    return strRaw;
//}
//
//
////JUB_RV JUB_SignTransactionAssociatedTokenCreateSOL(IN JUB_UINT16 contextID,
////                                                   IN JUB_CHAR_CPTR path,
////                                                   IN JUB_CHAR_CPTR recentHash,
////                                                   IN JUB_CHAR_CPTR owner,
////                                                   IN JUB_CHAR_CPTR tokenMint,
////                                                   OUT JUB_CHAR_PTR_PTR txRaw);
//- (NSString*)JUB_SignTransactionAssociatedTokenCreateSOL:(NSUInteger)contextID
//                                                    path:(NSString*)path
//                                              recentHash:(NSString*)recentHash
//                                                   owner:(NSString*)owner
//                                               tokenMint:(NSString*)tokenMint
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pPath = nullptr;
//    if (nil != path) {
//        pPath = (JUB_CHAR_PTR)[path UTF8String];
//    }
//    JUB_CHAR_PTR pRecentHash = nullptr;
//    if (nil != recentHash) {
//        pRecentHash = (JUB_CHAR_PTR)[recentHash UTF8String];
//    }
//    JUB_CHAR_PTR pOwner = nullptr;
//    if (nil != owner) {
//        pOwner = (JUB_CHAR_PTR)[owner UTF8String];
//    }
//    JUB_CHAR_PTR pTokenMint = nullptr;
//    if (nil != tokenMint) {
//        pTokenMint = (JUB_CHAR_PTR)[tokenMint UTF8String];
//    }
//    
//    JUB_CHAR_PTR raw = nullptr;
////    JUB_RV rv = JUB_SignTransactionAssociatedTokenCreateSOL(contextID,
////                                                            pPath,
////                                                            pRecentHash,
////                                                            pOwner,
////                                                            pTokenMint,
////                                                            &raw);
////    if (JUBR_OK != rv) {
////        self.lastError = rv;
////        return @"";
////    }
//    
//    NSString* strRaw = [NSString stringWithCString:raw
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(raw);
//    
//    return strRaw;
//}
@end
