//
//  BLEDevice.m
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//
//

#import <Foundation/Foundation.h>
#import "GDBLEDevice.h"

@implementation GDBLEDevice

@synthesize handle;
@synthesize type;
@synthesize adv;

@synthesize uuid;
@synthesize rssi;
@synthesize version;
@synthesize writeChar;
@synthesize notifyChar;
@synthesize connectedStatus;
@synthesize sendSerialQueue;
@synthesize recvSerialQueue;
@synthesize eachPackageSize;
@synthesize peripheral;
@synthesize eachPackageWaitTime;
@synthesize alreadyBonded;
@synthesize alreadySendSubPacketWithResponse;
@synthesize SendResponseCurWaitTimes;
@end
