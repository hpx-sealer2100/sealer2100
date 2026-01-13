//
//  BLEKeyManager.h
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//
//

#import "GDBLETransmitManager.h"


#define GDBLEKeyManager   GD_GDBLEKeyManager




@interface GDBLEKeyManager : NSObject

+ (instancetype)SharedInstance;

//断开连接函数
- (NSInteger)Disconnect:(unsigned long)handle;

//连接函数
- (NSInteger)Connect:(unsigned long*)handle Dev:(GDBLEDevice*)dev Timeout:(unsigned long)timeout;

@end
