//
//  JubSDKCore+COIN_ETH.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+COIN_ETH.h"
#import "JUB_SDK_ETH.h"

//typedef struct {
//    JUB_CHAR_PTR        main_path;
//    uint32_t            chainID;
//} CONTEXT_CONFIG_ETH;
@implementation ContextConfigETH
@synthesize mainPath;
@synthesize chainID;
@end

extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT);

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
@implementation ERC20TokenInfo
@synthesize tokenName;
@synthesize unitDP;
@synthesize contractAddress;
@end

// ERC20TokenInfo* -> ERC20_TOKEN_INFO
ERC20_TOKEN_INFO (^inlineERC20TokenInfo)(ERC20TokenInfo*) = ^(ERC20TokenInfo* tokenInfo) {
    ERC20_TOKEN_INFO token;
    
    if (nil != tokenInfo.tokenName) {
        token.tokenName = (JUB_CHAR_PTR)[tokenInfo.tokenName UTF8String];
    }
    token.unitDP = (JUB_UINT32)tokenInfo.unitDP;
    if (nil != tokenInfo.contractAddress) {
        token.contractAddress = (JUB_CHAR_PTR)[tokenInfo.contractAddress UTF8String];
    }
    
    return token;
};

// ERC20TokenInfo* <- ERC20_TOKEN_INFO
ERC20TokenInfo* (^inlineNSERC20TokenInfo)(ERC20_TOKEN_INFO) = ^(ERC20_TOKEN_INFO tokenInfo) {
    ERC20TokenInfo* token = [[ERC20TokenInfo alloc] init];
    
    if (nullptr != tokenInfo.tokenName) {
        token.tokenName = [[NSString alloc] init];
        token.tokenName = [[NSString stringWithCString:tokenInfo.tokenName
                                              encoding:NSUTF8StringEncoding] copy];
    }
    else {
        token.tokenName = @"";
    }
    JUB_FreeMemory(tokenInfo.tokenName);
    
    token.unitDP = (JUB_UINT16)tokenInfo.unitDP;
    
    if (nullptr != tokenInfo.contractAddress) {
        token.contractAddress = [[NSString alloc] init];
        token.contractAddress = [[NSString stringWithCString:tokenInfo.contractAddress
                                                    encoding:NSUTF8StringEncoding] copy];
    }
    else {
        token.contractAddress = @"";
    }
    JUB_FreeMemory(tokenInfo.contractAddress);
    
    return token;
};

@implementation JubSDKCore (COIN_ETH)

//JUB_RV JUB_CreateContextETH(IN CONTEXT_CONFIG_ETH cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextETH:(NSUInteger)deviceID
                               cfg:(ContextConfigETH*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_ETH cfgETH;
    if (nil != cfg.mainPath) {
        cfgETH.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    cfgETH.chainID = (uint32_t)cfg.chainID;
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextETH(cfgETH,
                                     deviceID,
                                     &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}

//JUB_RV JUB_GetAddressETH(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressETH:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_GetAddressETH(contextID,
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

//JUB_RV JUB_GetHDNodeETH(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeETH:(NSUInteger)contextID
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
    JUB_RV rv = JUB_GetHDNodeETH(contextID,
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

//JUB_RV JUB_GetMainHDNodeETH(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeETH:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetMainHDNodeETH(contextID,
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

//JUB_RV JUB_SetMyAddressETH(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressETH:(NSUInteger)contextID
                            path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_SetMyAddressETH(contextID,
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

//JUB_RV JUB_SetContrAddrETH(IN JUB_UINT16 contextID,
//                           IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetContrAddrETH:(NSUInteger)contextID
                  contrAddr:(NSString*)contractAddress
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pContractAddress = nullptr;
    if (nil != contractAddress) {
        pContractAddress = (JUB_CHAR_PTR)[contractAddress UTF8String];
    }
    
    self.lastError = JUB_SetContrAddrETH(contextID, pContractAddress);
}

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
                              input:(NSString*)input
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR vInWei = nullptr;
    if (NSOrderedSame != [valueInWei compare:@""]
        &&        nil !=  valueInWei
        ) {
        vInWei = (JUB_CHAR_PTR)[valueInWei UTF8String];
    }
    JUB_CHAR_PTR pGasPriceInWei = nullptr;
    if (nil != gasPriceInWei) {
        pGasPriceInWei = (JUB_CHAR_PTR)[gasPriceInWei UTF8String];
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
    JUB_RV rv = JUB_SignTransactionETH(contextID,
                                       bip32Path,
                                       (JUB_UINT32)nonce,
                                       (JUB_UINT32)gasLimit,
                                       pGasPriceInWei,
                                       pTo,
                                       vInWei,
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

//JUB_RV JUB_SetERC20TokensETH(IN JUB_UINT16 contextID,
//                             IN ERC20_TOKEN_INFO tokens[],
//                             IN JUB_UINT16 iCount);
- (void)JUB_SetERC20TokensETH:(NSUInteger)contextID
                   tokenArray:(NSArray*)tokenArray
{
    self.lastError = JUBR_OK;
    
    NSUInteger iToken = [tokenArray count];
    if (0 == iToken) {
        self.lastError = JUBR_ARGUMENTS_BAD;
    }
    
    ERC20_TOKEN_INFO* pTokens = (ERC20_TOKEN_INFO*)malloc(sizeof(ERC20_TOKEN_INFO)*iToken+1);
    memset((void*)pTokens, 0x00, sizeof(ERC20_TOKEN_INFO)*iToken+1);
    for (NSUInteger i=0; i < tokenArray.count; ++i) {
        ERC20TokenInfo* info = tokenArray[i];
        pTokens[i] = inlineERC20TokenInfo(info);
    }
    if (JUBR_OK != self.lastError) {
        if (nullptr != pTokens) {
            free(pTokens); pTokens = nullptr;
        }
        return ;
    }
    
    JUB_RV rv = JUB_SetERC20TokensETH(contextID, pTokens, iToken);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
    if (nullptr != pTokens) {
        free(pTokens); pTokens = nullptr;
    }
}

//JUB_RV JUB_SetERC20TokenETH(IN JUB_UINT16 contextID,
//                            IN JUB_CHAR_CPTR tokenName,
//                            IN JUB_UINT16 unitDP,
//                            IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetERC20TokenETH:(NSUInteger)contextID
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
    
    JUB_RV rv = JUB_SetERC20TokenETH(contextID, pTokenName, unitDP, pContractAddress);
    if (JUBR_OK != rv) {
        self.lastError = rv;
    }
}

//JUB_RV JUB_BuildERC20TransferAbiETH(IN JUB_UINT16 contextID,
//                                    IN JUB_CHAR_CPTR tokenTo, IN JUB_CHAR_CPTR tokenValue,
//                                    OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildERC20TransferAbiETH:(NSUInteger)contextID
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
    JUB_RV rv = JUB_BuildERC20TransferAbiETH(contextID,
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
                           input:(NSString*)input
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR vInWei = nullptr;
    if (NSOrderedSame != [valueInWei compare:@""]
        &&        nil !=  valueInWei
        ) {
        vInWei = (JUB_CHAR_PTR)[valueInWei UTF8String];
    }
    
    JUB_CHAR_PTR pGasPriceInWei = nullptr;
    if (nil != gasPriceInWei) {
        pGasPriceInWei = (JUB_CHAR_PTR)[gasPriceInWei UTF8String];
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
    JUB_RV rv = JUB_SignContractETH(contextID,
                                    bip32Path,
                                    (JUB_UINT32)nonce,
                                    (JUB_UINT32)gasLimit,
                                    pGasPriceInWei,
                                    pTo,
                                    vInWei,
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

//JUB_RV JUB_SignBytestringETH(IN JUB_UINT16 contextID,
//                             IN BIP32_Path path,
//                             IN JUB_CHAR_CPTR data,
//                             OUT JUB_CHAR_PTR_PTR signature);
- (NSString*)JUB_SignBytestringETH:(NSUInteger)contextID
                              path:(BIP32Path*)path
                              data:(NSString*)data
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pData = nullptr;
    if (nil != data) {
        pData = (JUB_CHAR_PTR)[data UTF8String];
    }
    
    JUB_CHAR_PTR signature = nullptr;
    JUB_RV rv = JUB_SignBytestringETH(contextID,
                                      bip32Path,
                                      pData,
                                      &signature);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strSignature = [NSString stringWithCString:signature
                                                encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(signature);
    
    return strSignature;
}

//JUB_RV JUB_SignTypedDataETH(IN JUB_UINT16 contextID,
//                            IN BIP32_Path path,
//                            IN JUB_CHAR_CPTR dataInJSON,
//                            IN JUB_BBOOL isMetamaskV4Compat,
//                            OUT JUB_CHAR_PTR_PTR signature);
- (NSString*)JUB_SignTypedDataETH:(NSUInteger)contextID
                             path:(BIP32Path*)path
                             data:(NSString*)dataInJSON
               isMetamaskV4Compat:(JUB_NS_ENUM_BOOL)bMetamaskV4Compat
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pDataInJSON = nullptr;
    if (nil != dataInJSON) {
        pDataInJSON = (JUB_CHAR_PTR)[dataInJSON UTF8String];
    }
    JUB_ENUM_BOOL isMetamaskV4Compat = inlineBool(bMetamaskV4Compat);
    
    JUB_CHAR_PTR signature = nullptr;
    JUB_RV rv = JUB_SignTypedDataETH(contextID,
                                     bip32Path,
                                     pDataInJSON,
                                     isMetamaskV4Compat,
                                     &signature);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strSignature = [NSString stringWithCString:signature
                                                encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(signature);
    
    return strSignature;
}

//JUB_RV JUB_BuildContractWithAddrAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR address,
//                                       OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                     address:(NSString*)address
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pMethodID = nullptr;
//    if (nil != methodID) {
//        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAddress = nullptr;
//    if (nil != address) {
//        pAddress = (JUB_CHAR_PTR)[address UTF8String];
//    }
//    
//    JUB_CHAR_PTR abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAddrAbiETH(contextID,
//                                                pMethodID,
//                                                pAddress,
//                                                &abi);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strAbi = [NSString stringWithCString:abi
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(abi);
//    
//    return strAbi;
//}
//
//JUB_RV JUB_BuildContractWithAddrAmtAbiETH(IN JUB_UINT16 contextID,
//                                          IN JUB_CHAR_PTR methodID,
//                                          IN JUB_CHAR_PTR address,
//                                          IN JUB_CHAR_PTR amount,
//                                          OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAmtAbiETH:(NSUInteger)contextID
//                                       methodID:(NSString*)methodID
//                                        address:(NSString*)address
//                                         amount:(NSString*)amount
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pMethodID = nullptr;
//    if (nil != methodID) {
//        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAddress = nullptr;
//    if (nil != address) {
//        pAddress = (JUB_CHAR_PTR)[address UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAmount = nullptr;
//    if (nil != amount) {
//        pAmount = (JUB_CHAR_PTR)[amount UTF8String];
//    }
//    
//    JUB_CHAR_PTR abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAddrAmtAbiETH(contextID,
//                                                   pMethodID,
//                                                   pAddress,
//                                                   pAmount,
//                                                   &abi);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strAbi = [NSString stringWithCString:abi
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(abi);
//    
//    return strAbi;
//}
//
//JUB_RV JUB_BuildContractWithTxIDAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR transactionID,
//                                       OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
                                    methodID:(NSString*)methodID
                               transactionID:(NSString*)transactionID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pMethodID = nullptr;
    if (nil != methodID) {
        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
    }
    
    JUB_CHAR_PTR pTransactionID = nullptr;
    if (nil != transactionID) {
        pTransactionID = (JUB_CHAR_PTR)[transactionID UTF8String];
    }
    
    JUB_CHAR_PTR abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithTxIDAbiETH(contextID,
                                                pMethodID,
                                                pTransactionID,
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

//JUB_RV JUB_BuildContractWithAmtAbiETH(IN JUB_UINT16 contextID,
//                                      IN JUB_CHAR_PTR methodID,
//                                      IN JUB_CHAR_PTR amount,
//                                      OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                      amount:(NSString*)amount
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pMethodID = nullptr;
//    if (nil != methodID) {
//        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAmount = nullptr;
//    if (nil != amount) {
//        pAmount = (JUB_CHAR_PTR)[amount UTF8String];
//    }
//    
//    JUB_CHAR_PTR abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAmtAbiETH(contextID,
//                                               pMethodID,
//                                               pAmount,
//                                               &abi);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strAbi = [NSString stringWithCString:abi
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(abi);
//    
//    return strAbi;
//}

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
                                               data:(NSString*)data
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pMethodID = nullptr;
    if (nil != methodID) {
        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
    }
    
    JUB_CHAR_PTR pAddress = nullptr;
    if (nil != address) {
        pAddress = (JUB_CHAR_PTR)[address UTF8String];
    }
    
    JUB_CHAR_PTR pAmount = nullptr;
    if (nil != amount) {
        pAmount = (JUB_CHAR_PTR)[amount UTF8String];
    }
    
    JUB_CHAR_PTR pData = nullptr;
    if (nil != data) {
        pData = (JUB_CHAR_PTR)[data UTF8String];
    }
    
    JUB_CHAR_PTR abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithAddrAmtDataAbiETH(contextID,
                                                       pMethodID,
                                                       pAddress,
                                                       pAmount,
                                                       pData,
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

@end
