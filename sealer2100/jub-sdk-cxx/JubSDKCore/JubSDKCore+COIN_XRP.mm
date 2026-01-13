//
//  JubSDKCore+COIN_XRP.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/11/29.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+COIN_XRP.h"
#import "JUB_SDK_XRP.h"

//typedef struct stMemoXRP {
//    JUB_CHAR_PTR type;
//    JUB_CHAR_PTR data;
//    JUB_CHAR_PTR format;
//} JUB_XRP_MEMO;
@implementation XRPMemo
@synthesize type;
@synthesize data;
@synthesize format;
@end

//typedef struct stAmount {
//    JUB_CHAR_PTR currency;  // [Optional]
//    JUB_CHAR_PTR value;
//    JUB_CHAR_PTR issuer;    // [Optional]
//} JUB_PYMT_AMOUNT;
@implementation PymtAmount
@synthesize currency;
@synthesize value;
@synthesize issuer;
@end

//typedef struct stPaymentXRP {
//    JUB_ENUM_XRP_PYMT_TYPE type;
//    JUB_PYMT_AMOUNT amount;
//    JUB_CHAR_PTR destination;
//    JUB_CHAR_PTR destinationTag;
//    JUB_CHAR_PTR invoiceID;     // [Optional]
//    JUB_PYMT_AMOUNT sendMax;    // [Optional]
//    JUB_PYMT_AMOUNT deliverMin; // [Optional]
//} JUB_PYMT_XRP;
@implementation PaymentXRP
@synthesize type;
@synthesize amount;
@synthesize destination;
@synthesize destinationTag;
@synthesize invoiceID;
@synthesize sendMax;
@synthesize deliverMin;
@end

//typedef struct stTxXRP {
//    JUB_CHAR_PTR account;
//    JUB_ENUM_XRP_TX_TYPE type;
//    JUB_CHAR_PTR fee;
//    JUB_CHAR_PTR sequence;
//    JUB_CHAR_PTR accountTxnID;
//    JUB_CHAR_PTR flags;
//    JUB_CHAR_PTR lastLedgerSequence;
//    JUB_XRP_MEMO memo;        // [Optional]
//    JUB_CHAR_PTR sourceTag;
//    union {
//        JUB_PYMT_XRP pymt;
//    };
//} JUB_TX_XRP;
@implementation TxXRP
@synthesize account;
@synthesize type;
@synthesize fee;
@synthesize sequence;
@synthesize accountTxnID;
@synthesize flags;
@synthesize lastLedgerSequence;
@synthesize memo;
@synthesize sourceTag;
@synthesize pymt;
@end

extern bool (^inlinebool)(JUB_NS_ENUM_BOOL);
extern JUB_NS_ENUM_BOOL (^inlineNSbool)(bool);
extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT);

//// JUB_NS_XRP_TX_TYPE -> JUB_ENUM_XRP_TX_TYPE
//JUB_ENUM_XRP_TX_TYPE (^inlineXRPTxType)(JUB_NS_XRP_TX_TYPE) = ^(JUB_NS_XRP_TX_TYPE argument) {
//    JUB_ENUM_XRP_TX_TYPE u;
//    switch (argument) {
//        case NS_PYMT:
//            u = JUB_ENUM_XRP_TX_TYPE::PYMT;
//            break;
//        default:
//            u = JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE;
//            break;
//    }
//    
//    return u;
//};
//
//// JUB_NS_XRP_TX_TYPE <- JUB_ENUM_XRP_TX_TYPE
//JUB_NS_XRP_TX_TYPE (^inlineNSXRPTxType)(JUB_ENUM_XRP_TX_TYPE) = ^(JUB_ENUM_XRP_TX_TYPE argument) {
//    JUB_NS_XRP_TX_TYPE u;
//    switch (argument) {
//        case JUB_ENUM_XRP_TX_TYPE::PYMT:
//            u = NS_PYMT;
//            break;
//        default:
//            u = NS_TX_XRP_TYPE_NS;
//            break;
//    }
//    
//    return u;
//};
//
//// JUB_NS_XRP_PYMT_TYPE -> JUB_ENUM_XRP_PYMT_TYPE
//JUB_ENUM_XRP_PYMT_TYPE (^inlineXRPPymtType)(JUB_NS_XRP_PYMT_TYPE) = ^(JUB_NS_XRP_PYMT_TYPE argument) {
//    JUB_ENUM_XRP_PYMT_TYPE u;
//    switch (argument) {
//        case NS_DXRP:
//            u = JUB_ENUM_XRP_PYMT_TYPE::DXRP;
//            break;
//        default:
//            u = JUB_ENUM_XRP_PYMT_TYPE::NS_ITEM_PYMT_TYPE;
//            break;
//    }
//    
//    return u;
//};
//
//// JUB_NS_XRP_PYMT_TYPE <- JUB_ENUM_XRP_PYMT_TYPE
//JUB_NS_XRP_PYMT_TYPE (^inlineNSXRPPymtType)(JUB_ENUM_XRP_PYMT_TYPE) = ^(JUB_ENUM_XRP_PYMT_TYPE argument) {
//    JUB_NS_XRP_PYMT_TYPE u;
//    switch (argument) {
//        case JUB_ENUM_XRP_PYMT_TYPE::DXRP:
//            u = NS_DXRP;
//            break;
//        default:
//            u = NS_DXRP_XRP_TYPE_NS;
//            break;
//    }
//    
//    return u;
//};
//
//// XRPMemo* -> JUB_XRP_MEMO
//void (^inlineXRPMemo)(XRPMemo*, JUB_XRP_MEMO&) = ^(XRPMemo* xrpMemo, JUB_XRP_MEMO& memo) {
//    if (nil != xrpMemo.type) {
//        memo.type   = (JUB_CHAR_PTR)[xrpMemo.type UTF8String];
//    }
//    if (nil != xrpMemo.data) {
//        memo.data   = (JUB_CHAR_PTR)[xrpMemo.data UTF8String];
//    }
//    if (nil != xrpMemo.format) {
//        memo.format = (JUB_CHAR_PTR)[xrpMemo.format UTF8String];
//    }
//};
//
//// XRPMemo* <- JUB_XRP_MEMO
//XRPMemo* (^inlineNSXRPMemo)(JUB_XRP_MEMO&) = ^(JUB_XRP_MEMO& xrpMemo) {
//    XRPMemo* memo = [[XRPMemo alloc] init];
//    
//    if (nullptr != memo.type) {
//        memo.type = [[NSString alloc] init];
//        memo.type = [[NSString stringWithCString:xrpMemo.type
//                                        encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpMemo.type);
//    }
//    
//    if (nullptr != memo.data) {
//        memo.data = [[NSString alloc] init];
//        memo.data = [[NSString stringWithCString:xrpMemo.data
//                                        encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpMemo.data);
//    }
//    
//    if (nullptr != memo.format) {
//        memo.format = [[NSString alloc] init];
//        memo.format = [[NSString stringWithCString:xrpMemo.format
//                                          encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpMemo.format);
//    }
//    
//    return memo;
//};
//
//// PymtAmount* -> JUB_PYMT_AMOUNT
//void (^inlinePymtAmount)(PymtAmount*, JUB_PYMT_AMOUNT&) = ^(PymtAmount* pymtAmount, JUB_PYMT_AMOUNT& amount) {
//    if (nil != pymtAmount.currency) {
//        amount.currency = (JUB_CHAR_PTR)[pymtAmount.currency UTF8String];
//    }
//    if (nil != pymtAmount.value) {
//        amount.value    = (JUB_CHAR_PTR)[pymtAmount.value UTF8String];
//    }
//    if (nil != pymtAmount.issuer) {
//        amount.issuer   = (JUB_CHAR_PTR)[pymtAmount.issuer UTF8String];
//    }
//};
//
//// PymtAmount* <- JUB_PYMT_AMOUNT
//PymtAmount* (^inlineNSPymtAmount)(JUB_PYMT_AMOUNT&) = ^(JUB_PYMT_AMOUNT& pymtAmount) {
//    PymtAmount* amount = [[PymtAmount alloc] init];
//    
//    if (nullptr != pymtAmount.currency) {
//        amount.currency = [[NSString alloc] init];
//        amount.currency = [[NSString stringWithCString:pymtAmount.currency
//                                              encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(pymtAmount.currency);
//    }
//    
//    if (nullptr != pymtAmount.value) {
//        amount.value = [[NSString alloc] init];
//        amount.value = [[NSString stringWithCString:pymtAmount.value
//                                           encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(pymtAmount.value);
//    }
//    
//    if (nullptr != pymtAmount.issuer) {
//        amount.issuer = [[NSString alloc] init];
//        amount.issuer = [[NSString stringWithCString:pymtAmount.issuer
//                                            encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(pymtAmount.issuer);
//    }
//    
//    return amount;
//};
//
//// Payment* -> JUB_PYMT_XRP
//void (^inlinePayment)(PaymentXRP*, JUB_PYMT_XRP&) = ^(PaymentXRP* pymt, JUB_PYMT_XRP& payment) {
//    payment.type           = inlineXRPPymtType(pymt.type);
//    inlinePymtAmount(pymt.amount, payment.amount);
//    if (nil != pymt.destination) {
//        payment.destination    = (JUB_CHAR_PTR)[pymt.destination UTF8String];
//    }
//    if (nil != pymt.destinationTag) {
//        payment.destinationTag = (JUB_CHAR_PTR)[pymt.destinationTag UTF8String];
//    }
//    if (nil != pymt.invoiceID) {
//        payment.invoiceID      = (JUB_CHAR_PTR)[pymt.invoiceID UTF8String];
//    }
//    inlinePymtAmount(pymt.sendMax, payment.sendMax);
//    inlinePymtAmount(pymt.deliverMin, payment.deliverMin);
//};
//
//// Payment* <- JUB_PYMT_XRP
//PaymentXRP* (^inlineNSPayment)(JUB_PYMT_XRP&) = ^(JUB_PYMT_XRP& pymt) {
//    PaymentXRP* payment = [[PaymentXRP alloc] init];
//    
//    payment.type   = inlineNSXRPPymtType(pymt.type);
//    payment.amount = inlineNSPymtAmount(pymt.amount);
//    
//    if (nullptr != pymt.destination) {
//        payment.destination = [[NSString alloc] init];
//        payment.destination = [[NSString stringWithCString:pymt.destination
//                                                  encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(pymt.destination);
//    }
//    
//    if (nullptr != pymt.destinationTag) {
//        payment.destinationTag = [[NSString alloc] init];
//        payment.destinationTag = [[NSString stringWithCString:pymt.destinationTag
//                                                     encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(pymt.destinationTag);
//    }
//    
//    if (nullptr != pymt.invoiceID) {
//        payment.invoiceID = [[NSString alloc] init];
//        payment.invoiceID = [[NSString stringWithCString:pymt.invoiceID
//                                                encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(pymt.invoiceID);
//    }
//    
//    payment.sendMax    = inlineNSPymtAmount(pymt.sendMax);
//    payment.deliverMin = inlineNSPymtAmount(pymt.deliverMin);
//    
//    return payment;
//};
//
//// TxXRP* -> JUB_TX_XRP
//void (^inlineTxXRP)(TxXRP*, JUB_TX_XRP&) = ^(TxXRP* xrpTx, JUB_TX_XRP& tx) {
//    if (nil != xrpTx.account) {
//        tx.account   = (JUB_CHAR_PTR)[xrpTx.account UTF8String];
//    }
//    tx.type      = inlineXRPTxType(xrpTx.type);
//    if (nil != xrpTx.fee) {
//        tx.fee       = (JUB_CHAR_PTR)[xrpTx.fee UTF8String];
//    }
//    if (nil != xrpTx.sequence) {
//        tx.sequence  = (JUB_CHAR_PTR)[xrpTx.sequence UTF8String];
//    }
//    if (nil != xrpTx.accountTxnID) {
//        tx.accountTxnID = (JUB_CHAR_PTR)[xrpTx.accountTxnID UTF8String];
//    }
//    if (nil != xrpTx.flags) {
//        tx.flags     = (JUB_CHAR_PTR)[xrpTx.flags UTF8String];
//    }
//    if (nil != xrpTx.lastLedgerSequence) {
//        tx.lastLedgerSequence = (JUB_CHAR_PTR)[xrpTx.lastLedgerSequence UTF8String];
//    }
//    if (nil != xrpTx.memo) {
//        inlineXRPMemo(xrpTx.memo, tx.memo);
//    }
//    if (nil != xrpTx.sourceTag) {
//        tx.sourceTag = (JUB_CHAR_PTR)[xrpTx.sourceTag UTF8String];
//    }
//    
//    switch (tx.type) {
//        case JUB_ENUM_XRP_TX_TYPE::PYMT:
//            inlinePayment(xrpTx.pymt, tx.pymt);
//            break;
//        case JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE:
//        default:
//            break;
//    }
//};
//
//// TxXRP* <- JUB_TX_XRP
//TxXRP* (^inlineNSTxXRP)(JUB_TX_XRP) = ^(JUB_TX_XRP xrpTx) {
//    TxXRP* tx = [[TxXRP alloc] init];
//    
//    if(nullptr != xrpTx.account) {
//        tx.account = [[NSString alloc] init];
//        tx.account = [[NSString stringWithCString:xrpTx.account
//                                         encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpTx.account);
//    }
//    
//    tx.type = inlineNSXRPTxType(xrpTx.type);
//    
//    if(nullptr != xrpTx.fee) {
//        tx.fee = [[NSString alloc] init];
//        tx.fee = [[NSString stringWithCString:xrpTx.fee
//                                     encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpTx.fee);
//    }
//    
//    if(nullptr != xrpTx.sequence) {
//        tx.sequence = [[NSString alloc] init];
//        tx.sequence = [[NSString stringWithCString:xrpTx.sequence
//                                          encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpTx.sequence);
//    }
//    
//    if(nullptr != xrpTx.accountTxnID) {
//        tx.accountTxnID = [[NSString alloc] init];
//        tx.accountTxnID = [[NSString stringWithCString:xrpTx.accountTxnID
//                                              encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpTx.accountTxnID);
//    }
//    
//    if(nullptr != xrpTx.flags) {
//        tx.flags = [[NSString alloc] init];
//        tx.flags = [[NSString stringWithCString:xrpTx.flags
//                                       encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpTx.flags);
//    }
//    
//    if(nullptr != xrpTx.lastLedgerSequence) {
//        tx.lastLedgerSequence = [[NSString alloc] init];
//        tx.lastLedgerSequence = [[NSString stringWithCString:xrpTx.lastLedgerSequence
//                                                    encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpTx.lastLedgerSequence);
//    }
//    
//    tx.memo = inlineNSXRPMemo(xrpTx.memo);
//    
//    if(nullptr != xrpTx.sourceTag) {
//        tx.sourceTag = [[NSString alloc] init];
//        tx.sourceTag = [[NSString stringWithCString:xrpTx.sourceTag
//                                           encoding:NSUTF8StringEncoding] copy];
//        JUB_FreeMemory(xrpTx.sourceTag);
//    }
//    
//    switch (xrpTx.type) {
//        case JUB_ENUM_XRP_TX_TYPE::PYMT:
//            tx.pymt = inlineNSPayment(xrpTx.pymt);
//            break;
//        case JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE:
//        default:
//            break;
//    }
//    
//    return tx;
//};

@implementation JubSDKCore (COIN_XRP)

////JUB_RV JUB_CreateContextXRP(IN CONTEXT_CONFIG_XRP cfg,
////                            IN JUB_UINT16 deviceID,
////                            OUT JUB_UINT16* contextID);
//- (NSUInteger)JUB_CreateContextXRP:(NSUInteger)deviceID
//                               cfg:(ContextConfigXRP*)cfg
//{
//    self.lastError = JUBR_OK;
//    
//    CONTEXT_CONFIG_XRP cfgXRP;
//    if (nil != cfg.mainPath) {
//        cfgXRP.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
//    }
//    
//    JUB_UINT16 contextID = 0;
//    JUB_RV rv = JUB_CreateContextXRP(cfgXRP,
//                                     deviceID,
//                                     &contextID);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return contextID;
//    }
//    
//    return contextID;
//}
//
////JUB_RV JUB_GetAddressXRP(IN JUB_UINT16 contextID,
////                         IN BIP32_Path path,
////                         IN JUB_ENUM_BOOL bShow,
////                         OUT JUB_CHAR_PTR_PTR address);
//- (NSString*)JUB_GetAddressXRP:(NSUInteger)contextID
//                          path:(BIP32Path*)path
//                         bShow:(JUB_NS_ENUM_BOOL)bShow
//{
//    self.lastError = JUBR_OK;
//    
//    BIP32_Path bip32Path;
//    bip32Path.change = inlineBool(path.change);
//    bip32Path.addressIndex = path.addressIndex;
//    JUB_ENUM_BOOL isShow = inlineBool(bShow);
//    JUB_CHAR_PTR address;
//    JUB_RV rv = JUB_GetAddressXRP(contextID,
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
////JUB_RV JUB_GetHDNodeXRP(IN JUB_UINT16 contextID,
////                        IN JUB_ENUM_PUB_FORMAT format,
////                        IN BIP32_Path path,
////                        OUT JUB_CHAR_PTR_PTR pubkey);
//- (NSString*)JUB_GetHDNodeXRP:(NSUInteger)contextID
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
//    JUB_CHAR_PTR pubkey = nullptr;
//    JUB_RV rv = JUB_GetHDNodeXRP(contextID,
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
////JUB_RV JUB_GetMainHDNodeXRP(IN JUB_UINT16 contextID,
////                            IN JUB_ENUM_PUB_FORMAT format,
////                            OUT JUB_CHAR_PTR_PTR xpub);
//- (NSString*)JUB_GetMainHDNodeXRP:(NSUInteger)contextID
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
//    JUB_RV rv = JUB_GetMainHDNodeXRP(contextID,
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
////JUB_RV JUB_SetMyAddressXRP(IN JUB_UINT16 contextID,
////                           IN BIP32_Path path,
////                           OUT JUB_CHAR_PTR_PTR address);
//- (NSString*)JUB_SetMyAddressXRP:(NSUInteger)contextID
//                            path:(BIP32Path*)path
//{
//    self.lastError = JUBR_OK;
//    
//    BIP32_Path bip32Path;
//    bip32Path.change = inlineBool(path.change);
//    bip32Path.addressIndex = path.addressIndex;
//    JUB_CHAR_PTR address;
//    JUB_RV rv = JUB_SetMyAddressXRP(contextID,
//                                    bip32Path,
//                                    &address);
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
////JUB_RV JUB_SignTransactionXRP(IN JUB_UINT16 contextID,
////                              IN BIP32_Path path,
////                              IN JUB_TX_XRP tx,
////                              OUT JUB_CHAR_PTR_PTR raw);
//- (NSString*)JUB_SignTransactionXRP:(NSUInteger)contextID
//                               path:(BIP32Path*)path
//                                 tx:(TxXRP*)tx
//{
//    self.lastError = JUBR_OK;
//    
//    BIP32_Path bip32Path;
//    bip32Path.change = inlineBool(path.change);
//    bip32Path.addressIndex = path.addressIndex;
//    JUB_TX_XRP xrpTx;
//    inlineTxXRP(tx, xrpTx);
//    JUB_CHAR_PTR raw = nullptr;
//    JUB_RV rv = JUB_SignTransactionXRP(contextID,
//                                       bip32Path,
//                                       xrpTx,
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

@end
