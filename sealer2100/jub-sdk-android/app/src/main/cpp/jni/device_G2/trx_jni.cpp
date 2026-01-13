//
// Created by feng on 2025/6/4.
//

#include <jni.h>
#include "trx_jni.h"
#include "JUB_SDK_COMM.h"
#include "json/reader.h"
#include "common_jni.h"
#include "JUB_SDK.h"
#include "utils/logUtils.h"
#include "jni_registry.h"


#ifdef __cplusplus
extern "C" {
#endif



JNIEXPORT jint JNICALL
native_TRXCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
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

    CONTEXT_CONFIG_TRX cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();


    JUB_RV rv = JUBR_OK;
    rv = JUB_CreateContextTRX(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextTRX: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jobjectArray JNICALL
native_TRXGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    jobjectArray array = env->NewObjectArray(2, clazz, 0);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressTRX(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressTRX: %08x", rv);
        errorCode = static_cast<int>(rv);

        env->SetObjectArrayElement(array, 0, NULL);
        env->SetObjectArrayElement(array, 1, NULL);
        return array;
    }

    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::HEX;

    JUB_CHAR_PTR pPubKey = NULL;
    JUB_RV nodeRv = JUB_GetHDNodeTRX(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (nodeRv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeTRX: %08x", nodeRv);
        errorCode = static_cast<int>(nodeRv);

        env->SetObjectArrayElement(array, 0, NULL);
        env->SetObjectArrayElement(array, 1, address);
        return array;
    }

    // hex 格式拼接 0x 前缀
    std::string hexPubKey = "0x";
    if (pPubKey != nullptr) {
        hexPubKey += pPubKey;
    }

    jstring pubKey = env->NewStringUTF(hexPubKey.c_str());
    JUB_FreeMemory(pPubKey);

    env->SetObjectArrayElement(array, 0, pubKey);
    env->SetObjectArrayElement(array, 1, address);

    return array;
}

JNIEXPORT jstring JNICALL
native_TRXGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::XPUB;
    if (useHex) {
        format = JUB_ENUM_PUB_FORMAT::HEX;
    }

    JUB_CHAR_PTR pPubKey = NULL;
    JUB_RV rv = JUB_GetHDNodeTRX(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeTRX: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    if (format == JUB_ENUM_PUB_FORMAT::HEX) {
        // 拼接 0x 前缀
        std::string hexPubKey = "0x";
        if (pPubKey != nullptr) {
            hexPubKey += pPubKey;
        }

        jstring pubKey = env->NewStringUTF(hexPubKey.c_str());
        JUB_FreeMemory(pPubKey);
        return pubKey;
    }

    jstring pubKey = env->NewStringUTF(pPubKey);
    JUB_FreeMemory(pPubKey);
    return pubKey;
}

JNIEXPORT jstring JNICALL
native_TRXShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressTRX(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressTRX: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_TRXSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressTRX(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressTRX: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_TRXTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON,
                      jbyteArray jPackedContractInPb) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["TRX"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["TRX"]["bip32_path"]["addressIndex"].asUInt();

//    BIP32_Path path;
//    if (root["TRX"]["bip32_path"]["change"].asBool()) {
//        path.change = JUB_ENUM_BOOL::BOOL_TRUE;
//    } else {
//        path.change = JUB_ENUM_BOOL::BOOL_FALSE;
//    }

    jbyte* src = (env->GetByteArrayElements(jPackedContractInPb, NULL));
    jsize len = env->GetArrayLength(jPackedContractInPb);
    std::string packedContractInPB(reinterpret_cast<const char*>(src), static_cast<size_t>(len));
    env->ReleaseByteArrayElements(jPackedContractInPb, src, JNI_ABORT);
    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionTRX(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       packedContractInPB.c_str(),
                                       &raw);
    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jint JNICALL native_TRXSetTRC20Token(JNIEnv *env, jclass obj, jlong contextID,
                                               jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *tokenName = (char *) root["TRC20"]["tokenName"].asCString();
    uint16_t unitDP = root["TRC20"]["dp"].asDouble();
    char *contractAddress = (char *) root["TRC20"]["contract_address"].asCString();


    JUB_RV rv = JUB_SetTRC20Token(contextID, tokenName, unitDP, contractAddress);

    env->ReleaseStringUTFChars(jJSON, pJSON);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetTRC20Token: %08x", rv);
    }
    return rv;
}

JNIEXPORT jstring JNICALL native_TRXBuildTRC20TransferAbi(JNIEnv *env, jclass obj, jlong contextID,
                                                          jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *token_to = (char *) root["TRC20"]["token_to"].asCString();
    char *token_value = (char *) root["TRC20"]["token_value"].asCString();

    char *abi = nullptr;
    JUB_RV rv = JUB_BuildTRC20TransferAbi(contextID, token_to,
                                          token_value, &abi);

    env->ReleaseStringUTFChars(jJSON, pJSON);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_BuildTRC20Abi: %08x", rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}

JNIEXPORT jint JNICALL native_TRXSetTRC10Asset(JNIEnv *env, jclass obj, jlong contextID,
                                               jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *assetName = (char *) root["TRC10"]["assetName"].asCString();
    uint16_t unitDP = root["TRC10"]["dp"].asDouble();
    char *assetID = (char *) root["TRC10"]["assetID"].asCString();

    JUB_RV rv = JUB_SetTRC10Asset(contextID, assetName, unitDP, assetID);

    env->ReleaseStringUTFChars(jJSON, pJSON);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetTRC10Asset: %08x", rv);
    }
    return rv;
}

JNIEXPORT jbyteArray JNICALL
native_TRXPackContract(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    JUB_TX_TRX tx;

    tx.ref_block_bytes = (char *) root["TRX"]["pack"]["ref_block_bytes"].asCString();
    tx.ref_block_hash = (char *) root["TRX"]["pack"]["ref_block_hash"].asCString();
    tx.ref_block_num = nullptr;
    tx.data = nullptr;
    tx.expiration = (char *) root["TRX"]["pack"]["expiration"].asCString();
    tx.timestamp = (char *) root["TRX"]["pack"]["timestamp"].asCString();
    tx.fee_limit = (char *) root["TRX"]["pack"]["fee_limit"].asCString();

    int numType = root["TRX"]["contracts"]["type"].asUInt();
    const char *sType = "1";

    std::vector<JUB_CONTRACT_TRX> contractTrxs;
    JUB_CONTRACT_TRX contractTrx;
    contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::NS_ITEM_TRX_CONTRACT;
    contractTrx.permissionId = root["TRX"]["contracts"]["permission_id"].asUInt64();
    switch (numType) {
        case 1: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::XFER_CONTRACT;
            sType = "1";
            JUB_XFER_CONTRACT_TRX transfer;
            transfer.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            transfer.to_address = (char *) root["TRX"]["contracts"][sType]["to_address"].asCString();
            transfer.amount = root["TRX"]["contracts"][sType]["amount"].asUInt64();
            contractTrx.transfer = transfer;
            break;
        }
        case 2: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::XFER_ASSET_CONTRACT;
            sType = "2";
            JUB_XFER_ASSET_CONTRACT_TRX transferAsset;
            transferAsset.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            transferAsset.asset_name = (char *) root["TRX"]["contracts"][sType]["asset_name"].asCString();
            transferAsset.to_address = (char *) root["TRX"]["contracts"][sType]["to_address"].asCString();
            transferAsset.amount = root["TRX"]["contracts"][sType]["amount"].asUInt64();
            contractTrx.transferAsset = transferAsset;
            break;
        }
        case 30: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::CREATE_SMART_CONTRACT;
            sType = "30";
            JUB_CREATE_SMART_CONTRACT_TRX createSmart;
            createSmart.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            createSmart.call_token_value = root["TRX"]["contracts"][sType]["call_token_value"].asUInt64();
            createSmart.token_id = root["TRX"]["contracts"][sType]["token_id"].asUInt64();
            createSmart.bytecode = (char *) root["TRX"]["contracts"][sType]["bytecode"].asCString();
            contractTrx.createSmart = createSmart;
            break;
        }
        case 31: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::TRIG_SMART_CONTRACT;
            sType = "31";
            JUB_TRIG_SMART_CONTRACT_TRX triggerSmart;
            triggerSmart.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            triggerSmart.contract_address = (char *) root["TRX"]["contracts"][sType]["contract_address"].asCString();
            triggerSmart.call_value = root["TRX"]["contracts"][sType]["call_value"].asUInt64();
            triggerSmart.data = (char *) root["TRX"]["contracts"][sType]["data"].asCString();
            triggerSmart.call_token_value = root["TRX"]["contracts"][sType]["call_token_value"].asUInt64();
            triggerSmart.token_id = root["TRX"]["contracts"][sType]["token_id"].asUInt64();
            contractTrx.triggerSmart = triggerSmart;
            break;
        }
        default:
            break;
    }
    contractTrxs.push_back(contractTrx);
    tx.contracts = &contractTrxs[0];
    tx.contractCount = 1;

    char *packedContractInPB = NULL;
    JUB_RV rv = JUB_PackContractTRX(static_cast<JUB_UINT16>(contextID), tx, &packedContractInPB);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    size_t len = strlen(reinterpret_cast<const char *>(packedContractInPB));
    jbyteArray contractByteArray = env->NewByteArray(len+1);
    env->SetByteArrayRegion(contractByteArray, 0, len,
                            reinterpret_cast<jbyte *>(packedContractInPB));
    env->ReleaseByteArrayElements(contractByteArray,
                                  env->GetByteArrayElements(contractByteArray, JNI_FALSE), 0);
    return contractByteArray;
}


#ifdef __cplusplus
}
#endif


// 定义 JNINativeMethod 数组
static JNINativeMethod g2TrxMethods[] = {
        {
                "nativeTRXCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_TRXCreateContext
        },
        {
                "nativeTRXGetAddress",
                "(JII)[Ljava/lang/String;",
                (void *) native_TRXGetAddress
        },
        {
                "nativeTRXGetHDNode",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_TRXGetHDNode
        },
        {
                "nativeTRXShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_TRXShowAddress
        },
        {
                "nativeTRXSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_TRXSetMyAddress
        },
        {
                "nativeTRXTransaction",
                "(JLjava/lang/String;[B)Ljava/lang/String;",
                (void *) native_TRXTransaction
        },
        {
                "nativeTRXSetTRC20Token",
                "(JLjava/lang/String;)I",
                (void *) native_TRXSetTRC20Token
        },
        {
                "nativeTRXBuildTRC20TransferAbi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_TRXBuildTRC20TransferAbi
        },
        {
                "nativeTRXSetTRC10Asset",
                "(JLjava/lang/String;)I",
                (void *) native_TRXSetTRC10Asset
        },
        {
                "nativeTRXPackContract",
                "(JLjava/lang/String;)[B",
                (void *) native_TRXPackContract
        },

};


#define G2_TRX_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2TrxNatives(JNIEnv* env) {
    LOG_INF("Register G2 TRX");
    REGISTER_NATIVES(env, G2_TRX_CLASS, g2TrxMethods, sizeof(g2TrxMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Trx(RegisterG2TrxNatives);