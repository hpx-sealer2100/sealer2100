//
//  JUB_SDK_SOL.h
//  JubSDK
//
//  Created by Pan Min on 2020/11/12.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#ifndef JUB_SDK_FIL_h
#define JUB_SDK_FIL_h

#include "JUB_SDK.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
typedef CONTEXT_CONFIG CONTEXT_CONFIG_SOL;

/*****************************************************************************
 * @function name : JUB_CreateContextSOL
 * @in  param : cfg
 *          : deviceID - device ID
 * @out param : contextID
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_CreateContextSOL(IN CONTEXT_CONFIG_SOL cfg,
                            IN JUB_UINT16 deviceID,
                            OUT JUB_UINT16* contextID);
/*****************************************************************************
 * @function name : JUB_GetAddressSOL
 * @in  param : contextID - context ID
 *          : path
 *          : bShow
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetAddressSOL(IN JUB_UINT16 contextID,
                         IN BIP32_Path path,
                         IN JUB_ENUM_BOOL bShow,
                         OUT JUB_CHAR_PTR_PTR address);

/*****************************************************************************
 * @function name : JUB_GetHDNodeSOL
 * @in  param : contextID - context ID
 *          : format - JUB_ENUM_PUB_FORMAT::HEX(0x00) for hex;
 *          : path
 * @out param : pubkey
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetHDNodeSOL(IN JUB_UINT16 contextID,
                        IN JUB_ENUM_PUB_FORMAT format,
                        IN BIP32_Path path,
                        OUT JUB_CHAR_PTR_PTR pubkey);

/*****************************************************************************
 * @function name : JUB_GetMainHDNodeSOL
 * @in  param : contextID - context ID
 *          : format - JUB_ENUM_PUB_FORMAT::HEX(0x00) for hex;
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetMainHDNodeSOL(IN JUB_UINT16 contextID,
                            IN JUB_ENUM_PUB_FORMAT format,
                            OUT JUB_CHAR_PTR_PTR xpub);


/*****************************************************************************
 * @function name : JUB_SignTransactionSOL
 * @in  param : contextID - context ID
 *          : path - path of funding account
 *          : recentHash - recent block hash of Solana network
 *          : dest - receipt account
 *          : amount - transfer amount in lamports
 * @out param : txRaw signed Solana tx
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTransactionSOL(IN JUB_UINT16 contextID,
                              IN BIP32_Path path,
                              IN JUB_CHAR_CPTR recentHash,
                              IN JUB_CHAR_CPTR dest,
                              IN JUB_UINT64 amount,
                              OUT JUB_CHAR_PTR_PTR txRaw);

/*****************************************************************************
 * @function name : JUB_SignTransactionTokenSOL
 * @in  param : contextID - context ID
 *          : path - path of funding account
 *          : recentHash - recent block hash of Solana network
 *          : dest - receipt account
 *          : amount - transfer amount in lamports
 * @out param : txRaw signed Solana tx
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTransactionTokenSOL(IN JUB_UINT16 contextID,
                                   IN BIP32_Path path,
                                   IN JUB_CHAR_CPTR recentHash,
                                   IN JUB_CHAR_CPTR tokenMint,
                                   IN JUB_CHAR_CPTR source,
                                   IN JUB_CHAR_CPTR dest,
                                   IN JUB_UINT64 amount,
                                   IN JUB_UINT8 decimal,
                                   OUT JUB_CHAR_PTR_PTR txRaw);

/*****************************************************************************
 * @function name : JUB_SignTxTokenCreateAndTransferSOL
 * @in  param : contextID - context ID
 *          : path - path of funding account
 *          : recentHash - recent block hash of Solana network
 *          : dest - receipt account
 *          : amount - transfer amount in lamports
 * @out param : txRaw signed Solana tx
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTxTokenCreateAndTransferSOL(IN JUB_UINT16 contextID,
                                   IN BIP32_Path path,
                                   IN JUB_CHAR_CPTR recentHash,
                                   IN JUB_CHAR_CPTR tokenMint,
                                   IN JUB_CHAR_CPTR destMainAddress,
                                   IN JUB_CHAR_CPTR source,
                                   IN JUB_CHAR_CPTR dest,
                                   IN JUB_UINT64 amount,
                                   IN JUB_UINT8 decimal,
                                   OUT JUB_CHAR_PTR_PTR txRaw);





#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_SOL_h */

