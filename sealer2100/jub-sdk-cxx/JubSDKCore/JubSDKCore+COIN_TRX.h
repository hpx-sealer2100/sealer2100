//
//  JubSDKCore+COIN_TRX.h
//  JubSDKCore
//
//  Created by Pan Min on 2020/12/13.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

//typedef stContextCfg CONTEXT_CONFIG_TRX;
typedef ContextConfig ContextConfigTRX;

//typedef enum {
//     NS_ITEM_TRX_CONTRACT =  0,
//            XFER_CONTRACT =  1, // TransferContract(balance_contract.proto)
//      XFER_ASSET_CONTRACT =  2, // TransferAssetContract(asset_issue_contract.proto)
//    CREATE_SMART_CONTRACT = 30, // CreateSmartContract(smart_contract.proto)
//      TRIG_SMART_CONTRACT = 31, // TriggerSmartContract(smart_contract.proto)
//ACCT_PERM_UPDATE_CONTRACT = 46, // AccountPermissionUpdateContract(account_contract.proto)
//     NS_ITEM_TRX_CONTRACT
//} JUB_ENUM_TRX_CONTRACT_TYPE;
typedef NS_ENUM(NSInteger, JUB_NS_TRX_CONTR_TYPE) {
    NS_TX_TRX_TYPE_NS = 0,
    NS_XFER_CONTRACT  = 1,
    NS_XFER_ASSET_CONTRACT = 2,
    NS_CREATE_SMART_CONTRACT = 30,
    NS_TRIG_SMART_CONTRACT = 31,
    NS_ACCT_PERM_UPDATE_CONTRACT = 46
};

//typedef struct stTronKey {
//    JUB_CHAR_PTR address;
//    JUB_INT64 weight;
//} JUB_KEY_TRX;
@interface TRXKey : NSObject
@property (atomic, copy  ) NSString* _Nonnull address;
@property (atomic, assign) NSInteger weight;
@end

//typedef struct stTronPermission {
//    PermissionType type;
//    JUB_UINT32 identity;
//    JUB_CHAR_PTR permission_name;
//    JUB_INT64 threshold;
//    JUB_CHAR_PTR operations;
//    JUB_KEY_TRX keys[5];
//    JUB_UINT16 keyCount;
//} JUB_PERMISSION_TRX;
enum PermissionType {
    Owner = 0,
    Witness = 1,
    Active = 2,
};
@interface TRXPermission : NSObject
@property (atomic, assign) enum PermissionType type;
@property (atomic, assign) NSInteger identity;
@property (atomic, copy  ) NSString* _Nonnull permissionName;
@property (atomic, assign) NSInteger threshold;
@property (atomic, copy  ) NSString* _Nonnull operations;
@property (atomic, strong) NSMutableArray* _Nonnull keys;
@end

//typedef struct stTronAccountPermissionUpdateContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_PERMISSION_TRX owner;
//    JUB_PERMISSION_TRX witness;
//    JUB_PERMISSION_TRX actives[5];
//    JUB_UINT16 activeCount;
//} JUB_ACCT_PERM_UPDATE_CONTRACT_TRX;
@interface TRXAccountPermissionUpdateContract : NSObject
@property (atomic, copy  ) NSString* _Nonnull ownerAddress;
@property (atomic, strong) TRXPermission* _Nonnull owner;
@property (atomic, strong) TRXPermission* _Nonnull witness;
@property (atomic, strong) NSMutableArray* _Nonnull actives;
@end

//typedef struct stTronTransferContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR    to_address;
//    JUB_INT64 amount;
//} JUB_XFER_CONTRACT_TRX;
@interface TRXTransferContract : NSObject
@property (atomic, copy  ) NSString* _Nonnull ownerAddress;
@property (atomic, copy  ) NSString* _Nonnull toAddress;
@property (atomic, assign) NSInteger amount;
@end

//typedef struct stTronTransferAssetContract {
//    JUB_CHAR_PTR asset_name;
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR    to_address;
//    JUB_INT64       amount;
//} JUB_XFER_ASSET_CONTRACT_TRX;
@interface TRXTransferAssetContract : NSObject
@property (atomic, copy  ) NSString* _Nonnull assetName;
@property (atomic, copy  ) NSString* _Nonnull ownerAddress;
@property (atomic, copy  ) NSString* _Nonnull toAddress;
@property (atomic, assign) NSInteger amount;
@end

//typedef struct stTronCreateSmartContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR bytecode;
//    JUB_INT64 call_token_value;
//    JUB_INT64 token_id;
//} JUB_CREATE_SMART_CONTRACT_TRX;
@interface TRXCreateSmartContract : NSObject
@property (atomic, copy  ) NSString* _Nonnull ownerAddress;
@property (atomic, copy  ) NSString* _Nonnull bytecode;
@property (atomic, assign) NSInteger callTokenValue;
@property (atomic, assign) NSInteger tokenId;
@end

//typedef struct stTronTriggerSmartContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR contract_address;
//    JUB_INT64 call_value;
//    JUB_CHAR_PTR data;
//    JUB_INT64 call_token_value;
//    JUB_INT64 token_id;
//} JUB_TRIG_SMART_CONTRACT_TRX;
@interface TRXTriggerSmartContract : NSObject
@property (atomic, copy  ) NSString* _Nonnull ownerAddress;
@property (atomic, copy  ) NSString* _Nonnull contractAddress;
@property (atomic, assign) NSInteger callValue;
@property (atomic, copy  ) NSString* _Nonnull data;
@property (atomic, assign) NSInteger callTokenValue;
@property (atomic, assign) NSInteger tokenId;
@end

//typedef struct stTronContract {
//    JUB_ENUM_TRX_CONTRACT_TYPE type;
//    union {
//  JUB_ACCT_PERM_UPDATE_CONTRACT_TRX acctPermUpdate;
//              JUB_XFER_CONTRACT_TRX transfer;
//        JUB_XFER_ASSET_CONTRACT_TRX transferAsset;
//      JUB_CREATE_SMART_CONTRACT_TRX createSmart;
//        JUB_TRIG_SMART_CONTRACT_TRX triggerSmart;
//    };
//    JUB_UINT32 permissionId;   // [Optional]
//} JUB_CONTRACT_TRX;
@interface TRXContract : NSObject
@property (atomic, assign) JUB_NS_TRX_CONTR_TYPE type;
@property (atomic, assign) NSInteger permissionId;
@property (atomic, strong) TRXAccountPermissionUpdateContract* _Nonnull acctPermUpdate;
@property (atomic, strong) TRXTransferContract* _Nonnull xfer;
@property (atomic, strong) TRXTransferAssetContract* _Nonnull assetXfer;
@property (atomic, strong) TRXTriggerSmartContract* _Nonnull triggerSmart;
@end

//typedef struct stTxTRX {
//    JUB_CHAR_PTR ref_block_bytes;
//    JUB_CHAR_PTR ref_block_num; // [Optional]
//    JUB_CHAR_PTR ref_block_hash;
//    JUB_CHAR_PTR expiration;
//    JUB_CHAR_PTR data;          // [Optional]
//    JUB_CONTRACT_TRX_PTR contracts;
//    JUB_UINT16 contractCount;
//    JUB_CHAR_PTR scripts;       // [Optional]
//    JUB_CHAR_PTR timestamp;
//    JUB_CHAR_PTR fee_limit;     // [Optional]
//} JUB_TX_TRX;
@interface TxTRX : NSObject
@property (atomic, copy  ) NSString* _Nonnull refBlockBytes;
@property (atomic, copy  ) NSString* _Nonnull refBlockNum;
@property (atomic, copy  ) NSString* _Nonnull refBlockHash;
@property (atomic, copy  ) NSString* _Nonnull expiration;
@property (atomic, copy  ) NSString* _Nonnull data;
@property (atomic, strong) NSMutableArray* _Nonnull contracts;
@property (atomic, copy  ) NSString* _Nonnull scripts;
@property (atomic, copy  ) NSString* _Nonnull timestamp;
@property (atomic, copy  ) NSString* _Nonnull feeLimit;
@end

NS_ASSUME_NONNULL_BEGIN

@interface JubSDKCore (COIN_TRX)

//JUB_RV JUB_CreateContextTRX(IN CONTEXT_CONFIG_TRX cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextTRX:(NSUInteger)deviceID
                               cfg:(ContextConfigTRX*)cfg;

//JUB_RV JUB_GetAddressTRX(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressTRX:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow;

//JUB_RV JUB_GetHDNodeTRX(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeTRX:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path;

//JUB_RV JUB_GetMainHDNodeTRX(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeTRX:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format;

//JUB_RV JUB_SetMyAddressTRX(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressTRX:(NSUInteger)contextID
                            path:(BIP32Path*)path;

//JUB_RV JUB_SignTransactionTRX(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_CHAR_PTR packedContractInPb,
//                              OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionTRX:(NSUInteger)contextID
                               path:(BIP32Path*)path
                 packedContractInPb:(NSString*)packedContractInPb;

//JUB_RV JUB_SetTRC10Asset(IN JUB_UINT16 contextID,
//                         IN JUB_CHAR_CPTR assetName,
//                         IN JUB_UINT16 unitDP,
//                         IN JUB_CHAR_CPTR assetID);
- (void)JUB_SetTRC10Asset:(NSUInteger)contextID
                assetName:(NSString*)assetName
                   unitDP:(NSUInteger)unitDP
                  assetID:(NSString*)assetID;


//JUB_RV JUB_SetTRC20Token(IN JUB_UINT16 contextID,
//                         IN JUB_CHAR_CPTR tokenName,
//                         IN JUB_UINT16 unitDP,
//                         IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetTRC20Token:(NSUInteger)contextID
                tokenName:(NSString*)tokenName
                   unitDP:(NSUInteger)unitDP
          contractAddress:(NSString*)contractAddress;

//JUB_RV JUB_BuildTRC20TransferAbi(IN JUB_UINT16 contextID,
//                                 IN JUB_CHAR_CPTR tokenTo, IN JUB_CHAR_CPTR tokenValue,
//                                 OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildTRC20TransferAbi:(NSUInteger)contextID
                               tokenTo:(NSString*)tokenTo
                            tokenValue:(NSString*)tokenValue;

//JUB_RV JUB_PackContractTRX(IN JUB_UINT16 contextID,
//                           IN JUB_TX_TRX tx,
//                           OUT JUB_CHAR_PTR_PTR packedContractInPB);
- (NSString*)JUB_PackContractTRX:(NSUInteger)contextID
                              tx:(TxTRX*)tx;

@end

NS_ASSUME_NONNULL_END
