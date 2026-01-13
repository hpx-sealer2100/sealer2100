//
//  JubSDKCore+COIN_TRX.mm
//  JubSDKCore
//
//  Created by Pan Min on 2020/12/13.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#import "JubSDKCore+COIN_TRX.h"
#import "JUB_SDK_TRX.h"

//typedef struct stTronKey {
//    JUB_CHAR_PTR address;
//    JUB_INT64 weight;
//} JUB_KEY_TRX;
@implementation TRXKey
@synthesize address;
@synthesize weight;
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
@implementation TRXPermission
@synthesize type;
@synthesize identity;
@synthesize permissionName;
@synthesize threshold;
@synthesize operations;
@synthesize keys;
@end

//typedef struct stTronAccountPermissionUpdateContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_PERMISSION_TRX owner;
//    JUB_PERMISSION_TRX witness;
//    JUB_PERMISSION_TRX actives[5];
//    JUB_UINT16 activeCount;
//} JUB_ACCT_PERM_UPDATE_CONTRACT_TRX;
@implementation TRXAccountPermissionUpdateContract
@synthesize ownerAddress;
@synthesize owner;
@synthesize witness;
@synthesize actives;
@end

//typedef struct stTronTransferContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR    to_address;
//    JUB_INT64 amount;
//} JUB_XFER_CONTRACT_TRX;
@implementation TRXTransferContract
@synthesize ownerAddress;
@synthesize toAddress;
@synthesize amount;
@end

//typedef struct stTronTransferAssetContract {
//    JUB_CHAR_PTR asset_name;
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR    to_address;
//    JUB_INT64       amount;
//} JUB_XFER_ASSET_CONTRACT_TRX;
@implementation TRXTransferAssetContract
@synthesize assetName;
@synthesize ownerAddress;
@synthesize toAddress;
@synthesize amount;
@end

//typedef struct stTronCreateSmartContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR bytecode;
//    JUB_INT64 call_token_value;
//    JUB_INT64 token_id;
//} JUB_CREATE_SMART_CONTRACT_TRX;
@implementation TRXCreateSmartContract
@synthesize ownerAddress;
@synthesize bytecode;
@synthesize callTokenValue;
@synthesize tokenId;
@end

//typedef struct stTronTriggerSmartContract {
//    JUB_CHAR_PTR owner_address;
//    JUB_CHAR_PTR contract_address;
//    JUB_INT64 call_value;
//    JUB_CHAR_PTR data;
//    JUB_INT64 call_token_value;
//    JUB_INT64 token_id;
//} JUB_TRIG_SMART_CONTRACT_TRX;
@implementation TRXTriggerSmartContract
@synthesize ownerAddress;
@synthesize contractAddress;
@synthesize callValue;
@synthesize data;
@synthesize callTokenValue;
@synthesize tokenId;
@end

//typedef struct stTronContract {
//    JUB_ENUM_TRX_CONTRACT_TYPE type;
//    union {
//              JUB_XFER_CONTRACT_TRX transfer;
//        JUB_XFER_ASSET_CONTRACT_TRX transferAsset;
//      JUB_CREATE_SMART_CONTRACT_TRX createSmart;
//        JUB_TRIG_SMART_CONTRACT_TRX triggerSmart;
//    };
//    JUB_UINT32 permissionId;   // [Optional]
//} JUB_CONTRACT_TRX;
@implementation TRXContract
@synthesize type;
@synthesize permissionId;
@synthesize xfer;
@synthesize assetXfer;
@synthesize triggerSmart;
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
@implementation TxTRX
@synthesize refBlockBytes;
@synthesize refBlockNum;
@synthesize refBlockHash;
@synthesize expiration;
@synthesize data;
@synthesize contracts;
@synthesize scripts;
@synthesize timestamp;
@synthesize feeLimit;
@end

extern bool (^inlinebool)(JUB_NS_ENUM_BOOL);
extern JUB_NS_ENUM_BOOL (^inlineNSbool)(bool);
extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT);


// JUB_NS_TRX_CONTR_TYPE -> JUB_ENUM_TRX_CONTRACT_TYPE
JUB_ENUM_TRX_CONTRACT_TYPE (^inlineTRXContractType)(JUB_NS_TRX_CONTR_TYPE) = ^(JUB_NS_TRX_CONTR_TYPE argument) {
    
    JUB_ENUM_TRX_CONTRACT_TYPE u;
    switch (argument) {
        case NS_XFER_CONTRACT:
            u = JUB_ENUM_TRX_CONTRACT_TYPE::XFER_CONTRACT;
            break;
        case NS_XFER_ASSET_CONTRACT:
            u = JUB_ENUM_TRX_CONTRACT_TYPE::XFER_ASSET_CONTRACT;
            break;
        case NS_CREATE_SMART_CONTRACT:
            u = JUB_ENUM_TRX_CONTRACT_TYPE::CREATE_SMART_CONTRACT;
            break;
        case NS_TRIG_SMART_CONTRACT:
            u = JUB_ENUM_TRX_CONTRACT_TYPE::TRIG_SMART_CONTRACT;
            break;
        case NS_ACCT_PERM_UPDATE_CONTRACT:
            u = JUB_ENUM_TRX_CONTRACT_TYPE::ACCT_PERM_UPDATE_CONTRACT;
            break;
        default:
            u = JUB_ENUM_TRX_CONTRACT_TYPE::NS_ITEM_TRX_CONTRACT;
            break;
    }
    
    return u;
};


// JUB_NS_TRX_CONTR_TYPE <- JUB_ENUM_TRX_CONTRACT_TYPE
JUB_NS_TRX_CONTR_TYPE (^inlineNSTRXContractType)(JUB_ENUM_TRX_CONTRACT_TYPE) = ^(JUB_ENUM_TRX_CONTRACT_TYPE argument) {
    
    JUB_NS_TRX_CONTR_TYPE u;
    switch (argument) {
        case JUB_ENUM_TRX_CONTRACT_TYPE::XFER_CONTRACT:
            u = JUB_NS_TRX_CONTR_TYPE::NS_XFER_CONTRACT;
            break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::XFER_ASSET_CONTRACT:
            u = JUB_NS_TRX_CONTR_TYPE::NS_XFER_ASSET_CONTRACT;
            break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::CREATE_SMART_CONTRACT:
            u = JUB_NS_TRX_CONTR_TYPE::NS_CREATE_SMART_CONTRACT;
            break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::TRIG_SMART_CONTRACT:
            u = JUB_NS_TRX_CONTR_TYPE::NS_TRIG_SMART_CONTRACT;
            break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::ACCT_PERM_UPDATE_CONTRACT:
            u = JUB_NS_TRX_CONTR_TYPE::NS_ACCT_PERM_UPDATE_CONTRACT;
            break;
        default:
            u = JUB_NS_TRX_CONTR_TYPE::NS_TX_TRX_TYPE_NS;
            break;
    }
    
    return u;
};


// TRXTransferContract* -> JUB_XFER_CONTRACT_TRX
void (^inlineTransferContract)(TRXTransferContract*, JUB_XFER_CONTRACT_TRX&) = ^(TRXTransferContract* transferContract, JUB_XFER_CONTRACT_TRX& xfer) {
    
    if (nil != transferContract.ownerAddress) {
        xfer.owner_address = (JUB_CHAR_PTR)[transferContract.ownerAddress UTF8String];
    }
    
    if (nil != transferContract.toAddress) {
        xfer.to_address    = (JUB_CHAR_PTR)[transferContract.toAddress UTF8String];
    }
    
    xfer.amount = transferContract.amount;
};


// TRXTransferContract* <- JUB_XFER_CONTRACT_TRX
TRXTransferContract* (^inlineNSTransferContract)(JUB_XFER_CONTRACT_TRX&) = ^(JUB_XFER_CONTRACT_TRX& xfer) {
    
    TRXTransferContract* transferContract = [[TRXTransferContract alloc] init];
    
    if (nullptr != xfer.owner_address) {
        transferContract.ownerAddress = [[NSString alloc] init];
        transferContract.ownerAddress = [[NSString stringWithCString:xfer.owner_address
                                                            encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xfer.owner_address);
    }
    
    if (nullptr != xfer.to_address) {
        transferContract.toAddress = [[NSString alloc] init];
        transferContract.toAddress = [[NSString stringWithCString:xfer.to_address
                                                         encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xfer.to_address);
    }
    
    transferContract.amount = xfer.amount;
    
    return transferContract;
};


// TRXTransferAssetContract* -> JUB_XFER_ASSET_CONTRACT_TRX
void (^inlineTransferAssetContract)(TRXTransferAssetContract*, JUB_XFER_ASSET_CONTRACT_TRX&) = ^(TRXTransferAssetContract* assetTransferContract, JUB_XFER_ASSET_CONTRACT_TRX& assetXfer) {
    
    if (nil != assetTransferContract.assetName) {
        assetXfer.asset_name = (JUB_CHAR_PTR)[assetTransferContract.assetName UTF8String];
    }
    
    if (nil != assetTransferContract.ownerAddress) {
        assetXfer.owner_address = (JUB_CHAR_PTR)[assetTransferContract.ownerAddress UTF8String];
    }
    
    if (nil != assetTransferContract.toAddress) {
        assetXfer.to_address    = (JUB_CHAR_PTR)[assetTransferContract.toAddress UTF8String];
    }
    
    assetXfer.amount = assetTransferContract.amount;
};


// TRXTransferAssetContract* <- JUB_XFER_ASSET_CONTRACT_TRX
TRXTransferAssetContract* (^inlineNSTransferAssetContract)(JUB_XFER_ASSET_CONTRACT_TRX&) = ^(JUB_XFER_ASSET_CONTRACT_TRX& assetXfer) {
    
    TRXTransferAssetContract* transferAssetContract = [[TRXTransferAssetContract alloc] init];
    
    if (nullptr != assetXfer.asset_name) {
        transferAssetContract.assetName = [[NSString alloc] init];
        transferAssetContract.assetName = [[NSString stringWithCString:assetXfer.asset_name
                                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(assetXfer.asset_name);
    }
    
    if (nullptr != assetXfer.owner_address) {
        transferAssetContract.ownerAddress = [[NSString alloc] init];
        transferAssetContract.ownerAddress = [[NSString stringWithCString:assetXfer.owner_address
                                                                 encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(assetXfer.owner_address);
    }
    
    if (nullptr != assetXfer.to_address) {
        transferAssetContract.toAddress = [[NSString alloc] init];
        transferAssetContract.toAddress = [[NSString stringWithCString:assetXfer.to_address
                                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(assetXfer.to_address);
    }
    
    transferAssetContract.amount = assetXfer.amount;
    
    return transferAssetContract;
};


// TRXCreateSmartContract* -> JUB_CREATE_SMART_CONTRACT_TRX
void (^inlineCreateSmartContract)(TRXCreateSmartContract*, JUB_CREATE_SMART_CONTRACT_TRX&) = ^(TRXCreateSmartContract* createSmartContract, JUB_CREATE_SMART_CONTRACT_TRX& createSmart) {
    
    if (nil != createSmartContract.ownerAddress) {
        createSmart.owner_address = (JUB_CHAR_PTR)[createSmartContract.ownerAddress UTF8String];
    }
    
    if (nil != createSmartContract.bytecode) {
        createSmart.bytecode = (JUB_CHAR_PTR)[createSmartContract.bytecode UTF8String];
    }
    
    createSmart.call_token_value = createSmartContract.callTokenValue;
    createSmart.token_id = createSmartContract.tokenId;
};


// TRXCreateSmartContract* <- JUB_CREATE_SMART_CONTRACT_TRX
TRXCreateSmartContract* (^inlineNSCreateSmartContract)(JUB_CREATE_SMART_CONTRACT_TRX&) = ^(JUB_CREATE_SMART_CONTRACT_TRX& createSmart) {
    
    TRXCreateSmartContract* createSmartContract = [[TRXCreateSmartContract alloc] init];
    
    if (nullptr != createSmart.owner_address) {
        createSmartContract.ownerAddress = [[NSString alloc] init];
        createSmartContract.ownerAddress = [[NSString stringWithCString:createSmart.owner_address
                                                               encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(createSmart.owner_address);
    }
    
    if (nullptr != createSmart.bytecode) {
        createSmartContract.bytecode = [[NSString alloc] init];
        createSmartContract.bytecode = [[NSString stringWithCString:createSmart.bytecode
                                                           encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(createSmart.bytecode);
    }
    
    createSmartContract.callTokenValue = createSmart.call_token_value;
    createSmartContract.tokenId = createSmart.token_id;
    
    return createSmartContract;
};


// TRXTriggerSmartContract* -> JUB_TRIG_SMART_CONTRACT_TRX
void (^inlineTriggerSmartContract)(TRXTriggerSmartContract*, JUB_TRIG_SMART_CONTRACT_TRX&) = ^(TRXTriggerSmartContract* triggerSmartContract, JUB_TRIG_SMART_CONTRACT_TRX& trigSmart) {
    
    if (nil != triggerSmartContract.ownerAddress) {
        trigSmart.owner_address = (JUB_CHAR_PTR)[triggerSmartContract.ownerAddress UTF8String];
    }
    
    if (nil != triggerSmartContract.contractAddress) {
        trigSmart.contract_address = (JUB_CHAR_PTR)[triggerSmartContract.contractAddress UTF8String];
    }
    
    trigSmart.call_value = triggerSmartContract.callValue;
    
    if (nil != triggerSmartContract.data) {
        trigSmart.data = (JUB_CHAR_PTR)[triggerSmartContract.data UTF8String];
    }
    
    trigSmart.call_token_value = triggerSmartContract.callTokenValue;
    trigSmart.token_id = triggerSmartContract.tokenId;
};


// TRXTriggerSmartContract* <- JUB_TRIG_SMART_CONTRACT_TRX
TRXTriggerSmartContract* (^inlineNSTriggerSmartContract)(JUB_TRIG_SMART_CONTRACT_TRX&) = ^(JUB_TRIG_SMART_CONTRACT_TRX& trigSmart) {
    
    TRXTriggerSmartContract* triggerSmartContract = [[TRXTriggerSmartContract alloc] init];
    
    if (nullptr != trigSmart.owner_address) {
        triggerSmartContract.ownerAddress = [[NSString alloc] init];
        triggerSmartContract.ownerAddress = [[NSString stringWithCString:trigSmart.owner_address
                                                                encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trigSmart.owner_address);
    }
    
    if (nullptr != trigSmart.contract_address) {
        triggerSmartContract.contractAddress = [[NSString alloc] init];
        triggerSmartContract.contractAddress = [[NSString stringWithCString:trigSmart.contract_address
                                                                   encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trigSmart.contract_address);
    }
    
    triggerSmartContract.callTokenValue = trigSmart.call_token_value;
    
    if (nullptr != trigSmart.data) {
        triggerSmartContract.data = [[NSString alloc] init];
        triggerSmartContract.data = [[NSString stringWithCString:trigSmart.data
                                                        encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trigSmart.contract_address);
    }
    
    triggerSmartContract.tokenId = trigSmart.token_id;
    
    return triggerSmartContract;
};


// TRXKey* -> JUB_KEY_TRX
void (^inlineTRXKey)(TRXKey*, JUB_KEY_TRX&) = ^(TRXKey* key, JUB_KEY_TRX& k) {
    
    if (nil != key.address) {
        k.address = (JUB_CHAR_PTR)[key.address UTF8String];
    }
    
    k.weight = key.weight;
};


// TRXKey* <- JUB_KEY_TRX
TRXKey* (^inlineNSTRXKey)(JUB_KEY_TRX&) = ^(JUB_KEY_TRX& k) {
    
    TRXKey* key = [[TRXKey alloc] init];
    
    if (nullptr != key.address) {
        key.address = [[NSString alloc] init];
        key.address = [[NSString stringWithCString:k.address
                                          encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(k.address);
    }
    
    key.weight = k.weight;
    
    return key;
};


// TRXPermission* -> JUB_PERMISSION_TRX
void (^inlineTRXPermission)(TRXPermission*, JUB_PERMISSION_TRX&) = ^(TRXPermission* permission, JUB_PERMISSION_TRX& perm) {
    
    perm.type = permission.type;
    
    perm.identity = (JUB_UINT32)permission.identity;
    
    if (nil != permission.permissionName) {
        perm.permission_name = (JUB_CHAR_PTR)[permission.permissionName UTF8String];
    }
    
    perm.threshold = permission.threshold;
    
    if (nil != permission.operations) {
        perm.operations = (JUB_CHAR_PTR)[permission.operations UTF8String];
    }
    
    for (NSUInteger i=0; i<permission.keys.count; ++i) {
        inlineTRXKey(permission.keys[i], perm.keys[i]);
    }
    perm.keyCount = permission.keys.count;
};


// TRXPermission* <- JUB_PERMISSION_TRX
TRXPermission* (^inlineNSTRXPermission)(JUB_PERMISSION_TRX&) = ^(JUB_PERMISSION_TRX& perm) {
    
    TRXPermission* permission = [[TRXPermission alloc] init];
    
    permission.type = (PermissionType)perm.type;
    
    permission.identity = (JUB_UINT32)perm.identity;
    
    if (nullptr != perm.permission_name) {
        permission.permissionName = [[NSString alloc] init];
        permission.permissionName = [[NSString stringWithCString:perm.permission_name
                                                        encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(perm.permission_name);
    }
    
    permission.threshold = perm.threshold;
    
    if (nullptr != perm.operations) {
        permission.operations = [[NSString alloc] init];
        permission.operations = [[NSString stringWithCString:perm.operations
                                                    encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(perm.operations);
    }
    
    for (JUB_UINT16 i=0; i<perm.keyCount; ++i) {
        [permission.keys addObject:inlineNSTRXKey(perm.keys[i])];
    }
    
    return permission;
};


// TRXAccountPermissionUpdateContract* -> JUB_ACCT_PERM_UPDATE_CONTRACT_TRX
void (^inlineAccountPermissionUpdateContract)(TRXAccountPermissionUpdateContract*, JUB_ACCT_PERM_UPDATE_CONTRACT_TRX&) = ^(TRXAccountPermissionUpdateContract* acctPermUpdateContract, JUB_ACCT_PERM_UPDATE_CONTRACT_TRX& acctPermUpdate) {
    
    if (nil != acctPermUpdateContract.ownerAddress) {
        acctPermUpdate.owner_address = (JUB_CHAR_PTR)[acctPermUpdateContract.ownerAddress UTF8String];
    }
    
    if (nil != acctPermUpdateContract.owner) {
        inlineTRXPermission(acctPermUpdateContract.owner, acctPermUpdate.owner);
    }
    
    if (nil != acctPermUpdateContract.witness) {
        inlineTRXPermission(acctPermUpdateContract.witness, acctPermUpdate.witness);
    }
    
    for (JUB_UINT16 i=0; i<acctPermUpdateContract.actives.count; ++i) {
        inlineTRXPermission(acctPermUpdateContract.actives[i], acctPermUpdate.actives[i]);
    }
    acctPermUpdate.activeCount = acctPermUpdateContract.actives.count;
};


// TRXAccountPermissionUpdateContract* <- JUB_ACCT_PERM_UPDATE_CONTRACT_TRX
TRXAccountPermissionUpdateContract* (^inlineNSAccountPermissionUpdateContract)(JUB_ACCT_PERM_UPDATE_CONTRACT_TRX&) = ^(JUB_ACCT_PERM_UPDATE_CONTRACT_TRX& acctPermUpdate) {
    
    TRXAccountPermissionUpdateContract* acctPermUpdateContract = [[TRXAccountPermissionUpdateContract alloc] init];
    
    if (nullptr != acctPermUpdate.owner_address) {
        acctPermUpdateContract.ownerAddress = [[NSString alloc] init];
        acctPermUpdateContract.ownerAddress = [[NSString stringWithCString:acctPermUpdate.owner_address
                                                                  encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(acctPermUpdate.owner_address);
    }
    
    if (nullptr != acctPermUpdate.owner.permission_name) {
        acctPermUpdateContract.owner = inlineNSTRXPermission(acctPermUpdate.owner);
    }
    
    if (nullptr != acctPermUpdate.witness.permission_name) {
        acctPermUpdateContract.witness = inlineNSTRXPermission(acctPermUpdate.witness);
    }
    
    if (acctPermUpdate.activeCount > 0) {
        acctPermUpdateContract.actives = [[NSMutableArray alloc] init];
    }
    for (JUB_UINT16 i=0; i<acctPermUpdate.activeCount; ++i) {
        [acctPermUpdateContract.actives addObject:inlineNSTRXPermission(acctPermUpdate.actives[i])];
    }
    
    return acctPermUpdateContract;
};


// TRXContract* -> JUB_CONTRACT_TRX
void (^inlineContractTRX)(TRXContract*, JUB_CONTRACT_TRX&) = ^(TRXContract* contractTRX, JUB_CONTRACT_TRX& contract) {
    
    contract.type = inlineTRXContractType(contractTRX.type);
    contract.permissionId = (JUB_INT32)contractTRX.permissionId;
    switch (contractTRX.type) {
        case JUB_NS_TRX_CONTR_TYPE::NS_XFER_CONTRACT:
            inlineTransferContract(contractTRX.xfer, contract.transfer);
            break;
        case JUB_NS_TRX_CONTR_TYPE::NS_XFER_ASSET_CONTRACT:
            inlineTransferAssetContract(contractTRX.assetXfer, contract.transferAsset);
            break;
        case JUB_NS_TRX_CONTR_TYPE::NS_TRIG_SMART_CONTRACT:
            inlineTriggerSmartContract(contractTRX.triggerSmart, contract.triggerSmart);
            break;
        case JUB_NS_TRX_CONTR_TYPE::NS_ACCT_PERM_UPDATE_CONTRACT:
            inlineAccountPermissionUpdateContract(contractTRX.acctPermUpdate, contract.acctPermUpdate);
            break;
        case JUB_NS_TRX_CONTR_TYPE::NS_CREATE_SMART_CONTRACT:
        case JUB_NS_TRX_CONTR_TYPE::NS_TX_TRX_TYPE_NS:
        default:
            break;
    }
};


// TRXContract* <- JUB_CONTRACT_TRX
TRXContract* (^inlineNSContractTRX)(JUB_CONTRACT_TRX&) = ^(JUB_CONTRACT_TRX& contract) {
    
    TRXContract* contractTRX = [[TRXContract alloc] init];
    
    contractTRX.type      = inlineNSTRXContractType(contract.type);
    contractTRX.permissionId = contract.permissionId;
    
    contractTRX.xfer      = inlineNSTransferContract(contract.transfer);
    contractTRX.assetXfer = inlineNSTransferAssetContract(contract.transferAsset);
    contractTRX.triggerSmart = inlineNSTriggerSmartContract(contract.triggerSmart);
    contractTRX.acctPermUpdate = inlineNSAccountPermissionUpdateContract(contract.acctPermUpdate);
    
    return contractTRX;
};


// TxTRX* -> JUB_TX_TRX
void (^inlineTxTRX)(TxTRX*, JUB_TX_TRX&) = ^(TxTRX* trxTx, JUB_TX_TRX& tx) {
    
    if (nil != trxTx.refBlockBytes) {
        tx.ref_block_bytes = (JUB_CHAR_PTR)[trxTx.refBlockBytes UTF8String];
    }
    
    if (nil != trxTx.refBlockNum) {
        tx.ref_block_num = (JUB_CHAR_PTR)[trxTx.refBlockNum UTF8String];
    }
    
    if (nil != trxTx.refBlockHash) {
        tx.ref_block_hash = (JUB_CHAR_PTR)[trxTx.refBlockHash UTF8String];
    }
    
    if (nil != trxTx.expiration) {
        tx.expiration = (JUB_CHAR_PTR)[trxTx.expiration UTF8String];
    }
    
    if (nil != trxTx.data) {
        tx.data = (JUB_CHAR_PTR)[trxTx.data UTF8String];
    }
    
    tx.contractCount = [trxTx.contracts count];
    
    if (0 < tx.contractCount) {
        tx.contracts = new JUB_CONTRACT_TRX[tx.contractCount];
        for (NSUInteger i=0; i<[trxTx.contracts count]; ++i) {
            inlineContractTRX([trxTx.contracts objectAtIndex:i], tx.contracts[i]);
        }
    }
    
    if (nil != trxTx.scripts) {
        tx.scripts = (JUB_CHAR_PTR)[trxTx.scripts UTF8String];
    }
    
    if (nil != trxTx.timestamp) {
        tx.timestamp = (JUB_CHAR_PTR)[trxTx.timestamp UTF8String];
    }
    
    if (nil != trxTx.feeLimit) {
        tx.fee_limit = (JUB_CHAR_PTR)[trxTx.feeLimit UTF8String];
    }
};


//// TxTRX* <- JUB_TX_TRX
TxTRX* (^inlineNSTxTRX)(JUB_TX_TRX) = ^(JUB_TX_TRX trxTx) {
    
    TxTRX* tx = [[TxTRX alloc] init];
    
    if(nullptr != trxTx.ref_block_bytes) {
        tx.refBlockBytes = [[NSString alloc] init];
        tx.refBlockBytes = [[NSString stringWithCString:trxTx.ref_block_bytes
                                               encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.ref_block_bytes);
    }
    
    if(nullptr != trxTx.ref_block_num) {
        tx.refBlockNum = [[NSString alloc] init];
        tx.refBlockNum = [[NSString stringWithCString:trxTx.ref_block_num
                                             encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.ref_block_num);
    }
    
    if(nullptr != trxTx.ref_block_hash) {
        tx.refBlockHash = [[NSString alloc] init];
        tx.refBlockHash = [[NSString stringWithCString:trxTx.ref_block_hash
                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.ref_block_hash);
    }
    
    if(nullptr != trxTx.expiration) {
        tx.expiration = [[NSString alloc] init];
        tx.expiration = [[NSString stringWithCString:trxTx.expiration
                                            encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.expiration);
    }
    
    if(nullptr != trxTx.data) {
        tx.data = [[NSString alloc] init];
        tx.data = [[NSString stringWithCString:trxTx.data
                                      encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.data);
    }
    
    tx.contracts = [[NSMutableArray alloc] initWithCapacity:0];
    for (JUB_UINT16 i=0; i<trxTx.contractCount; ++i) {
        [tx.contracts addObject:inlineNSContractTRX(trxTx.contracts[i])];
    }
    
    if(nullptr != trxTx.scripts) {
        tx.scripts = [[NSString alloc] init];
        tx.scripts = [[NSString stringWithCString:trxTx.scripts
                                         encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.scripts);
    }
    
    if(nullptr != trxTx.timestamp) {
        tx.timestamp = [[NSString alloc] init];
        tx.timestamp = [[NSString stringWithCString:trxTx.timestamp
                                           encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.timestamp);
    }
    
    if(nullptr != trxTx.fee_limit) {
        tx.feeLimit = [[NSString alloc] init];
        tx.feeLimit = [[NSString stringWithCString:trxTx.fee_limit
                                          encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(trxTx.fee_limit);
    }
    
    return tx;
};


@implementation JubSDKCore (COIN_TRX)


//JUB_RV JUB_CreateContextTRX(IN CONTEXT_CONFIG_TRX cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextTRX:(NSUInteger)deviceID
                               cfg:(ContextConfigTRX*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_XRP cfgTRX;
    if (nil != cfg.mainPath) {
        cfgTRX.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextTRX(cfgTRX,
                                     deviceID,
                                     &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}


//JUB_RV JUB_GetAddressTRX(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressTRX:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_GetAddressTRX(contextID,
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


//JUB_RV JUB_GetHDNodeTRX(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeTRX:(NSUInteger)contextID
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
    JUB_RV rv = JUB_GetHDNodeTRX(contextID,
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


//JUB_RV JUB_GetMainHDNodeTRX(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeTRX:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetMainHDNodeTRX(contextID,
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


//JUB_RV JUB_SetMyAddressTRX(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressTRX:(NSUInteger)contextID
                            path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_SetMyAddressTRX(contextID,
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


//JUB_RV JUB_SignTransactionTRX(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_CHAR_PTR packedContractInPb,
//                              OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionTRX:(NSUInteger)contextID
                               path:(BIP32Path*)path
                 packedContractInPb:(NSString*)packedContractInPb
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTransactionTRX(contextID,
                                       bip32Path,
                                       (JUB_CHAR_PTR)[packedContractInPb UTF8String],
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


//JUB_RV JUB_SetTRC10Asset(IN JUB_UINT16 contextID,
//                         IN JUB_CHAR_CPTR assetName,
//                         IN JUB_UINT16 unitDP,
//                         IN JUB_CHAR_CPTR assetID);
- (void)JUB_SetTRC10Asset:(NSUInteger)contextID
                assetName:(NSString*)assetName
                   unitDP:(NSUInteger)unitDP
                  assetID:(NSString*)assetID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pAssetName = nullptr;
    if (nil != assetName) {
        pAssetName = (JUB_CHAR_PTR)[assetName UTF8String];
    }
    JUB_CHAR_PTR pAssetID = nullptr;
    if (nil != assetID) {
        pAssetID = (JUB_CHAR_PTR)[assetID UTF8String];
    }
    
    JUB_RV rv = JUB_SetTRC10Asset(contextID,
                                  pAssetName,
                                  unitDP,
                                  pAssetID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
}


//JUB_RV JUB_SetTRC20Token(IN JUB_UINT16 contextID,
//                         IN JUB_CHAR_CPTR tokenName,
//                         IN JUB_UINT16 unitDP,
//                         IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetTRC20Token:(NSUInteger)contextID
                tokenName:(NSString*)tokenName
                   unitDP:(NSUInteger)unitDP
          contractAddress:(NSString*)contractAddress
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pTokenName = nullptr;
    if (nil != tokenName) {
        pTokenName = (JUB_CHAR_PTR)[tokenName UTF8String];
    }
    JUB_CHAR_PTR pContractAddress = nullptr;
    if (nil != contractAddress) {
        pContractAddress = (JUB_CHAR_PTR)[contractAddress UTF8String];
    }
    
    JUB_RV rv = JUB_SetTRC20Token(contextID,
                                  pTokenName,
                                  unitDP,
                                  pContractAddress);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
}


//JUB_RV JUB_BuildTRC20TransferAbi(IN JUB_UINT16 contextID,
//                                 IN JUB_CHAR_CPTR tokenTo, IN JUB_CHAR_CPTR tokenValue,
//                                 OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildTRC20TransferAbi:(NSUInteger)contextID
                               tokenTo:(NSString*)tokenTo
                            tokenValue:(NSString*)tokenValue
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pTokenTo = nullptr;
    if (nil != tokenTo) {
        pTokenTo = (JUB_CHAR_PTR)[tokenTo UTF8String];
    }
    JUB_CHAR_PTR pTokenValue = nullptr;
    if (nil != tokenValue) {
        pTokenValue = (JUB_CHAR_PTR)[tokenValue UTF8String];
    }
    
    JUB_CHAR_PTR abi = nullptr;
    JUB_RV rv = JUB_BuildTRC20TransferAbi(contextID,
                                          pTokenTo,
                                          pTokenValue,
                                          &abi);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAbi = [NSString stringWithCString:abi
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(abi);
    
    return strAbi;
}


//JUB_RV JUB_PackContractTRX(IN JUB_UINT16 contextID,
//                           IN JUB_TX_TRX tx,
//                           OUT JUB_CHAR_PTR_PTR packedContractInPB);
- (NSString*)JUB_PackContractTRX:(NSUInteger)contextID
                              tx:(TxTRX*)tx
{
    self.lastError = JUBR_OK;
    
    JUB_TX_TRX trxTx;
    inlineTxTRX(tx, trxTx);
    
    JUB_CHAR_PTR packedContractInPb = nullptr;
    JUB_RV rv = JUB_PackContractTRX(contextID,
                                    trxTx,
                                    &packedContractInPb);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strPackedContractInPb = [NSString stringWithCString:packedContractInPb
                                                         encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(packedContractInPb);
    
    return strPackedContractInPb;
}

@end
