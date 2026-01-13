//
// Created by feng on 2025/6/4.
//

#include <jni.h>
#include "eos_jni.h"
#include "JUB_SDK.h"
#include "json/reader.h"
#include "utils/logUtils.h"
#include "common_jni.h"
#include "jni_registry.h"


#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
native_EOSCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
                        jstring jJSON, jlong deviceInfo) {

#define MAIN_PATH      "main_path"

    if (NULL == jJSON) {
        return JUBR_ARGUMENTS_BAD;
    }

    int length = env->GetStringLength(jJSON);
    if (0 == length) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_EOS cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();


    JUB_RV rv = JUBR_OK;
    rv = JUB_CreateContextEOS(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("native_EOSCreateContext: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jstring JNICALL
native_EOSGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressEOS(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressEOS: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_EOSShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressEOS(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressEOS: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_EOSSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressEOS(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressEOS: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_EOSTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(root["EOS"]["bip32_path"]["change"].asBool());
    path.addressIndex = static_cast<JUB_UINT64>(root["EOS"]["bip32_path"]["addressIndex"].asUInt());
    char *expiration = (char *) root["EOS"]["expiration"].asCString();

    char *chainID = nullptr;
    if (!root["EOS"]["chainID"].empty()) {
        chainID = (char *) root["EOS"]["chainID"].asCString();
    }
    char *referenceBlockId = (char *) root["EOS"]["referenceBlockId"].asCString();
    char *referenceBlockTime = (char *) root["EOS"]["referenceBlockTime"].asCString();


    std::vector<JUB_ACTION_EOS> actionArray;
    for (Json::Value::iterator it = root["EOS"]["actions"].begin();
         it != root["EOS"]["actions"].end(); ++it) {
        JUB_ACTION_EOS action;
        int typeInt = (*it)["type"].asUInt();
        const char *sType = "0";
        switch (typeInt) {
            case 0x00:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::XFER;
                sType = "0";
                break;
            case 0x01:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::DELE;
                sType = "1";
                break;
            case 0x02:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::UNDELE;
                sType = "2";
                break;
            case 0x03:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::BUYRAM;
                sType = "3";
                break;
            case 0x04:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::SELLRAM;
                sType = "4";
                break;
            default:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE;
                break;
        }
        action.currency = (char *) (*it)["currency"].asCString();
        action.name = (char *) (*it)["name"].asCString();

        switch (action.type) {
            case JUB_ENUM_EOS_ACTION_TYPE::XFER:
                action.transfer;
                action.transfer.from = (char *) (*it)[sType]["from"].asCString();
                action.transfer.to = (char *) (*it)[sType]["to"].asCString();
                action.transfer.asset = (char *) (*it)[sType]["asset"].asCString();
                action.transfer.memo = (char *) (*it)[sType]["memo"].asCString();
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::DELE:
                action.delegate.from = (char *) (*it)[sType]["from"].asCString();
                action.delegate.receiver = (char *) (*it)[sType]["receiver"].asCString();
                action.delegate.netQty = (char *) (*it)[sType]["stake_net_quantity"].asCString();
                action.delegate.cpuQty = (char *) (*it)[sType]["stake_cpu_quantity"].asCString();
                action.delegate.bStake = JUB_ENUM_BOOL::BOOL_TRUE;
            case JUB_ENUM_EOS_ACTION_TYPE::UNDELE:
                action.delegate;
                action.delegate.from = (char *) (*it)[sType]["from"].asCString();
                action.delegate.receiver = (char *) (*it)[sType]["receiver"].asCString();
                action.delegate.netQty = (char *) (*it)[sType]["unstake_net_quantity"].asCString();
                action.delegate.cpuQty = (char *) (*it)[sType]["unstake_cpu_quantity"].asCString();
                action.delegate.bStake = JUB_ENUM_BOOL::BOOL_FALSE;
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::BUYRAM:
                action.buyRam;
                action.buyRam.payer = (char *) (*it)[sType]["payer"].asCString();
                action.buyRam.quant = (char *) (*it)[sType]["quant"].asCString();
                action.buyRam.receiver = (char *) (*it)[sType]["receiver"].asCString();
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::SELLRAM:
                action.sellRam;
                action.sellRam.account = (char *) (*it)[sType]["account"].asCString();
                action.sellRam.bytes = (char *) (*it)[sType]["bytes"].asCString();
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE:
                break;
        }
        actionArray.push_back(action);
    }

    char *actions = NULL;
    JUB_BuildActionEOS(static_cast<JUB_UINT16>(contextID), &actionArray[0],
                       static_cast<JUB_UINT16>(actionArray.size()), &actions);


    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionEOS(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       chainID,
                                       expiration,
                                       referenceBlockId,
                                       referenceBlockTime,
                                       actions,
                                       &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断多签是否完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}


#ifdef __cplusplus
}
#endif


// 定义 JNINativeMethod 数组
static JNINativeMethod g2EosMethods[] = {
        {
                "nativeEOSCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_EOSCreateContext
        },
        {
                "nativeEOSGetAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_EOSGetAddress
        },
        {
                "nativeEOSShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_EOSShowAddress
        },
        {
                "nativeEOSSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_EOSSetMyAddress
        },
        {
                "nativeEOSTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_EOSTransaction
        },
};


#define G2_EOS_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2EosNatives(JNIEnv* env) {
    LOG_INF("Register G2 EOS");
    REGISTER_NATIVES(env, G2_EOS_CLASS, g2EosMethods, sizeof(g2EosMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Eos(RegisterG2EosNatives);