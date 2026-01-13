//
//  test_sol.mm
//  JubSDKCore
//
//  Created by Pan Min on 2022/8/25.
//  Copyright © 2022 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+COIN_SOL.h"

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

void            get_address_pubkey_SOL(NSUInteger contextID, NSString* mainPath);
void              transaction_test_SOL(NSUInteger contextID, Json::Value root);

void SOL_test(NSUInteger deviceID, const char* json_file) {
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
    
    ContextConfigSOL* cfgSOL = [[ContextConfigSOL alloc] init];
    cfgSOL.mainPath = mainPath;
    NSUInteger contextID = [g_sdk JUB_CreateContextSOL:deviceID
                                                   cfg:cfgSOL];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_CreateContextSOL(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"ContextSOL: %li\n", contextID);
    
    while (true) {
        NSLog(@"|----------------------------------------|\n");
        NSLog(@"|********** Jubiter Wallet SOL **********|\n");
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
            get_address_pubkey_SOL(contextID, mainPath);
            break;
        case 2:
            transaction_test_SOL(contextID, root);
            break;
        case 9:
            main_test();
        default:
            continue;
        }
    }
}

void get_address_pubkey_SOL(NSUInteger contextID, NSString* mainPath) {
    NSUInteger lastError = JUBR_ERROR;
    
    NSString* pubkey = [g_sdk JUB_GetMainHDNodeSOL:contextID
                                            format:NS_HEX];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetMainHDNodeSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"MainXpub in hex format: %@\n", pubkey);
    
    NSMutableString* path = [[NSMutableString alloc] init];
    [path appendString:mainPath];
    
    std::string line;
    NSLog(@"please input change level (non-zero means 1):\n");
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(cin, line, '\n');
    [path appendString:[NSString stringWithFormat:@"%s", "/"]];
    [path appendString:[NSString stringWithFormat:@"%s", line.c_str()]];
    NSLog(@"please input index:\n");
    getline(cin, line, '\n');
    [path appendString:[NSString stringWithFormat:@"%s", "/"]];
    [path appendString:[NSString stringWithFormat:@"%s", line.c_str()]];
    
    pubkey = [g_sdk JUB_GetHDNodeSOL:contextID
                              format:NS_HEX
                                path:[NSString stringWithString:path]];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetHDNodeSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"pubkey in hex format: %@\n", pubkey);
    
    NSString* address = [g_sdk JUB_GetAddressSOL:contextID
                                            path:[NSString stringWithString:path]
                                           bShow:BOOL_NS_TRUE];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetAddressSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"address: %@\n", address);
}


void transaction_test_SOL(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }

    //SOL Test
    NSString* raw = [g_sdk JUB_SignTransactionSOL:(NSUInteger)contextID
                                             path:[NSString stringWithUTF8String:(char*)root["SOL"]["bip32_path"].asCString()]
                                       recentHash:[NSString stringWithUTF8String:(char*)root["SOL"]["recent_hash"].asCString()]
                                             dest:[NSString stringWithUTF8String:(char*)root["SOL"]["dest"].asCString()]
                                           amount:[NSNumber numberWithLongLong:root["SOL"]["amount"].asUInt64()]];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"JUB_SignTransactionSOL return raw: %@\n", raw);
    
    // TOKEN-TRANSFER token_name
    [g_sdk JUB_SetTokenSOL:(NSUInteger)contextID
                      name:[NSString stringWithUTF8String:(char*)root["TOKEN-TRANSFER"]["token_name"].asCString()]
                   decimal:root["TOKEN-TRANSFER"]["decimal"].asUInt()
                 tokenMint:[NSString stringWithUTF8String:(char*)root["TOKEN-TRANSFER"]["token_mint"].asCString()]];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SetTokenSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    raw = [g_sdk JUB_SignTransactionTokenSOL:(NSUInteger)contextID
                                        path:[NSString stringWithUTF8String:(char*)root["TOKEN-TRANSFER"]["bip32_path"].asCString()]
                                  recentHash:[NSString stringWithUTF8String:(char*)root["TOKEN-TRANSFER"]["recent_hash"].asCString()]
                                   tokenMint:[NSString stringWithUTF8String:(char*)root["TOKEN-TRANSFER"]["token_mint"].asCString()]
                                      source:[NSString stringWithUTF8String:(char*)root["TOKEN-TRANSFER"]["source"].asCString()]
                                        dest:[NSString stringWithUTF8String:(char*)root["TOKEN-TRANSFER"]["dest"].asCString()]
                                      amount:[NSNumber numberWithLongLong:root["TOKEN-TRANSFER"]["amount"].asUInt64()]
                                     decimal:root["TOKEN-TRANSFER"]["decimal"].asUInt()];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionTokenSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"JUB_SignTransactionTokenSOL return raw: %@\n", raw);
    
    // ASSOCIATED-TOKEN-CREATE
    [g_sdk JUB_SetTokenSOL:(NSUInteger)contextID
                      name:[NSString stringWithUTF8String:(char*)root["ASSOCIATED-TOKEN-CREATE"]["token_name"].asCString()]
                   decimal:root["ASSOCIATED-TOKEN-CREATE"]["decimal"].asUInt()
                 tokenMint:[NSString stringWithUTF8String:(char*)root["ASSOCIATED-TOKEN-CREATE"]["token_mint"].asCString()]];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SetTokenSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    raw = [g_sdk JUB_SignTransactionAssociatedTokenCreateSOL:(NSUInteger)contextID
                                                        path:[NSString stringWithUTF8String:(char*)root["ASSOCIATED-TOKEN-CREATE"]["bip32_path"].asCString()]
                                                  recentHash:[NSString stringWithUTF8String:(char*)root["ASSOCIATED-TOKEN-CREATE"]["recent_hash"].asCString()]
                                                       owner:[NSString stringWithUTF8String:(char*)root["ASSOCIATED-TOKEN-CREATE"]["owner"].asCString()]
                                                   tokenMint:[NSString stringWithUTF8String:(char*)root["ASSOCIATED-TOKEN-CREATE"]["token_mint"].asCString()]];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionAssociatedTokenCreateSOL(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"JUB_SignTransactionAssociatedTokenCreateSOL return raw: %@\n", raw);
}
