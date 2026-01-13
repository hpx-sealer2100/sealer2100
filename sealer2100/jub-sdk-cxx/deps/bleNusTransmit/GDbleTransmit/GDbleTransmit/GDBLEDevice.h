//
//  BLEDevice.h
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//
//



#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>






@interface GDBLEDevice : NSObject

//扫描时添加的属性
@property (assign, nonatomic) unsigned long handle;   //设备句柄
@property (assign, nonatomic) int           type;     //设备类型
@property (retain, nonatomic) NSDictionary* adv;      //广播数据
@property (copy, nonatomic)   NSString*     uuid;     //uuid
@property (assign, nonatomic) int           rssi;     //广播信号强度
@property (assign, nonatomic) int        version;     //key的版本
@property (assign, nonatomic) int connectedStatus;          //现在连接状态
@property (retain, nonatomic) CBPeripheral* peripheral;

//连接后添加的属性

@property (retain, nonatomic) CBCharacteristic* writeChar;  //写特征
@property (retain, nonatomic) CBCharacteristic* notifyChar; //读特征
//@property (assign, nonatomic) pthread_mutex_t mutex;  //发送数据的锁，直接sy锁BLEDevice即可
@property (retain, nonatomic) dispatch_queue_t sendSerialQueue;//发送串行队列
@property (retain, nonatomic) dispatch_queue_t recvSerialQueue;//回调上反接收数据串行队列
@property (assign, nonatomic) int eachPackageSize;             //分包最大包长
@property (assign, nonatomic) float eachPackageWaitTime;       //两包之间的间隔时间
@property (assign, nonatomic) BOOL alreadyBonded;       //是否已配对完成
@property (assign, nonatomic) BOOL alreadySendSubPacketWithResponse;  //response每包的应答状态
@property (assign, nonatomic) int  SendResponseCurWaitTimes;      //SendResponse 当前等待次数
@end

