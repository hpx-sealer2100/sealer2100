//
//  JubSDKCore+COIN_ETH.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

//typedef struct {
//    JUB_CHAR_PTR    main_path;
//    uint64_t        chainID;
//} CONTEXT_CONFIG_ETH;
@interface ContextConfigETH : NSObject
@property (atomic, copy  ) NSString* _Nonnull mainPath;
@property (atomic, assign) NSInteger chainID;
@end

//typedef struct stERC20TokenInfo {
//    JUB_CHAR_PTR tokenName;
//    JUB_UINT16 unitDP;
//    JUB_CHAR_PTR contractAddress;
//
//    stERC20TokenInfo() {
//        unitDP = 0;
//    }
//    virtual ~stERC20TokenInfo() {}
//} ERC20_TOKEN_INFO;
@interface ERC20TokenInfo : NSObject
@property (atomic, copy  ) NSString* _Nonnull tokenName;
@property (atomic, assign) NSInteger unitDP;
@property (atomic, copy  ) NSString* _Nonnull contractAddress;
@end

//typedef struct {
//    uint64_t chainID;      // Chain ID of the network, must match CONTEXT_CONFIG_ETH.chainID
//    uint64_t slip44ID;     // SLIP-0044 coin type of the network (reserved, currently unused)
//    JUB_CHAR_CPTR name;    // Network name
//    JUB_CHAR_CPTR symbol;  // Native currency symbol of the network
//} JUB_ETH_NETWORK_INFO;
//
@interface ETHNetworkInfoV2 : NSObject
@property (atomic, copy) NSString* _Nonnull name;
@property (atomic, copy) NSString* _Nonnull symbol;
@property (atomic, assign) NSInteger chain_id;
@property (atomic, assign) NSInteger slip44_id;
@end

//typedef struct {
//    uint64_t chainID;      // Chain ID of the network to which the token belongs
//    JUB_CHAR_CPTR name;    // Token name
//    JUB_CHAR_CPTR symbol;  // Token symbol
//    JUB_CHAR_CPTR address; // Token contract address
//    JUB_UINT32 decimals;   // Token decimal precision
//} JUB_ERC20_TOKEN_INFO;
@interface ERC20TokenInfoV2 : NSObject
@property (atomic, copy) NSString* _Nonnull name;
@property (atomic, copy) NSString* _Nonnull symbol;
@property (atomic, copy) NSString* _Nonnull address;
@property (atomic, assign) NSInteger chain_id;
@property (atomic, assign) NSInteger decimals;
@end

//typedef struct {
//    JUB_CHAR_CPTR network;    // The network
//    JUB_CHAR_CPTR token;      // The contract address
//    JUB_CHAR_CPTR name;       // The name of the contract
//    JUB_CHAR_CPTR owner;      // The owner of this NFT
//    JUB_CHAR_CPTR id;         // NFT id
//    JUB_CHAR_CPTR extension;  // image extension, support(png, jpg, jpeg)
//    struct {
//        JUB_BYTE_CPTR payload;
//        JUB_UINT32 size;
//    } image;                  // NFT image content, the image pix size is 432x432
//    struct {
//        JUB_BYTE_CPTR payload;
//        JUB_UINT32 size;
//    } thumbnail;              // NFT image thumbnail content, the image pix size is 108x108
//}JUB_ETH_NFT_INFO;
@interface ETHNFTInfo : NSObject
@property (atomic, copy) NSString* _Nonnull id;
@property (atomic, copy) NSString* _Nonnull name;
@property (atomic, copy) NSString* _Nonnull token;
@property (atomic, copy) NSString* _Nonnull network;
@property (atomic, copy) NSString* _Nonnull owner;
@property (atomic, copy) NSString* _Nonnull extension;
@property (atomic, assign) NSData* _Nonnull image;
@property (atomic, assign) NSData* _Nonnull thumbnail;
@property (atomic, assign) NSData* _Nonnull wallpaper;
@end

NS_ASSUME_NONNULL_BEGIN

@interface JubSDKCore (COIN_ETH)

//JUB_RV JUB_CreateContextETH(IN CONTEXT_CONFIG_ETH cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextETH:(NSUInteger)deviceID
                               cfg:(ContextConfigETH*)cfg;

//JUB_RV JUB_GetAddressETH(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressETH:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow;

//JUB_RV JUB_GetHDNodeETH(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeETH:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path;

//JUB_RV JUB_GetMainHDNodeETH(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeETH:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format;

//JUB_RV JUB_SetMyAddressETH(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressETH:(NSUInteger)contextID
                            path:(BIP32Path*)path;

//JUB_RV JUB_SetContrAddrETH(IN JUB_UINT16 contextID,
//                           IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetContrAddrETH:(NSUInteger)contextID
                  contrAddr:(NSString*)contractAddress;

//JUB_RV JUB_SignTransactionETH(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_UINT32 nonce,
//                              IN JUB_UINT32 gasLimit,
//                              IN JUB_CHAR_PTR gasPriceInWei,
//                              IN JUB_CHAR_PTR to,
//                              IN JUB_CHAR_PTR valueInWei,
//                              IN JUB_CHAR_PTR input,
//                              OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionETH:(NSUInteger)contextID
                               path:(BIP32Path*)path
                              nonce:(NSUInteger)nonce
                           gasLimit:(NSUInteger)gasLimit
                      gasPriceInWei:(NSString*)gasPriceInWei
                                 to:(NSString*)to
                         valueInWei:(NSString*)valueInWei
                              input:(NSString*)input;

//JUB_RV JUB_SetERC20TokensETH(IN JUB_UINT16 contextID,
//                             IN ERC20_TOKEN_INFO tokens[],
//                             IN JUB_UINT16 iCount);
- (void)JUB_SetERC20TokensETH:(NSUInteger)contextID
                   tokenArray:(NSArray*)tokenArray;

//JUB_RV JUB_SetERC20TokenETH(IN JUB_UINT16 contextID,
//                            IN JUB_CHAR_CPTR tokenName,
//                            IN JUB_UINT16 unitDP,
//                            IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetERC20TokenETH:(NSUInteger)contextID
                   tokenName:(NSString*)tokenName
                      unitDP:(NSUInteger)unitDP
             contractAddress:(NSString*)contractAddress;

- (void)JUB_SetETHV2: (NSUInteger)contextID
             network:(ETHNetworkInfoV2*)network;

- (void)JUB_SetETHV2:(NSUInteger)contextID
             network:(ETHNetworkInfoV2*)network
               token:(ERC20TokenInfoV2*)token;

//JUB_RV JUB_BuildERC20TransferAbiETH(IN JUB_UINT16 contextID,
//                                    IN JUB_CHAR_CPTR tokenTo, IN JUB_CHAR_CPTR tokenValue,
//                                    OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildERC20TransferAbiETH:(NSUInteger)contextID
                                  tokenTo:(NSString*)tokenTo
                               tokenValue:(NSString*)tokenValue;

//JUB_RV JUB_SignContractETH(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           IN JUB_UINT32 nonce,
//                           IN JUB_UINT32 gasLimit,
//                           IN JUB_CHAR_PTR gasPriceInWei,
//                           IN JUB_CHAR_PTR to,
//                           IN JUB_CHAR_PTR valueInWei,
//                           IN JUB_CHAR_PTR input,
//                           OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignContractETH:(NSUInteger)contextID
                            path:(BIP32Path*)path
                           nonce:(NSUInteger)nonce
                        gasLimit:(NSUInteger)gasLimit
                   gasPriceInWei:(NSString*)gasPriceInWei
                              to:(NSString*)to
                      valueInWei:(NSString*)valueInWei
                           input:(NSString*)input;

//JUB_RV JUB_SignBytestringETH(IN JUB_UINT16 contextID,
//                             IN BIP32_Path path,
//                             IN JUB_CHAR_CPTR data,
//                             OUT JUB_CHAR_PTR_PTR signature);
- (NSString*)JUB_SignBytestringETH:(NSUInteger)contextID
                              path:(BIP32Path*)path
                              data:(NSString*)data;

//JUB_RV JUB_SignTypedDataETH(IN JUB_UINT16 contextID,
//                            IN BIP32_Path path,
//                            IN JUB_CHAR_CPTR dataInJSON,
//                            IN JUB_BBOOL isMetamaskV4Compat,
//                            OUT JUB_CHAR_PTR_PTR signature);
- (NSString*)JUB_SignTypedDataETH:(NSUInteger)contextID
                             path:(BIP32Path*)path
                             data:(NSString*)dataInJSON
               isMetamaskV4Compat:(JUB_NS_ENUM_BOOL)isMetamaskV4Compat;

//JUB_RV JUB_BuildContractWithAddrAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR address,
//                                       OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                     address:(NSString*)address;
//
//JUB_RV JUB_BuildContractWithAddrAmtAbiETH(IN JUB_UINT16 contextID,
//                                          IN JUB_CHAR_PTR methodID,
//                                          IN JUB_CHAR_PTR address,
//                                          IN JUB_CHAR_PTR amount,
//                                          OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAmtAbiETH:(NSUInteger)contextID
//                                       methodID:(NSString*)methodID
//                                        address:(NSString*)address
//                                         amount:(NSString*)amount;
//
//JUB_RV JUB_BuildContractWithTxIDAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR transactionID,
//                                       OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
                                    methodID:(NSString*)methodID
                               transactionID:(NSString*)transactionID;

//JUB_RV JUB_BuildContractWithAmtAbiETH(IN JUB_UINT16 contextID,
//                                      IN JUB_CHAR_PTR methodID,
//                                      IN JUB_CHAR_PTR amount,
//                                      OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                      amount:(NSString*)amount;

//JUB_RV JUB_BuildContractWithAddrAmtDataAbiETH(IN JUB_UINT16 contextID,
//                                              IN JUB_CHAR_PTR methodID,
//                                              IN JUB_CHAR_PTR address,
//                                              IN JUB_CHAR_PTR amount,
//                                              IN JUB_CHAR_PTR data,
//                                              OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildContractWithAddrAmtDataAbiETH:(NSUInteger)contextID
                                           methodID:(NSString*)methodID
                                            address:(NSString*)address
                                             amount:(NSString*)amount
                                               data:(NSString*)data;

- (void)JUB_Upload: (NSUInteger)contextId
               nft: (ETHNFTInfo*) nft;

@end

NS_ASSUME_NONNULL_END
