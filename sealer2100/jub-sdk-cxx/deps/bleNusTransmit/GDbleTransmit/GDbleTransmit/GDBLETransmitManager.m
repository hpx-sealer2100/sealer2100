//
//  BLETransmitManager.m
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//
//

#import <Foundation/Foundation.h>
#import "GDBLETransmitManager.h"

#import "GDBLEUtils.h"
#import "GDErrorCodesAndMacros.h"
#import "GDBLEDevice.h"
#import "GDBLEKeyManager.h"
#import "GDQueuePool.h"

static GDBLETransmitManager *_instance = nil;

@interface GDBLETransmitManager()

@property (nonatomic, assign) BOOL isFirstRecvData;
@property (nonatomic, assign) BOOL isRecvDone;
@property (nonatomic, strong) NSMutableData *recvDat;
@property (nonatomic, strong) NSMutableData *recvnewDat;
@property (nonatomic, assign) NSInteger Total_Length;
@property (nonatomic, assign) NSInteger CurrentLength;
@property (nonatomic, assign) NSInteger MessageType;


@end



@implementation  GDBLETransmitManager
{
    BLE_AUX_INIT_PARAM m_callBackPara;   //回调
    //centralManager
    NSInteger m_BLEState ;   //BLE的状态
    Boolean m_isAlreadyInit; //是否已经初始化过
    Boolean m_isScaning;     //是否正在扫描

    NSMutableDictionary* m_handle2PerMap;//对外句柄和内部句柄map。
    NSMutableDictionary* m_type2ManagerMap;//设备类型及其manager map。初始化写死.
    dispatch_queue_t m_callbackQueue; //扫描连接等回调函数
    char m_companyFirstTagList[10][16];//厂商首字符表示数组
    int  m_companyFirstTagListCount;
    char m_companyLastTagList[10][16];//厂商尾字符表示数组
    int  m_companyLastTagListCount;
    unsigned char m_supportProductTypeList[256];//支持设备列表
    int  m_supportProductTypeListCount;
    
    unsigned long ghandle;
    unsigned long m_last_start_time;
    
}
@synthesize m_centralManager;
@synthesize m_bondCallback;

@synthesize m_connectBLEDeviceList;

#pragma -mark   单例方法
//+ (instancetype)allocWithZone:(struct _NSZone *)zone
//{
//    if (_instance) {
//        return _instance;
//    }
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        _instance = [super allocWithZone:zone];
//    });
//    return _instance;
//}

+ (instancetype)SharedInstance
{
    if (_instance) {
        return _instance;
    }
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [[self alloc] init];
    });
    return _instance;
}

- (id)copyWithZone:(NSZone *)zone
{
    return _instance;
}

#pragma -mark   类成员变量初始化函数
-(void)initVariable
{
    m_BLEState = CKR_CRYPTOKI_NOT_INITIALIZED;
    m_isAlreadyInit = NO;
    m_isScaning = NO;
    self.isFirstRecvData = YES;
    self.isRecvDone = NO;
    self.recvDat = [NSMutableData data];
    self.recvnewDat = [NSMutableData data];
    self.Total_Length = 0;
    self.CurrentLength = 0;
    self.MessageType = 0;
    
    m_centralManager = nil;
    self.callingThread = nil;
//    m_scanBLEDeviceList = [[NSMutableArray alloc] initWithCapacity:100];
    self.m_scanBLEDeviceList = [NSMutableArray array];
    m_connectBLEDeviceList = [[NSMutableArray alloc]initWithCapacity:MAX_CONNECTED_DEVICE_COUNT];
    m_handle2PerMap = [[NSMutableDictionary alloc] initWithCapacity:10];
    m_type2ManagerMap = [[NSMutableDictionary alloc] initWithCapacity:10];
    [m_type2ManagerMap setObject:self forKey:[NSString stringWithFormat:@"%d",BLE_GENERAL_DEVICE]];
    GDBLEKeyManager* keyManager = [GDBLEKeyManager SharedInstance];
    [m_type2ManagerMap setObject:keyManager forKey:[NSString stringWithFormat:@"%d",BLE_KEY_DEVICE]];
    m_bondCallback = NULL;
    ADDPRE(SetPrintLog(true));//打开log
    //为通讯库的配置选项赋值
//    char * tempCompanyFirstTag[] = SUPPORT_COMPANY_FIRST_TAG;
//    char * tempCompanyLastTag[] = SUPPORT_COMPANY_LAST_TAG;
//    unsigned char tempSupportProductType[] = SUPPORT_PRODUCT_TYPE;
  //  [self SetLibConfig:tempCompanyFirstTag CompanySecondTag:tempCompanyLastTag SupportProductType:tempSupportProductType];
   
}

#pragma -mark  外部调用模块初始化  结束 函数

- (NSInteger)Initialize:(BLE_AUX_INIT_PARAM*)initPara
{
    [self initVariable];
    [[GDQueuePool SharedInstance] Initialize];
    NSInteger ret = 0 ;
    do {
        if(m_isAlreadyInit)
        {
            ret = CKR_CRYPTOKI_ALREADY_INITIALIZED;
            break;
        }
        //加载蓝牙centralManager
        dispatch_queue_t centralQueue = dispatch_queue_create("com.ftsafe.blereader", DISPATCH_QUEUE_CONCURRENT);
        m_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:centralQueue options:@{CBPeripheralManagerOptionShowPowerAlertKey:[NSNumber numberWithBool:YES]}];
        
        unsigned long ulstarttime = ADDPRE(getMilSeconds());
        //判断权限
        while(m_BLEState == CKR_CRYPTOKI_NOT_INITIALIZED)
        {
            if (ADDPRE(getMilSeconds()) - ulstarttime > INITIALIZE_TIMEOUT) {
                ret = CKR_TIMEOUT;
                break;
            }
            [NSThread sleepForTimeInterval:1];
        }
        if (m_BLEState == CKR_CRYPTOKI_NOT_INITIALIZED) {
            break;
        }
        if (m_BLEState != CBCentralManagerStatePoweredOn) {
            switch (m_BLEState) {
                case CBCentralManagerStateUnsupported:
                    ret = CKR_BLE_NOT_SUPPORT;
                    break;
                case CBCentralManagerStateUnauthorized:
                    ret = CKR_BLE_NOT_AUTHORIZE;
                    break;
                case CBCentralManagerStatePoweredOff:
                    ret = CKR_BLE_POWEROFF;
                    break;
                default:
                    ret = CKR_BLE_UNKNOW;
                    break;
            }
            break;
        }
        //注册回调
        if (initPara != NULL) {
            m_callBackPara.connFuncCallBack = initPara->connFuncCallBack;
            m_callBackPara.disconnFuncCallBack = initPara->disconnFuncCallBack;
            m_callBackPara.rcvDataFuncCallBack = initPara->rcvDataFuncCallBack;
            m_callBackPara.scanFuncCallBack = initPara->scanFuncCallBack;
        }
        //扫描队列
        dispatch_queue_t normal_queue ;
        [[GDQueuePool SharedInstance] GetFreeQueue:&normal_queue];
        m_callbackQueue = normal_queue;
        m_isAlreadyInit = YES;
    } while (0);
    return ret;
}

- (NSInteger)FinalizedBLE
{
    NSInteger ret = 0 ;
    
    
    NSLog(@"release---------->relasewallet");
    do {
        if(!m_isAlreadyInit)
        {
            NSLog(@"1release---------->relasewallet");
            ret = CKR_CRYPTOKI_NOT_INITIALIZED;
            break;
        }
        [[GDQueuePool SharedInstance] Finalized];
        //释放蓝牙centralManager
        
        
        
    
        //注册回调
        m_callBackPara.connFuncCallBack = NULL;
        m_callBackPara.disconnFuncCallBack = NULL;
        m_callBackPara.rcvDataFuncCallBack = NULL;
        m_callBackPara.scanFuncCallBack = NULL;
        m_isAlreadyInit = NO;
        
        
        if (m_centralManager == nil) {
            
            return ret;
        }
        
        [m_centralManager stopScan];
         m_centralManager = nil;
        
        
    } while (0);
    return ret;
}
//

/*
- (NSInteger)SetLibConfig:(char *[])companyFirstTag CompanySecondTag:(char *[]) companySecondTag SupportProductType:(unsigned char*)supportProductType
{
    NSInteger ret = 0;
    do {
        //判断参数有效性
        if (companyFirstTag == NULL || companySecondTag ==NULL || supportProductType == NULL) {
            ret = CKR_ARGUMENTS_BAD;
            break;
        }
        //为通讯库的配置选项赋值

        if((atoi(companyFirstTag[0])  <= 10))
//         &&  (sizeof(companyFirstTag) / (sizeof(char*)) == atoi(companyFirstTag[0]) + 1)
        {
            m_companyFirstTagListCount = atoi(companyFirstTag[0]);
            for (int i = 0; i < m_companyFirstTagListCount; i++) {
                if (strlen(companyFirstTag[i+1]) < 16) {
                    memcpy(&m_companyFirstTagList[i], companyFirstTag[i+1], strlen(companyFirstTag[i+1]));
                }
            }
        }
        else
        {
            //无过滤
            m_companyFirstTagListCount = 0;
        }
        
        if((atoi(companySecondTag[0])  <= 10))
//          && (sizeof(companySecondTag) / sizeof(char*) == atoi(companySecondTag[0]) + 1)
        {
            //有过滤
            m_companyLastTagListCount = atoi(companySecondTag[0]);
            for (int i = 0; i < m_companyLastTagListCount; i++) {
                if (strlen(companySecondTag[i+1]) < 16) {
                    memcpy(&m_companyLastTagList[i], companySecondTag[i+1], strlen(companySecondTag[i+1]));
                }
            }
        }
        else
        {
            //无过滤
            m_companyLastTagListCount = 0;
        }
        
        m_supportProductTypeListCount = supportProductType[0];
        for (int i = 0 ; i < m_supportProductTypeListCount; i++) {
            m_supportProductTypeList[i] = supportProductType[i+1];
        }
    } while (0);
    return ret;
}
*/
//扫描函数
#pragma -mark   蓝牙功能函数
-(NSInteger)CheckBLEState
{
    NSInteger ret = 0 ;
    do {
        //检查蓝牙资源
        if(!m_isAlreadyInit)
        {
            ret = CKR_CRYPTOKI_NOT_INITIALIZED;
            break;
        }
        if (m_BLEState != CBCentralManagerStatePoweredOn) {
            switch (m_BLEState) {
                case CBCentralManagerStateUnsupported:
                    ret = CKR_BLE_NOT_SUPPORT;
                    break;
                case CBCentralManagerStateUnauthorized:
                    ret = CKR_BLE_NOT_AUTHORIZE;
                    break;
                case CBCentralManagerStatePoweredOff:
                    ret = CKR_BLE_POWEROFF;
                    break;
                default:
                    ret = CKR_BLE_UNKNOW;
                    break;
            }
            break;
        }
    } while (0);
    return ret;
}
- (NSInteger)Scan
{
    NSInteger ret = CKR_OK ;
    do {
        //检查蓝牙资源
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        [self.m_scanBLEDeviceList removeAllObjects];
        [m_centralManager scanForPeripheralsWithServices:nil options:nil];
        m_isScaning = YES;
    } while (0);
    return ret;
}

- (NSInteger)StopScan
{
    NSInteger ret = CKR_OK ;
    do {
        //检查蓝牙资源
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        //打开扫描
        if (m_isScaning) {
            [m_centralManager stopScan];
            m_isScaning = NO;
        }
    } while (0);
    return ret;
}

//触发配对
- (NSInteger)StartBond:(unsigned long) handle
{
    NSInteger ret = CKR_OK ;
    do {
        //检查蓝牙资源
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        //判断handle有效性
        GDBLEDevice* dev = [self GetBLEDeviceByHandle:handle];
        if (dev == nil) {
            ret = CKR_ARGUMENTS_BAD;
            break;
        }
        if ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
            ret = CKR_DEVICE_IS_BUSY;
            break;
        }
        //读个数 触发蓝牙的配对
        [[dev peripheral] readValueForCharacteristic:[dev notifyChar]];
    } while (0);
    return ret;
}


- (NSInteger)Connect:(const char*) uuid Handle:(unsigned long*)handle Timeout:(unsigned long)timeout
{
    //判断设备类型  如果设备是key  调用key的自定义连接接口  add later
    NSInteger ret = 0;
    do {
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        //判断是否为有效uuid
        int i = 0;
        NSString * struid =  [NSString stringWithUTF8String:uuid];
        NSLog(@"strudi---->%@",struid);
        GDBLEDevice* dev;
        for (i= 0; i < [self.m_scanBLEDeviceList count]; i++) {
            dev = [self.m_scanBLEDeviceList objectAtIndex:i];
            if ([[dev uuid] isEqualToString:[NSString stringWithUTF8String:uuid]]) {
                break;
            }
        }
        if (i == [self.m_scanBLEDeviceList count]) {
            ret = CKR_ARGUMENTS_BAD;
            break;
        }
        //连接设备  从扫描列表移除  添加到连接列表
        [dev setConnectedStatus:BLES_ISCONNECTING];
        [m_connectBLEDeviceList addObject:dev];
//        [self.m_scanBLEDeviceList removeObject:dev];
        //调各种type设备对应的连接
        switch ([dev type]) {
            case BLE_KEY_DEVICE:
                //执行key的连接  并返回
                ret = [[GDBLEKeyManager SharedInstance] Connect:handle Dev:dev Timeout:timeout];
                return ret;
//                break;
            default:
                break;
        }
        if (ADDPRE(isPrintLog())) {
            NSLog(@"start connect device");
        }
        [m_centralManager connectPeripheral:[dev peripheral] options:nil];
        unsigned long startTime = ADDPRE(getMilSeconds());
        //等待连接成功
        while ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
            if([dev connectedStatus] == BLES_UNCONNECT)
            {
                ret = CKR_USER_CANCEL;
                break;
            }
            if (ADDPRE(getMilSeconds()) - startTime > timeout) {
                //取消连接
                [m_centralManager cancelPeripheralConnection:[dev peripheral]];
                //将设备放入扫描状态
                [dev setConnectedStatus:BLES_UNCONNECT];
//                [self.m_scanBLEDeviceList addObject:dev];
                [m_connectBLEDeviceList removeObject:dev];
                ret = CKR_TIMEOUT;
                break;
            }
        }
        if (ret != CKR_OK) {
            break;
        }
        [dev setHandle:(unsigned long)dev];
        self.currentPeripheral = dev.peripheral;
        *handle = (unsigned long)dev;
        
    } while (0);
    
    NSLog(@"ret------->%d",ret);
    return ret;
    
}

//取消连接
- (NSInteger)CancelConnect:(const char*) uuid
{
    NSInteger ret = 0;
    do {
        //根据uuid找到dev
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        //判断是否为有效uuid
        int i = 0;
        GDBLEDevice* dev;
        for (i= 0; i < [m_connectBLEDeviceList count]; i++) {
            dev = [m_connectBLEDeviceList objectAtIndex:i];
            if ([[dev uuid] isEqualToString:[NSString stringWithUTF8String:uuid]]) {
                break;
            }
        }
        if (i == [m_connectBLEDeviceList count]) {
            ret = CKR_ARGUMENTS_BAD;
            if (ADDPRE(isPrintLog())) {
                NSLog(@"cancel not found device");
            }
            break;
        }
        //判断dev的状态  调用断开连接接口。
        //补上dev的handle
        if([dev handle] == 0)
        {
            [dev setHandle:(unsigned long)dev];
        }
        [self Disconnect:(unsigned long)dev];
    } while (0);
    return ret;
}

//断开连接函数
- (NSInteger)Disconnect:(unsigned long)handle
{
    NSInteger ret = 0;
    do {
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        //判断handle有效性
        GDBLEDevice* dev = [self GetBLEDeviceByHandle:handle];
        if (dev == nil) {
            ret = CKR_ARGUMENTS_BAD;
            break;
        }
        //判断连接状态
        if ([dev connectedStatus] == BLES_ISDISCONNECTING
            ||[dev connectedStatus] == BLES_UNCONNECT) {
            ret = CKR_DEVICE_IS_BUSY;
            break;
        }
        //调各种type设备对应的连接
//        switch ([dev type]) {
//            case BLE_KEY_DEVICE:
//                //执行key的断开连接  并返回
//                ret = [[GDBLEKeyManager SharedInstance] Disconnect:handle];
//                return ret;
//            default:
//                break;
//        }
        
        [dev setConnectedStatus:BLES_ISDISCONNECTING];
        [m_centralManager cancelPeripheralConnection:[dev peripheral]];
        while ([dev connectedStatus] != BLES_UNCONNECT) {
            [NSThread sleepForTimeInterval:0.010];
        }
    } while (0);
    return ret;
    
}

-(NSInteger)SendDatafirst:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len messagetype:(unsigned int)messagetype
{
    
    NSInteger ret = 0 ;
    unsigned char cmd[9]={0};
    unsigned char tmp[4096]={0};
    unsigned int length = 0;
    
    cmd[0] = 0x3F;
    cmd[1] = 0x23;
    cmd[2] = 0x23;
    
    cmd[3] = (unsigned char) (messagetype >> 8) & 0xFF;
    cmd[4] = (unsigned char) messagetype & 0xFF;
    
    cmd[5] =  (unsigned char) (len >> 24) & 0xFF;
    cmd[6] =  (unsigned char) (len >> 16) & 0xFF;
    cmd[7] =  (unsigned char) (len >> 8) & 0xFF;
    cmd[8] =  (unsigned char) len & 0xFF;
    
    
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    
    memcpy(tmp, cmd, 9);
    
    NSLog(@"llll----->>>%d",len);

    int ff = len;
    
    if(ff > 55)
    {
        
        i = ff-55;
        
        j = i/63;
        
        k = i%63;
        
        if(j>0)  //>63
        {
            
            memcpy(tmp+9, data, 55);
            
            
           if(k==0)
           {
               for(int o=0;o<j;o++)
               {
                   tmp[64+64*o] = 0x3F;
                   memcpy(tmp+64*(o+1)+1,data+55+63*o,63);
                   
               }
               
               length = 64*(j+1);
               
           }else
           {
               
               for(int o=0;o<j;o++)
               {
                   tmp[64+64*o] = 0x3F;
                   memcpy(tmp+64*(o+1)+1,data+55+63*o,63);
                   
               }
               tmp[64+64*j] = 0x3f;
               
               memcpy(tmp+64*(j+1)+1, data+55+63*j, k);
               
               unsigned int  u = 63-k;
               
               for(int i = 0; i< u;i++)
               {
                   tmp[len + j + 10+i]=0x00;
               }
               
               length = len+j+10+u;
               
           }
            
        }else      // <63
        {
           
            memcpy(tmp+9, data, 55);
            int llent = ff -55;
            tmp[64]=0x3F;
            memcpy(tmp+65, data+55, llent);
            int kk = 63-llent;
            for(int jj=0;jj<kk;jj++)
            {
                tmp[65+llent+jj]=0x00;
            }
            length = kk+len+10;
        }
        
    }else
    {
        
        int kk = 55 - ff;
        memcpy(tmp+9, data, ff);
        for(int jj=0;jj<kk;jj++)
        {
            tmp[9+len+jj]=0x00;
        }
        length = 9+len+kk;
    
    }
    
    NSLog(@"length--------->%d",length);

    NSMutableData *RevData = [NSMutableData data];
    [RevData appendBytes:tmp length:length];
    NSMutableString *deviceTokenString = [NSMutableString string];
    const char *bytes = (const char *)RevData.bytes;
    NSInteger count = RevData.length;
     for (int i = 0; i < count; i++) 
     {
        if((i+1)%4 == 0)
        {
            if(i+1 == count)
            {
                [deviceTokenString appendFormat:@"%02x", bytes[i]&0x000000FF];
            }else
            {
                [deviceTokenString appendFormat:@"%02x ", bytes[i]&0x000000FF];
            }
        }else
        {
            [deviceTokenString appendFormat:@"%02x", bytes[i]&0x000000FF];
        }
        
      }
                             
    NSString *str1 =  [NSString stringWithFormat:@"<%@>", deviceTokenString];
    
    
    
    NSLog(@"zhuhe--------------->%@",str1);

    
    ret = [self SendData:handle Data:tmp Length:length];
    
    return ret;
    
}




-(NSInteger)SendDatafirstnew:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len recvData:(unsigned char*)recvData Length:(unsigned int*)recvlen Timeout:(unsigned int)timeout;
{
    
    NSLog(@"------------------>sendDataFirst----len==------->%d",len);
    NSInteger ret = 0 ;
    
    [self resetTransmitFlag];
    ghandle = handle;
    self.callingThread = [NSThread currentThread];
    
    unsigned long startTime = ADDPRE(getMilSeconds());
    BOOL istimeout  = NO;
    
    NSMutableData* datasend = nil;
    
    if(data == NULL || len == 0) {
        return -1;
    }else {
        datasend  = [NSMutableData dataWithBytes:data length:len];
    }
    
    [NSThread detachNewThreadSelector:@selector(SendDatanew:) toTarget:self withObject:datasend];
    
    while (!self.isRecvDone) {
        if (ADDPRE(getMilSeconds()) - startTime > timeout) {
            self.isRecvDone = YES;
            istimeout = YES;
        }
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
    
    
    if(istimeout)
    {
        NSLog(@"send and recv--------->timeout");
        return CKR_TIMEOUT;
    }
    
    
    NSLog(@"recvnew------->%@----->%lu",self.recvnewDat,self.recvnewDat.length);
    
//    recvData = (unsigned char*)[self.recvnewDat bytes];
    if ([self.recvnewDat length] > *recvlen) {
        return CKR_BUFFER_TOO_SMALL;
    }
    
    [self.recvnewDat getBytes:recvData length:self.recvnewDat.length];
    *recvlen = (unsigned int)[self.recvnewDat length];
    
  
    return  ret;
    
    
}
-(NSInteger)SendDataonly:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len;
{
    
    NSLog(@"------------------>SendDataonly----len==------->%d",len);
    NSInteger ret = 0 ;
    
    if(data[0] == 0x3F && data[1] == 0x23 && data[2] == 0x23)
    {
        [self resetTransmitFlag];   
    }
    if(data[0] == 0x23 && data[1] == 0x3F && data[2] == 0x3F)
    {
         [self resetTransmitFlag];  
    }

    ghandle = handle;
    self.callingThread = [NSThread currentThread];
    
    m_last_start_time = ADDPRE(getMilSeconds());
//    BOOL istimeout  = NO;
    
    NSMutableData* datasend = nil;
    
    if(data == NULL || len == 0) {
        return -1;
    }else {
        datasend  = [NSMutableData dataWithBytes:data length:len];
    }
    
    [NSThread detachNewThreadSelector:@selector(SendDatanew:) toTarget:self withObject:datasend];   
    
  
    return  ret;
    
    
}

-(NSInteger)ReceiveDataOnly:(unsigned long)handle recvData:(unsigned char*)recvData Length:(unsigned int*)recvlen Timeout:(unsigned int)timeout;
{
    
    NSLog(@"------------------>ReceiveDataOnly----");
    NSInteger ret = 0 ;
    
    
    ghandle = handle;
    self.callingThread = [NSThread currentThread];

    BOOL istimeout  = NO;
    
    while (!self.isRecvDone) {
        unsigned long currenttime = ADDPRE(getMilSeconds());
        if (currenttime- m_last_start_time > timeout) {
            self.isRecvDone = YES;
            istimeout = YES;
        }
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
    
    
    if(istimeout)
    {
        NSLog(@"send and recv--------->timeout");
        return CKR_TIMEOUT;
    }
    
    
    NSLog(@"recvnew------->%@----->%lu",self.recvnewDat,self.recvnewDat.length);
    [self.recvnewDat getBytes:recvData length:self.recvnewDat.length];
    *recvlen = (unsigned int)[self.recvnewDat length];
    
    return CKR_OK;
}

-(void)resetTransmitFlag
{
    //初始化状态标志
    self.isFirstRecvData = YES;
    self.isRecvDone = NO;
    
    self.Total_Length = 0;
    self.CurrentLength = 0;
    self.MessageType = 0;
    
    [self.recvDat resetBytesInRange: NSMakeRange(0, [self.recvDat length])];
    [self.recvDat setLength:0];
    
    [self.recvnewDat resetBytesInRange: NSMakeRange(0, [self.recvDat length])];
    [self.recvnewDat setLength:0];
    
    
//    TransmitResult = CKR_OK;
//    memset(recvDataBuf, 0, RECV_BUFFER_SIZE);
//    recvDataLen = 0;
    
    
    
}

-(NSInteger)SendDatanew:(NSData*)senddata
{

    
    unsigned char * data =  (unsigned char *)[senddata bytes];
    unsigned len = (unsigned int)[senddata length];
    
    
    [self resetTransmitFlag];
    
    NSInteger ret = 0 ;
    do {
        
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        //判断handle有效性
        GDBLEDevice* dev = [self GetBLEDeviceByHandle:ghandle];
        if (dev == nil) {
            ret = CKR_ARGUMENTS_BAD;
            break;
        }
        if ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
            ret = CKR_DEVICE_IS_BUSY;
            break;
        }
        //收发数据要上同步锁
        @synchronized (dev) {
            //设置response的初始值
            [dev setAlreadySendSubPacketWithResponse:YES];
            [dev setSendResponseCurWaitTimes:0];
            unsigned int curSendOffset = 0;
            NSData* sendData = nil;
            //获取dev的分包包长和每包之间的间距时间
            while (true) {
                //判断设备的连接状态
                if ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
                    ret = CKR_NO_DEVICE;
                    break;
                }
                if (len - curSendOffset > [dev eachPackageSize]) {
                    sendData = [NSData dataWithBytes:data+curSendOffset length:[dev eachPackageSize]];
                    //判断特征是否支持CBCharacteristicPropertyWriteWithoutResponse
//                    if([dev writeChar].properties & CBCharacteristicPropertyWriteWithoutResponse)
//                    {
//                        NSLog(@"writeChar-----%@",dev.writeChar);
//                        if (ADDPRE(isPrintLog())) {
//
//
//                            NSLog(@"1---->senddata %@",sendData);
//                        }
//                        [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithoutResponse];
//                    }
//                    else
//                    {
                        //每次要等待上一包发送完成
                        if([dev alreadySendSubPacketWithResponse])
                        {
                            if (ADDPRE(isPrintLog())) {
                                NSLog(@"senddata %@",sendData);
                            }
                            [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithoutResponse];
                        }
                        else
                        {
                            //如果未完成要继续等，直到次数达到20次  每次1ms
                            if ([dev SendResponseCurWaitTimes] < 20) {
                                [dev setSendResponseCurWaitTimes:[dev SendResponseCurWaitTimes] + 1];
                                [NSThread sleepForTimeInterval:0.01];
                            }
                            else
                            {
                            //超过20次算超时
                                ret = CKR_TIMEOUT;
                                break;
                            }
                            continue;
                        }
//                    }
                    curSendOffset += [dev eachPackageSize];
                }
                else
                {
                    sendData = [NSData dataWithBytes:data+curSendOffset length:(len - curSendOffset)];
                    if (ADDPRE(isPrintLog())) {
                        NSLog(@"senddata %@",sendData);
                    }
//                    //判断特征是否支持CBCharacteristicPropertyWriteWithoutResponse
//                    if([dev writeChar].properties & CBCharacteristicPropertyWriteWithoutResponse)
//                    {
//
//                        NSLog(@"--------outResponse");
//                        [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithoutResponse];
//                    }
//                    else
//                    {
                        NSLog(@"11--------response");
                        [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithoutResponse];
//                    }
                    break;
                }
                [NSThread sleepForTimeInterval:0.004];
            }
        }
    } while (0);
    
    
    
    
    return ret;
    
    
    
}

-(NSInteger)SendData:(unsigned long)handle Data:(const unsigned char*)data Length:(unsigned int)len
{
    
    [self resetTransmitFlag];
    
    NSInteger ret = 0 ;
    do {
        
        ret = [self CheckBLEState];
        if (ret != CKR_OK) {
            break;
        }
        //判断handle有效性
        GDBLEDevice* dev = [self GetBLEDeviceByHandle:handle];
        if (dev == nil) {
            ret = CKR_ARGUMENTS_BAD;
            break;
        }
        if ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
            ret = CKR_DEVICE_IS_BUSY;
            break;
        }
        //收发数据要上同步锁
        @synchronized (dev) {
            //设置response的初始值
            [dev setAlreadySendSubPacketWithResponse:YES];
            [dev setSendResponseCurWaitTimes:0];
            unsigned int curSendOffset = 0;
            NSData* sendData = nil;
            //获取dev的分包包长和每包之间的间距时间
            while (true) {
                //判断设备的连接状态
                if ([dev connectedStatus] != BLES_CONNECTED_CAN_SEND) {
                    ret = CKR_NO_DEVICE;
                    break;
                }
                if (len - curSendOffset > [dev eachPackageSize]) {
                    sendData = [NSData dataWithBytes:data+curSendOffset length:[dev eachPackageSize]];
                    //判断特征是否支持CBCharacteristicPropertyWriteWithoutResponse
//                    if([dev writeChar].properties & CBCharacteristicPropertyWriteWithoutResponse)
//                    {
//                        NSLog(@"writeChar-----%@",dev.writeChar);
//                        if (ADDPRE(isPrintLog())) {
//                        
//                            
//                            NSLog(@"1---->senddata %@",sendData);
//                        }
//                        [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithoutResponse];
//                    }
//                    else
//                    {
                        //每次要等待上一包发送完成
                        if([dev alreadySendSubPacketWithResponse])
                        {
                            if (ADDPRE(isPrintLog())) {
                                NSLog(@"senddata %@",sendData);
                            }
                            [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithResponse];
                        }
                        else
                        {
                            //如果未完成要继续等，直到次数达到20次  每次1ms
                            if ([dev SendResponseCurWaitTimes] < 20) {
                                [dev setSendResponseCurWaitTimes:[dev SendResponseCurWaitTimes] + 1];
                                [NSThread sleepForTimeInterval:0.001];
                            }
                            else
                            {
                            //超过20次算超时
                                ret = CKR_TIMEOUT;
                                break;
                            }
                            continue;
                        }
//                    }
                    curSendOffset += [dev eachPackageSize];
                }
                else
                {
                    sendData = [NSData dataWithBytes:data+curSendOffset length:(len - curSendOffset)];
                    if (ADDPRE(isPrintLog())) {
                        NSLog(@"senddata %@",sendData);
                    }
//                    //判断特征是否支持CBCharacteristicPropertyWriteWithoutResponse
//                    if([dev writeChar].properties & CBCharacteristicPropertyWriteWithoutResponse)
//                    {
//                        
//                        NSLog(@"--------outResponse");
//                        [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithoutResponse];
//                    }
//                    else
//                    {
                        NSLog(@"11--------response");
                        [[dev peripheral] writeValue:sendData forCharacteristic:[dev writeChar] type:CBCharacteristicWriteWithResponse];
//                    }
                    break;
                }
                [NSThread sleepForTimeInterval:0.001];
            }
        }
    } while (0);
    return ret;
}









-(void)BLEPowerOff
{
    
    NSLog(@"------->relase99");
    GDBLEDevice * dev = nil;
    while ([m_connectBLEDeviceList count]) {
        dev = [m_connectBLEDeviceList objectAtIndex:0] ;
        if (dev != nil) {
            [dev setConnectedStatus:BLES_UNCONNECT];
            //回调
            [self DisconnectCallback:dev];
            //修改连接列表
            [m_connectBLEDeviceList removeObject:dev];
        }
        else
        {
            break;
        }
    }
    
}

#pragma mark - FT蓝牙辅助函数
//type OUT   1--service  2--写char  3--读char





#pragma mark - 系统代理
#pragma mark - centralManager代理
-(void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    m_BLEState = [central state];
    switch ([central state])
    {
        case CBCentralManagerStateUnsupported:
//            state = @"The platform/hardware doesn't support Bluetooth Low Energy.";
//            isPhoneHaveBleAbility = NO;
            break;
        case CBCentralManagerStateUnauthorized:
//            state = @"The app is not authorized to use Bluetooth Low Energy.";
//            isAppAuthorize = NO;
            break;
        case CBCentralManagerStatePoweredOff:
//            state = @"Bluetooth is currently powered off.";
//            isPhoneHaveBleAbility = YES;
//            isAppAuthorize = YES;
//            isBluetoothPowerOn = NO;
//            
//            //蓝牙断开连接主动调用断开连接回调
            [self BLEPowerOff];
            break;
        case CBCentralManagerStatePoweredOn:
//            state = @"Bluetooth power on";
//            isPhoneHaveBleAbility  = YES;
//            isAppAuthorize = YES;
//            isBluetoothPowerOn = YES;
            break;
        case CBCentralManagerStateUnknown:
            
        default:
//            state = @"Unknown State";
            break;
    }
}

-(void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI
{
    
    
    NSString *device = peripheral.name;
//    NSLog(@"扫描uuid---->%@",peripheral.identifier.UUIDString);
//    NSLog(@"扫描name----->>>%@",device);
    

    //过滤wallet  不是wallet直接return  不向上层反
    if(![device containsString:@"HyperMateMax"] && ![device containsString:@"Sealer2100"] && ![device containsString:@"Sealer"]&& ![device containsString:@"sansec"])
    {
        return;
    }
    
    
    //通过广播数据创建BLEDevice类
    BOOL needCallback = YES;
    GDBLEDevice * dev = [self BuildDevByPer:peripheral Adv:advertisementData RSSI:RSSI];
    for(int i = 0 ; i < [self.m_scanBLEDeviceList count]; i++)
    {
        GDBLEDevice * devTemp =[self.m_scanBLEDeviceList objectAtIndex:i];
        if ([[devTemp uuid] isEqualToString:[dev uuid]]  ) {
            [self.m_scanBLEDeviceList removeObject:devTemp];
            needCallback = NO;
            break;
        }
    }
    [self.m_scanBLEDeviceList addObject:dev];
    
    //给上层回调
    if(needCallback)
    {
        [self ScanCallback:dev];
    }
}

-(void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    if (ADDPRE(isPrintLog())) {
        NSLog(@"------------>ddidConnectPeripheral");
    }
    [peripheral setDelegate:self];
    
//    NSMutableArray *uuids = [[NSMutableArray alloc] initWithObjects:[CBUUID UUIDWithString:UUIDSTR_DEVICE_INFO_SERVICE], [CBUUID UUIDWithString:UUIDSTR_ISSC_PROPRIETARY_SERVICE], [CBUUID UUIDWithString:UUIDSTR_C3_PROPRIETARY_SERVICE], [CBUUID UUIDWithString:UUIDSTR_E1_PROPRIETARY_SERVICE], [CBUUID UUIDWithString:UUIDSTR_C4_PROPRIETARY_SERVICE], [CBUUID UUIDWithString:UUIDSTR_SMART_C4_PROPRIETARY_SERVICE], [CBUUID UUIDWithString:UUIDSTR_NORDIC_PROPRIETARY_SERVICE], nil];
    
    [peripheral discoverServices:nil];
    
    
    GDBLEDevice* dev = [self getBLEDeviceFromConnectList:peripheral];
    if (dev == nil) {
        //设备已移除
        return;
    }
    //添加通用回调队列
    [dev setConnectedStatus:BLES_CONNECTED_DISVERING_CHAR];
}





-(void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    [self RemoveBLEDeviceFromConnectList:peripheral];
}

-(void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    
    NSLog(@"fffg--------->disconnect");
    GDBLEDevice * dev = [self getBLEDeviceFromConnectList:peripheral];
    if (dev == nil) {
        return;
    }
    [dev setConnectedStatus:BLES_UNCONNECT];
    
    NSLog(@"fff--------->disconnect");
    //回调
    [self DisconnectCallback:dev];
    //修改连接列表
    [self RemoveBLEDeviceFromConnectList:[dev peripheral]];
}
#pragma mark 蓝牙设备代理




-(void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    if (ADDPRE(isPrintLog())) {
        NSLog(@"didDiscoverServices");
    }
    for (CBService* service in peripheral.services) {
        if([service.UUID isEqual:[CBUUID UUIDWithString:UUIDSTR_WALLET_PROPRIETARY_SERVICE]])
        {
            
            NSLog(@"---------->>>>didDiscoverServices");
            [peripheral discoverCharacteristics:nil forService:service];
            
            
        }
    }
    
  NSInteger ff = [peripheral maximumWriteValueLengthForType:CBCharacteristicWriteWithResponse];
    
    NSLog(@"maximum----------->%d",ff);
    
}

-(void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
    GDBLEDevice* dev = [self getBLEDeviceFromConnectList:peripheral];
    if (dev == nil) {
        if (ADDPRE(isPrintLog())) {
            NSLog(@"didDiscoverCharacteristicsForService but no peripheral");
        }
        //断开连接
        return;
    }
    
    
    
    
    for (CBCharacteristic* characteristic in service.characteristics) {
        
        
        //找寻写特征
        CBUUID* uuid = [characteristic UUID];
        
        
        if ( [uuid isEqual:[CBUUID UUIDWithString:UUIDSTR_WALLET_TRANS_WX]]) {
            NSLog(@"------writeChar---%@",characteristic);
            NSLog(@"Write Channel is open successed W uuid is %@",characteristic.UUID.UUIDString);
            
            [dev setWriteChar:characteristic];
//            if (ADDPRE(isPrintLog())) {
                NSLog(@"find write char");
//            }
        }
        //找寻读特征
        if ([uuid isEqual:[CBUUID UUIDWithString:UUIDSTR_WALLET_TRANS_NX]]) {
            
           
            [dev setConnectedStatus:BLES_CONNECTED_OPENING_NOTIFY];
            
            [dev setNotifyChar:characteristic];
            NSLog(@"------ReadChar---%@",characteristic);
            NSLog(@"notify Channel is open successed  R uuid is %@",characteristic.UUID.UUIDString);
            
            //打开通知
            [peripheral setNotifyValue:YES forCharacteristic:characteristic];
            
//            if (ADDPRE(isPrintLog())) {
                NSLog(@"find notify char");
//            }
        }
    }
    
}

-(void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    GDBLEDevice* dev = [self getBLEDeviceFromConnectList:peripheral];
    if (dev == nil) {
        //设备已移除
        [dev setConnectedStatus:BLES_UNCONNECT];
        return;
    }
    
    // Check if pairing was cancelled
    if (error != nil) {
        NSLog(@"Notification state update failed: %@", error);
        [dev setConnectedStatus:BLES_UNCONNECT];
        return;
    }

    //添加收发数据的队列
    dispatch_queue_t send_queue ;
    NSInteger ret = [[GDQueuePool SharedInstance] GetFreeQueue:&send_queue];
    if (ret != 0) {
        if (ADDPRE(isPrintLog())) {
            NSLog(@"GetFreeQueue Error %ld",(long)ret);
        }
        send_queue = dispatch_get_global_queue(0, 0);
    }
    [dev setSendSerialQueue:send_queue];
    dispatch_queue_t rcv_queue ;
    ret = [[GDQueuePool SharedInstance] GetFreeQueue:&rcv_queue];
    if (ret != 0) {
        if (ADDPRE(isPrintLog())) {
            NSLog(@"GetFreeQueue Error %ld",(long)ret);
        }
        rcv_queue = dispatch_get_global_queue(0, 0);
    }
    [dev setRecvSerialQueue:rcv_queue];
    [dev setConnectedStatus:BLES_CONNECTED_CAN_SEND];
    if (ADDPRE(isPrintLog())) {
        NSLog(@"BLES_CONNECTED_CAN_SEND");
    }
    [self ConnectCallback:dev];
}

-(void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (ADDPRE(isPrintLog())) {
        NSLog(@"rcv %@",characteristic.value);
    }
    GDBLEDevice* dev = [self getBLEDeviceFromConnectList:peripheral];
    if (dev == nil) {
        
        NSLog(@"设备移除");
        //设备已移除
        return;
    }
    
    
    NSLog(@"接收数据---------->%@",characteristic.value);
    NSData *data = characteristic.value;
    
    unsigned int recvLen = (unsigned int)[data length];
    unsigned char *tmpBuf = (unsigned char *)[data bytes];
    bool is_encrypted = false;
    if(self.isFirstRecvData)
    {
        self.isFirstRecvData = false;
        if(tmpBuf[0] != 0x3F && tmpBuf[1] != 0x23 && tmpBuf[2] != 0x23) {
            if(tmpBuf[0] != 0x23 && tmpBuf[1] != 0x3F && tmpBuf[2] != 0x3F) {
                NSLog(@"接收数据头错误，丢弃");
                return;
            }else{
                //这是收到了密文
                is_encrypted = true;
                if(tmpBuf[3] == 0x05){
                    //密文通讯数据
                    uint16_t iv_len = (uint16_t)(tmpBuf[4]);
                    uint16_t mac_tag_len = (uint16_t)(tmpBuf[4 + 1 + iv_len]);
                    uint16_t encrypted_msg_len = (tmpBuf[4 + 1 + iv_len + 1 + mac_tag_len]) << 8 |
                                                 (tmpBuf[4 + 1 + iv_len + 1 + mac_tag_len + 1]);

                    self.Total_Length = encrypted_msg_len + 3 + 1 + iv_len + 1 + mac_tag_len + 2;
                    self.CurrentLength = recvLen;
                }else{
                        //密文命令数据，一包就够了，直接返回
                        [self.recvDat appendData:data];
                        [self recvDone:dev andMessageType:0x00];
                        return;
                    }
            }
        }
        
    
        if(tmpBuf[0] == 0x3F && tmpBuf[1] == 0x23 && tmpBuf[2] == 0x23)
        {
            
            self.Total_Length = tmpBuf[5]*256*256*256+tmpBuf[6]*256*256+ tmpBuf[7]*256 +tmpBuf[8];
            self.MessageType = tmpBuf[3]*256+tmpBuf[4];
            
            NSLog(@"recev--length---------=====>%d",self.Total_Length);
            
            NSLog(@"MessageType---------=====>%d",self.MessageType);
            self.CurrentLength = recvLen -9;
        }

        
        if(self.CurrentLength >= self.Total_Length)
        {
            [self.recvDat appendData:data];
            [self recvDone:dev andMessageType:is_encrypted ? 0x00 : self.MessageType];
            return;
        }else
        {
            [self.recvDat appendData:data];
        }
        
    }else
    {
        
        self.CurrentLength += recvLen;
        
        if(self.CurrentLength >= self.Total_Length)
        {
            [self.recvDat appendData:data];
            
            [self recvDone:dev andMessageType:self.MessageType];
        }else
        {
            [self.recvDat appendData:data];
        }
        
    }
    
    
    
}


-(void)recvDone:(GDBLEDevice*)dev andMessageType:(NSInteger)msgtype
{

    unsigned char *tmpBuf = (unsigned char *)[self.recvDat bytes];
    if(tmpBuf[0] == 0x3F && tmpBuf[1] == 0x23 && tmpBuf[2] == 0x23)
    {
        //明文
        NSData*  subdata = [self.recvDat subdataWithRange:NSMakeRange(9, self.recvDat.length-9)];
        
        NSData * subdata1 = [self.recvDat subdataWithRange:NSMakeRange(3, 2)];
        
        [self.recvnewDat appendData:subdata1];
        [self.recvnewDat appendData:subdata];
        
        
        NSData*  subdata2 = [self.recvnewDat subdataWithRange:NSMakeRange(0, self.recvnewDat.length)];
        
        [self RecvCallback:dev Data:subdata2 MessageType:msgtype];
        
        [self awakeCallingThread];
    }else if(tmpBuf[0] == 0x23 && tmpBuf[1] == 0x3F && tmpBuf[2] == 0x3F)
    {
        //密文
        NSData *prefix = [NSData dataWithBytes:(unsigned char[]){0x00, 0x00} length:2];
        [self.recvnewDat appendData:prefix];
        [self.recvnewDat appendData:self.recvDat];
        
        NSLog(@"recvDat------->%@----->%lu",self.recvDat,self.recvDat.length);

        NSData *subdata2 = [self.recvnewDat subdataWithRange:NSMakeRange(0, self.recvnewDat.length)];

        [self RecvCallback:dev Data:subdata2 MessageType:msgtype];
        
        [self awakeCallingThread];
    }

    

    
    
}


-(void)awakeCallingThread
{
    
        [self performSelector:@selector(recvsetCompleted) onThread:self.callingThread withObject:nil waitUntilDone:NO];

}
-(void)recvsetCompleted
{
    self.isRecvDone = YES;
}


 - (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error
{
    
    NSLog(@"didwriteValue----------->%@",error);
    
    if (ADDPRE(isPrintLog())) {
        NSLog(@"didWriteValueForCharacteristic");
    }
    GDBLEDevice* dev = [self getBLEDeviceFromConnectList:peripheral];
    if (dev == nil) {
        //设备已移除
        return;
    }
}
#pragma mark - 成员变量相关函数
//通过广播信息给BLEDevice赋值
-(GDBLEDevice*)BuildDevByPer:(CBPeripheral*)per Adv:(NSDictionary<NSString *,id> *)adv RSSI:(NSNumber*)RSSI
{
    GDBLEDevice * dev =[[GDBLEDevice alloc]init];
    if (adv!=nil) {
        //防崩溃
        [dev setAdv:adv];
    }
    [dev setUuid:[[per identifier] UUIDString]];
    [dev setRssi:[RSSI intValue]];
//    NSLog(@"adv%@", adv);
    //根据UUID设置设备类型
    CBUUID* servicaUUID = [[adv valueForKey:CBAdvertisementDataServiceUUIDsKey] objectAtIndex:0];
    NSData* dserviceUUID = [servicaUUID data];
//    Byte bServiceUUID[100] = {0};
//    char cServiceUUID[100] = {0};
//    [dserviceUUID getBytes:bServiceUUID];
//    ADDPRE(HexToStr)(cServiceUUID,bServiceUUID,(unsigned int)dserviceUUID.length);
    BLE_DEV_ADV_INFO bleDevAdvInfo;
    memset(&bleDevAdvInfo, 0, sizeof(bleDevAdvInfo));
    //zxf
//    if([self CheckFTBLEDeviceByUUID:dserviceUUID BLEDevAdvInfo:&bleDevAdvInfo])
//    {
//        if(bleDevAdvInfo.devType == BLE_KEY_DEVICE)
//        {
            [dev setType:BLE_KEY_DEVICE];
//            [dev setVersion:bleDevAdvInfo.devVerion];
    
            [dev setEachPackageSize:DEV_KEY_MAX_PACKAGE_LEN];
            [dev setEachPackageWaitTime:DEV_KEY_EACH_PACKAGE_WAIT_TIME];
 
//        }
//        else
//        {
//            [dev setType:BLE_GENERAL_DEVICE];
//        }
//    }
//    else
//    {
        //如果没有服务的UUID
//        [dev setType:BLE_KEY_DEVICE];
//        [dev setVersion:1];
//        [dev setEachPackageSize:DEV_KEY_MAX_PACKAGE_LEN];
//        [dev setEachPackageWaitTime:DEV_KEY_EACH_PACKAGE_WAIT_TIME];
//    }
    [dev setConnectedStatus:BLES_UNCONNECT];
    [dev setPeripheral:per];
    [dev setAlreadyBonded:YES];
    return dev;
}

//通过CBPeripheral类在设备连接设备列表里找BLEDevice
-(GDBLEDevice*)getBLEDeviceFromConnectList:(CBPeripheral*)per
{
    GDBLEDevice* dev = nil;
    int i = 0;
    for(; i < [m_connectBLEDeviceList count]; i++)
    {
        dev = [m_connectBLEDeviceList objectAtIndex:i] ;
        if ([[dev peripheral] isEqual:per]) {
            break;
        }
    }
    if (i == [m_connectBLEDeviceList count]) {
        dev = nil;
    }
    return dev;
}
//通过handle获取BLEDevice
-(GDBLEDevice*)GetBLEDeviceByHandle:(unsigned long)handle
{
    GDBLEDevice* dev = nil;
    int i = 0;
    for(; i < [m_connectBLEDeviceList count]; i++)
    {
        dev = [m_connectBLEDeviceList objectAtIndex:i] ;
        if ([dev handle] == handle) {
            break;
        }
    }
    if (i == [m_connectBLEDeviceList count]) {
        dev = nil;
    }
    return dev;
}
-(bool) IsConnected:(unsigned long)handle
{
    bool ret = false;
    GDBLEDevice* dev = nil;
    int i = 0;
    for(; i < [m_connectBLEDeviceList count]; i++)
    {
        dev = [m_connectBLEDeviceList objectAtIndex:i] ;
        if ([dev handle] == handle) {
            break;
        }
    }
    if (i == [m_connectBLEDeviceList count]) {
        dev = nil;
    }
    if (dev) {
        if ([dev connectedStatus] == BLES_CONNECTED_CAN_SEND) {
            ret = true;
        }
    };
    return ret;
}

-(void)ISReConnected:(bool)on
{
    
    self.isReconnected = on;
    
}




//通过CBPeripheral类在设备连接设备列表里删除BLEDevice
-(Boolean)RemoveBLEDeviceFromConnectList:(CBPeripheral*)per
{
    Boolean ret = NO;
    GDBLEDevice* dev = nil;
    int i = 0;
    for(; i < [m_connectBLEDeviceList count]; i++)
    {
        dev = [m_connectBLEDeviceList objectAtIndex:i] ;
        if ([[dev peripheral] isEqual:per]) {
            ret = YES;
            [m_connectBLEDeviceList removeObject:dev];
            break;
        }
    }
    return ret;
}

#pragma mark - 对外回调函数
-(void)ScanCallback:(GDBLEDevice*) dev
{
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        if (m_callBackPara.scanFuncCallBack) {
            unsigned int devType = 0;
            if ([dev version] == 1) {
                devType = DEV_TYPE_BLE_JUSTWORK;
            }
            else{
                devType = DEV_TYPE_BLE;
            }
            m_callBackPara.scanFuncCallBack([dev.peripheral.name UTF8String],
                                            [[[dev.peripheral identifier] UUIDString] UTF8String],
                                            DEV_TYPE_BLE
                                            );
        }
    });
    
}

-(void)ConnectCallback:(GDBLEDevice*)dev
{
    dispatch_async(m_callbackQueue, ^{
    if(m_callBackPara.connFuncCallBack){
        m_callBackPara.connFuncCallBack([[dev uuid] UTF8String], [dev handle]);
    }
    });
}

-(void)DisconnectCallback:(GDBLEDevice*)dev
{
    dispatch_async(m_callbackQueue, ^{
    if(m_callBackPara.disconnFuncCallBack){
        NSLog(@"disconnect callback----->");
        m_callBackPara.disconnFuncCallBack([[dev uuid] UTF8String]);
    }
    });
}




-(void)RecvCallback:(GDBLEDevice*)dev Data:(NSData*)data MessageType:(NSInteger)msgtype
{
    dispatch_queue_t queue = nil;
    if([dev recvSerialQueue])
    {
        queue = [dev recvSerialQueue];
    }
    else
    {
        queue = dispatch_get_global_queue(0, 0);
    }
    dispatch_async([dev recvSerialQueue], ^{
    //bond成功回调
    if(m_bondCallback && (![dev alreadyBonded]) && (data != nil))
    {
        m_bondCallback([dev handle], (unsigned char*)[data bytes],(unsigned int)[data length]);
    }
    //数据返回回调
    else if (m_callBackPara.rcvDataFuncCallBack) {
        m_callBackPara.rcvDataFuncCallBack([dev handle], (unsigned char*)[data bytes],(unsigned int)[data length]);
    }
    });
}

@end
