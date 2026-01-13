//
//  gdbleTransmit.cpp
//  GDbleTransmit
//
//  Created by wangan on 2025/6/7.
//

#import "gdbleTransmit.h"


#import "GDBLEInterface.h"



#define __PASTE(x,y)                           x##y
#define CK_DECLARE_FUNCTION(returnType, name)  returnType name
#define CK_FUNCTION_INFO(name)                 CK_DECLARE_FUNCTION(unsigned int, __PASTE(GD_, name))
#define ADDPRE(name)                           __PASTE(GD_, name)


#define CK_NEED_ARG_LIST

#ifndef IN
#define IN
#define COMMFRAMEWORKF_IN
#endif

#ifndef OUT
#define OUT
#define COMMFRAMEWORKF_OUT
#endif


CK_FUNCTION_INFO(BLE_Initialize)
#ifdef CK_NEED_ARG_LIST
(GDBLE_INIT_PARAM param)
#endif
{
    unsigned int ret = IFD_SUCCESS;

    //whl 20170327
//    [[BLEManager sharedManager] setParam:param];
    BLE_AUX_INIT_PARAM initPara ;
    //memset(&initPara, 0, sizeof(initPara));
    initPara.connFuncCallBack = nullptr;
    initPara.disconnFuncCallBack = (BLE_AUX_DisconnFuncCallBack)param.discCallBack;
    initPara.rcvDataFuncCallBack = (BLE_AUX_RcvDataFuncCallBack)param.callBack;
    initPara.scanFuncCallBack = (BLE_AUX_ScanFuncCallBack)param.scanCallBack;
    ret = ADDPRE(BLEInitialize(&initPara));
//    ADDPRE(BLESetIsReConnected(false));

    return ret;
}

CK_FUNCTION_INFO(BLE_Finalize)
#ifdef CK_NEED_ARG_LIST
(void)
#endif
{
    unsigned int ret = IFD_SUCCESS;

    
    ret = ADDPRE(BLEFinalizedBLE());


    return ret;
}

CK_FUNCTION_INFO(BLE_Scan)
#ifdef CK_NEED_ARG_LIST
(void)
#endif
{
    unsigned int ret = IFD_SUCCESS;

    
    ret = ADDPRE(BLEScan());

    return ret;
}

CK_FUNCTION_INFO(BLE_StopScan)
#ifdef CK_NEED_ARG_LIST
(void)
#endif
{
    unsigned int ret = IFD_SUCCESS;

    
    ret = ADDPRE(BLEStopScan());;

    return ret;
}

CK_FUNCTION_INFO(BLE_ConnDev)
#ifdef CK_NEED_ARG_LIST
(
 unsigned char* bBLEUUID,      /**< ble device UUID */
 unsigned int   connectType,   /**< ble device connect type */
 unsigned long* pDevHandle,    /**< output ble device connect handle */
 unsigned int   timeout
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;

   
    ret = ADDPRE(BLEConnectDev((const char*)bBLEUUID, pDevHandle, timeout));



    return ret;
}

CK_FUNCTION_INFO(BLE_CancelConnDev)
#ifdef CK_NEED_ARG_LIST
(
 unsigned char* bBLEUUID      /**< ble device UUID */
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;

  
    ret = ADDPRE(BLECancelConnect((const char*)bBLEUUID));


    return ret;
}

CK_FUNCTION_INFO(BLE_DisConn)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long devHandle               /**< output ble device connect handle */
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;

    
    ret = ADDPRE(BLEDisconnect(devHandle));

    return ret;
}

/**
 * add by fs - 20170602
 * 该接口按 bool 值处理。 0 - false; 非 0 - true;
 */
CK_FUNCTION_INFO(BLE_IsConn)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long devHandle       /**< output ble device connect handle */
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;

    ret = ADDPRE(BLEIsConnected(devHandle));

    return ret;
}

/**
 * add by Pan Min - 20190719
 * 该接口按 bool 值处理。 0 - false; 非 0 - true;
 */

CK_FUNCTION_INFO(BLE_SetIsReConnected)
#ifdef CK_NEED_ARG_LIST
(
 bool on       /**<reconnect or not  */
 )
#endif
{

    unsigned int ret = IFD_SUCCESS;
    
    ADDPRE(BLESetIsReConnected(on));

    return ret;
}



CK_FUNCTION_INFO(BLE_SendAPDU)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long  devHandle,
 unsigned char* apdu,
 unsigned int   apduLen,
 unsigned int   messageType
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;

  

    ret = ADDPRE(BLESendData(devHandle,apdu, apduLen,messageType));

    return ret;
}


CK_FUNCTION_INFO(BLE_SendAPDU_Sync)
#ifdef CK_NEED_ARG_LIST
(
    unsigned long  devHandle,
    unsigned char* apdu,
    unsigned int   apduLen,
    unsigned char* recvData,
    unsigned int* recvDataLen,
    unsigned int timeout
)
#endif
{
    
    unsigned int ret = IFD_SUCCESS;

  
    ret = ADDPRE(BLESendData1(devHandle,apdu,apduLen,recvData,recvDataLen,timeout));

    return ret;
}

CK_FUNCTION_INFO(BLE_SendAPDU_only)
//unsigned int GD_BLE_SendAPDU_only
#ifdef CK_NEED_ARG_LIST
(
    unsigned long  devHandle,
    unsigned char* apdu,
    unsigned int   apduLen
)
#endif
{
    
    unsigned int ret = IFD_SUCCESS;

  
    ret = ADDPRE(BLESendDataonly(devHandle,apdu,apduLen));

    return ret;
}


CK_FUNCTION_INFO(BLE_ReceiveAPDU_only)
#ifdef CK_NEED_ARG_LIST
(
    unsigned long  devHandle,
    unsigned char* recvData,
    unsigned int* recvDataLen,
    unsigned int timeout
)
#endif
{
    unsigned int ret = IFD_SUCCESS;

  
    ret = ADDPRE(BLEReceiveDataOnly(devHandle,recvData,recvDataLen,timeout));

    return ret;
}



#undef CK_NEED_ARG_LIST

#ifdef COMMFRAMEWORKF_IN
#undef IN
#undef COMMFRAMEWORKF_IN
#endif

#ifdef COMMFRAMEWORKF_OUT
#undef OUT
#undef COMMFRAMEWORKF_OUT
#endif


#undef ADDPRE
#undef CK_CALLBACK_FUNC
#undef CK_FUNCTION_INFO
#undef CK_DECLARE_FUNCTION
#undef __PASTE

