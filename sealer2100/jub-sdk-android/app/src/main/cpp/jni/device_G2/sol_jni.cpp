//
// Created by feng on 2025/8/10.
//

#include "sol_jni.h"
#include "common_jni.h"
#include "json/reader.h"
#include "jni_registry.h"
#include "JUB_SDK_SOL.h"
#include <JUB_SDK.h>


#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
native_SOLCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo) {

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
    rv = JUB_CreateContextSOL(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextSOL: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jobjectArray JNICALL
native_SOLGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
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
    JUB_RV rv = JUB_GetAddressSOL(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressSOL: %08x", rv);
        errorCode = static_cast<int>(rv);

        env->SetObjectArrayElement(array, 0, NULL);
        env->SetObjectArrayElement(array, 1, NULL);
        return array;
    }

    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::HEX;

//    JUB_CHAR_PTR pPubKey = NULL;//sol无法获取公钥，直接返回null
//    JUB_RV nodeRv = JUB_GetHDNodeSOL(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
//    if (nodeRv != JUBR_OK) {
//        LOG_ERR("JUB_GetHDNodeSOL: %08x", nodeRv);
//        errorCode = static_cast<int>(nodeRv);
//
//        env->SetObjectArrayElement(array, 0, NULL);
//        env->SetObjectArrayElement(array, 1, address);
//        return array;
//    }
//
//    // hex 格式拼接 0x 前缀
//    std::string hexPubKey = "0x";
//    if (pPubKey != nullptr) {
//        hexPubKey += pPubKey;
//    }
//
//    jstring pubKey = env->NewStringUTF(hexPubKey.c_str());
//    JUB_FreeMemory(pPubKey);

//    env->SetObjectArrayElement(array, 0, pubKey);
    env->SetObjectArrayElement(array, 0, NULL);
    env->SetObjectArrayElement(array, 1, address);

    return array;
}
JNIEXPORT jobjectArray JNICALL
native_SOLShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
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
    JUB_RV rv = JUB_GetAddressSOL(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressSOL: %08x", rv);
        errorCode = static_cast<int>(rv);

        env->SetObjectArrayElement(array, 0, NULL);
        env->SetObjectArrayElement(array, 1, NULL);
        return array;
    }

    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::HEX;

//    JUB_CHAR_PTR pPubKey = NULL;//sol无法获取公钥，直接返回null
//    JUB_RV nodeRv = JUB_GetHDNodeSOL(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
//    if (nodeRv != JUBR_OK) {
//        LOG_ERR("JUB_GetHDNodeSOL: %08x", nodeRv);
//        errorCode = static_cast<int>(nodeRv);
//
//        env->SetObjectArrayElement(array, 0, NULL);
//        env->SetObjectArrayElement(array, 1, address);
//        return array;
//    }
//
//    // hex 格式拼接 0x 前缀
//    std::string hexPubKey = "0x";
//    if (pPubKey != nullptr) {
//        hexPubKey += pPubKey;
//    }
//
//    jstring pubKey = env->NewStringUTF(hexPubKey.c_str());
//    JUB_FreeMemory(pPubKey);

//    env->SetObjectArrayElement(array, 0, pubKey);
    env->SetObjectArrayElement(array, 0, NULL);
    env->SetObjectArrayElement(array, 1, address);

    return array;
}

JNIEXPORT jstring JNICALL
native_SOLGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex) {

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::HEX;

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR xpub;
    JUB_RV rv = JUB_GetHDNodeSOL(contextID, format, path, &xpub);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeSOL: %08x", rv);
        return NULL;
    }
    jstring mainPub = env->NewStringUTF(xpub);
    JUB_FreeMemory(xpub);
    return mainPub;
}

JNIEXPORT jstring JNICALL native_SOLGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID) {

    JUB_CHAR_PTR xpub;
    JUB_RV rv = JUB_GetMainHDNodeSOL(contextID, XPUB, &xpub);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetMainHDNodeSOL: %08x", rv);
        return NULL;
    }
    jstring mainPub = env->NewStringUTF(xpub);
    JUB_FreeMemory(xpub);
    return mainPub;
}

JNIEXPORT jstring JNICALL native_SOLTransaction(JNIEnv *env, jclass obj,
                                                jlong contextID, jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);


    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["bip32_path"]["change"].asBool();
    path.addressIndex = root["bip32_path"]["addressIndex"].asUInt();

    char *dest = (char *) root["dest"].asCString();
    char *recentHash = (char *) root["recentHash"].asCString();
    JUB_UINT64 amount = root["amount"].asUInt64();

    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionSOL(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       recentHash,
                                       dest,
                                       amount,
                                       &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        LOG_ERR("JUB_SignTransactionSOL: %08x", rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jstring JNICALL native_SOLTokenTransaction(JNIEnv *env, jclass obj,
                                                jlong contextID, jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);


    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["bip32_path"]["change"].asBool();
    path.addressIndex = root["bip32_path"]["addressIndex"].asUInt();

    char *dest = (char *) root["dest"].asCString();
    char *recentHash = (char *) root["recentHash"].asCString();
    JUB_UINT64 amount = root["amount"].asUInt64();

    char *tokenMint = (char *) root["tokenMint"].asCString();
    char *source = (char *) root["source"].asCString();
    JUB_UINT8 decimal = root["decimal"].asUInt();

    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionTokenSOL(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       recentHash,
                                       tokenMint,
                                       source,
                                       dest,
                                       amount,
                                       decimal,
                                       &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        LOG_ERR("JUB_SignTransactionSOL: %08x", rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}


JNIEXPORT jstring JNICALL native_SOLTokenCreateTransaction(JNIEnv *env, jclass obj,
                                                     jlong contextID, jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);


    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["bip32_path"]["change"].asBool();
    path.addressIndex = root["bip32_path"]["addressIndex"].asUInt();

    char *dest = (char *) root["dest"].asCString();
    char *recentHash = (char *) root["recentHash"].asCString();
    JUB_UINT64 amount = root["amount"].asUInt64();

    char *tokenMint = (char *) root["tokenMint"].asCString();
    char *destMainAddress = (char *) root["destMainAddress"].asCString();
    char *source = (char *) root["source"].asCString();
    JUB_UINT8 decimal = root["decimal"].asUInt();

    char *raw = NULL;
    JUB_RV rv = JUB_SignTxTokenCreateAndTransferSOL(static_cast<JUB_UINT16>(contextID),
                                            path,
                                            recentHash,
                                            tokenMint,
                                            destMainAddress,
                                            source,
                                            dest,
                                            amount,
                                            decimal,
                                            &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        LOG_ERR("JUB_SignTransactionSOL: %08x", rv);
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
static JNINativeMethod g2SolMethods[] = {
        {
                "nativeSOLCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_SOLCreateContext
        },
        {
                "nativeSOLGetAddress",
                "(JII)[Ljava/lang/String;",
                (void *) native_SOLGetAddress
        },
        {
                "nativeSOLShowAddress",
                "(JII)[Ljava/lang/String;",
                (void *) native_SOLShowAddress
        },
        {
                "nativeSOLGetMainHDNode",
                "(J)Ljava/lang/String;",
                (void *) native_SOLGetMainHDNode
        },
        {
                "nativeSOLGetHDNode",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_SOLGetHDNode
        },
        {
                "nativeSOLTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_SOLTransaction
        },
        {
                "nativeSOLTokenTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_SOLTokenTransaction
        },
        {
                "nativeSOLTokenCreateTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_SOLTokenCreateTransaction
        },
};


#define G2_SOL_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2SolNatives(JNIEnv* env) {
    LOG_INF("Register G2 SOL");
    REGISTER_NATIVES(env, G2_SOL_CLASS, g2SolMethods, sizeof(g2SolMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Sol(RegisterG2SolNatives);
