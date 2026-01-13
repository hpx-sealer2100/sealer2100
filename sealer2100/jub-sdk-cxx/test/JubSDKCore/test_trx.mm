//
//  test_trx.mm
//  JubSDKCore
//
//  Created by Pan Min on 2020/12/15.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+COIN_TRX.h"

#include <json/json.h>
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

extern JubSDKCore* g_sdk;

void error_exit(const char* message);
void main_test();

NSUInteger verify_pin(NSUInteger contextID);
void set_timeout_test(NSUInteger contextID);

void  get_address_pubkey_TRX(NSUInteger contextID);
void    transaction_test_TRX(NSUInteger contextID, Json::Value root, int choice);
void set_my_address_test_TRX(NSUInteger contextID);
NSUInteger transaction_proc_TRX(NSUInteger contextID, Json::Value root, int choice);
NSUInteger pack_contract_proc(NSUInteger contextID, Json::Value root,
                              int choice,
                              std::string& packedContract,
                              bool bERC20);

void TRX_test(NSUInteger deviceID, const char* json_file) {
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
    
    ContextConfigTRX* cfgTRX = [[ContextConfigTRX alloc] init];
    cfgTRX.mainPath = mainPath;
    NSUInteger contextID = [g_sdk JUB_CreateContextTRX:deviceID
                                                   cfg:cfgTRX];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_CreateContextTRX(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"ContextEOS: %li\n", contextID);
    
    while (true) {
        NSLog(@"|--------------------------------------------|\n");
        NSLog(@"|************ Jubiter Wallet TRX ************|\n");
        NSLog(@"| 1.    show_address_pubkey_test.            |\n");
        NSLog(@"|                                            |\n");
        NSLog(@"| 21.       transfer_contract_test.          |\n");
        NSLog(@"| 22. transfer_asset_contract_test.          |\n");
        NSLog(@"| 23.  trigger_smart_contr_erc20_test.       |\n");
        NSLog(@"| 24.  trigger_smart_contr_trc20_test.       |\n");
        NSLog(@"|                                            |\n");
        NSLog(@"| 46.  account_permission_update_contr_test. |\n");
        NSLog(@"|                                            |\n");
        NSLog(@"|  3. set_my_address_test.                   |\n");
        NSLog(@"|  4. set_timeout_test.                      |\n");
        NSLog(@"|                                            |\n");
        NSLog(@"|  9. return.                                |\n");
        NSLog(@"|--------------------------------------------|\n");
        NSLog(@"* Please enter your choice:\n");
        
        int choice = 0;
        std::cin >> choice;
        switch (choice) {
            case 1:
                get_address_pubkey_TRX(contextID);
                break;
            case 21:
            case 22:
            case 23:
            case 24:
            case 46:
                transaction_test_TRX(contextID, root, choice);
                break;
            case 3:
                set_my_address_test_TRX(contextID);
                break;
            case 4:
                set_timeout_test(contextID);
                break;
            case 9:
                main_test();
            default:
                continue;
        }
    }
}

void get_address_pubkey_TRX(NSUInteger contextID) {
    NSUInteger lastError = JUBR_ERROR;
    
    NSLog(@"please input change level (non-zero means 1):\n");
    int change = 0;
    cin >> change;
    NSLog(@"please input index \n");
    long index = 0;
    cin >> index;
    
    BIP32Path* path = [[BIP32Path alloc] init];
    path.change = JUB_NS_ENUM_BOOL(change);
    path.addressIndex = index;
    NSString* address = [g_sdk JUB_GetAddressTRX:contextID
                                            path:path
                                           bShow:BOOL_NS_TRUE];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetAddressTRX(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"address: %@\n", address);
}

void transaction_test_TRX(NSUInteger contextID, Json::Value root, int choice) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    lastError = transaction_proc_TRX(contextID, root, choice);
    if (JUBR_OK != lastError) {
        return;
    }
}

NSUInteger transaction_proc_TRX(NSUInteger contextID, Json::Value root, int choice) {
    
    NSUInteger lastError = JUBR_ERROR;
    
    BIP32Path* path = [[BIP32Path alloc] init];
    path.change = (JUB_NS_ENUM_BOOL)root["TRX"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["TRX"]["bip32_path"]["addressIndex"].asUInt();
    
    //TRX Test
//    typedef NS_ENUM(NSInteger, JUB_NS_TRX_CONTR_TYPE) {
//        NS_TX_XRP_TYPE_NS = 0,
//        NS_XFER_CONTRACT  = 1,
//        NS_XFER_ASSET_CONTRACT = 2,
//        NS_CREATE_SMART_CONTRACT = 30,
//        NS_TRIG_SMART_CONTRACT = 31
//    };
    bool bERC20 = false;
    if (   21 == choice
        || 22 == choice
        ) {
        choice -= 20;
    }
    else if (   23 == choice
             || 24 == choice
             ) {
        if (23 == choice) {
            bERC20 = true;
        }
        choice = 31;
    }
    
    std::string packedContractInPb;
    lastError = pack_contract_proc(contextID, root,
                                   choice,
                                   packedContractInPb,
                                   bERC20);
    if (JUBR_OK != lastError) {
        return lastError;
    }
    NSLog(@"Packed Contract[%li]: %s\n", (packedContractInPb.length()/2), packedContractInPb.c_str());
    
    NSString* raw = [g_sdk JUB_SignTransactionTRX:contextID path:path
                               packedContractInPb:[NSString stringWithUTF8String:packedContractInPb.c_str()]];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionTRX(%li) return", lastError, contextID);
    if (JUBR_OK != lastError) {
        return lastError;
    }
    else {
        NSLog(@"raw[%li]: %@\n", ([raw length]/2), raw);
    }
    
    return lastError;
}


NSUInteger pack_contract_proc(NSUInteger contextID, Json::Value root,
                              int choice,
                              std::string& packedContract,
                              bool bERC20) {
    
    NSUInteger lastError = JUBR_OK;
    
    if (2 == choice) {
        NSString* assetName = [NSString stringWithUTF8String:(char*)root["TRX"]["TRC10"]["assetName"].asCString()];
        NSUInteger unitDP = root["TRX"]["TRC10"]["dp"].asUInt64();
        NSString* assetID = [NSString stringWithUTF8String:(char*)root["TRX"]["TRC10"]["assetID"].asCString()];
        
        [g_sdk JUB_SetTRC10Asset:contextID
                       assetName:assetName
                          unitDP:unitDP
                         assetID:assetID];
        lastError = (long)[g_sdk JUB_LastError];
        if (JUBR_OK != lastError) {
            return lastError;
        }
    }
    
    NSString* trc20Abi;
    NSString* contractAddress = [NSString stringWithUTF8String:(char*)root["TRX"]["TRC20"]["contract_address"].asCString()];
    if (bERC20) {
        NSString* tokenName = [NSString stringWithUTF8String:(char*)root["TRX"]["TRC20"]["tokenName"].asCString()];
        NSUInteger unitDP = root["TRX"]["TRC20"]["dp"].asUInt64();
        [g_sdk JUB_SetTRC20Token:contextID
                       tokenName:tokenName
                          unitDP:unitDP
                 contractAddress:contractAddress];
        lastError = (long)[g_sdk JUB_LastError];
        if (JUBR_OK != lastError) {
            return lastError;
        }
        
        NSString* tokenTo = [NSString stringWithUTF8String:(char*)root["TRX"]["TRC20"]["token_to"].asCString()];
        NSString* tokenValue = [NSString stringWithUTF8String:(char*)root["TRX"]["TRC20"]["token_value"].asCString()];
        trc20Abi = [g_sdk JUB_BuildTRC20TransferAbi:contextID
                                            tokenTo:tokenTo
                                         tokenValue:tokenValue];
        lastError = (long)[g_sdk JUB_LastError];
        if (JUBR_OK != lastError) {
            return lastError;
        }
    }
    
    TxTRX* tx = [[TxTRX alloc] init];
    
    tx.refBlockBytes = [NSString stringWithUTF8String:(char*)root["TRX"]["pack"]["ref_block_bytes"].asCString()];
    tx.refBlockNum = [NSString stringWithUTF8String:(char*)""];
    tx.refBlockHash = [NSString stringWithUTF8String:(char*)root["TRX"]["pack"]["ref_block_hash"].asCString()];
    tx.data = [NSString stringWithUTF8String:(char*)""];;
    tx.expiration = [NSString stringWithUTF8String:(char*)root["TRX"]["pack"]["expiration"].asCString()];
    tx.timestamp = [NSString stringWithUTF8String:(char*)root["TRX"]["pack"]["timestamp"].asCString()];
    tx.feeLimit = [NSString stringWithUTF8String:(char*)"0"];
    
    TRXContract* contrTRX = [[TRXContract alloc] init];
    const char* sType = std::to_string((unsigned int)choice).c_str();
    switch ((JUB_NS_TRX_CONTR_TYPE)choice) {
    case JUB_NS_TRX_CONTR_TYPE::NS_XFER_CONTRACT: {
        contrTRX.xfer = [[TRXTransferContract alloc] init];
        contrTRX.xfer.ownerAddress = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"]["owner_address"].asCString()];
        contrTRX.xfer.toAddress    = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["to_address"].asCString()];
        contrTRX.xfer.amount = root["TRX"]["contracts"][sType]["amount"].asUInt64();
    } break;
    case JUB_NS_TRX_CONTR_TYPE::NS_XFER_ASSET_CONTRACT: {
        contrTRX.assetXfer = [[TRXTransferAssetContract alloc] init];
        contrTRX.assetXfer.assetName = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["asset_name"].asCString()];
        contrTRX.assetXfer.ownerAddress = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"]["owner_address"].asCString()];
        contrTRX.assetXfer.toAddress = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["to_address"].asCString()];
        contrTRX.assetXfer.amount = root["TRX"]["contracts"][sType]["amount"].asUInt64();
    } break;
    case JUB_NS_TRX_CONTR_TYPE::NS_TRIG_SMART_CONTRACT: {
        tx.feeLimit = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["fee_limit"].asCString()];
        
        contrTRX.triggerSmart = [[TRXTriggerSmartContract alloc] init];
        contrTRX.triggerSmart.ownerAddress = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"]["owner_address"].asCString()];
        if (bERC20) {
            contrTRX.triggerSmart.contractAddress = contractAddress;
            contrTRX.triggerSmart.data = trc20Abi;
        }
        else {
            contrTRX.triggerSmart.contractAddress = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["contract_address"].asCString()];
            contrTRX.triggerSmart.data = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["data"].asCString()];
        }
        contrTRX.triggerSmart.callValue = root["TRX"]["contracts"][sType]["call_value"].asUInt64();
        contrTRX.triggerSmart.callTokenValue = root["TRX"]["contracts"][sType]["call_token_value"].asUInt64();
        contrTRX.triggerSmart.tokenId = root["TRX"]["contracts"][sType]["token_id"].asUInt64();
    } break;
    case JUB_NS_TRX_CONTR_TYPE::NS_ACCT_PERM_UPDATE_CONTRACT: {
        contrTRX.acctPermUpdate = [[TRXAccountPermissionUpdateContract alloc] init];
        contrTRX.acctPermUpdate.ownerAddress = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"]["owner_address"].asCString()];
        
        contrTRX.acctPermUpdate.owner = [[TRXPermission alloc] init];
        auto strType = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["owner"]["type"].asCString()];
        contrTRX.acctPermUpdate.owner.type = PermissionType::Owner;
        if ([strType isEqualToString:@"Witness"]) {
            contrTRX.acctPermUpdate.owner.type = PermissionType::Witness;
        }
        else if ([strType isEqualToString:@"Active"]) {
            contrTRX.acctPermUpdate.owner.type = PermissionType::Active;
        }
        contrTRX.acctPermUpdate.owner.identity = root["TRX"]["contracts"][sType]["owner"]["id"].asUInt();
        contrTRX.acctPermUpdate.owner.permissionName = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["owner"]["permission_name"].asCString()];
        contrTRX.acctPermUpdate.owner.threshold = root["TRX"]["contracts"][sType]["owner"]["threshold"].asUInt64();
        int szOwnerKeys = root["TRX"]["contracts"][sType]["owner"]["keys"].size();
        if (szOwnerKeys > 0) {
            contrTRX.acctPermUpdate.owner.keys = [[NSMutableArray alloc] init];
        }
        for (int i=0; i<szOwnerKeys; ++i) {
            TRXKey *key = [[TRXKey alloc] init];
            key.address = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["owner"]["keys"][i]["address"].asCString()];
            key.weight = root["TRX"]["contracts"][sType]["owner"]["keys"][i]["weight"].asUInt64();
            
            [contrTRX.acctPermUpdate.owner.keys addObject:key];
        }
        
        int szActives = root["TRX"]["contracts"][sType]["actives"].size();
        if (szActives > 0) {
            contrTRX.acctPermUpdate.actives = [[NSMutableArray alloc] init];
        }
        for (int i=0; i<szActives; ++i) {
            TRXPermission *active = [[TRXPermission alloc] init];
            strType = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["actives"][i]["type"].asCString()];
            if ([strType isEqualToString:@"Witness"]) {
                active.type = PermissionType::Witness;
            }
            else if ([strType isEqualToString:@"Active"]) {
                active.type = PermissionType::Active;
            }
            active.identity = root["TRX"]["contracts"][sType]["actives"][i]["id"].asUInt();
            active.permissionName = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["actives"][i]["permission_name"].asCString()];
            active.threshold = root["TRX"]["contracts"][sType]["actives"][i]["threshold"].asUInt64();
            active.operations = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["actives"][i]["operations"].asCString()];
            int szActiveKeys = root["TRX"]["contracts"][sType]["actives"][i]["keys"].size();
            if (szActiveKeys > 0) {
                active.keys = [[NSMutableArray alloc] init];
            }
            for (int j=0; j<szActiveKeys; ++j) {
                TRXKey *key = [[TRXKey alloc] init];
                key.address = [NSString stringWithUTF8String:(char*)root["TRX"]["contracts"][sType]["actives"][i]["keys"][j]["address"].asCString()];
                key.weight = root["TRX"]["contracts"][sType]["actives"][i]["keys"][j]["weight"].asUInt64();
                
                [active.keys addObject:key];
            }
            [contrTRX.acctPermUpdate.actives addObject:active];
        }
    } break;
    case JUB_NS_TRX_CONTR_TYPE::NS_CREATE_SMART_CONTRACT:
    default: {
        lastError = JUBR_ARGUMENTS_BAD;
    } break;
    }
    if (JUBR_OK != lastError) {
        return lastError;
    }
    contrTRX.type = (JUB_NS_TRX_CONTR_TYPE)choice;
    contrTRX.permissionId = root["TRX"]["contracts"]["permission_id"].asUInt64();
    
    tx.contracts = [NSMutableArray array];
    [tx.contracts addObject:contrTRX];
    
    NSString* packContractInPb = [g_sdk JUB_PackContractTRX:contextID
                                                         tx:tx];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_PackContractTRX(%li) return", lastError, contextID);
    if (JUBR_OK == lastError) {
        packedContract = std::string([packContractInPb UTF8String]);
    }
    
    return lastError;
}


void set_my_address_test_TRX(NSUInteger contextID) {
    
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    NSLog(@"please input change level (non-zero means 1):\n");
    int change = 0;
    std::cin >> change;
    NSLog(@"please input index:\n");
    long index = 0;
    std::cin >> index;
    
    BIP32Path* path = [[BIP32Path alloc] init];
    path.change = JUB_NS_ENUM_BOOL(change);
    path.addressIndex = index;
    
    NSString* address = [g_sdk JUB_SetMyAddressTRX:contextID
                                              path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SetMyAddressEOS(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"set my address is: %@\n", address);
}
