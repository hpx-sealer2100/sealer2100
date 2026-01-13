//
//  JUB_SDK_FIL.cpp
//  JubSDK
//
//  Created by Pan Min on 2020/11/12.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#include "JUB_SDK_SOL.h"

#include "utility/util.h"

#include "context/ContextSOL.h"
#include "token/interface/TokenInterface.hpp"


JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf);

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
                            OUT JUB_UINT16* contextID){
    if (nullptr == jub::TokenManager::GetInstance()->GetOne(deviceID)) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    jub::ContextSOL* context = new jub::ContextSOL(cfg, deviceID);
    *contextID = jub::ContextManager::GetInstance()->AddOne(context);
    context->ActiveSelf();

    return JUBR_OK;
    
}
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
                         OUT JUB_CHAR_PTR_PTR address){
    JUB_CHECK_CONTEXT_SOL(contextID);

    auto context = (jub::ContextSOL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_address;
    JUB_VERIFY_RV(context->GetAddress(path, bShow, str_address));
    JUB_VERIFY_RV(_allocMem(address, str_address));

    return JUBR_OK;
}

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
                        OUT JUB_CHAR_PTR_PTR pubkey){
    JUB_CHECK_CONTEXT_SOL(contextID);

    auto context = (jub::ContextSOL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_pubkey;
    JUB_VERIFY_RV(context->GetHDNode(format,path, str_pubkey));
    JUB_VERIFY_RV(_allocMem(pubkey, str_pubkey));

    return JUBR_OK;
}

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
                            OUT JUB_CHAR_PTR_PTR xpub){
    JUB_CHECK_CONTEXT_SOL(contextID);

    auto context = (jub::ContextSOL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_xpub;
    JUB_VERIFY_RV(context->GetMainHDNode(format, str_xpub));
    JUB_VERIFY_RV(_allocMem(xpub, str_xpub));

    return JUBR_OK;
}


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
                              OUT JUB_CHAR_PTR_PTR txRaw){
    JUB_CHECK_CONTEXT_SOL(contextID);

    auto context = (jub::ContextSOL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);
    
    std::string str_raw;
    JUB_VERIFY_RV(context->SignTransferTx(path, recentHash, dest, amount, str_raw));
    JUB_VERIFY_RV(_allocMem(txRaw, str_raw));
    
    return JUBR_OK;
}
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTransactionTokenSOL(IN JUB_UINT16 contextID,
                                   IN BIP32_Path path,
                                   IN JUB_CHAR_CPTR recentHash,
                                   IN JUB_CHAR_CPTR tokenMint,
                                   IN JUB_CHAR_CPTR source,
                                   IN JUB_CHAR_CPTR dest,
                                   IN JUB_UINT64 amount,
                                   IN JUB_UINT8 decimal,
                                   OUT JUB_CHAR_PTR_PTR txRaw){
    JUB_CHECK_CONTEXT_SOL(contextID);

    auto context = (jub::ContextSOL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);
    
    std::string str_raw;
    JUB_VERIFY_RV(context->SignTokenTransferTx(path, recentHash, tokenMint, source, dest, amount, decimal, str_raw));
    JUB_VERIFY_RV(_allocMem(txRaw, str_raw));
    
    return JUBR_OK;
    
}
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
                                   OUT JUB_CHAR_PTR_PTR txRaw){
    JUB_CHECK_CONTEXT_SOL(contextID);

    auto context = (jub::ContextSOL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);
    
    std::string str_raw;
    JUB_VERIFY_RV(context->SignTokenCreateAndTransferTx(path, recentHash, tokenMint, destMainAddress,source, dest, amount, decimal, str_raw));
    JUB_VERIFY_RV(_allocMem(txRaw, str_raw));
    
    return JUBR_OK;
    
}
