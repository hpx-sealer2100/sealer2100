
#ifndef JUBSDKANDROID_COMMON_MAX_JNI_H
#define JUBSDKANDROID_COMMON_MAX_JNI_H


#include <jni.h>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "trezorProtocol/ProtocolDecoder.h"
#include "JUB_SDK_COMM.h"
#include "device/JubiterBLENusDevice.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern jclass g_clazz;
extern jmethodID g_recv_method;

// 线程同步
extern std::mutex g_mutex;
extern std::condition_variable g_cv;
extern bool g_allDataReceived;

struct PassphraseData {
    std::string passphrase;
    bool onDevice;
};

extern PassphraseData sPassphraseData;

//extern std::mutex g_passphrase_mutex;
//extern bool g_passphrase_ready;




JNIEXPORT void JNICALL
sendDataToNativeMax(JNIEnv *env, jclass clazz, jbyteArray data);

JNIEXPORT void JNICALL
sendPassphraseToNativeMax(JNIEnv *env, jclass clazz, jstring passphrase, jboolean onDevice);


#ifdef __cplusplus
}
#endif



void initMax(JNIEnv *env, jclass clazz);

void sendDataToJava(JNIEnv *env, std::vector<unsigned char> data);

void notifyJavaWithMessage(JNIEnv *env, int messageType);

void setPassphraseToJava(JNIEnv *env, JUB_BYTE_PTR pData, JUB_ULONG_PTR pDataLen, bool *on_device);

MessageResponse waitForResponse();

void waitForPassphraseUpdate();

void notifyUpdatePercentage(JNIEnv *env, JUB_UINT16 percentage);


JUB_RV sendDataToJavaWrapper(JUB_CHAR_PTR pData, JUB_UINT16 dataLen);

JUB_RV readDataFromJavaWrapper(JUB_CHAR_PTR_PTR pData, JUB_UINT16_PTR pDataLen);

void notifyJavaWithMessageWrapper(JUB_UINT16 messageType);

void setPassphraseWrapper(JUB_BYTE_PTR pData, JUB_ULONG_PTR pDataLen, bool *on_device);

void notifyUpdatePercentageWrapper(JUB_UINT16 percentage);

#endif //JUBSDKANDROID_COMMON_MAX_JNI_H
