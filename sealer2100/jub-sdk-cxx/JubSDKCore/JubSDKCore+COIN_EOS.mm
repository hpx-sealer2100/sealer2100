//
//  JubSDKCore+COIN_EOS.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/10/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+COIN_EOS.h"
#import "JUB_SDK_EOS.h"

//typedef struct stTransferAction {
//    JUB_CHAR_PTR from;
//    JUB_CHAR_PTR to;
//    JUB_CHAR_PTR asset;
//    JUB_CHAR_PTR memo;
//} JUB_ACTION_TRANSFER;
@implementation XferAction
@synthesize from;
@synthesize to;
@synthesize asset;
@synthesize memo;
@end

//typedef struct stDelegateAction {
//    JUB_CHAR_PTR from;
//    JUB_CHAR_PTR receiver;
//    JUB_CHAR_PTR netQty; // stake_net_quantity/unstake_net_quantity
//    JUB_CHAR_PTR cpuQty; // stake_cpu_quantity/unstake_cpu_quantity
//    bool bStake;
//} JUB_ACTION_DELEGATE;
@implementation DeleAction
@synthesize from;
@synthesize receiver;
@synthesize netQty;
@synthesize cpuQty;
@synthesize bStake;
@end

//typedef struct stBuyRamAction {
//    JUB_CHAR_PTR payer;
//    JUB_CHAR_PTR quant;
//    JUB_CHAR_PTR receiver;
//} JUB_ACTION_BUYRAM;
@implementation BuyRamAction
@synthesize payer;
@synthesize quant;
@synthesize receiver;
@end

//typedef struct stSellRamAction {
//    JUB_CHAR_PTR account;
//    JUB_CHAR_PTR bytes;
//} JUB_ACTION_SELLRAM;
@implementation SellRamAction
@synthesize account;
@synthesize bytes;
@end

//typedef struct stActionEOS {
//    JUB_ENUM_EOS_ACTION_TYPE type;
//    JUB_CHAR_PTR currency;
//    JUB_CHAR_PTR name;
//    union {
//        JUB_ACTION_TRANSFER transfer;
//        JUB_ACTION_DELEGATE delegate;
//        JUB_ACTION_BUYRAM buyRam;
//        JUB_ACTION_SELLRAM sellRam;
//    };
//} JUB_ACTION_EOS;
@implementation ActionEOS
@synthesize type;
@synthesize currency;
@synthesize name;
@synthesize xfer;
@synthesize dele;
@synthesize buyRam;
@synthesize sellRam;
@end

extern bool (^inlinebool)(JUB_NS_ENUM_BOOL);
extern JUB_NS_ENUM_BOOL (^inlineNSbool)(bool);

// JUB_NS_EOS_ACTION_TYPE -> JUB_ENUM_EOS_ACTION_TYPE
JUB_ENUM_EOS_ACTION_TYPE (^inlineEOSActionType)(JUB_NS_EOS_ACTION_TYPE) = ^(JUB_NS_EOS_ACTION_TYPE argument) {
    JUB_ENUM_EOS_ACTION_TYPE u;
    switch (argument) {
        case NS_XFER:
            u = JUB_ENUM_EOS_ACTION_TYPE::XFER;
            break;
        case NS_DELE:
            u = JUB_ENUM_EOS_ACTION_TYPE::DELE;
            break;
        case NS_UNDELE:
            u = JUB_ENUM_EOS_ACTION_TYPE::UNDELE;
            break;
        case NS_BUYRAM:
            u = JUB_ENUM_EOS_ACTION_TYPE::BUYRAM;
            break;
        case NS_SELLRAM:
            u = JUB_ENUM_EOS_ACTION_TYPE::SELLRAM;
            break;
        default:
            u = JUB_ENUM_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE;
            break;
    }
    
    return u;
};

// JUB_NS_EOS_ACTION_TYPE <- JUB_ENUM_EOS_ACTION_TYPE
JUB_NS_EOS_ACTION_TYPE (^inlineNSEOSActionType)(JUB_ENUM_EOS_ACTION_TYPE) = ^(JUB_ENUM_EOS_ACTION_TYPE argument) {
    JUB_NS_EOS_ACTION_TYPE u;
    switch (argument) {
        case JUB_ENUM_EOS_ACTION_TYPE::XFER:
            u = NS_XFER;
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::DELE:
            u = NS_DELE;
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::UNDELE:
            u = NS_UNDELE;
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::BUYRAM:
            u = NS_BUYRAM;
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::SELLRAM:
            u = NS_SELLRAM;
            break;
        default:
            u = NS_ITEM_ACTION_TYPE_NS;
            break;
    }
    
    return u;
};

// XferAction* -> JUB_ACTION_TRANSFER
void (^inlineXferAction)(XferAction*, JUB_ACTION_TRANSFER&) = ^(XferAction* xferAction, JUB_ACTION_TRANSFER& xfer) {
    if (nil != xferAction.from) {
        xfer.from = (JUB_CHAR_PTR)[xferAction.from UTF8String];
    }
    if (nil != xferAction.to) {
        xfer.to = (JUB_CHAR_PTR)[xferAction.to UTF8String];
    }
    if (nil != xferAction.asset) {
        xfer.asset = (JUB_CHAR_PTR)[xferAction.asset UTF8String];
    }
    if (nil != xferAction.memo) {
        xfer.memo = (JUB_CHAR_PTR)[xferAction.memo UTF8String];
    }
};

// XferAction* <- JUB_ACTION_TRANSFER
XferAction* (^inlineNSXferAction)(JUB_ACTION_TRANSFER&) = ^(JUB_ACTION_TRANSFER& xferAction) {
    XferAction* xfer = [[XferAction alloc] init];
    
    if (nullptr != xferAction.from) {
        xfer.from = [[NSString alloc] init];
        xfer.from = [[NSString stringWithCString:xferAction.from
                                        encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xferAction.from);
    }
    
    if (nullptr != xferAction.to) {
        xfer.to = [[NSString alloc] init];
        xfer.to = [[NSString stringWithCString:xferAction.to
                                      encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xferAction.to);
    }
    
    if (nullptr != xferAction.asset) {
        xfer.asset = [[NSString alloc] init];
        xfer.asset = [[NSString stringWithCString:xferAction.asset
                                         encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xferAction.asset);
    }
    
    if (nullptr != xferAction.memo) {
        xfer.memo = [[NSString alloc] init];
        xfer.memo = [[NSString stringWithCString:xferAction.memo
                                        encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(xferAction.memo);
    }
    
    return xfer;
};

// DeleAction* -> JUB_ACTION_DELEGATE
void (^inlineDeleAction)(DeleAction*, JUB_ACTION_DELEGATE&) = ^(DeleAction* deleAction, JUB_ACTION_DELEGATE& dele) {
    if (nil != deleAction.from) {
        dele.from = (JUB_CHAR_PTR)[deleAction.from UTF8String];
    }
    if (nil != deleAction.receiver) {
        dele.receiver = (JUB_CHAR_PTR)[deleAction.receiver UTF8String];
    }
    if (nil != deleAction.netQty) {
        dele.netQty = (JUB_CHAR_PTR)[deleAction.netQty UTF8String];
    }
    if (nil != deleAction.cpuQty) {
        dele.cpuQty = (JUB_CHAR_PTR)[deleAction.cpuQty UTF8String];
    }
    
    dele.bStake = inlinebool(deleAction.bStake);
};

// DeleAction* <- JUB_ACTION_DELEGATE
DeleAction* (^inlineNSDeleAction)(JUB_ACTION_DELEGATE&) = ^(JUB_ACTION_DELEGATE& deleAction) {
    DeleAction* dele = [[DeleAction alloc] init];
    
    if (nullptr != deleAction.from) {
        dele.from = [[NSString alloc] init];
        dele.from = [[NSString stringWithCString:deleAction.from
                                        encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(deleAction.from);
    }
    
    if (nullptr != deleAction.receiver) {
        dele.receiver = [[NSString alloc] init];
        dele.receiver = [[NSString stringWithCString:deleAction.receiver
                                      encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(deleAction.receiver);
    }
    
    if (nullptr != deleAction.netQty) {
        dele.netQty = [[NSString alloc] init];
        dele.netQty = [[NSString stringWithCString:deleAction.netQty
                                          encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(deleAction.netQty);
    }
    
    if (nullptr != deleAction.cpuQty) {
        dele.cpuQty = [[NSString alloc] init];
        dele.cpuQty = [[NSString stringWithCString:deleAction.cpuQty
                                          encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(deleAction.cpuQty);
    }
    
    dele.bStake = inlineNSbool(deleAction.bStake);
    
    return dele;
};

// BuyRamAction* -> JUB_ACTION_BUYRAM
void (^inlineBuyRamAction)(BuyRamAction*, JUB_ACTION_BUYRAM&) = ^(BuyRamAction* buyRamAction, JUB_ACTION_BUYRAM& buyRam) {
    if (nil != buyRamAction.payer) {
        buyRam.payer = (JUB_CHAR_PTR)[buyRamAction.payer UTF8String];
    }
    if (nil != buyRamAction.quant) {
        buyRam.quant = (JUB_CHAR_PTR)[buyRamAction.quant UTF8String];
    }
    if (nil != buyRamAction.receiver) {
        buyRam.receiver = (JUB_CHAR_PTR)[buyRamAction.receiver UTF8String];
    }
};

// BuyRamAction* <- JUB_ACTION_BUYRAM
BuyRamAction* (^inlineNSBuyRamAction)(JUB_ACTION_BUYRAM&) = ^(JUB_ACTION_BUYRAM& buyRamAction) {
    BuyRamAction* buyRam = [[BuyRamAction alloc] init];
    
    if (nullptr != buyRamAction.payer) {
        buyRam.payer = [[NSString alloc] init];
        buyRam.payer = [[NSString stringWithCString:buyRamAction.payer
                                           encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(buyRamAction.payer);
    }
    
    if (nullptr != buyRamAction.quant) {
        buyRam.quant = [[NSString alloc] init];
        buyRam.quant = [[NSString stringWithCString:buyRamAction.quant
                                           encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(buyRamAction.quant);
    }
    
    if (nullptr != buyRamAction.receiver) {
        buyRam.receiver = [[NSString alloc] init];
        buyRam.receiver = [[NSString stringWithCString:buyRamAction.receiver
                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(buyRamAction.receiver);
    }
    
    return buyRam;
};

// SellRamAction* -> JUB_ACTION_BUYRAM
void (^inlineSellRamAction)(SellRamAction*, JUB_ACTION_SELLRAM&) = ^(SellRamAction* sellRamAction, JUB_ACTION_SELLRAM& sellRam) {
    if (nil != sellRamAction.account) {
        sellRam.account = (JUB_CHAR_PTR)[sellRamAction.account UTF8String];
    }
    if (nil != sellRamAction.bytes) {
        sellRam.bytes   = (JUB_CHAR_PTR)[sellRamAction.bytes UTF8String];
    }
};

// SellRamAction* <- JUB_ACTION_BUYRAM
SellRamAction* (^inlineNSSellRamAction)(JUB_ACTION_SELLRAM&) = ^(JUB_ACTION_SELLRAM& sellRamAction) {
    SellRamAction* sellRam = [[SellRamAction alloc] init];
    
    if (nullptr != sellRamAction.account) {
        sellRam.account = [[NSString alloc] init];
        sellRam.account = [[NSString stringWithCString:sellRamAction.account
                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(sellRamAction.account);
    }
    
    if (nullptr != sellRamAction.bytes) {
        sellRam.bytes = [[NSString alloc] init];
        sellRam.bytes = [[NSString stringWithCString:sellRamAction.bytes
                                            encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(sellRamAction.bytes);
    }
    
    return sellRam;
};

// ActionEOS* -> JUB_ACTION_EOS
void (^inlineActionEOS)(ActionEOS*, JUB_ACTION_EOS&) = ^(ActionEOS* actionEOS, JUB_ACTION_EOS& action) {
    action.type = inlineEOSActionType(actionEOS.type);
    if (nil != actionEOS.currency) {
        action.currency = (JUB_CHAR_PTR)[actionEOS.currency UTF8String];
    }
    if (nil != actionEOS.name) {
        action.name = (JUB_CHAR_PTR)[actionEOS.name UTF8String];
    }
    
    switch (actionEOS.type) {
        case JUB_NS_EOS_ACTION_TYPE::NS_XFER:
            inlineXferAction(actionEOS.xfer, action.transfer);
            break;
        case JUB_NS_EOS_ACTION_TYPE::NS_DELE:
        case JUB_NS_EOS_ACTION_TYPE::NS_UNDELE:
            inlineDeleAction(actionEOS.dele, action.delegate);
            break;
        case JUB_NS_EOS_ACTION_TYPE::NS_BUYRAM:
            inlineBuyRamAction(actionEOS.buyRam, action.buyRam);
            break;
        case JUB_NS_EOS_ACTION_TYPE::NS_SELLRAM:
            inlineSellRamAction(actionEOS.sellRam, action.sellRam);
            break;
        case JUB_NS_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE_NS:
        default:
            break;
    }
};

// ActionEOS* <- JUB_ACTION_EOS
ActionEOS* (^inlineNSActionEOS)(JUB_ACTION_EOS) = ^(JUB_ACTION_EOS actionEOS) {
    ActionEOS* action = [[ActionEOS alloc] init];
    
    action.type = inlineNSEOSActionType(actionEOS.type);
    
    if (nullptr != actionEOS.currency) {
        action.currency = [[NSString alloc] init];
        action.currency = [[NSString stringWithCString:actionEOS.currency
                                              encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(actionEOS.currency);
    }
    
    if (nullptr != actionEOS.name) {
        action.name = [[NSString alloc] init];
        action.name = [[NSString stringWithCString:actionEOS.name
                                          encoding:NSUTF8StringEncoding] copy];
        JUB_FreeMemory(actionEOS.name);
    }
    
    switch (actionEOS.type) {
        case JUB_ENUM_EOS_ACTION_TYPE::XFER:
            inlineNSXferAction(actionEOS.transfer);
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::DELE:
        case JUB_ENUM_EOS_ACTION_TYPE::UNDELE:
            inlineNSDeleAction(actionEOS.delegate);
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::BUYRAM:
            inlineNSBuyRamAction(actionEOS.buyRam);
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::SELLRAM:
            inlineNSSellRamAction(actionEOS.sellRam);
            break;
        case JUB_ENUM_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE:
        default:
            break;
    }
    
    return action;
};

extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);

@implementation JubSDKCore (COIN_EOS)

//JUB_RV JUB_CreateContextEOS(IN CONTEXT_CONFIG_EOS cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextEOS:(NSUInteger)deviceID
                               cfg:(ContextConfigEOS*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_EOS cfgEOS;
    if (nil != cfg.mainPath) {
        cfgEOS.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextEOS(cfgEOS,
                                     deviceID,
                                     &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}

//JUB_RV JUB_GetAddressEOS(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressEOS:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_GetAddressEOS(contextID,
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

//JUB_RV JUB_SetMyAddressEOS(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressEOS:(NSUInteger)contextID
                            path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_SetMyAddressEOS(contextID,
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

//JUB_RV JUB_SignTransactionEOS(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_CHAR_PTR chainID,
//                              IN JUB_CHAR_PTR expiration,
//                              IN JUB_CHAR_PTR referenceBlockId,
//                              IN JUB_CHAR_PTR referenceBlockTime,
//                              IN JUB_CHAR_PTR actionsInJSON,
//                              OUT JUB_CHAR_PTR_PTR rawInJSON);
- (NSString*)JUB_SignTransactionEOS:(NSUInteger)contextID
                               path:(BIP32Path*)path
                            chainID:(NSString*)chainID
                         expiration:(NSString*)expiration
                   referenceBlockId:(NSString*)referenceBlockId
                 referenceBlockTime:(NSString*)referenceBlockTime
                             action:(NSString*)actionsInJSON
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pChainID = nullptr;
    if (nil != chainID) {
        pChainID = (JUB_CHAR_PTR)[chainID UTF8String];
    }
    JUB_CHAR_PTR pExpiration = nullptr;
    if (nil != expiration) {
        pExpiration = (JUB_CHAR_PTR)[expiration UTF8String];
    }
    JUB_CHAR_PTR pReferenceBlockId = nullptr;
    if (nil != referenceBlockId) {
        pReferenceBlockId = (JUB_CHAR_PTR)[referenceBlockId UTF8String];
    }
    JUB_CHAR_PTR pReferenceBlockTime = nullptr;
    if (nil != referenceBlockTime) {
        pReferenceBlockTime = (JUB_CHAR_PTR)[referenceBlockTime UTF8String];
    }
    JUB_CHAR_PTR pActionsInJSON = nullptr;
    if (nil != actionsInJSON) {
        pActionsInJSON = (JUB_CHAR_PTR)[actionsInJSON UTF8String];
    }
    
    JUB_CHAR_PTR rawInJSON = nullptr;
    JUB_RV rv = JUB_SignTransactionEOS(contextID,
                                       bip32Path,
                                       pChainID,
                                       pExpiration,
                                       pReferenceBlockId,
                                       pReferenceBlockTime,
                                       pActionsInJSON,
                                       &rawInJSON);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strRawInJSON = [NSString stringWithCString:rawInJSON
                                                encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(rawInJSON);
    
    return strRawInJSON;
}

//JUB_RV JUB_BuildActionEOS(IN JUB_UINT16 contextID,
//                          IN JUB_ACTION_EOS_PTR actions,
//                          IN JUB_UINT16 actionCount,
//                          OUT JUB_CHAR_PTR_PTR actionsInJSON);
- (NSString*)JUB_BuildActionEOS:(NSUInteger)contextID
                         action:(NSMutableArray*)actionArray
{
    self.lastError = JUBR_OK;
    
    NSUInteger actionCnt = [actionArray count];
    JUB_ACTION_EOS_PTR actions = new JUB_ACTION_EOS[actionCnt+1];
    memset(actions, 0x00, actionCnt+1);
    
    for (NSUInteger i = 0; i < actionCnt; i++) {
        inlineActionEOS(actionArray[i], actions[i]);
    }
    JUB_CHAR_PTR actionsInJSON;
    JUB_RV rv = JUB_BuildActionEOS(contextID,
                                   actions,
                                   actionCnt,
                                   &actionsInJSON);
    free(actions); actions = NULL;
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return nil;
    }
    
    NSString* strActionInJSON = [NSString stringWithCString:actionsInJSON
                                                   encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(actionsInJSON);
    
    return strActionInJSON;
}

@end
