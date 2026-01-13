//
// Created by feng on 2025/6/4.
//

#include <jni.h>
#include "xrp_jni.h"
#include "JUB_SDK_COMM.h"
#include "common_jni.h"
#include "json/reader.h"
#include "JUB_SDK.h"
#include "utils/logUtils.h"
#include "utility/Debug.hpp"
#include "jni_registry.h"


#ifdef __cplusplus
extern "C" {
#endif



JNIEXPORT jint JNICALL
native_XRPCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
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

    CONTEXT_CONFIG_XRP cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();


    JUB_RV rv = JUBR_OK;
    rv = JUB_CreateContextXRP(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("native_XRPCreateContext: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jobjectArray JNICALL
native_XRPGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
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
    JUB_RV rv = JUB_GetAddressXRP(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressXRP: %08x", rv);
        errorCode = static_cast<int>(rv);

        env->SetObjectArrayElement(array, 0, NULL);
        env->SetObjectArrayElement(array, 1, NULL);
        return array;
    }

    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::HEX;

    JUB_CHAR_PTR pPubKey = NULL;
    JUB_RV nodeRv = JUB_GetHDNodeXRP(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (nodeRv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeXRP: %08x", nodeRv);
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
native_XRPGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
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
    JUB_RV rv = JUB_GetHDNodeXRP(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeXRP: %08x", rv);
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
native_XRPShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressXRP(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressXRP: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_XRPSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressXRP(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressXRP: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_XRPTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {
    DEBUG_LOG(" JSON: %s\n", env->GetStringUTFChars(jJSON, FALSE));

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    JUB_TX_XRP tx;
    int numType = root["XRP"]["type"].asUInt();
    const char *sType = "0";

    tx.type = JUB_ENUM_XRP_TX_TYPE::PYMT;
    if (numType != 0) {
        sType = "1";
        tx.type = JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE;
    }
    switch (tx.type) {
        case JUB_ENUM_XRP_TX_TYPE::PYMT: {
            tx.account = (char *) root["XRP"]["account"].asCString();
            tx.fee = (char *) root["XRP"]["fee"].asCString();
            tx.sequence = (char *) root["XRP"]["sequence"].asCString();
            tx.flags = (char *) root["XRP"]["flags"].asCString();
            tx.lastLedgerSequence = (char *) root["XRP"]["lastLedgerSequence"].asCString();

            tx.pymt;
            int txPymtType = root["XRP"][sType]["type"].asUInt();
            tx.pymt.type = txPymtType == 0 ? JUB_ENUM_XRP_PYMT_TYPE::DXRP
                                           : JUB_ENUM_XRP_PYMT_TYPE::NS_ITEM_PYMT_TYPE;
//            const char* pymtType = std::to_string(tx.pymt.type).c_str();
            switch (tx.pymt.type) {
                case JUB_ENUM_XRP_PYMT_TYPE::DXRP: {
                    tx.pymt.amount;
                    tx.pymt.amount.value = (char *) root["XRP"][sType]["amount"]["value"].asCString();
                    tx.pymt.destination = (char *) root["XRP"][sType]["destination"].asCString();
                    if (!root["XRP"][sType]["destinationTag"].empty()) {
                        tx.pymt.destinationTag = (char *) root["XRP"][sType]["destinationTag"].asCString();
                    }
                    break;
                }
                case JUB_ENUM_XRP_PYMT_TYPE::NS_ITEM_PYMT_TYPE:
                    break;
            }
            break;
        }
        case JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE: {
            tx.accountTxnID = (char *) root["XRP"]["accountTxnID"].asCString();
            tx.sourceTag = (char *) root["XRP"]["sourceTag"].asCString();
            break;
        }
    }
    JUB_XRP_MEMO memo;
    memo.type = const_cast<JUB_CHAR_PTR>("");
    memo.data = const_cast<JUB_CHAR_PTR>("");
    memo.format = const_cast<JUB_CHAR_PTR>("");
    if (!root["XRP"]["memo"].empty()) {
        if (!root["XRP"]["memo"]["type"].empty()) {
            memo.type = (char *) root["XRP"]["memo"]["type"].asCString();
        }
        if (!root["XRP"]["memo"]["data"].empty()) {
            memo.data = (char *) root["XRP"]["memo"]["data"].asCString();
        }
        if (!root["XRP"]["memo"]["format"].empty()) {
            memo.format = (char *) root["XRP"]["memo"]["format"].asCString();
        }
    }
    tx.memo = memo;
    BIP32_Path path;
    if (root["XRP"]["bip32_path"]["change"].asBool()) {
        path.change = JUB_ENUM_BOOL::BOOL_TRUE;
    } else {
        path.change = JUB_ENUM_BOOL::BOOL_FALSE;
    }
    path.addressIndex = root["XRP"]["bip32_path"]["addressIndex"].asUInt();



    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionXRP(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       tx,
                                       &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}






#ifdef __cplusplus
}
#endif


// 定义 JNINativeMethod 数组
static JNINativeMethod g2XrpMethods[] = {
        {
                "nativeXRPCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_XRPCreateContext
        },
        {
                "nativeXRPGetAddress",
                "(JII)[Ljava/lang/String;",
                (void *) native_XRPGetAddress
        },
        {
                "nativeXRPGetHDNode",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_XRPGetHDNode
        },
        {
                "nativeXRPShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_XRPShowAddress
        },
        {
                "nativeXRPSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_XRPSetMyAddress
        },
        {
                "nativeXRPTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_XRPTransaction
        },

};


#define G2_XRP_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2XrpNatives(JNIEnv* env) {
    LOG_INF("Register G2 XRP");
    REGISTER_NATIVES(env, G2_XRP_CLASS, g2XrpMethods, sizeof(g2XrpMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Xrp(RegisterG2XrpNatives);