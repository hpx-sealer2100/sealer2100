//
//  QueuePool.m
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//

#import <Foundation/Foundation.h>
#import "GDQueuePool.h"


static GDQueuePool *_instance = nil;

@implementation GDQueuePool
{
    Boolean   m_isInit;
    NSInteger m_currentQueueCount;
    NSInteger m_maxQueueCount;
    NSMutableArray* m_array;    //保存非空闲线程
    NSMutableArray* m_freeArray;//保存空闲的线程。
}

#pragma -mark   单例方法
+ (instancetype)allocWithZone:(struct _NSZone *)zone
{
    if (_instance) {
        return _instance;
    }
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [super allocWithZone:zone];
    });
    return _instance;
}

+ (instancetype)SharedInstance
{
    if (_instance) {
        return _instance;
    }
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [[self alloc] init];
        [_instance initPara];
    });
    return _instance;
}

- (id)copyWithZone:(NSZone *)zone
{
    return _instance;
}

#pragma -mark  接口方法
-(NSInteger) GetFreeQueue :(dispatch_queue_t*)pqueue
{
    NSInteger ret = 0;
    do {
        if (!m_isInit) {
            ret = 4;//未初始化
            break;
        }
        if(m_currentQueueCount >= m_maxQueueCount)
        {
            ret = 1; //无可用线程
            break;
        }
        if ([m_freeArray count] == 0) {
            NSString* lable = [NSString stringWithFormat:@"com.ftsafe.blequeue%d",(unsigned int)m_currentQueueCount++];
            dispatch_queue_t new_queue = dispatch_queue_create([lable UTF8String], DISPATCH_QUEUE_SERIAL);
            *pqueue = new_queue;
            [m_array addObject:new_queue];
        }
        else{
            *pqueue = [m_freeArray objectAtIndex:0];
            [m_array addObject:(*pqueue)];
            [m_freeArray removeObjectAtIndex:0];
        }
    } while (0);
    return ret;
}
-(NSInteger) SetQueueFree :(dispatch_queue_t)queue
{
    NSInteger ret = 0;
    do {
        if (!m_isInit) {
            ret = 4;//未初始化
            break;
        }
        if (queue == nil) {
            ret = 2;//参数错误
            break;
        }
        NSInteger count_begin = [m_array count];
        for (dispatch_queue_t temp_queue in m_array) {
            if (queue == temp_queue) {
                [m_freeArray addObject:queue];
                [m_array removeObject:queue];
                break;
            }
        }
        if ([m_array count] == count_begin) {
            ret = 3;//not found
            break;
        }
    } while (0);
    return ret;
}

#pragma -mark 初始化  及  回收资源方法
-(void) initPara
{
    m_isInit = NO;
    m_maxQueueCount = 30;
    m_currentQueueCount = 0;
    m_array = [[NSMutableArray alloc] initWithCapacity:20];
    m_freeArray = [[NSMutableArray alloc] initWithCapacity:20];
}
-(void)clear
{
    m_isInit = NO;
    m_maxQueueCount = 20;
    m_currentQueueCount = 0;
    [m_array removeAllObjects];
    [m_array removeAllObjects];
}

-(NSInteger) Initialize
{
    NSInteger ret = 0;
    do {
        [self initPara];
        m_isInit = YES;
    } while (0);
    return ret;
}
-(NSInteger) Finalized
{
    NSInteger ret = 0;
    do {
        [self clear];
    } while (0);
    return ret;
}
@end
