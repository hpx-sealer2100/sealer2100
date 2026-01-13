//
//  JubSDKCore+COIN_FIL.h
//  JubSDKCore
//
//  Created by Pan Min on 2021/4/27.
//  Copyright © 2021 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

//typedef stContextCfg CONTEXT_CONFIG_FIL;
typedef ContextConfig ContextConfigFIL;

NS_ASSUME_NONNULL_BEGIN

@interface JubSDKCore (COIN_FIL)

//JUB_RV JUB_CreateContextFIL(IN CONTEXT_CONFIG_FIL cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextFIL:(NSUInteger)deviceID
                               cfg:(ContextConfigFIL*)cfg;

//JUB_RV JUB_GetAddressFIL(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressFIL:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow;

//JUB_RV JUB_GetHDNodeFIL(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeFIL:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path;

//JUB_RV JUB_GetMainHDNodeFIL(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeFIL:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format;

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
                              input:(NSString*)input;
@end

NS_ASSUME_NONNULL_END
