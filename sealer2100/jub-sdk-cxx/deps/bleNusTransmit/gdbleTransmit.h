//
//  gdbleTransmit.h
//  GDbleTransmit
//
//  Created by wangan on 2025/6/7.
//

#ifndef gdbleTransmit_h
#define gdbleTransmit_h

#define DISCALL                     0xFFFFFFFF

#define    IFD_SUCCESS                 0   /**< no error */
#define IFD_TIMEOUT                 1   /**< conn time out */
#define IFD_COMMUNICATION_ERROR        612 /**< generic error */
#define IFD_RESPONSE_TIMEOUT        613 /**< timeout */
#define IFD_NOT_SUPPORTED           614 /**< request is not supported */
#define IFD_NO_DEVICE               615 /**< no device>*/

typedef int (*GDBLE_ReadCallBack)(
                                unsigned long  devHandle,
                                unsigned char* data,
                                unsigned int   dataLen
                                );
typedef void (*GDBLE_ScanCallBack)(
                                 unsigned char* devName,
                                 unsigned char* uuid,
                                 unsigned int   type
                                 );
//typedef void (*BLE_StopScanCallback) ();
typedef void (*GDBLE_DiscCallBack)(unsigned char* uuid);

typedef struct
{
    void*            param;
    GDBLE_ReadCallBack callBack;
    GDBLE_ScanCallBack scanCallBack;
//    BLE_StopScanCallback stopScanCallback;
    GDBLE_DiscCallBack discCallBack;
} GDBLE_INIT_PARAM;


// define __PASTE
#define __PASTE(x,y)                           x##y
// define CK_DECLARE_FUNCTION
#define CK_DECLARE_FUNCTION(returnType, name)  returnType name

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    // define CK_FUNCTION_INFO

#define CK_FUNCTION_INFO(name)                 CK_DECLARE_FUNCTION(unsigned int, __PASTE(GD_, name))

    
    // define CK_NEED_ARG_LIST
#define CK_NEED_ARG_LIST 1
    
#include "gdbleTransmitF.h"
    
    // undef CK_NEED_ARG_LIST
#undef CK_NEED_ARG_LIST
    // undef CK_FUNCTION_INFO
#undef CK_FUNCTION_INFO
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

// undef CK_DECLARE_FUNCTION
#undef CK_DECLARE_FUNCTION
// undef __PASTE
#undef __PASTE

#endif /* gdbleTransmit_h */
