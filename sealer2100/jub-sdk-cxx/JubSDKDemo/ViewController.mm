//
//  ViewController.m
//  JubSDKDemo
//
//  Created by Pan Min on 2019/6/22.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "ViewController.h"
#import "JUB_SDK.h"
#import "DevicePool.h"
#import "Notification.h"
#import "Object.h"
#import "JubSDKCore+COIN_ETH.h"
#import "JubSDKCore+COIN_BTC.h"
#import "JubSDKCore+COIN_SOL.h"
#import "JubSDKCore+COIN_FIL.h"
#import "JubSDKCore+COIN_XRP.h"
#import "JubSDKCore+DEV_BLE.h"
#import "JubSDKCore+DEV.h"
#import "JUB_SDK_DEV_BLE_NUS.h"
#import "JubSDKCore.h"
#import <UIKit/UIKit.h>

JubSDKCore* _sdk;


@implementation NSData (HexString)
- (NSString *)hexString {
    const unsigned char *dataBuffer = (const unsigned char *)self.bytes;
    if (!dataBuffer) return [NSString string];

    NSUInteger dataLength  = self.length;
    NSMutableString *hexString  = [NSMutableString stringWithCapacity:(dataLength * 2)];
    for (int i = 0; i < dataLength; ++i)
        [hexString appendFormat:@"%02x", dataBuffer[i]];
    return [NSString stringWithString:hexString];
}
@end


@interface ViewController ()

@property (weak, nonatomic) IBOutlet UITableView *tableview;
@property (nonatomic, strong) NSMutableArray *scanArray;


@end

@implementation ViewController
@synthesize _deviceDict;

void Scan(unsigned char* devName, unsigned char* uuid, unsigned int type) {
    NSLog(@"Scan: [%s:%s]", devName, uuid);
    ScanDeviceInfo* deviceInfo = [[ScanDeviceInfo alloc] init];
    deviceInfo.name = [NSString stringWithCString:(char*)devName
                                         encoding:NSUTF8StringEncoding];
    deviceInfo.uuid = [NSString stringWithCString:(char*)uuid
                                         encoding:NSUTF8StringEncoding];
    deviceInfo.type = NSUInteger(type);
    [[NSNotificationCenter defaultCenter] postNotificationName:NOTIF_DEVICE_SCAN
                                                        object:nil
                                                      userInfo:@{@"uuid":[NSString stringWithCString:(char*)uuid
                                                                                            encoding:NSUTF8StringEncoding],
                                                                 @"device":deviceInfo
                                                               }];
}

void Disc(unsigned char* uuid) {
    NSLog(@"Disc: [%s]", uuid);
    [[NSNotificationCenter defaultCenter] postNotificationName:NOTIF_DEVICE_DISC
                                                        object:nil
                                                      userInfo:@{@"uuid":[NSString stringWithCString:(char*)uuid
                                                                                            encoding:NSUTF8StringEncoding]
                                                               }];
}

- (void)deviceArrivalObserver:notification {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self updateUI];
        
        DeviceInitParam* init = [[DeviceInitParam alloc] init];
        init.readCallBack = nil;
        init.scanCallBack = &Scan;
        init.discCallBack = &Disc;
        [_sdk JUB_initDevice:init];
        NSLog(@"[%li] JUB_initDevice(void)", (long)[_sdk JUB_LastError]);
    });
}

- (void)deviceScanObserver:(NSNotification *) notification {
    if (nil == [_deviceDict objectForKey:[[notification userInfo] objectForKey:@"uuid"]]) {
        
        [_deviceDict setObject:[[notification userInfo] objectForKey:@"device"]
                        forKey:[[notification userInfo] objectForKey:@"uuid"]];
    }
    
    [self updateUI];
}

- (void)updateUI {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (self->_deviceDict && 0 == [self->_deviceDict count]) {
            [self->_tableview setHidden:true];
        }
        else {
            self.scanArray = [NSMutableArray arrayWithArray:self._deviceDict.allValues];
            [self->_tableview setHidden:false];
            [self->_tableview reloadData];
        }
    });
}


- (NSInteger)tableView:(UITableView *)tableView
 numberOfRowsInSection:(NSInteger)section {
    if (   _deviceDict
        && _deviceDict.allKeys.count > 0
        ) {
        return _deviceDict.allKeys.count;
    }
    else {
        return 0;
    }
}
//panmin JUB_GetDeviceInfo

void (^notifyCallback)(NSUInteger messageType) = ^(NSUInteger messageType) {
    NSLog(@"notifyCallback called with messageType: %lu", (unsigned long)messageType);
    // 可以根据 messageType 做进一步处理
};
void (^percentageCallback)(NSUInteger percentage) = ^(NSUInteger percentage) {
    NSLog(@"percentageCallback called with percentage: %lu", (unsigned long)percentage);
    // 可以根据 percentage 做进一步处理
};

- (void)       tableView:(UITableView *)tableView
 didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    ScanDeviceInfo* deviceInfo = [self.scanArray objectAtIndex:indexPath.row];
    NSLog(@"selected--------name---->%@", deviceInfo.name);
    NSLog(@"selected--------uuid----->%@", deviceInfo.uuid);
    //    __deviceID = [_sdk JUB_connectDevice:deviceInfo.uuid connectType:deviceInfo.type timeout:30000];
    
    __deviceID =  [_sdk JUB_connectDevice:deviceInfo.name uuid:deviceInfo.uuid connectType:deviceInfo.type timeout:30000];
    
    NSLog(@"wwwwwwwwwwwwconnect ret-------------====>%d",__deviceID);
    
    NSLog(@"[%li] JUB_connectDevice(...)", [_sdk JUB_LastError]);
    
    if (__deviceID > 0) {
        NSString *strTmp = [_sdk JUB_GetFeatures:__deviceID];
        NSLog(@"[%li] wwwwwwwJUB_GetFeatures(%li),data = %@", __deviceID, (long)[_sdk JUB_LastError],strTmp);
        
        [_sdk JUB_BleNusSetNotifyMessageCallback:__deviceID
                                          notify:notifyCallback];
        NSLog(@"[%li] JUB_BleNusSetNotifyMessageCallback(%li)", __deviceID, (long)[_sdk JUB_LastError]);
        [_sdk JUB_BleNusSetUpdatePercentageCallback:__deviceID
                                         percentage:percentageCallback];
        NSLog(@"[%li] JUB_BleNusSetUpdatePercentageCallback(%li)", __deviceID, (long)[_sdk JUB_LastError]);
        [_sdk JUB_BleNusSetPassphraseCallback:__deviceID callback:^void(InputPassphrase* input){
            // input on app
            //            input->on_device = false;
            //            input->passphrase = @"xyzabc";
            
            // input on device
            input->on_device = true;
            input->passphrase = nil;
        }];
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                                   reuseIdentifier:@"cell"];
    ScanDeviceInfo* deviceInfo = [self.scanArray objectAtIndex:indexPath.row];
    cell.textLabel.text = deviceInfo.name;
    return cell;
}


- (IBAction)enumAction:(UIButton *)sender {
    [_sdk JUB_enumDevices];
    NSLog(@"[%li] JUB_enumDevices(void)", (long)[_sdk JUB_LastError]);
}

- (IBAction)stopEnumAction:(UIButton *)sender {
    [_sdk JUB_stopEnumDevices];
    NSLog(@"[%li] JUB_stopEnumDevices(void)", (long)[_sdk JUB_LastError]);
}

- (IBAction)loginAction:(UIButton *)sender {
    ContextConfigETH *cfg = [[ContextConfigETH alloc] init];
    cfg.mainPath = @"m/44'/60'/0'";
    cfg.chainID = 1;
    __contextID = [_sdk JUB_CreateContextETH:__deviceID
                                         cfg:cfg];
    NSLog(@"[%li] JUB_CreateContextETH(%li)", __deviceID, (long)[_sdk JUB_LastError]);
    BIP32Path *path = [[BIP32Path alloc] init];
    path.change = BOOL_NS_FALSE;
    path.addressIndex = 0;
    NSString *address = [_sdk JUB_GetAddressETH:__contextID path:path
                                          bShow:BOOL_NS_FALSE];
    NSLog(@"[%li] JUB_GetAddressETH(%@)", __contextID, address);
    
    NSString *rawTx =  [_sdk JUB_SignTransactionETH:__contextID
                                               path:path
                                              nonce:13
                                           gasLimit:310000
                                      gasPriceInWei:@"100000000"
                                                 to:@"0xed5c2f0b3e8f4a1c6b7c9d8e2f3b4a5c6d7e8f9a"
                                         valueInWei:@"1000000000000000000"
                                              input:@""];
    NSLog(@"[%li] JUB_SignTransactionETH: %@", __contextID, rawTx);
}
- (IBAction)SetLable:(UIButton *)sender{
    NSString * strTmp = @"new lable";
    [_sdk JUB_SetLable:__deviceID pLable:strTmp];
    NSLog(@"[%li] JUB_SetLable(%li)", __contextID,(long)[_sdk JUB_LastError]);
    [_sdk JUB_UsePassphrase:__deviceID bUse:FALSE];
    NSLog(@"[%li] JUB_UsePassphrase(%li)", __contextID,(long)[_sdk JUB_LastError]);
}

- (IBAction)GetFeatures:(UIButton *)sender{
    NSString *strTmp = [_sdk JUB_GetFeatures:__deviceID];
    NSLog(@"[%li] JUB_GetFeatures(%li),data = %@", __deviceID, (long)[_sdk JUB_LastError],strTmp);
    
}
- (IBAction)GetCertSignData:(UIButton *)sender{
    NSString *strcert = [_sdk JUB_GetDeviceCert:__deviceID];
    NSLog(@"[%li] JUB_GetDeviceCert(%li),data = %@", __deviceID, (long)[_sdk JUB_LastError],strcert);
    NSString * strData = @"11122233";
    NSString *signString = [_sdk JUB_GetDeviceSignData:__deviceID hashData:strData];
    NSLog(@"[%li] JUB_GetDeviceSignData(%li),data = %@", __deviceID, (long)[_sdk JUB_LastError], signString);
    
}
- (IBAction)EnumCoins:(UIButton *)sender{
    NSString *strTmp = [_sdk Jub_EnumSupportCoins:__deviceID];
    NSLog(@"[%li] Jub_EnumSupportCoins(%li),data = %@", __deviceID, (long)[_sdk JUB_LastError],strTmp);
    
}
- (IBAction)EndSessionAction:(UIButton *)sender{
        [_sdk JUB_EndSession:__deviceID];
        NSLog(@"[%li] JUB_EndSession(%li)", __contextID,(long)[_sdk JUB_LastError]);
    [_sdk JUB_OpCancel:__deviceID];
    NSLog(@"[%li] JUB_OpCancel(%li)", __contextID,(long)[_sdk JUB_LastError]);
    // 1. 获取 Documents 目录路径
//    NSString *documentsPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
//    
//    // 2. 拼接固件文件路径（假设叫 firmware.bin）
//    NSString *filePath = [documentsPath stringByAppendingPathComponent:@"firmware.bin"];
//    NSURL *fileURL = [NSURL fileURLWithPath:filePath];
//    
//    // 3. 读取文件内容
//    NSError *error;
//    NSData *firmwareData = [NSData dataWithContentsOfURL:fileURL options:0 error:&error];
//    
//    if (error) {
//        NSLog(@"❌ 读取文件失败: %@", error.localizedDescription);
//        return;
//    }
//    
//    if (!firmwareData || firmwareData.length == 0) {
//        NSLog(@"❌ 文件为空或不存在！请确保 firmware.bin 已放入 Documents 目录");
//        return;
//    }
//    
//    NSLog(@"✅ 文件大小: %lu 字节", (unsigned long)firmwareData.length);
//    
//    [_sdk JUB_UpdateFirmware:__deviceID firmwareFilePayload:firmwareData firmwareFileSize:firmwareData.length reboot_on_success:TRUE];
//    
//    NSLog(@"[%li] JUB_UpdateFirmware(%li)", __contextID,(long)[_sdk JUB_LastError]);
    
    
}
// This method is called once we click inside the textField
-(void)textFieldDidBeginEditing:(UITextField *)textField {
    NSLog(@"Text field did begin editing");
    
}
//- (IBAction)getPublicKey:(UIButton *)sender {
//    ContextConfigSOL *cfg = [[ContextConfigSOL alloc] init];
//    cfg.mainPath = @"m/44'/501'/0'/0'";
//    __contextID = [_sdk JUB_CreateContextSOL:__deviceID
//                                         cfg:cfg];
//    NSLog(@"[%li] JUB_CreateContextSOL(%li)", __deviceID, (long)[_sdk JUB_LastError]);
//    BIP32Path *path = [[BIP32Path alloc] init];
//    path.change = BOOL_NS_FALSE;
//    path.addressIndex = 1;
//    NSString *address = [_sdk JUB_GetAddressSOL:__contextID path:path bShow:BOOL_NS_FALSE];
//    NSLog(@"[%li] JUB_GetAddressSOL(%@)", __contextID, address);
//    dispatch_queue_t queue = dispatch_queue_create("getting address", NULL);
//    dispatch_async(queue, ^{
//        NSString *rawtx = [_sdk JUB_SignTxTokenCreateAndTransferSOL:__contextID path:path recentHash:@"7PKCCCdtNLkVsjCR2VKHG1nDxMXdsV9TSj69amCs3X7y" tokenMint:@"6p6xgHyF7AeE6TZkSmFsko444wqoP15icUSqi2jfGiPN" destMainAddress:@"42e5dwJkAMRDfQn18sGhrt2APaSeroasnVisYK7Sqbvs" source:@"3WVYpQd9E1qhJV5tqVPGxh16bQeF6M8oYzjZ77JieT3q" dest:@"NGZouQMPdc5HGGuSpXFTPz9jCZj4LRUdmXjXbqDb3R8" amount:@1000 decimal:6];
//        
//    //    NSString *rawtx = [_sdk JUB_SignTransactionTokenSOL:__contextID path:path recentHash:@"7PKCCCdtNLkVsjCR2VKHG1nDxMXdsV9TSj69amCs3X7y" tokenMint:@"6p6xgHyF7AeE6TZkSmFsko444wqoP15icUSqi2jfGiPN" source:@"3WVYpQd9E1qhJV5tqVPGxh16bQeF6M8oYzjZ77JieT3q" dest:@"NGZouQMPdc5HGGuSpXFTPz9jCZj4LRUdmXjXbqDb3R8" amount:@1000 decimal:6];
//        
//    //    NSString *rawtx = [_sdk JUB_SignTransactionSOL:__contextID path:path recentHash:@"7PKCCCdtNLkVsjCR2VKHG1nDxMXdsV9TSj69amCs3X7y" dest:@"DZv25oNCWFvGXu9tH63BiAXvG94syweGZhbvdN3HxDxT" amount:@1000];
//        NSLog(@"[%li] JUB_SignTransactionSOL(%@)", __contextID, rawtx);
//        NSLog(@"[%li] JUB_SignTransactionSOL(...)----%d", [_sdk JUB_LastError]);
//        
//    });
//
//    
//}
- (IBAction)getPublicKey:(UIButton *)sender {
//    ContextConfigXRP *cfg = [[ContextConfigXRP alloc] init];
//    cfg.mainPath = @"m/44'/144'/0'";
//
//    __contextID = [_sdk JUB_CreateContextXRP:__deviceID
//                                         cfg:cfg];
//    NSLog(@"[%li] JUB_CreateContextXRP(%li)", __deviceID, (long)[_sdk JUB_LastError]);
//    BIP32Path *path = [[BIP32Path alloc] init];
//    path.change = BOOL_NS_FALSE;
//    path.addressIndex = 0;
//    NSString *publicKey = [_sdk JUB_GetHDNodeXRP:__contextID format:NS_HEX path:path];
//    NSLog(@"[%li] JUB_GetHDNodeXRP(%@)", __contextID, publicKey);
//    // 创建 TxXRP 实例
//    TxXRP *tx = [[TxXRP alloc] init];
// 
//    tx.account = @"rGjMFWKpAqFxkdfRWXuBPVKvGo2PZQdYkv";
//    tx.type = NS_PYMT; // PYMT 类型
//    tx.fee = @"10";
//    tx.sequence = @"148";
//    tx.flags = @"0";
//    tx.lastLedgerSequence = @"0";
//
//    // 设置 accountTxnID 和 sourceTag（在 PYMT 类型中可能为空）
//    tx.accountTxnID = @"";
//    tx.sourceTag = @"";
//
//    // 创建并赋值 PaymentXRP
//    PaymentXRP *payment = [[PaymentXRP alloc] init];
//    payment.type = NS_DXRP;
//    payment.destination = @"ruhDsG298KQorsMnXXGDvJJFBLegsq6Hb";
//    payment.destinationTag = @"0";
//    
//
//    PymtAmount *amountDict = [[PymtAmount alloc] init];
//    amountDict.value = @"1000";
//    
//    payment.amount = amountDict;
//    
//    tx.pymt = payment;
//
//    // 创建并赋值 Memo
//    XRPMemo *memo = [[XRPMemo alloc] init];
//    // 根据你的 JSON 数据，memo 字段包含 memoData 和 memoType
//    memo.data = @"";
//    memo.type = @"";
//
//    tx.memo = memo;
//    NSString *rawtx = [_sdk JUB_SignTransactionXRP:__contextID path:path tx:tx];
//    NSLog(@"[%li] JUB_SignTransactionSOL(%@)", __contextID, rawtx);
    ContextConfigETH *cfg = [[ContextConfigETH alloc] init];
    cfg.mainPath = @"m/44'/60'/0'";
    cfg.chainID = 1;
    __contextID = [_sdk JUB_CreateContextETH:__deviceID
                                         cfg:cfg];
    NSLog(@"[%li] JUB_CreateContextETH(%li)", __deviceID, (long)[_sdk JUB_LastError]);
    BIP32Path *path = [[BIP32Path alloc] init];
    path.change = BOOL_NS_FALSE;
    path.addressIndex = 0;
//    NSString *publicKey = [_sdk JUB_GetHDNodeETH:__contextID format:NS_HEX path:path];
//    NSLog(@"[%li] JUB_PublicKey(%@)", __contextID, publicKey);
    NSString *address = [_sdk JUB_GetAddressETH:__contextID path:path
                                          bShow:BOOL_NS_TRUE];
    NSLog(@"[%li] JUB_GetAddressETH(%@)", __contextID, address);
    
    ContextConfigBTC *cfgbtc = [[ContextConfigBTC alloc] init];
    cfgbtc.mainPath = @"m/44'/0'/0'";
    cfgbtc.coinType = NS_COINBTC;
    cfgbtc.transType = ns_p2pkh;
    __contextID = [_sdk JUB_CreateContextBTC:__deviceID
                                         cfg:cfgbtc];
//    publicKey = [_sdk JUB_GetHDNodeBTC:__contextID  path:path];
//    NSLog(@"[%li] JUB_PublicKey 1(%@)", __contextID, publicKey);
    address = [_sdk JUB_GetAddressBTC:__contextID path:path
                                          bShow:BOOL_NS_TRUE];
    NSLog(@"[%li] JUB_GetAddressBTC(%@)", __contextID, address);
    
    ContextConfigBTC *cfgbch = [[ContextConfigBTC alloc] init];
    cfgbch.mainPath = @"m/44'/5'/0'";
    cfgbch.coinType = NS_COINDASH;
    cfgbch.transType = ns_p2pkh;
    __contextID = [_sdk JUB_CreateContextBTC:__deviceID
                                         cfg:cfgbch];
//    publicKey = [_sdk JUB_GetHDNodeBTC:__contextID  path:path];
//    NSLog(@"[%li] JUB_PublicKey 2(%@)", __contextID, publicKey);
    address = [_sdk JUB_GetAddressBTC:__contextID path:path
                                          bShow:BOOL_NS_TRUE];
    NSLog(@"[%li] JUB_GetAddressBTC(%@)", __contextID, address);
    
    
}
- (IBAction)connectAction:(UIButton *)sender{
    
   // [_sdk JUB_RebootToBootLoader:__deviceID];
     //   NSUInteger deviceID =  [_sdk JUB_connectDevice:@"Sealer B4C3" uuid:@"772FCAFD-86A1-D628-735C-41AA33CDE265" connectType:1 timeout:30000];
    __deviceID =  [_sdk JUB_connectDevice:@"Sealer 7DFC" uuid:@"4EE08804-78FC-2913-B8A1-00B71BF337EF" connectType:1 timeout:30000];
    
        NSLog(@"JUB_connectDevice deviceID-------------====>%d",__deviceID);
    
        NSLog(@"[%li] JUB_connectDevice(...)----%d", [_sdk JUB_LastError]);
    
}
- (IBAction)cancelconnectAction:(UIButton *)sender{
 //   [_sdk JUB_RebootToNormal:__deviceID];
    [_sdk JUB_cancelConnect:@"Sealer 7DFC" uuid: @"4EE08804-78FC-2913-B8A1-00B71BF337EF"];
    NSLog(@"[%li] JUB_cancelConnect(...)----%d", [_sdk JUB_LastError]);
    
}
- (IBAction)disconnectAction:(UIButton *)sender{

     [_sdk JUB_isDeviceConnect:__deviceID];
    if([_sdk JUB_LastError] == 0){
        NSLog(@" deviceID-------------====>%d 即将断开链接",__deviceID);
        [_sdk JUB_disconnectDevice:__deviceID];
        NSLog(@"[%li] JUB_disconnectDevice(...)----%d", [_sdk JUB_LastError]);
    }
   
    
    
}

- (IBAction)deviceInfoAction: (UIButton*) sender {
    DeviceInfo* info = [_sdk JUB_GetDeviceInfo:__deviceID];
    if( [_sdk JUB_LastError] != 0) {
        return;
    }
    NSLog(@"Device info: ");
    NSLog(@"\t label: %@", info.label);
    NSLog(@"\t label: %@", info.sn);
    NSLog(@"\t label: %ld", info.pinRetry);
    NSLog(@"\t label: %ld", info.pinMaxRetry);
    NSLog(@"\t label: %@", info.bleVersion);
    NSLog(@"\t label: %@", info.firmwareVersion);
}
//
//- (IBAction)cancelconnectAction:(UIButton *)sender{
////    [_sdk JUB_cancelConnect:@"HyperMateMax 6241" uuid: @"1562B8B3-0286-0200-5B43-EF064096268D"];
////    NSLog(@"[%li] JUB_cancelConnect(...)----%d", [_sdk JUB_LastError]);
//    NSString *documentsPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
//    
//    // 2. 拼接固件文件路径（假设叫 firmware.bin）
//    NSString *filePath = [documentsPath stringByAppendingPathComponent:@"res.zip"];
//    NSURL *fileURL = [NSURL fileURLWithPath:filePath];
//    
//    // 3. 读取文件内容
//    NSError *error;
//    NSData *firmwareData = [NSData dataWithContentsOfURL:fileURL options:0 error:&error];
//    
//    if (error) {
//        NSLog(@"❌ 读取文件失败: %@", error.localizedDescription);
//        return;
//    }
//    
//    if (!firmwareData || firmwareData.length == 0) {
//        NSLog(@"❌ 文件为空或不存在！请确保 firmware.bin 已放入 Documents 目录");
//        return;
//    }
//    
//    NSLog(@"✅ 文件大小: %lu 字节", (unsigned long)firmwareData.length);
//
//[_sdk JUB_UpdateResource:__deviceID resourceZipPayload:firmwareData resourceZipSize:firmwareData.length reboot_on_success:TRUE];
//
//}

// This method is called once we complete editing
-(void)textFieldDidEndEditing:(UITextField *)textField {
    NSLog(@"Text field ended editing");
}

// called when 'return' key pressed. return
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    __pwd = textField.text;
    NSLog(@"Text field should return %@", __pwd);
    NSUInteger retry = [_sdk JUB_VerifyPIN:__contextID
                                    pinMix:__pwd];
    NSLog(@"[%li] JUB_VerifyPIN(%li) retry=%li", __contextID, retry, (long)[_sdk JUB_LastError]);
    return true;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self updateUI];
    
    if (nil == _deviceDict) {
        _deviceDict = [NSMutableDictionary dictionary];
    }
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(deviceArrivalObserver:)
                                                 name:NSNotificationName(@"jubitherwallet.device.arrival")
                                               object:[DevicePool shareDevicePool]];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(deviceScanObserver:)
                                                 name:NSNotificationName(@"jubitherwallet.device.scan")
                                               object:nil];
    _sdk = [JubSDKCore alloc];
    
    NSInteger lastError = [_sdk JUB_LastError];
    NSLog(@"[%li] JUB_LastError(void)", (long)lastError);
    
    NSString* version = [_sdk JUB_GetVersion];
    lastError = [_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetVersion: %@", (long)lastError, version);
}

//    BIP32Path *path32;
//    NSString* strRaw = [sdk JUB_SignTransactionETH:2
//                                              path:path32
//                                             nonce:13
//                                          gasLimit:310000
//                                     gasPriceInWei:@"1"
//                                                to:@"xxxxxxx"
//                                        valueInWei:@""
//                                             input:@""];
@end
