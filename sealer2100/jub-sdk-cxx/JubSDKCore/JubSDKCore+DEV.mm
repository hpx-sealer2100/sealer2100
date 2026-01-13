//
//  JubSDKCore+DEV.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+DEV.h"
#import "JUB_SDK_DEV.h"




@implementation JubSDKCore (DEV)




////JUB_RV JUB_GetDeviceCert(IN JUB_UINT16 deviceID,
////                         OUT JUB_CHAR_PTR_PTR cert);
//- (NSString*)JUB_GetDeviceCert:(NSUInteger)deviceID
//{
//    self.lastError = JUBR_OK;
//    
//    char *cert = nullptr;
//    JUB_RV rv = JUB_GetDeviceCert(deviceID,
//                                  &cert);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString *strCert = [NSString stringWithCString:cert
//                                           encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(cert);
//    
//    return strCert;
//}



@end
