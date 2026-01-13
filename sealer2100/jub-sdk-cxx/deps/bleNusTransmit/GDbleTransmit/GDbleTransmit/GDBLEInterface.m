//
//  BLEInterface.m
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//
//

#import <Foundation/Foundation.h>
#import "GDBLEInterface.h"
#import "GDBLETransmitManager.h"

#pragma - mark 设置通讯库配置
unsigned int  ADDPRE(BLESetLibConfig(char * companyFirstTag[] ,char * companySecondTag[],unsigned char*supportProductType))
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] SetLibConfig:companyFirstTag CompanySecondTag:companySecondTag SupportProductType:supportProductType];
    }while(0);
    return ret;
}
#pragma - mark 模块使用初始化函数
unsigned int  ADDPRE(BLEInitialize(BLE_AUX_INIT_PARAM* initPara))
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] Initialize:initPara];
    }while(0);
    return ret;
}
#pragma - mark 模块使用结束函数
//模块使用结束函数
unsigned int  ADDPRE(BLEFinalizedBLE())
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] FinalizedBLE];
    }while(0);
    return ret;
}
#pragma - mark 扫描
//扫描函数
unsigned int  ADDPRE(BLEScan())
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] Scan];
    }while(0);
    return ret;
}
#pragma - mark 停止扫描
//停止扫描函数
unsigned int  ADDPRE(BLEStopScan())
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] StopScan];
    }while(0);
    return ret;
}

#pragma - mark 连接函数
//连接函数   超时时间单位为ms
unsigned int  ADDPRE(BLEConnectDev(const char* pcuuid, unsigned long* pulHandle, unsigned long ulTimeout))
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] Connect:pcuuid Handle:pulHandle Timeout:ulTimeout];
    }while(0);
    return ret;
}
#pragma - mark 取消连接
//取消连接
unsigned int  ADDPRE(BLECancelConnect(const char* pcuuid))
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] CancelConnect:pcuuid];
    }while(0);
    return ret;
}
#pragma - mark 断开连接
//断开连接函数
unsigned int  ADDPRE(BLEDisconnect(unsigned long ulHandle))
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] Disconnect:ulHandle];
    }while(0);
    return ret;
}
#pragma - mark 发送数据
//发送数据
unsigned int  ADDPRE(BLESendData(unsigned long ulHandle,const unsigned char* bData, unsigned int dlen,unsigned int messageType))
{
    unsigned int ret = 0 ;
    do{
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] SendDatafirst:ulHandle Data:bData Length:dlen messagetype:messageType];
    }while(0);
    return ret;
}

unsigned int  ADDPRE(BLESendData1(unsigned long ulHandle,const unsigned char* bData, unsigned int dlen,unsigned char*rData,unsigned int* rlen,unsigned int timeout))
{
    
    unsigned int ret = 0 ;
    do{

        
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] SendDatafirstnew:ulHandle Data:bData Length:dlen recvData:rData Length:rlen Timeout:timeout];
        
    }while(0);
    return ret;
}
unsigned int  ADDPRE(BLESendDataonly(unsigned long ulHandle,const unsigned char* bData, unsigned int dlen))
{
    
    unsigned int ret = 0 ;
    do{

        
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] SendDataonly:ulHandle Data:bData Length:dlen];
        
    }while(0);
    return ret;
}

unsigned int  ADDPRE(BLEReceiveDataOnly(unsigned long ulHandle,unsigned char*rData,unsigned int* rlen,unsigned int timeout))
{
    
    unsigned int ret = 0 ;
    do{

        
        ret = (unsigned int)[[GDBLETransmitManager SharedInstance] ReceiveDataOnly:ulHandle recvData:rData Length:rlen Timeout:timeout];
        
    }while(0);
    return ret;
}





#pragma - mark 判断连接状态
bool ADDPRE(BLEIsConnected(unsigned long ulHandle))
{
    return [[GDBLETransmitManager SharedInstance] IsConnected:ulHandle];
}

void          ADDPRE(BLESetIsReConnected(bool on))
{
    
}



#pragma - mark 获取版本号
//版本号  测试版前面加个0 逐次升级  发布版去掉0后重新计算

//获取库版本
unsigned int  ADDPRE(BLEGetLibVersion(char * libVersion))
{
    char * tempLibVersion = "1.0.01";
    int libVerionLen = strlen(tempLibVersion);
    memcpy(libVersion, tempLibVersion, libVerionLen);
    libVersion[libVerionLen] = 0;
    return 0;
}

