
#ifndef JUBSDKANDROID_DEVICE_JNI_H
#define JUBSDKANDROID_DEVICE_JNI_H


#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL native_getDeviceType(JNIEnv *env, jclass obj, jlong deviceID);
JNIEXPORT jint JNICALL native_show(JNIEnv *env, jclass obj, jlong contextID);
JNIEXPORT jint JNICALL native_CancelVirtualPwd(JNIEnv *env, jclass obj, jlong contextID);
JNIEXPORT jint JNICALL
native_verifyPIN(JNIEnv *env, jclass obj, jlong contextID, jbyteArray jPin);
JNIEXPORT jint JNICALL native_GetDeviceInfo(JNIEnv *env, jclass obj,
                                            jobject deviceInfo, jlong deviceHandle);
JNIEXPORT jstring JNICALL native_sendAPDU(JNIEnv *env, jclass obj, jlong deviceID,
                                          jstring jApdu);
JNIEXPORT jstring JNICALL native_GetDeviceCert(JNIEnv *env, jclass obj, jlong deviceHandle);
JNIEXPORT jstring JNICALL native_GetDeviceSignData(JNIEnv *env, jclass obj, jlong deviceHandle,jstring jhashData);
JNIEXPORT jstring JNICALL
native_GetAppletVersion(JNIEnv *env, jclass obj, jlong deviceHandle, jstring appID);
JNIEXPORT jstring JNICALL native_EnumApplets(JNIEnv *env, jclass obj, jlong deviceHandle);
JNIEXPORT jint JNICALL native_SetTimeOut(JNIEnv *env, jclass obj, jlong contextID, jint jTimeOut);
JNIEXPORT jint JNICALL native_SetLable(JNIEnv *env, jclass obj, jlong contextID, jstring jLable);
JNIEXPORT jstring JNICALL native_GetFeatures(JNIEnv *env, jclass obj, jlong contextID);
JNIEXPORT jint JNICALL native_EndSession(JNIEnv *env, jclass obj, jlong contextID);
JNIEXPORT jint JNICALL native_OpCancel(JNIEnv *env, jclass obj, jlong contextID);


#ifdef __cplusplus
}
#endif

#endif //JUBSDKANDROID_DEVICE_JNI_H
