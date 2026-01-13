//
//  test_fgpt.mm
//  JubSDKCore
//
//  Created by Pan Min on 2020/7/6.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+DEV.h"
#import "JubSDKCore+DEV_HID.h"
#import "JubSDKCore+DEV_BIO.h"

#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>

using namespace std;

extern JubSDKCore* g_sdk;

void error_exit(const char* message);
void main_test();

NSUInteger device_fgpt_enroll_test(NSUInteger deviceID, int fgptIndex, int fgptID) {
    
    NSUInteger lastError = JUBR_ERROR;
    NSUInteger fgptTimeout = 60;
    
    NSUInteger assignedID = fgptID;
    stFgptEnrollInfo info = [g_sdk JUB_EnrollFingerprint:deviceID
                                             fgptTimeout:fgptTimeout
                                               fgptIndex:fgptIndex
                                                  fgptID:assignedID];
    lastError = [g_sdk lastError];
    NSLog(@"[%li] JUB_EnrollFingerprint(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return lastError;
    }
    NSLog(@"The total number of times that need to enroll for _NO.%lu_ fingerprint is _%lu/%lu_.\n", info.modalityID, info.nextIndex, info.times);
    
    assignedID = info.modalityID;
    for (NSUInteger i=info.nextIndex; i<info.times; ++i) {
        stFgptEnrollInfo _info;
        if (1 == i) {
            _info = info;
        }
        NSLog(@"_NO.%lu_ fingerprint continue enroll _%lu/%lu_:\n", _info.modalityID, _info.nextIndex, _info.times);
        _info = [g_sdk JUB_EnrollFingerprint:deviceID
                                 fgptTimeout:fgptTimeout
                                   fgptIndex:_info.nextIndex
                                      fgptID:assignedID];
        lastError = [g_sdk lastError];
        NSLog(@"[%li] JUB_EnrollFingerprint(%li)\n", lastError, deviceID);
        if (JUBR_OK != lastError) {
            break;
        }
        assignedID = _info.modalityID;
    }
    
    return lastError;
}


NSUInteger verify_identity_via_9grids(NSUInteger deviceID) {

    NSUInteger lastError = JUBR_ERROR;
    while (lastError) {
        //����pin��λ�ã�������123456789
        cout << "1 2 3" << endl;
        cout << "4 5 6" << endl;
        cout << "7 8 9" << endl;

        NSLog(@"to cancel the virtualpwd iput 'c'\n");
        [g_sdk JUB_IdentityShowNineGrids:deviceID];
        lastError = [g_sdk lastError];
        if (   JUBR_OK               != lastError
            && JUBR_IMPL_NOT_SUPPORT != lastError
            ) {
            NSLog(@"[%li] JUB_IdentityShowNineGrids(%li)\n", lastError, deviceID);
            break;
        }

        char str[9] = {0,};

        cin >> str;
        cout << str << endl;
        
        if (   'c' == str[0]
            || 'C' == str[0]
            ) {
            NSLog(@"cancel the VirtualPwd");
            [g_sdk JUB_IdentityCancelNineGrids:deviceID];
            lastError = [g_sdk lastError];
            if (JUBR_OK != lastError) {
                NSLog(@"[%li] JUB_IdentityCancelNineGrids(%li)\n", lastError, deviceID);
                break;
            }
            return lastError;
        }
        
        NSUInteger retry = [g_sdk JUB_IdentityVerifyPIN:deviceID
                                                   mode:JUB_NS_ENUM_IDENTITY_VERIFY_MODE::NS_VIA_9GRIDS
                            pinMix:[NSString stringWithUTF8String:str]];
        lastError = [g_sdk lastError];
        if (JUBR_OK != lastError) {
            NSLog(@"[%li] JUB_IdentityVerifyPIN(%li)\n", lastError, deviceID);
            break;
        }
        NSLog(@"Verify id remaining %li time(s).", retry);
    }
    
    return lastError;
}



void device_fgpt_test(NSUInteger deviceID) {
    
    NSUInteger fgptTimeout = 60;
    
    while (true) {
        NSLog(@"|-------------------------------------------|\n");
        NSLog(@"|*********** G2 fingerprint test ***********|\n");
        NSLog(@"| 1. Enumerate Fingerprint.                 |\n");
        NSLog(@"| 2. Enroll    Fingerprint.                 |\n");
        NSLog(@"| 3. Delete    Fingerprint.                 |\n");
        NSLog(@"| 4. Erase     Fingerprint.                 |\n");
        NSLog(@"| 5. Verify Identity-via 9Grids.            |\n");
//        NSLog(@"| 6. Verify Identity-via fingerprint.       |\n");
        NSLog(@"|                                           |\n");
        NSLog(@"| 9. return.                                |\n");
        NSLog(@"|-------------------------------------------|\n");
        NSLog(@"* Please enter your choice:                  \n");
        
        int choice = 0;
        std::cin >> choice;
        if (0 == choice) {
            exit(0);
        }
        
        NSUInteger lastError = JUBR_ERROR;
        JUB_NS_ENUM_BOOL b = [g_sdk JUB_IsBootLoader:deviceID];
        lastError = [g_sdk lastError];
        NSLog(@"[%li] JUB_IsBootLoader(%li)\n", lastError, b);
        
        switch (choice) {
        case 1:
        {
            NSString* fgptList = [g_sdk JUB_EnumFingerprint:deviceID];
            lastError = [g_sdk lastError];
            NSLog(@"[%li] JUB_EnumFingerprint(%li)\n", lastError, deviceID);
            NSLog(@"finger print list: %@\n", fgptList);
            
            break;
        }
        case 2:
        {
            int fgptIndex = 0;
            std::cout << "Please enter your fingerprint index for enroll:" << std::endl;
            std::cin >> fgptIndex;
            device_fgpt_enroll_test(deviceID, fgptIndex, 0);
            break;
        }
        case 3:
        {
            int fgptID = 0;
            std::cout << "Please enter your fingerprint ID for delete:" << std::endl;
            std::cin >> fgptID;
            [g_sdk JUB_DeleteFingerprint:deviceID
                             fgptTimeout:fgptTimeout
                                  fgptID:fgptID];
            lastError = [g_sdk lastError];
            NSLog(@"[%li] JUB_DeleteFingerprint(%li)\n", lastError, deviceID);
            break;
        }
        case 4:
        {
            char ch;
            std::cout << "Erase all fingerprint? Y:N" << std::endl;
            std::cin >> ch;
            if ('Y' == ch) {
                [g_sdk JUB_EraseFingerprint:deviceID
                                fgptTimeout:fgptTimeout];
                lastError = [g_sdk lastError];
                NSLog(@"[%li] JUB_EraseFingerprint(%li)\n", lastError, deviceID);
            }
            break;
        }
        case 5:
        {
            verify_identity_via_9grids(deviceID);
            break;
        }
//        case 6:
//        {
//            JUB_NS_ENUM_IDENTITY_VERIFY_MODE mode = JUB_NS_ENUM_IDENTITY_VERIFY_MODE::NS_VIA_FPGT;
//            NSUInteger retry = [g_sdk JUB_IdentityVerify:deviceID
//                                                    mode:mode];
//            lastError = [g_sdk lastError];
//            NSLog(@"[%li] JUB_IdentityVerify(%li)\n", lastError, deviceID);
//            NSLog(@"Verify id remaining %li time(s).", retry);
//            break;
//        }
        case 9:
            main_test();
            break;
        default:
            continue;
        }
    }
}

