//
//  BLETransmitManager.h
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//
//

#import <Foundation/Foundation.h>
#import "GDBLETemplates.h"
#import <CoreBluetooth/CoreBluetooth.h>
#import "GDBLEDevice.h"


#define    GDBLETransmitManager   GD_GDBLETransmitManager



@interface GDBLETransmitManager : NSObject<CBCentralManagerDelegate, CBPeripheralDelegate>
@property (retain,nonatomic) CBCentralManager* m_centralManager;
@property (assign,nonatomic) BLE_AUX_DidBondFuncCallBack m_bondCallback;
@property (strong,nonatomic) NSMutableArray* m_scanBLEDeviceList; //扫描到的设备列表
@property (strong,nonatomic) NSMutableArray* m_connectBLEDeviceList; //连接上或正在连接的设备列表
@property (assign,nonatomic) bool isReconnected;
@property(nonatomic, strong) NSThread *callingThread;

@property(retain,atomic) CBPeripheral *currentPeripheral;

+ (instancetype)SharedInstance;
//模块初始化函数
- (NSInteger)Initialize:(BLE_AUX_INIT_PARAM*)initPara;
//模块使用结束函数
- (NSInteger)FinalizedBLE;
//设置通讯库参数
- (NSInteger)SetLibConfig:(char *[])companyFirstTag CompanySecondTag:(char *[]) companySecondTag SupportProductType:(unsigned char*)supportProductType;
//扫描函数
- (NSInteger)Scan;
//停止扫描函数
- (NSInteger)StopScan;
//触发配对
- (NSInteger)StartBond:(unsigned long) handle;
//连接函数   超时时间单位为ms
- (NSInteger)Connect:(const char*) uuid Handle:(unsigned long*)handle Timeout:(unsigned long)timeout;
//取消连接
- (NSInteger)CancelConnect:(const char*) uuid;
//断开连接函数
- (NSInteger)Disconnect:(unsigned long)handle;
//发送数据
-(NSInteger)SendDatafirst:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len messagetype:(unsigned int)messagetype;

-(NSInteger)SendData:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len;


//同步发送
-(NSInteger)SendDatafirstnew:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len recvData:(unsigned char*)recvData Length:(unsigned int*)recvlen Timeout:(unsigned int)timeout;

-(NSInteger)SendDataonly:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len;

-(NSInteger)ReceiveDataOnly:(unsigned long)handle recvData:(unsigned char*)recvData Length:(unsigned int*)recvlen Timeout:(unsigned int)timeout;

-(NSInteger)SendDatanew:(NSData*)senddata;




//根据handle 获取dev
-(GDBLEDevice*)GetBLEDeviceByHandle:(unsigned long)handle;
//查询是否已连接
-(bool) IsConnected:(unsigned long)handle;


-(void)ISReConnected:(bool)on;


@end
