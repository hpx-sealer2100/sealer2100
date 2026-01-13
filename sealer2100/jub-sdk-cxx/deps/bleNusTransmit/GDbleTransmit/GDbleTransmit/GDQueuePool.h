//
//  Header.h
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//

#import <Foundation/Foundation.h>


#define GDQueuePool   GD_GDQueuePool



@interface GDQueuePool : NSObject

+ (instancetype)SharedInstance;

-(NSInteger) Initialize;
-(NSInteger) Finalized;
-(NSInteger) GetFreeQueue :(dispatch_queue_t*)pqueue;
-(NSInteger) SetQueueFree :(dispatch_queue_t)queue;

@end
