//
//  BLEKeyManager.m
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//
//

#import <Foundation/Foundation.h>
#import "GDBLEKeyManager.h"
#import "GDBLEDevice.h"
#import "GDErrorCodesAndMacros.h"
#import "GDBLEUtils.h"

#pragma pack (1)


static GDBLEKeyManager *_instance1 = nil;


void  GDDidBondFuncCallBack
(
const unsigned long handle, //device handle
 unsigned char * data,   /*  rcv data */
 unsigned int    len     /*  data len */
)
{
    GDBLEDevice* dev = [[GDBLETransmitManager SharedInstance] GetBLEDeviceByHandle:handle];
    if(dev == nil)
    {
        return ;
    }
    
    [dev setAlreadyBonded:YES];
};

@implementation GDBLEKeyManager

#pragma -mark   单例方法
//+ (instancetype)allocWithZone:(struct _NSZone *)zone
//{
//    if (_instance1) {
//        return _instance1;
//    }
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        _instance1 = [super allocWithZone:zone];
//        
//    });
//    return _instance1;
//}

+ (GDBLEKeyManager*)SharedInstance
{
    if (_instance1) {
        return _instance1;
    }
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance1 = [[self alloc] init];
        [_instance1 initVariable];
    });
    return _instance1;
}

- (id)copyWithZone:(NSZone *)zone
{
    return _instance1;
}
#pragma -mark   类成员变量初始化函数
-(void)initVariable
{
}
#pragma -mark   特殊功能函数

//连接函数
- (NSInteger)Connect:(unsigned long*)handle Dev:(GDBLEDevice*)dev Timeout:(unsigned long)timeout
{
    NSInteger ret = 0;
    do {
        
        //
        [[[GDBLETransmitManager SharedInstance] m_centralManager] connectPeripheral:[dev peripheral] options:nil];
        unsigned long startTime = ADDPRE(getMilSeconds());
        //等待连接成功
        while ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
            if (ADDPRE(getMilSeconds()) - startTime > timeout) {
                ret = CKR_BLE_BOND_FAIL;
                break;
            }
            if([dev connectedStatus] == BLES_UNCONNECT)
            {
                ret = CKR_BLE_BOND_FAIL;
                break;
            }
            [NSThread sleepForTimeInterval:0.010];
        }
        if(ret != CKR_OK)
        {
            break;
        }
        //如果需要配对的设备  等待配对成功
        if(([dev version] & 0x0f) != 0)
        {
            //注册bond回调
            [dev setAlreadyBonded:NO];
            [[GDBLETransmitManager SharedInstance] setM_bondCallback:GDDidBondFuncCallBack];

            //whl 工行、农行要用这个
#ifdef ICBC_ABC
            [[dev peripheral] readValueForCharacteristic:[dev writeChar]];
#else
            [[dev peripheral] readValueForCharacteristic:[dev notifyChar]];
#endif
            while ([dev alreadyBonded] != YES) {
                if (ADDPRE(getMilSeconds()) - startTime > timeout) {
                    ret = CKR_TIMEOUT;
                    break;
                }
                if([dev connectedStatus] == BLES_UNCONNECT)
                {
                    ret = CKR_USER_CANCEL;
                    break;
                }
                [NSThread sleepForTimeInterval:0.010];
            }
        }
        if(ret != CKR_OK)
        {
            break;
        }
        if(ret == CKR_TIMEOUT)
        {
            //取消连接
            [[[GDBLETransmitManager SharedInstance] m_centralManager] cancelPeripheralConnection:[dev peripheral]];
            //将设备放入扫描状态
            [dev setConnectedStatus:BLES_UNCONNECT];
            [[[GDBLETransmitManager SharedInstance] m_scanBLEDeviceList] addObject:dev];
            [[[GDBLETransmitManager SharedInstance] m_connectBLEDeviceList] removeObject:dev];
        }
        if (ret != CKR_OK) {
            break;
        }
        [dev setHandle:(unsigned long)dev];
        *handle = (unsigned long)dev;
    } while (0);
    return ret;
}

//断开连接函数
- (NSInteger)Disconnect:(unsigned long)handle
{
    NSInteger ret = 0;
    do {
        GDBLEDevice* dev = [[GDBLETransmitManager SharedInstance] GetBLEDeviceByHandle:handle];
        if(dev == nil)
        {
            ret = CKR_NO_DEVICE;
            return ret;
        }
        if ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
            if (ADDPRE(isPrintLog())) {
                NSLog(@"cancelPeripheralConnection");
            }
            [dev setConnectedStatus:BLES_ISDISCONNECTING];
            [[[GDBLETransmitManager SharedInstance] m_centralManager] cancelPeripheralConnection:[dev peripheral]];
        }
        else
        {
            //指令断

           
        }
    
    } while (0);
    return ret;
}




@end
