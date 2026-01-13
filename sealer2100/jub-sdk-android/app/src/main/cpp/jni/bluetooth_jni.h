
#ifndef JUBSDKANDROID_BLUETOOTH_JNI_H
#define JUBSDKANDROID_BLUETOOTH_JNI_H


#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL native_initDevice(JNIEnv *env, jclass obj);
JNIEXPORT jint JNICALL native_startScan(JNIEnv *env, jclass obj, jobject scanCallback);
JNIEXPORT jint JNICALL native_stopScan(JNIEnv *env, jclass obj);
JNIEXPORT jint JNICALL native_connectDevice(JNIEnv *env, jclass obj, jstring devName,
                                            jstring address, jint devType, jlongArray handle,
                                            jint timeout, jobject disCallback);
JNIEXPORT jint JNICALL native_cancelConnect(JNIEnv *env, jobject obj, jstring devName,
                                            jstring address);
JNIEXPORT jint JNICALL native_disconnectDevice(JNIEnv *env, jclass obj, jlong deviceHandle);
JNIEXPORT jint JNICALL native_isConnectDevice(JNIEnv *env, jclass obj, jlong deviceHandle);

#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_BLUETOOTH_JNI_H
