
#include "common_max_jni.h"
#include "trezorProtocol/ProtocolDecoder.h"
#include "trezorProtocol/ProtocolEncoder.h"
#include "common_jni.h"
#include "JUB_SDK_COMM.h"
#include "JUB_SDK_DEV_BLE_NUS.h"
#include "log_utils.h"
#include "jni_registry.h"


#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------------

jclass g_clazz = nullptr;
jmethodID g_recv_method = nullptr;
jmethodID g_notify_method = nullptr;
jmethodID g_passphrase_method = nullptr;
jmethodID g_percentage_method = nullptr;

// 线程同步
std::mutex g_mutex;
std::condition_variable g_cv;
std::condition_variable g_passphrase_cv;
bool g_allDataReceived = false;

PassphraseData sPassphraseData;
//bool g_passphrase_ready = false;
//std::mutex g_passphrase_mutex;

//--------------------------------------------------------------------------------------



#define MAX_NATIVE_API "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiMax"

#define RECV_METHOD "onNativeDataReceived"
#define NOTIFY_MESSAGE_METHOD "onNotifyMessage"
#define SET_PASSPHRASE_METHOD "onSetPassphrase"
#define NOTIFY_PERCENTAGE_METHOD "onUpdatePercentage"


/**
* 接收 java 透传数据
*/
JNIEXPORT void JNICALL
sendDataToNativeMax(JNIEnv *env, jclass clazz, jbyteArray data) {
    jbyte *bytes = env->GetByteArrayElements(data, nullptr);
    jint length = env->GetArrayLength(data);
    LOG_INF("[C++] Received size: %d", length);
//    LOG_BIN("[C++] Received", reinterpret_cast<const uint8_t *>(bytes), length);

    g_allDataReceived = ProtocolDecoder::packetCompletionCheck(
            reinterpret_cast<const uint8_t *>(bytes),
            length);
    if (!g_allDataReceived) {
        return;
    }
    LOG_INF("[C++] 接收完成");
    // 唤醒等待线程
    g_cv.notify_one();

    env->ReleaseByteArrayElements(data, bytes, 0);
}

/**
* 接收 java 透传数据
*/
JNIEXPORT void JNICALL
sendPassphraseToNative(JNIEnv *env, jclass clazz, jstring passphrase, jboolean onDevice) {
    LOG_INF("[C++] sendPassphraseToNative");

    JUB_BYTE_PTR pPassphrase = (JUB_BYTE_PTR) (env->GetStringUTFChars(passphrase, NULL));
    JUB_ULONG length = env->GetStringUTFLength(passphrase);

    std::string newPassphrase(reinterpret_cast<const char *>(pPassphrase), length);

    {
        // 使用 std::lock_guard 确保对 sPassphraseData 和 g_passphrase_ready 的访问是线程安全的
//        std::lock_guard<std::mutex> lock(g_passphrase_mutex);
        // 给 sPassphraseData 赋值
        sPassphraseData.passphrase = newPassphrase;
        sPassphraseData.onDevice = onDevice;

//        g_passphrase_ready = true;
    }
    LOG_INF("[C++] passphrase 接收完成");
    // 唤醒等待线程
//    g_passphrase_cv.notify_one();
    LOG_INF("[C++] passphrase notify_one");

    env->ReleaseStringUTFChars(passphrase, reinterpret_cast<const char *>(pPassphrase));
}

//------------------------------------------- 自动注册 ----------------------------------------------

#define MAX_COMM_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

// 定义 JNINativeMethod 数组
static JNINativeMethod maxCommMethods[] = {
        {
                "sendDataToNative",
                "([B)V",
                (void *) sendDataToNativeMax
        },
        {
                "nativeSendPassphraseToNative",
                "(Ljava/lang/String;Z)V",
                (void *) sendPassphraseToNative
        },
};


jint RegisterMaxCommNatives(JNIEnv *env) {
    LOG_INF("Register Max COMM");
    REGISTER_NATIVES(env, MAX_COMM_CLASS, maxCommMethods,
                     sizeof(maxCommMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegMaxComm(RegisterMaxCommNatives);


#ifdef __cplusplus
}
#endif

//-----------------------------------------------------------------------------------------

void initMax(JNIEnv *env, jclass clazz) {

    env->GetJavaVM(&g_vm);

    jclass tmp_clazz = (jclass) env->FindClass(MAX_NATIVE_API);
    if (nullptr == tmp_clazz) {
        LOG_ERR("Class not found");
        return;
    }
    // 创建全局引用，防止被垃圾回收器回收
    g_clazz = static_cast<jclass>(env->NewGlobalRef(tmp_clazz));

    g_recv_method = env->GetStaticMethodID(g_clazz, RECV_METHOD, "([B)V");
    if (nullptr == g_recv_method) {
        LOG_ERR("recv method not found");
        return;
    }

    g_notify_method = env->GetStaticMethodID(g_clazz, NOTIFY_MESSAGE_METHOD, "(I)V");
    if (nullptr == g_notify_method) {
        LOG_ERR("notify message method not found");
        return;
    }

    g_passphrase_method = env->GetStaticMethodID(g_clazz, SET_PASSPHRASE_METHOD, "(Ljava/lang/String;Z)V");
    if (nullptr == g_passphrase_method) {
        LOG_ERR("set passphrase method not found");
        return;
    }

    g_percentage_method = env->GetStaticMethodID(g_clazz, NOTIFY_PERCENTAGE_METHOD, "(I)V");
    if (nullptr == g_percentage_method) {
        LOG_ERR("notify percentage method not found");
        return;
    }

}

JUB_RV sendDataToJavaWrapper(JUB_CHAR_PTR pData, JUB_UINT16 dataLen) {
    if (nullptr == pData || 0 == dataLen) {
        return JUBR_ARGUMENTS_BAD;
    }
    JNIEnv *env = nullptr;
    if (g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOG_ERR("Failed to attach current thread");
            return JUBR_ERROR;
        }
    }
    std::vector<unsigned char> data(pData, pData + dataLen);
    sendDataToJava(env, data);
    return JUBR_OK;
}

/**
 * 指令转发 java 层
 */
void sendDataToJava(JNIEnv *env, std::vector<unsigned char> data) {
    if (nullptr == env) {
        g_vm->AttachCurrentThread(&env, nullptr);
        LOG_ERR("env is nullptr, attached now");
    }

    // 创建 jbyteArray，长度为 data 的长度
    jbyteArray byteArray = env->NewByteArray(data.size());
    if (byteArray == nullptr) {
        LOG_ERR("Failed to create jbyteArray");
        return;
    }

    env->SetByteArrayRegion(byteArray,
                            0,
                            data.size(),
                            reinterpret_cast<const jbyte *>(data.data()));

    LOG_INF("Send to Java, size: %zu", data.size());

    // 调用 Java 静态方法
    env->CallStaticVoidMethod(g_clazz, g_recv_method, byteArray);

    // 检查 Java 调用是否有异常
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOG_ERR("Exception occurred when calling Java method");
    }

    // 删除局部引用，避免内存泄漏
    env->DeleteLocalRef(byteArray);
}

typedef JUB_RV (*JUB_BleNusRead_PTR)(
        JUB_CHAR_PTR_PTR pData,
        JUB_UINT16_PTR pDataLen
);

JUB_RV readDataFromJavaWrapper(JUB_CHAR_PTR_PTR pData, JUB_UINT16_PTR pDataLen) {
    if (nullptr == pData || nullptr == pDataLen) {
        return JUBR_ARGUMENTS_BAD;
    }
    MessageResponse response = waitForResponse();
    // 存在 data 为空的正常响应数据
//    if (response.data.empty()) {
//        return JUBR_ERROR;
//    }
    *pDataLen = static_cast<JUB_UINT16>(response.data.size() + 2); // 多2字节存type
    *pData = new char[*pDataLen];
    if (nullptr == *pData) {
        return JUBR_ERROR;
    }
    uint16_t type = static_cast<uint16_t>(response.messageType);
    (*pData)[0] = static_cast<char>((type >> 8) & 0xFF);
    (*pData)[1] = static_cast<char>(type & 0xFF);
    // 后面存data
    memcpy(*pData + 2, response.data.data(), response.data.size());
    return JUBR_OK;
}

MessageResponse waitForResponse() {
    std::unique_lock<std::mutex> lock(g_mutex);
    // 重置标志位
    g_allDataReceived = false;
    g_cv.wait(lock, [] { return g_allDataReceived; });
    return ProtocolDecoder::decode();
}

void notifyJavaWithMessageWrapper(JUB_UINT16 messageType) {
    JNIEnv *env = nullptr;
    if (g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOG_ERR("Failed to attach current thread");
            return;
        }
    }
    notifyJavaWithMessage(env, messageType);
}


//void waitForPassphraseUpdate() {
//    std::unique_lock<std::mutex> lock(g_passphrase_mutex);
//    g_passphrase_ready = false;
//    LOG_INF("[C++] waitForPassphraseUpdate");
//    g_passphrase_cv.wait(lock, [] { return g_passphrase_ready;});
//    LOG_INF("[C++] Passphrase wait 已解锁");
//}

void notifyJavaWithMessage(JNIEnv *env, int messageType) {
    if (nullptr == env) {
        g_vm->AttachCurrentThread(&env, nullptr);
        LOG_ERR("env is nullptr, attached now");
    }

    // 调用 Java 静态方法
    env->CallStaticVoidMethod(g_clazz, g_notify_method, messageType);

    // 检查 Java 调用是否有异常
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOG_ERR("Exception occurred when calling Java method");
    }
}

void setPassphraseWrapper(JUB_BYTE_PTR pData, JUB_ULONG_PTR pDataLen, bool *on_device) {
    LOG_INF("[C++] setPassphraseWrapper called");

    JNIEnv *env = nullptr;
    if (g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOG_ERR("Failed to attach current thread");
            return;
        }
    }
    setPassphraseToJava(env, pData, pDataLen, on_device);
//    waitForPassphraseUpdate();

    LOG_ERR("waitForPassphraseUpdate 已唤醒");

    // 将 sPassphraseData 中的数据赋值给 pData 和 pDataLen
    if (*pDataLen < sPassphraseData.passphrase.size()) {
        LOG_ERR("Buffer is too small to hold the passphrase");
        return;
    }
    std::memcpy(pData, sPassphraseData.passphrase.data(), sPassphraseData.passphrase.size());
    *pDataLen = sPassphraseData.passphrase.size();
    *on_device = sPassphraseData.onDevice;
}

/**
 * 转发数据到 java
 *
 * @param env
 * @param pData
 * @param pDataLen
 * @param on_device
 */
void setPassphraseToJava(JNIEnv *env, JUB_BYTE_PTR pData, JUB_ULONG_PTR pDataLen, bool *on_device) {
    if (nullptr == env) {
        g_vm->AttachCurrentThread(&env, nullptr);
        LOG_ERR("env is nullptr, attached now");
    }

    jstring dataStr = env->NewStringUTF(reinterpret_cast<const char *>(pData));

    // 调用 Java 静态方法
    env->CallStaticVoidMethod(g_clazz, g_passphrase_method, dataStr, *on_device);
    LOG_INF("[C++] setPassphraseToJava - CallStaticVoidMethod");

    // 检查 Java 调用是否有异常
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOG_ERR("Exception occurred when calling Java method");
    }

    // 删除局部引用，避免内存泄漏
    env->DeleteLocalRef(dataStr);
}

// ******************************* 升级进度 ************************************************

void notifyUpdatePercentageWrapper(JUB_UINT16 percentage) {
    JNIEnv *env = nullptr;
    if (g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOG_ERR("Failed to attach current thread");
            return;
        }
    }
    notifyUpdatePercentage(env, percentage);
}

void notifyUpdatePercentage(JNIEnv *env, JUB_UINT16 percentage) {
    if (nullptr == env) {
        g_vm->AttachCurrentThread(&env, nullptr);
        LOG_ERR("env is nullptr, attached now");
    }

    // 调用 Java 静态方法
    env->CallStaticVoidMethod(g_clazz, g_percentage_method, percentage);

    // 检查 Java 调用是否有异常
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOG_ERR("Exception occurred when calling Java method");
    }
}