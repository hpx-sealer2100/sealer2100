
#include <jni.h>
#include "device_jni.h"
#include "JUB_SDK_COMM.h"
#include "JUB_SDK_DEV.h"
#include "utils/log_utils.h"
#include "json/json.h"
#include "common_jni.h"
#include "jni_registry.h"

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL native_ClearContext(JNIEnv *env, jclass obj, jlong jContextId) {
    JUB_RV rv = JUB_ClearContext(jContextId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_ClearContext: %08x", rv);
        errorCode = rv;
    }
    return rv;
}

JNIEXPORT jstring JNICALL native_getDeviceType(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_ENUM_COMMODE pComMode;
    JUB_ENUM_DEVICE pDevice;
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_GetDeviceType(deviceId, &pComMode, &pDevice);
    if (rv != JUBR_OK) {
        errorCode = rv;
        LOG_ERR("JUB_GetDeviceType rv: %08x", rv);
        return NULL;
    }
    Json::FastWriter writer;
    Json::Value root;
    root["COMMODE"] = pComMode;
    root["DEVICE"] = pDevice;
    jstring result = env->NewStringUTF(writer.write(root).c_str());
    return result;
}

JNIEXPORT jint JNICALL native_show(JNIEnv *env, jclass obj, jlong contextID) {
    JUB_RV rv = JUB_ShowVirtualPwd(static_cast<JUB_UINT16>(contextID));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_showVirtualPwd rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL native_CancelVirtualPwd(JNIEnv *env, jclass obj, jlong contextID) {
    JUB_RV rv = JUB_CancelVirtualPwd(static_cast<JUB_UINT16>(contextID));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CancelVirtualPwd rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL
native_verifyPIN(JNIEnv *env, jclass obj, jlong contextID, jbyteArray jPin) {
    JUB_CHAR_PTR pPin = (JUB_CHAR_PTR) (env->GetByteArrayElements(jPin, NULL));
    int length = env->GetArrayLength(jPin);

    // java数组没有结束符，jni层需补充
    *(pPin + length) = '\0';

    JUB_ULONG retry;
    JUB_RV ret = JUB_VerifyPIN(static_cast<JUB_UINT16>(contextID), pPin, &retry);
    if (ret != JUBR_OK) {
        LOG_ERR("JUB_VerifyPIN: %08x", ret);
    }
    env->ReleaseByteArrayElements(jPin, (jbyte *) pPin, JNI_ABORT);
    return ret;
}

JNIEXPORT jint JNICALL native_GetDeviceInfo(JNIEnv *env, jclass obj,
                                            jobject deviceInfo, jlong deviceHandle) {

#define SET_SN               "setSn"
#define SET_LABEL            "setLabel"
#define SETPIN_RETRY         "setPin_retry"
#define SETPIN_MAX_RETRY     "setPin_max_retry"
#define SETBLE_VERSION       "setBle_version"
#define SET_FIRMWARE_VERSION "setFirmware_version"

    JUB_DEVICE_INFO info;
    JUB_RV rv = JUB_GetDeviceInfo((JUB_UINT16) deviceHandle, &info);
    if (rv == JUBR_OK) {
        jclass clazz = env->GetObjectClass(deviceInfo);
        jmethodID setSN = env->GetMethodID(clazz, SET_SN, "(Ljava/lang/String;)V");
        env->CallVoidMethod(deviceInfo, setSN, env->NewStringUTF(info.sn));

        jmethodID setLabel = env->GetMethodID(clazz, SET_LABEL, "(Ljava/lang/String;)V");
        env->CallVoidMethod(deviceInfo, setLabel, env->NewStringUTF(info.label));

        jmethodID pin_retry = env->GetMethodID(clazz, SETPIN_RETRY, "(I)V");
        env->CallVoidMethod(deviceInfo, pin_retry, info.pinRetry);

        jmethodID pin_max_retry = env->GetMethodID(clazz, SETPIN_MAX_RETRY, "(I)V");
        env->CallVoidMethod(deviceInfo, pin_max_retry, info.pinMaxRetry);

        char tmpVersion[5] = {0,};
        memcpy(tmpVersion, info.bleVersion, 4);
        jmethodID ble_version = env->GetMethodID(clazz, SETBLE_VERSION, "(Ljava/lang/String;)V");
        jstring version = env->NewStringUTF(tmpVersion);
        env->CallVoidMethod(deviceInfo, ble_version, version);

        char tmpFirewareVersion[5] = {0,};
        memcpy(tmpFirewareVersion, info.firmwareVersion, 4);
        jmethodID firmware_version = env->GetMethodID(clazz, SET_FIRMWARE_VERSION,
                                                      "(Ljava/lang/String;)V");
        jstring firmware_versionStr = env->NewStringUTF(tmpFirewareVersion);
        env->CallVoidMethod(deviceInfo, firmware_version, firmware_versionStr);
    }
    return rv;
} ;

JNIEXPORT jstring JNICALL native_sendAPDU(JNIEnv *env, jclass obj, jlong deviceID,
                                          jstring jApdu) {
    JUB_CHAR_PTR pApdu = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jApdu, NULL));
    JUB_CHAR_PTR response = nullptr;
    JUB_RV ret = JUB_SendOneApdu(static_cast<JUB_UINT16>(deviceID), pApdu, &response);
    if (ret == JUBR_OK) {
        jstring result = env->NewStringUTF(response);
        JUB_FreeMemory(response);
        return result;
    } else {
        return NULL;
    }
}

JNIEXPORT jstring JNICALL native_GetDeviceCert(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_CHAR_PTR cert = NULL;
    JUB_RV rv = JUB_GetDeviceCert((JUB_UINT16) deviceHandle, &cert);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(cert);
        JUB_FreeMemory(cert);
        return result;
    } else {
        LOG_ERR("JUB_GetDeviceCert error");
        return NULL;
    }
}

JNIEXPORT jstring JNICALL native_GetDeviceSignData(JNIEnv *env, jclass obj, jlong deviceHandle,jstring jhashData) {
    JUB_CHAR_PTR phashData = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jhashData, NULL));
    JUB_CHAR_PTR SignData = NULL;
    JUB_RV rv = JUB_GetDeviceSignData((JUB_UINT16) deviceHandle, phashData,&SignData);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(SignData);
        JUB_FreeMemory(SignData);
        return result;
    } else {
        LOG_ERR("JUB_GetDeviceSignData error");
        return NULL;
    }
}

JNIEXPORT jstring JNICALL
native_GetAppletVersion(JNIEnv *env, jclass obj, jlong deviceHandle, jstring appID) {
    JUB_CHAR_PTR pAppID = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(appID, NULL));
    JUB_CHAR_PTR appVersion = NULL;
    JUB_RV rv = JUB_GetAppletVersion((JUB_UINT16) deviceHandle, pAppID, &appVersion);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(appVersion);
        JUB_FreeMemory(appVersion);
        return result;
    } else {
        LOG_ERR("JUB_GetAppletVersion error");
        return NULL;
    }
}

JNIEXPORT jstring JNICALL native_EnumApplets(JNIEnv *env, jclass obj, jlong deviceHandle) {

    JUB_CHAR_PTR list = NULL;
    JUB_RV rv = JUB_EnumApplets((JUB_UINT16) deviceHandle, &list);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(list);
        JUB_FreeMemory(list);
        return result;
    } else {
        LOG_ERR("JUB_EnumApplets error");
        return NULL;
    }
}

JNIEXPORT jint JNICALL native_SetTimeOut(JNIEnv *env, jclass obj, jlong contextID, jint jTimeOut) {
    return static_cast<jint>(JUB_SetTimeOut(static_cast<JUB_UINT16>(contextID),
                                            static_cast<JUB_UINT16>(jTimeOut)));
}
JNIEXPORT jint JNICALL native_SetLable(JNIEnv *env, jclass obj, jlong contextID, jstring jLable) {
    //whb
    JUB_CHAR_PTR pLable = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jLable, NULL));
    JUB_RV rv = JUB_SetLable(static_cast<JUB_UINT16>(contextID), pLable);
    return static_cast<jint>(rv);
}
JNIEXPORT jint JNICALL native_EndSession(JNIEnv *env, jclass obj, jlong contextID) {
    //whb
    JUB_RV rv = JUB_EndSession(static_cast<JUB_UINT16>(contextID));
    return static_cast<jint>(rv);
}
JNIEXPORT jint JNICALL native_OpCancel(JNIEnv *env, jclass obj, jlong contextID) {
    //whb
    JUB_RV rv = JUB_OpCancel(static_cast<JUB_UINT16>(contextID));
    return static_cast<jint>(rv);
}

JNIEXPORT jstring JNICALL native_GetFeatures(JNIEnv *env, jclass obj, jlong contextID){
    //whb
    JUB_CHAR_PTR Features = NULL;
    JUB_RV rv = JUB_GetFeatures((JUB_UINT16) contextID, &Features);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(Features);
        JUB_FreeMemory(Features);
        return result;
    } else {
        LOG_ERR("native_GetFeatures error");
        return NULL;
    }
}


JNIEXPORT jint JNICALL
native_QueryBattery(JNIEnv *env, jclass obj, jlong deviceID, jintArray batteryArray) {

    jint *pBattery = env->GetIntArrayElements(batteryArray, NULL);
    JUB_BYTE battery = 0;
    JUB_RV rv = JUB_QueryBattery(deviceID, &battery);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_QueryBattery rv: %08lx", rv);
        env->ReleaseIntArrayElements(batteryArray, pBattery, 0);
        return rv;
    }
    *pBattery = battery & 0x0FF;
    env->ReleaseIntArrayElements(batteryArray, pBattery, 0);
    return rv;
}

JNIEXPORT jint JNICALL native_IsInitialize(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IsInitialize(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IsInitialize rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL native_IsBootLoader(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IsBootLoader(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IsBootLoader rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jstring JNICALL native_EnumSupportCoins(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_CHAR_PTR pCoinList = NULL;
    JUB_RV rv = Jub_EnumSupportCoins(deviceID, &pCoinList);
    if (rv != JUBR_OK) {
        LOG_ERR("Jub_EnumSupportCoins rv: %08x", rv);
        return env->NewStringUTF("");
    }
    jstring coinList = env->NewStringUTF(pCoinList);
    JUB_FreeMemory(pCoinList);
    return coinList;
}


JNIEXPORT jstring JNICALL native_GetVersion(JNIEnv *env, jclass obj) {
    JUB_CHAR_PTR pVersion = JUB_GetVersion();
    if (pVersion == NULL) {
        LOG_ERR("JUB_GetVersion : %s", pVersion);
        return NULL;
    }
    jstring version = env->NewStringUTF(pVersion);
    return version;
}

JNIEXPORT jint JNICALL native_getErrorCode(JNIEnv *env, jclass obj) {
    return errorCode;
}


//-----------------------------------------------------------------------------------------------


// 定义 JNINativeMethod 数组
static JNINativeMethod g2DeviceMethods[] = {
        {
                "nativeClearContext",
                "(J)I",
                (void *) native_ClearContext
        },
        {
                "nativeShowVirtualPwd",
                "(J)I",
                (void *) native_show
        },
        {
                "nativeCancelVirtualPwd",
                "(J)I",
                (void *) native_CancelVirtualPwd
        },
        {
                "nativeGetDeviceType",
                "(J)Ljava/lang/String;",
                (void *) native_getDeviceType
        },
        {
                "nativeVerifyPIN",
                "(J[B)I",
                (void *) native_verifyPIN
        },
        {
                "nativeGetDeviceInfo",
                "(Lcom/legendwd/hyperpay/onchain/hardware/jubnative/utils/JUB_DEVICE_INFO;J)I",
                //"(Lcom/jubiter/sdk/demo/jubnative/utils/JUB_DEVICE_INFO;J)I",
                (void *) native_GetDeviceInfo
        },
        {
                "nativeSendApdu",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_sendAPDU
        },
        {
                "nativeGetDeviceCert",
                "(J)Ljava/lang/String;",
                (void *) native_GetDeviceCert
        },
        {
                "nativeEnumApplets",
                "(J)Ljava/lang/String;",
                (void *) native_EnumApplets
        },
        {
                "nativeGetAppletVersion",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_GetAppletVersion
        },
        {
                "nativeSetTimeOut",
                "(JI)I",
                (void *) native_SetTimeOut
        },
        {
                "nativeQueryBattery",
                "(J[I)I",
                (void *) native_QueryBattery
        },
        {
                "nativeIsInitialize",
                "(J)I",
                (void *) native_IsInitialize
        },
        {
                "nativeIsBootLoader",
                "(J)I",
                (void *) native_IsBootLoader
        },
        {
                "nativeGetVersion",
                "()Ljava/lang/String;",
                (void *) native_GetVersion
        },
        {
                "nativeEnumSupportCoins",
                "(J)Ljava/lang/String;",
                (void *) native_EnumSupportCoins
        },
        {
                "nativeGetErrorCode",
                "()I",
                (void *) native_getErrorCode
        },
        {
                "nativeSetLable",
                "(JLjava/lang/String;)I",
                (void *) native_SetLable
        },
        {
                "nativeGetFeatures",
                "(J)Ljava/lang/String;",
                (void *) native_GetFeatures
        },
        {
                "nativeGetDeviceSignData",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_GetDeviceSignData
        },
        {
                "nativeEndSession",
                "(J)I",
                (void *) native_EndSession
        },
        {
                "nativeOpCancel",
                "(J)I",
                (void *) native_OpCancel
        },
};

#define MAX_DEVICE_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2DeviceNatives(JNIEnv* env) {
    LOG_INF("Register G2 Device");
    REGISTER_NATIVES(env, MAX_DEVICE_CLASS, g2DeviceMethods, sizeof(g2DeviceMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Device(RegisterG2DeviceNatives);

#ifdef __cplusplus
}
#endif