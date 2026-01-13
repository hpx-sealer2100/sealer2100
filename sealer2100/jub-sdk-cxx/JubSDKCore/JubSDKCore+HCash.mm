//
//  JubSDKCore+HCash.m
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/12.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+HCash.h"
#import "JUB_SDK_Hcash.h"

//typedef struct {
//    JUB_UINT64  amount;
//    BIP32_Path  path;
//} INPUT_HC;
@implementation InputHC
@synthesize amount;
@synthesize path;
@end

//typedef struct {
//    JUB_ENUM_BOOL   change_address;
//    BIP32_Path      path;
//} OUTPUT_HC;
@implementation OutputHC
@synthesize isChangeAddress;
@synthesize path;
@end

extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_BTC_TRANS_TYPE (^inlineTransType)(JUB_NS_BTC_TRANS_TYPE);

@implementation JubSDKCore (HCash)

//JUB_RV JUB_CreateContextHC(IN CONTEXT_CONFIG_HC cfg,
//                           IN JUB_UINT16 deviceID,
//                           OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextHC:(NSUInteger)deviceID
                              cfg:(ContextConfigHC*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_HC cfgHC;
    if (nil != cfg.mainPath) {
        cfgHC.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextHC(cfgHC,
                                    deviceID,
                                    &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}

//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_CreateMultiSigContextHC(IN CONTEXT_CONFIG_MULTISIG_HC cfg,
//                                   IN JUB_UINT16 deviceID,
//                                   OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateMultiSigContextHC:(NSUInteger)deviceID
                                      cfg:(ContextCfgMultisigHC*)cfg {
    self.lastError = JUBR_OK;
    
    JUB_BTC_TRANS_TYPE transType = inlineTransType(cfg.transType);
    if (trans_type_ns_item == transType) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return 0;
    }
    
    JUB_CHAR_PTR mainPath = nullptr;
    if (nil != cfg.mainPath) {
        mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    if (!mainPath) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return 0;
    }
    
    ContextCfgMultisigHC* multisigCfg = (ContextCfgMultisigHC*)cfg;
    CONTEXT_CONFIG_MULTISIG_HC ctxCfgMultisig;
    ctxCfgMultisig.mainPath = mainPath;
    ctxCfgMultisig.transType = transType;
    ctxCfgMultisig.m = multisigCfg.m;
    ctxCfgMultisig.n = multisigCfg.n;
    for (NSString* cosignerMainXpub in multisigCfg.vCosignerMainXpub) {
        ctxCfgMultisig.vCosignerMainXpub.push_back([cosignerMainXpub UTF8String]);
    }
    
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateMultiSigContextHC(ctxCfgMultisig,
                                            deviceID,
                                            &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}

//JUB_RV JUB_GetAddressHC(IN JUB_UINT16 contextID,
//                        IN BIP32_Path path,
//                        IN JUB_ENUM_BOOL bShow,
//                        OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressHC:(NSUInteger)contextID
                         path:(BIP32Path*)path
                        bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address = nullptr;
    JUB_RV rv = JUB_GetAddressHC(contextID,
                                 bip32Path,
                                 isShow,
                                 &address);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return nil;
    }
    
    NSString* strAddress = [NSString stringWithCString:address
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(address);
    
    return strAddress;
}

//JUB_RV JUB_GetHDNodeHC(IN JUB_UINT16 contextID,
//                       IN BIP32_Path path,
//                       OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetHDNodeHC:(NSUInteger)contextID
                        path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    BIP32_Path p;
    p.change = inlineBool(path.change);
    p.addressIndex = path.addressIndex;
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetHDNodeHC(contextID,
                                p,
                                &xpub);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return nil;
    }
    
    NSString* strXpub = [NSString stringWithCString:xpub
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(xpub);
    
    return strXpub;
}

//JUB_RV JUB_GetMainHDNodeHC(IN JUB_UINT16 contextID,
//                           OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeHC:(NSUInteger)contextID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetMainHDNodeHC(contextID,
                                    &xpub);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return nil;
    }
    
    NSString* strXpub = [NSString stringWithCString:xpub
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(xpub);
    
    return strXpub;
}

//JUB_RV JUB_SignTransactionHC(IN JUB_UINT16 contextID,
//                             IN INPUT_HC inputs[], IN JUB_UINT16 iCount,
//                             IN OUTPUT_HC outputs[], IN JUB_UINT16 oCount,
//                             IN JUB_CHAR_PTR unsigned_trans,
//                             OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionHC:(NSUInteger)contextID
                        inputArray:(NSArray*)inputArray
                       outputArray:(NSArray*)outputArray
                     unsignedTrans:(NSString*)unsignedTrans
{
    NSLog(@"[0x%08lx] JUB_SignTransactionHC begin(...)", JUBR_OK);
    self.lastError = JUBR_OK;
    
    NSUInteger iCnt = [inputArray count];
    NSUInteger oCnt = [outputArray count];
    if (   0 == iCnt
        || 0 == oCnt
        ) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return nil;
    }
    
    INPUT_HC* pInputs = (INPUT_HC*)malloc(sizeof(INPUT_HC)*iCnt+1);
    memset(pInputs, 0x00, sizeof(INPUT_HC)*iCnt+1);
    for (NSUInteger i=0; i < inputArray.count; ++i) {
        InputHC* input = inputArray[i];
        pInputs[i].amount = input.amount;
        pInputs[i].path.addressIndex = input.path.addressIndex;
        pInputs[i].path.change = inlineBool(input.path.change);
    }
    OUTPUT_HC* pOutputs = (OUTPUT_HC*)malloc(sizeof(OUTPUT_HC)*oCnt+1);
    memset(pOutputs, 0x00, sizeof(OUTPUT_HC)*oCnt+1);
    for (NSUInteger i=0; i < outputArray.count; ++i) {
        OutputHC* output = outputArray[i];
        pOutputs[i].changeAddress = inlineBool(output.isChangeAddress);
        pOutputs[i].path.change = inlineBool(output.path.change);
        pOutputs[i].path.change = inlineBool(output.path.change);
        pOutputs[i].path.addressIndex = output.path.addressIndex;
    }
    
    JUB_CHAR_PTR pUnsignedTrans = nullptr;
    if (nil != unsignedTrans) {
        pUnsignedTrans = (JUB_CHAR_PTR)[unsignedTrans UTF8String];
    }
    
    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTransactionHC(contextID,
                                      pInputs, iCnt,
                                      pOutputs, oCnt,
                                      pUnsignedTrans,
                                      &raw);
    if (nullptr != pInputs) {
        free(pInputs); pInputs = nullptr;
    }
    if (nullptr != pOutputs) {
        free(pOutputs); pOutputs = nullptr;
    }
    
    self.lastError = rv;
    if (   JUBR_OK != rv
        && JUBR_MULTISIG_OK != rv
        ) {
        return @"";
    }
    
    NSString *strRaw = [NSString stringWithCString:raw
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(raw);
    
    return strRaw;
}

@end // @implementation JubSDKCore (HCash)
