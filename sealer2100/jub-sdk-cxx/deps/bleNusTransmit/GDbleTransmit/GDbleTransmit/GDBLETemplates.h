//
//  GDBLETemplates.h
//  GDbleTransmit
//
//  Created by wangan on 2025/6/7.
//

#ifndef GDBLETemplates_h
#define GDBLETemplates_h

#define BLE_AUX_ScanFuncCallBack     GD_BLE_AUX_ScanFuncCallBack
#define BLE_AUX_ConnFuncCallBack     GD_BLE_AUX_ConnFuncCallBack
#define BLE_AUX_DisconnFuncCallBack  GD_BLE_AUX_DisconnFuncCallBack
#define BLE_AUX_RcvDataFuncCallBack  GD_BLE_AUX_RcvDataFuncCallBack
#define BLE_AUX_INIT_PARAM           GD_BLE_AUX_INIT_PARAM
#define BLE_AUX_DidBondFuncCallBack  GD_BLE_AUX_DidBondFuncCallBack
#define BLE_DEV_ADV_INFO             GD_BLE_DEV_ADV_INFO


typedef void (*BLE_AUX_ScanFuncCallBack)
(
const char* devname, /* device name */
const char* uuid,    /* IOS:     device uuid;
                      */
unsigned int    devType  /* device type */
);

typedef void  (*BLE_AUX_ConnFuncCallBack)
(
const char* uuid,     /* IOS:     device uuid;
                       */
const unsigned long handle //设备句柄
);

typedef void  (*BLE_AUX_DisconnFuncCallBack)
(
const char* uuid     /* IOS:     device uuid;
                      */
);

typedef void  (*BLE_AUX_RcvDataFuncCallBack)
(
const unsigned long handle, //device handle
unsigned char * data,   /*  rcv data */
unsigned int    len   /*  data len */
);

typedef struct {
    BLE_AUX_ConnFuncCallBack     connFuncCallBack;        /* conn callback */
    BLE_AUX_RcvDataFuncCallBack  rcvDataFuncCallBack;     /* rcv data callback */
    BLE_AUX_ScanFuncCallBack     scanFuncCallBack;        /* scan callback */
    BLE_AUX_DisconnFuncCallBack  disconnFuncCallBack;     /* disconn callback */
}__attribute__ ((packed)) BLE_AUX_INIT_PARAM;

typedef void  (*BLE_AUX_DidBondFuncCallBack)
(
const unsigned long handle, //device handle
unsigned char * data,   /*  rcv data */
unsigned int    len     /*  data len */
);

typedef struct {
    NSInteger uuidType;
    NSInteger devType;
    NSInteger devVerion;
}BLE_DEV_ADV_INFO;

#endif /* GDBLETemplates_h */
