//
//  JubSDKCore+COIN_SOL.h
//  JubSDKCore
//
//  Created by Pan Min on 2022/8/25.
//  Copyright © 2022 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

//typedef stContextCfg CONTEXT_CONFIG_SOL;
typedef ContextConfig ContextConfigSOL;

NS_ASSUME_NONNULL_BEGIN

@interface JubSDKCore (COIN_SOL)
//
////JUB_RV JUB_CreateContextSOL(IN CONTEXT_CONFIG_SOL cfg,
////                            IN JUB_UINT16 deviceID,
////                            OUT JUB_UINT16* contextID);
//- (NSUInteger)JUB_CreateContextSOL:(NSUInteger)deviceID
//                               cfg:(ContextConfigSOL*)cfg;
//
////JUB_COINCORE_DLL_EXPORT
////JUB_RV JUB_GetAddressSOL(IN JUB_UINT16 contextID,
////                         IN BIP32_Path path,
////                         IN JUB_ENUM_BOOL bShow,
////                         OUT JUB_CHAR_PTR_PTR address);
//- (NSString*)JUB_GetAddressSOL:(NSUInteger)contextID
//                          path:(BIP32Path*)path
//                         bShow:(JUB_NS_ENUM_BOOL)bShow;
//
////JUB_RV JUB_GetHDNodeSOL(IN JUB_UINT16 contextID,
////                        IN JUB_ENUM_PUB_FORMAT format,
////                        IN BIP32_Path path,
////                        OUT JUB_CHAR_PTR_PTR pubkey);
//- (NSString*)JUB_GetHDNodeSOL:(NSUInteger)contextID
//                       format:(JUB_NS_ENUM_PUB_FORMAT)format
//                         path:(BIP32Path*)path;
//
//////JUB_RV JUB_GetMainHDNodeFIL(IN JUB_UINT16 contextID,
////JUB_RV JUB_GetMainHDNodeSOL(IN JUB_UINT16 contextID,
////                            IN JUB_ENUM_PUB_FORMAT format,
////                            OUT JUB_CHAR_PTR_PTR xpub);
//- (NSString*)JUB_GetMainHDNodeSOL:(NSUInteger)contextID
//                           format:(JUB_NS_ENUM_PUB_FORMAT)format;
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
//                             amount:(NSNumber*)amount;

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
//                                 decimal:(NSInteger)decimal;
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
//                                               tokenMint:(NSString*)tokenMint;
@end

NS_ASSUME_NONNULL_END
