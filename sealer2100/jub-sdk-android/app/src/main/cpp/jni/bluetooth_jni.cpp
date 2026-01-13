#include "bluetooth_jni.h"
#include "JUB_SDK_COMM.h"
#include "JUB_SDK.h"
#include "utils/log_utils.h"
#include "utils/utils.h"
#include "common_jni.h"
#include "common_max_jni.h"
#include "jni_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL native_initDevice(JNIEnv *env, jclass obj) {
    LOG_ERR(">>>> in native_initDevice");
    // 初始化 Max
    initMax(env, obj);

    DEVICE_INIT_PARAM initParam;

    // 初始化参数转换
    jobjectToInitParam(env, g_vm, &initParam);

    JUB_RV rv = JUB_initDevice(initParam);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_initDevice ret: %08x", rv);
        return rv;
    }
    return rv;
}


JNIEXPORT jint JNICALL native_startScan(JNIEnv *env, jclass obj, jobject scanCallback) {
    jobject objParam = env->NewGlobalRef(scanCallback);
    setScanCallbackObj(g_vm, objParam);

    JUB_RV rv = JUB_enumDevices();
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_enumDevices rv: %08x", rv);
        return rv;
    }
    return rv;
}

JNIEXPORT jint JNICALL native_stopScan(JNIEnv *env, jclass obj) {
    JUB_RV rv = JUB_stopEnumDevices();
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_stopEnumDevices rv: %08x", rv);
        return rv;
    }
    return rv;
}

JNIEXPORT jint JNICALL
native_connectDevice(JNIEnv *env, jclass obj, jstring devName, jstring address, jint devType,
                     jlongArray handle, jint timeout, jobject disCallback) {
    JUB_BYTE_PTR pAddress = (JUB_BYTE_PTR) (env->GetStringUTFChars(address, NULL));
    JUB_BYTE_PTR pDevName = (JUB_BYTE_PTR) (env->GetStringUTFChars(devName, NULL));
    JUB_UINT16 *pHandle = reinterpret_cast<JUB_UINT16 *>(env->GetLongArrayElements(handle, NULL));

    jobject objParam = env->NewGlobalRef(disCallback);
    setDiscCallbackObj(g_vm, objParam);


    JUB_RV rv = JUB_connectDevice(pDevName, pAddress, devType, pHandle, timeout);

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_connectDevice rv: %08x", rv);

        return rv;
    }
    if (   0 == std::string((char*)pDevName).find(FAKE_BLE_NAME_MAGIC)
            && 0 == std::string((char*)pAddress).find(FAKE_BLE_UUID_MAGIC)
        ) 
    {
       rv = JUB_BleNusSetReadWriteCallback(*pHandle, sendDataToJavaWrapper, readDataFromJavaWrapper);
       rv = JUB_BleNusSetNotifyMessageCallback(*pHandle, notifyJavaWithMessageWrapper);
       rv = JUB_BleNusSetPasephraseCallback(*pHandle, setPassphraseWrapper);
       rv = JUB_BleNusSetUpdatePercentageCallback(*pHandle, notifyUpdatePercentageWrapper);
    }

    // rv = JUB_BleNusOpenSecureChannel(*pHandle);

    env->ReleaseLongArrayElements(handle, reinterpret_cast<jlong *>(pHandle), 0);
    env->ReleaseStringUTFChars(devName, reinterpret_cast<const char *>(pDevName));
    env->ReleaseStringUTFChars(address, reinterpret_cast<const char *>(pAddress));

    return rv;
}


JNIEXPORT jint JNICALL
native_cancelConnect(JNIEnv *env, jobject obj, jstring devName, jstring address) {
    JUB_BYTE_PTR pAddress = (JUB_BYTE_PTR) env->GetStringUTFChars(address, NULL);
    JUB_BYTE_PTR pDevName = (JUB_BYTE_PTR) env->GetStringUTFChars(devName, NULL);

    JUB_RV rv = JUB_cancelConnect(pDevName, pAddress);

    env->ReleaseStringUTFChars(address, reinterpret_cast<const char *>(pAddress));
    env->ReleaseStringUTFChars(devName, reinterpret_cast<const char *>(pDevName));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_cancelConnect rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL native_disconnectDevice(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_RV rv = JUB_disconnectDevice(static_cast<JUB_UINT16>(deviceHandle));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_disconnectDevice rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL native_isConnectDevice(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_RV rv = JUB_isDeviceConnect(static_cast<JUB_UINT16>(deviceHandle));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_isDeviceConnect rv: %08x", rv);
    }
    return rv;
}


// **************************************** 升级 ********************************************


JNIEXPORT jint JNICALL native_rebootToBootLoader(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_RV rv = JUB_RebootToBootLoader(static_cast<JUB_UINT16>(deviceHandle));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_RebootToBootLoader rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL native_rebootToNormal(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_RV rv = JUB_RebootToNormal(static_cast<JUB_UINT16>(deviceHandle));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_RebootToNormal rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL
native_updateFirmware(JNIEnv *env, jclass obj, jlong deviceHandle, jbyteArray firmware, jboolean rebootOnSuccess) {
    JUB_BYTE_PTR firmwareData = (JUB_BYTE_PTR) env->GetByteArrayElements(firmware, NULL);
    JUB_UINT32 firmwareSize = env->GetArrayLength(firmware);

    JUB_RV rv = JUB_UpdateFirmware(static_cast<JUB_UINT16>(deviceHandle),
                                   firmwareData,
                                   firmwareSize,
                                   rebootOnSuccess);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_UpdateFirmware rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL
native_updateResource(JNIEnv *env, jclass obj, jlong deviceHandle, jbyteArray resource, jboolean rebootOnSuccess) {
    JUB_BYTE_PTR resourceData = (JUB_BYTE_PTR) env->GetByteArrayElements(resource, NULL);
    JUB_UINT32 resourceSize = env->GetArrayLength(resource);

    JUB_RV rv = JUB_UpdateResource(static_cast<JUB_UINT16>(deviceHandle),
                                   resourceData,
                                   resourceSize,
                                   rebootOnSuccess);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_UpdateResource rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL
native_usePassphrase(JNIEnv *env, jclass obj, jlong deviceHandle, jboolean usePassphrase) {

    JUB_RV rv = JUB_UsePassphrase(static_cast<JUB_UINT16>(deviceHandle), usePassphrase);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_UsePassphrase rv: %08x", rv);
    }
    return rv;
}



// ******************************************************************************************



// 定义 JNINativeMethod 数组
static JNINativeMethod bleMethods[] = {
        {
                "nativeInitDevice",
                "()I",
                (void *) native_initDevice
        },
        {
                "nativeStartScan",
                "(Lcom/legendwd/hyperpay/onchain/hardware/jubnative/InnerScanCallback;)I",
                //"(Lcom/jubiter/sdk/demo/jubnative/InnerScanCallback;)I",
                (void *) native_startScan
        },
        {
                "nativeStopScan",
                "()I",
                (void *) native_stopScan
        },
        {
                "nativeConnectDevice",
                "(Ljava/lang/String;Ljava/lang/String;I[JILcom/legendwd/hyperpay/onchain/hardware/jubnative/InnerDiscCallback;)I",
                (void *) native_connectDevice
        },
        {
                "nativeDisconnect",
                "(J)I",
                (void *) native_disconnectDevice
        },
        {
                "nativeIsConnected",
                "(J)I",
                (void *) native_isConnectDevice
        },
        {
                "nativeRebootToBootloader",
                "(J)I",
                (void *) native_rebootToBootLoader
        },
        {
                "nativeRebootToNormal",
                "(J)I",
                (void *) native_rebootToNormal
        },
        {
                "nativeUpdateFirmware",
                "(J[BZ)I",
                (void *) native_updateFirmware
        },
        {
                "nativeUpdateResource",
                "(J[BZ)I",
                (void *) native_updateResource
        },
        {
                "nativeUsePassphrase",
                "(JZ)I",
                (void *) native_usePassphrase
        },
};


#define MAX_BLE_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterBluetoothNatives(JNIEnv *env) {
    LOG_INF("Register BLE");
    REGISTER_NATIVES(env, MAX_BLE_CLASS, bleMethods, sizeof(bleMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegBluetooth(RegisterBluetoothNatives);


#ifdef __cplusplus
}
#endif