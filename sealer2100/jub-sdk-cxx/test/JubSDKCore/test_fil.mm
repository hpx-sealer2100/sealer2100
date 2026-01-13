//
//  test_fil.mm
//  JubSDKCore
//
//  Created by Pan Min on 2021/4/28.
//  Copyright © 2021 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+COIN_FIL.h"

#include <json/json.h>
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>
#include <fstream>

using namespace std;

extern JubSDKCore* g_sdk;

void error_exit(const char* message);
void main_test();

NSUInteger verify_pin(NSUInteger contextID);
void set_timeout_test(NSUInteger contextID);

void            get_address_pubkey_FIL(NSUInteger contextID);
void              transaction_test_FIL(NSUInteger contextID, Json::Value root);

void FIL_test(NSUInteger deviceID, const char* json_file) {
    NSUInteger lastError = JUBR_ERROR;
    
    Json::CharReaderBuilder builder;
    Json::Value root;
    ifstream in(json_file, ios::binary);
    if (!in.is_open()) {
        error_exit("Error opening json file\n");
    }
    
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, in, &root, &errs)) {
        error_exit("Error parse json file\n");
    }
    
    NSString* mainPath = [NSString stringWithUTF8String:(char*)root["main_path"].asCString()];
    
    ContextConfigFIL* cfgFIL = [[ContextConfigFIL alloc] init];
    cfgFIL.mainPath = mainPath;
    NSUInteger contextID = [g_sdk JUB_CreateContextFIL:deviceID
                                                   cfg:cfgFIL];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_CreateContextFIL(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"ContextFIL: %li\n", contextID);
    
    while (true) {
        NSLog(@"|----------------------------------------|\n");
        NSLog(@"|********** Jubiter Wallet FIL **********|\n");
        NSLog(@"| 1.   show_address_pubkey_test.         |\n");
        NSLog(@"|                                        |\n");
        NSLog(@"| 2. transaction_test.                   |\n");
        NSLog(@"|                                        |\n");
        NSLog(@"| 9. return.                             |\n");
        NSLog(@"|----------------------------------------|\n");
        NSLog(@"* Please enter your choice:               \n");
        
        int choice = 0;
        std::cin >> choice;
        switch (choice) {
        case 1:
            get_address_pubkey_FIL(contextID);
            break;
        case 2:
            transaction_test_FIL(contextID, root);
            break;
        case 9:
            main_test();
        default:
            continue;
        }
    }
}

void get_address_pubkey_FIL(NSUInteger contextID) {
    NSUInteger lastError = JUBR_ERROR;
    
    NSLog(@"please input change level (non-zero means 1):\n");
    int change = 0;
    cin >> change;
    NSLog(@"please input index \n");
    long index = 0;
    cin >> index;
    
    NSString* pubkey = [g_sdk JUB_GetMainHDNodeFIL:contextID
                                            format:NS_HEX];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetMainHDNodeFIL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"MainXpub in hex format: %@\n", pubkey);
    
    pubkey = [g_sdk JUB_GetMainHDNodeFIL:contextID
                                  format:NS_XPUB];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetMainHDNodeFIL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"MainXpub in xpub format: %@\n", pubkey);
    
    BIP32Path* path = [[BIP32Path alloc] init];
    path.change = JUB_NS_ENUM_BOOL(change);
    path.addressIndex = index;
    pubkey = [g_sdk JUB_GetHDNodeFIL:contextID
                              format:NS_HEX
                                path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetHDNodeFIL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"pubkey in hex format: %@\n", pubkey);
    
    pubkey = [g_sdk JUB_GetHDNodeFIL:contextID
                              format:NS_XPUB
                                path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetHDNodeFIL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"pubkey in xpub format: %@\n", pubkey);
    
    NSString* address = [g_sdk JUB_GetAddressFIL:contextID
                                            path:path
                                           bShow:BOOL_NS_TRUE];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetAddressFIL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"address: %@\n", address);
}


void transaction_test_FIL(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    //FIL Test
    BIP32Path* path = [[BIP32Path alloc] init];
    if (root["ETH"]["bip32_path"]["change"].asBool()) {
        path.change = BOOL_NS_TRUE;
    }
    else {
        path.change = BOOL_NS_FALSE;
    }
    path.addressIndex = root["FIL"]["bip32_path"]["addressIndex"].asUInt();
    
    NSUInteger nonce = root["FIL"]["nonce"].asUInt();//.asDouble();
    NSUInteger gasLimit = root["FIL"]["gasLimit"].asUInt();//.asDouble();
    NSString* gasFeeCapInAtto  = [NSString stringWithUTF8String:(char*)root["FIL"]["gasFeeCapInAtto"].asCString()];
    NSString* gasPremiumInAtto = [NSString stringWithUTF8String:(char*)root["FIL"]["gasPremiumInAtto"].asCString()];
    NSString* valueInAtto = [NSString stringWithUTF8String:(char*)root["FIL"]["valueInAtto"].asCString()];
    NSString* to = [NSString stringWithUTF8String:(char*)root["FIL"]["to"].asCString()];
    NSString* data = [NSString stringWithUTF8String:(char*)root["FIL"]["data"].asCString()];
    NSString* raw = [g_sdk JUB_SignTransactionFIL:contextID
                                             path:path
                                            nonce:nonce
                                         gasLimit:gasLimit
                                  gasFeeCapInAtto:gasFeeCapInAtto
                                 gasPremiumInAtto:gasPremiumInAtto
                                               to:to
                                      valueInAtto:valueInAtto
                                            input:data];;
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionFIL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"raw: %@\n", raw);
}
