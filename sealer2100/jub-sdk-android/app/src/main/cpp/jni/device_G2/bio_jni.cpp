//
// Created by feng on 2025/6/4.
//

#include <jni.h>
#include "bio_jni.h"
#include "JUB_SDK.h"
#include "JUB_SDK_DEV_BIO.h"
#include "utils/logUtils.h"
#include "common_jni.h"
#include "json/writer.h"
#include "jni_registry.h"


#ifdef __cplusplus
extern "C" {
#endif


//JNIEXPORT jint JNICALL
//native_identityVerify(JNIEnv *env, jclass obj, jlong contextID, jint verifyMode,
//                      jlongArray retryTimes) {
//    JUB_ULONG_PTR pRetryTimes = reinterpret_cast<JUB_ULONG_PTR>(env->GetLongArrayElements(
//            retryTimes, NULL));
//    JUB_ENUM_IDENTITY_VERIFY_MODE mode;
//    switch (verifyMode) {
//        case 1:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_DEVICE;
//            break;
//        case 2:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
//            break;
//        case 3:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_APDU;
//            break;
//        case 4:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_FPGT;
//            break;
//        default:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
//            break;
//    }
//    JUB_RV ret = JUB_IdentityVerify(static_cast<JUB_UINT16>(contextID), mode, pRetryTimes);
//    if (ret != JUBR_OK) {
//        LOG_ERR("JUB_IdentityVerify: %08x", ret);
//    }
//    env->ReleaseLongArrayElements(retryTimes, reinterpret_cast<jlong *>(pRetryTimes), 0);
//    return static_cast<jint>(ret);
//}

JNIEXPORT jint JNICALL
native_identityVerifyPIN(JNIEnv *env, jclass obj, jlong deviceID, jint verifyMode, jstring jPin,
                         jlongArray retryTimes) {
    JUB_CHAR_PTR pPin = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jPin, NULL));
    JUB_ULONG_PTR pRetryTimes = reinterpret_cast<JUB_ULONG_PTR>(env->GetLongArrayElements(
            retryTimes, NULL));
    JUB_ENUM_IDENTITY_VERIFY_MODE mode;
    switch (verifyMode) {
//        case 1:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_DEVICE;
//            break;
        case 2:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
            break;
//        case 3:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_APDU;
//            break;
        case 4:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_FPGT;
            break;
        default:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
            break;
    }
    JUB_RV ret = JUB_IdentityVerifyPIN(static_cast<JUB_UINT16>(deviceID), mode, pPin, pRetryTimes);
    if (ret != JUBR_OK) {
        LOG_ERR("JUB_IdentityVerifyPIN: %08x", ret);
    }
    env->ReleaseStringUTFChars(jPin, reinterpret_cast<const char *>(pPin));
    env->ReleaseLongArrayElements(retryTimes, reinterpret_cast<jlong *>(pRetryTimes), 0);
    return static_cast<jint>(ret);
}

JNIEXPORT jint JNICALL native_identityShowNineGrids(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IdentityShowNineGrids(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IdentityShowNineGrids rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL native_identityCancelNineGrids(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IdentityCancelNineGrids(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IdentityCancelNineGrids rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jstring JNICALL
native_enrollFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut,
                         jbyteArray fgptIndex) {
    JUB_UINT16 deviceId = deviceID;
    JUB_BYTE *pFgptIndex = (JUB_BYTE *) env->GetByteArrayElements(fgptIndex, NULL);
    JUB_ULONG pTimes;
    JUB_BYTE pFgptID;
    JUB_RV rv = JUB_EnrollFingerprint(deviceId, timeOut, pFgptIndex,
                                      &pTimes, &pFgptID);
    if (rv != JUBR_OK) {
        errorCode = rv;
        LOG_ERR("JUB_EnrollFingerprint rv: %08x", rv);
        return NULL;
    }
    errorCode = 0;
    env->ReleaseByteArrayElements(fgptIndex, reinterpret_cast<jbyte *>(pFgptIndex), 0);
    Json::FastWriter writer;
    Json::Value root;
    Json::Int64 tmstp = pTimes;
    root["times"] = tmstp;
    root["fgptID"] = pFgptID;
    jstring result = env->NewStringUTF(writer.write(root).c_str());
    return result;
}

JNIEXPORT jstring JNICALL native_enumFingerprint(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_CHAR_PTR pFgptList = nullptr;
    JUB_RV rv = JUB_EnumFingerprint(deviceId, &pFgptList);
    if (rv != JUBR_OK) {
        errorCode = rv;
        LOG_ERR("JUB_EnumFingerprint rv: %08x", rv);
        return NULL;
    }
    jstring fgptList = env->NewStringUTF(pFgptList);

    return fgptList;
}

JNIEXPORT jint JNICALL
native_eraseFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_EraseFingerprint(deviceId, timeOut);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_EraseFingerprint rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL
native_deleteFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut, jbyte fgptID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_DeleteFingerprint(deviceId, timeOut, fgptID);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_DeleteFingerprint rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL
native_verifyFingerprint(JNIEnv *env, jclass obj, jlong contextID, jlongArray retryTimes) {
    JUB_ULONG_PTR pRetryTimes = reinterpret_cast<JUB_ULONG_PTR>(env->GetLongArrayElements(
            retryTimes, NULL));
    JUB_RV ret = JUB_VerifyFingerprint(static_cast<JUB_UINT16>(contextID), pRetryTimes);
    if (ret != JUBR_OK) {
        LOG_ERR("JUB_VerifyFingerprint: %08x", ret);
    }
    env->ReleaseLongArrayElements(retryTimes, reinterpret_cast<jlong *>(pRetryTimes), 0);
    return static_cast<jint>(ret);
}


#ifdef __cplusplus
}
#endif



// 定义 JNINativeMethod 数组
static JNINativeMethod g2BioMethods[] = {
//        {
//                "nativeIdentityVerify",
//                "(JI[J)I",
//                (void *) native_identityVerify
//        },
        {
                "nativeIdentityVerifyPIN",
                "(JILjava/lang/String;[J)I",
                (void *) native_identityVerifyPIN
        },
        {
                "nativeIdentityShowNineGrids",
                "(J)I",
                (void *) native_identityShowNineGrids
        },
        {
                "nativeIdentityCancelNineGrids",
                "(J)I",
                (void *) native_identityCancelNineGrids
        },
        {
                "nativeEnrollFingerprint",
                "(JJ[B)Ljava/lang/String;",
                (void *) native_enrollFingerprint
        },
        {
                "nativeEnumFingerprint",
                "(J)Ljava/lang/String;",
                (void *) native_enumFingerprint
        },
        {
                "nativeEraseFingerprint",
                "(JJ)I",
                (void *) native_eraseFingerprint
        },
        {
                "nativeDeleteFingerprint",
                "(JJB)I",
                (void *) native_deleteFingerprint
        },
        {
                "nativeVerifyFingerprint",
                "(J[J)I",
                (void *) native_verifyFingerprint
        },

};

#define G2_BIO_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2BioNatives(JNIEnv* env) {
    LOG_INF("Register G2 BIO");
    REGISTER_NATIVES(env, G2_BIO_CLASS, g2BioMethods, sizeof(g2BioMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Bio(RegisterG2BioNatives);
