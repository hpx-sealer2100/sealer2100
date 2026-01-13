//
// Created by feng on 2025/6/4.
//

#include <jni.h>
#include "hc_jni.h"
#include "common_jni.h"
#include "json/reader.h"
#include "utils/logUtils.h"
#include "jni_registry.h"
#include "JUB_SDK_FIL.h"
#include <JUB_SDK.h>


#ifdef __cplusplus
extern "C" {
#endif



JNIEXPORT jint JNICALL
native_FILCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
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

    CONTEXT_CONFIG_FIL cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();


    JUB_RV rv = JUBR_OK;
    rv = JUB_CreateContextFIL(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("native_FILCreateContext: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jobjectArray JNICALL
native_FILGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

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
    JUB_RV rv = JUB_GetAddressFIL(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressFIL: %08x", rv);
        errorCode = static_cast<int>(rv);

        env->SetObjectArrayElement(array, 0, NULL);
        env->SetObjectArrayElement(array, 1, NULL);
        return array;
    }

    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::HEX;

    JUB_CHAR_PTR pPubKey = NULL;
    JUB_RV nodeRv = JUB_GetHDNodeFIL(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (nodeRv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeFIL: %08x", nodeRv);
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
native_FILGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex) {

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::XPUB;
    if (useHex) {
        format = JUB_ENUM_PUB_FORMAT::HEX;
    }

    JUB_CHAR_PTR pPubKey = NULL;
    JUB_RV rv = JUB_GetHDNodeFIL(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeFIL: %08x", rv);
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
native_FILShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressFIL(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressFIL: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_FILSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressFIL(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressFIL: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_FILTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);


    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["bip32_path"]["change"].asBool();
    path.addressIndex = root["bip32_path"]["addressIndex"].asUInt();
//    BIP32_Path path;
//    if (root["bip32_path"]["change"].asBool()) {
//        path.change = JUB_ENUM_BOOL::BOOL_TRUE;
//    } else {
//        path.change = JUB_ENUM_BOOL::BOOL_FALSE;
//    }

//    uint16_t nonce = root["nonce"].asDouble();
//    uint16_t gasLimit = root["gasLimit"].asDouble();
    uint64_t nonce = root["nonce"].asUInt64();
    int64_t gasLimit = root["gasLimit"].asInt64();
    char *gasFeeCapInAtto = (char *) root["gasFeeCapInAtto"].asCString();
    char *gasPremiumInAtto = (char *) root["gasPremiumInAtto"].asCString();
    char *to = (char *) root["to"].asCString();
    char *valueInAtto = (char *) root["valueInAtto"].asCString();
    char *input = (char *) root["data"].asCString();

    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionFIL(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       nonce,
                                       gasLimit,
                                       gasFeeCapInAtto,
                                       gasPremiumInAtto,
                                       to,
                                       valueInAtto,
                                       input,
                                       &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        LOG_ERR("JUB_SignTransactionFIL: %08x", rv);
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
static JNINativeMethod g2FilMethods[] = {
        {
                "nativeFILCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_FILCreateContext
        },
        {
                "nativeFILGetAddress",
                "(JII)[Ljava/lang/String;",
                (void *) native_FILGetAddress
        },
        {
                "nativeFILGetHDNode",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_FILGetHDNode
        },
        {
                "nativeFILShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_FILShowAddress
        },
        {
                "nativeFILSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_FILSetMyAddress
        },
        {
                "nativeFILTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_FILTransaction
        },
};

#ifdef HC

#define G2_FIL_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2FilNatives(JNIEnv* env) {
    LOG_INF("Register G2 FIL");
    REGISTER_NATIVES(env, G2_FIL_CLASS, g2FilMethods, sizeof(g2FilMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Hc(RegisterG2FilNatives);

#endif
