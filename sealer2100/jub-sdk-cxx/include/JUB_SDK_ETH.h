//
//  JUB_SDK_ETH.h
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef JUB_SDK_ETH_h
#define JUB_SDK_ETH_h

#include "JUB_SDK.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


typedef struct stContextCfgETH : stContextCfg {
    uint64_t chainID;

    stContextCfgETH() {
        chainID = 0;
    }
    virtual ~stContextCfgETH() {}
} CONTEXT_CONFIG_ETH;


typedef struct stERC20TokenInfo {
    JUB_CHAR_PTR tokenName;
    JUB_UINT16 unitDP;
    JUB_CHAR_PTR contractAddress;

    stERC20TokenInfo() {
        unitDP = 0;
    }
    virtual ~stERC20TokenInfo() {}
} ERC20_TOKEN_INFO;

typedef struct {
    uint64_t chainID;      // Chain ID of the network, must match CONTEXT_CONFIG_ETH.chainID
    uint64_t slip44ID;     // SLIP-0044 coin type of the network (reserved, currently unused)
    JUB_CHAR_CPTR name;    // Network name
    JUB_CHAR_CPTR symbol;  // Native currency symbol of the network
} JUB_ETH_NETWORK_INFO;

typedef struct {
    uint64_t chainID;      // Chain ID of the network to which the token belongs
    JUB_CHAR_CPTR name;    // Token name
    JUB_CHAR_CPTR symbol;  // Token symbol
    JUB_CHAR_CPTR address; // Token contract address
    JUB_UINT32 decimals;   // Token decimal precision
} JUB_ERC20_TOKEN_INFO;

/*****************************************************************************
 * @function name : JUB_CreateContextETH
 * @in  param : cfg
 *            : deviceID - device ID
 * @out param : contextID
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_CreateContextETH(IN CONTEXT_CONFIG_ETH cfg,
                            IN JUB_UINT16 deviceID,
                            OUT JUB_UINT16* contextID);


/*****************************************************************************
 * @function name : JUB_GetAddressETH
 * @in  param : contextID - context ID
 *          : path
 *          : bShow
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetAddressETH(IN JUB_UINT16 contextID,
                         IN BIP32_Path path,
                         IN JUB_ENUM_BOOL bShow,
                         OUT JUB_CHAR_PTR_PTR address);


/*****************************************************************************
 * @function name : JUB_GetHDNodeETH
 * @in  param : contextID - context ID
 *          : format - JUB_ENUM_PUB_FORMAT::HEX (0x00) for hex;
 *                 JUB_ENUM_PUB_FORMAT::XPUB(0x01) for xpub
 *          : path
 * @out param : pubkey
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetHDNodeETH(IN JUB_UINT16 contextID,
                        IN JUB_ENUM_PUB_FORMAT format,
                        IN BIP32_Path path,
                        OUT JUB_CHAR_PTR_PTR pubkey);


/*****************************************************************************
 * @function name : JUB_GetMainHDNodeETH
 * @in  param : contextID - context ID
 *          : format - JUB_ENUM_PUB_FORMAT::HEX (0x00) for hex;
 *                 JUB_ENUM_PUB_FORMAT::XPUB(0x01) for xpub
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetMainHDNodeETH(IN JUB_UINT16 contextID,
                            IN JUB_ENUM_PUB_FORMAT format,
                            OUT JUB_CHAR_PTR_PTR xpub);


/*****************************************************************************
 * @function name : JUB_SetMyAddressETH
 * @in  param : contextID - context ID
 *          : path
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetMyAddressETH(IN JUB_UINT16 contextID,
                           IN BIP32_Path path,
                           OUT JUB_CHAR_PTR_PTR address);


/*****************************************************************************
 * @function name : JUB_SignTransactionETH
 * @in  param : contextID - context ID
 *          : path
 *          : nonce - nonce
 *          : gasLimit - gas limit
 *          : gasPriceInWei - gas price in wei
 *          : to
 *          : valueInWei - value in wei
 *          : input
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTransactionETH(IN JUB_UINT16 contextID,
                              IN BIP32_Path path,
                              IN JUB_UINT32 nonce,
                              IN JUB_UINT32 gasLimit,
                              IN JUB_CHAR_PTR gasPriceInWei,
                              IN JUB_CHAR_PTR to,
                              IN JUB_CHAR_PTR valueInWei,
                              IN JUB_CHAR_PTR input,
                              OUT JUB_CHAR_PTR_PTR raw);
/*****************************************************************************
 * @function name : JUB_SignTypedDataETH_MAX
 * @in  param : contextID - context ID
 *          : path
 *          : TypedData - TypedData in JSON format
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTypedDataETH_MAX(IN JUB_UINT16 contextID,
                                IN BIP32_Path path,
                                IN JUB_CHAR_PTR TypedData,
                                OUT JUB_CHAR_PTR_PTR raw);


/*****************************************************************************
 * @function name : JUB_SetERC20TokensETH
 * @in  param : contextID - context ID
 *          : tokens - token info list
 *          : iCount - token info count
 * @out param : ERC20 abi
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetERC20TokensETH(IN JUB_UINT16 contextID,
                             IN ERC20_TOKEN_INFO tokens[],
                             IN JUB_UINT16 iCount);


/*****************************************************************************
 * @function name : JUB_SetERC20TokenETH
 * @description   : Set ERC20 token information for the current ETH context.
 *                  (Deprecated, use JUB_SetERC20TokenETHV2 instead.)
 *
 * @in  param     : contextID        - Context ID
 *                : tokenName        - ERC20 token name
 *                : unitDP           - Token decimal precision
 *                : contractAddress  - ERC20 token contract address
 *
 * @out param     : none
 *
 * @return        : JUB_RV           - Result code
 *
 * @last change   :
 *****************************************************************************/
JUB_DEPRECATED
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetERC20TokenETH(IN JUB_UINT16 contextID,
                            IN JUB_CHAR_CPTR tokenName,
                            IN JUB_UINT16 unitDP,
                            IN JUB_CHAR_CPTR contractAddress);


/*****************************************************************************
 * @function name : JUB_SetERC20TokenETHV2
 * @note This only works on firmware versions >= 1.2.35.
 * @description   : Set ERC20 token information with network configuration
 *                  for the current ETH context.
 *
 * @in  param     : contextID        - Context ID
 *                : networkInfo      - Network information (chain ID, name, symbol)
 *                : tokenInfo        - ERC20 token information
 *
 * @out param     : none
 *
 * @return        : JUB_RV           - Result code
 *
 * @last change   :
 *****************************************************************************/
JUB_RV JUB_SetERC20TokenETHV2(IN JUB_UINT16 contextID,
                              IN JUB_ETH_NETWORK_INFO networkInfo,
                              IN JUB_ERC20_TOKEN_INFO tokenInfo);


/*****************************************************************************
 * @function name : JUB_SetNetworkETHV2
 * @description   : Set network information for the current ETH context.
 * @note This only works on firmware versions >= 1.2.35.
 * @in  param     : contextID        - Context ID
 *                : networkInfo          - Network information (chain ID, name, symbol)
 *
 * @out param     : none
 *
 * @return        : JUB_RV           - Result code
 *
 * @last change   :
 *****************************************************************************/
JUB_RV JUB_SetNetworkETHV2(IN JUB_UINT16 contextID,
                           IN JUB_ETH_NETWORK_INFO networkInfo);

/*****************************************************************************
 * @function name : JUB_BuildERC20TransferAbiETH
 * @in  param : contextID - context ID
 *          : tokenTo - token to
 *          : tokenValue - value for token transaction
 * @out param : ERC20 abi
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BuildERC20TransferAbiETH(IN JUB_UINT16 contextID,
                                    IN JUB_CHAR_CPTR tokenTo, IN JUB_CHAR_CPTR tokenValue,
                                    OUT JUB_CHAR_PTR_PTR abi);


/*****************************************************************************
* @function name : JUB_SetContrAddrETH
* @in  param : contextID - context ID
*            : contractAddress - contract address
* @out param :
* @last change :
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetContrAddrETH(IN JUB_UINT16 contextID,
                           IN JUB_CHAR_CPTR contractAddress);


/*****************************************************************************
 * @function name : JUB_SignContractETH
 * @in  param : contextID - context ID
 *          : path
 *          : nonce - nonce
 *          : gasLimit - gas limit
 *          : gasPriceInWei - gas price in wei
 *          : to
 *          : valueInWei - value in wei
 *          : input
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignContractETH(IN JUB_UINT16 contextID,
                           IN BIP32_Path path,
                           IN JUB_UINT32 nonce,
                           IN JUB_UINT32 gasLimit,
                           IN JUB_CHAR_PTR gasPriceInWei,
                           IN JUB_CHAR_PTR to,
                           IN JUB_CHAR_PTR valueInWei,
                           IN JUB_CHAR_PTR input,
                           OUT JUB_CHAR_PTR_PTR raw);


/*****************************************************************************
 * @function name : JUB_SignBytestringETH
 * @in  param : contextID - context ID
 *          : path
 *          : data - message to be signed
 * @out param : signature
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignBytestringETH(IN JUB_UINT16 contextID,
                             IN BIP32_Path path,
                             IN JUB_CHAR_CPTR data,
                             OUT JUB_CHAR_PTR_PTR signature);


/*****************************************************************************
 * @function name : JUB_SignTypedDataETH
 * @in  param : contextID - context ID
 *          : path
 *          : data - typed structured data in JSON
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTypedDataETH(IN JUB_UINT16 contextID,
                            IN BIP32_Path path,
                            IN JUB_CHAR_CPTR dataInJSON,
                            IN JUB_BBOOL isMetamaskV4Compat,
                            OUT JUB_CHAR_PTR_PTR signature);

///*****************************************************************************
//* @function name : JUB_BuildContractWithAddrAbiETH
//* @in  param : contextID - context ID
//*                     : methodID - contract methodID
//*                     : address - The address at which you need to operate
//* @out param : specified contract abi
//* @last change :
//*****************************************************************************/
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_BuildContractWithAddrAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR address,
//                                       OUT JUB_CHAR_PTR_PTR abi);


/*****************************************************************************
* @function name : JUB_BuildContractWithAddrAmtDataAbiETH
* @in  param : contextID - context ID
*                     : methodID - contract methodID
*                     : address - The address at which you need to operate
*                     : amount - The amount at which you need to operate
*                     : data - The amount at which you need to operate
* @out param : specified contract abi
* @last change :
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BuildContractWithAddrAmtDataAbiETH(IN JUB_UINT16 contextID,
                                              IN JUB_CHAR_PTR methodID,
                                              IN JUB_CHAR_PTR address,
                                              IN JUB_CHAR_PTR amount,
                                              IN JUB_CHAR_PTR data,
                                              OUT JUB_CHAR_PTR_PTR abi);


/*****************************************************************************
* @function name : JUB_BuildContractWithTxIDAbiETH
* @in  param : contextID - context ID
*                     : methodID - contract methodID
*                     : transactionID - transaction ID
* @out param : specified contract abi
* @last change :
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BuildContractWithTxIDAbiETH(IN JUB_UINT16 contextID,
                                       IN JUB_CHAR_PTR methodID,
                                       IN JUB_CHAR_PTR transactionID,
                                       OUT JUB_CHAR_PTR_PTR abi);


///*****************************************************************************
//* @function name : JUB_BuildContractWithAmtAbiETH
//* @in  param : contextID - context ID
//*                     : methodID - contract methodID
//*                     : amount - The amount at which you need to operate
//* @out param : specified contract abi
//* @last change :
//*****************************************************************************/
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_BuildContractWithAmtAbiETH(IN JUB_UINT16 contextID,
//                                      IN JUB_CHAR_PTR methodID,
//                                      IN JUB_CHAR_PTR amount,
//                                      OUT JUB_CHAR_PTR_PTR abi);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_ETH_h */
