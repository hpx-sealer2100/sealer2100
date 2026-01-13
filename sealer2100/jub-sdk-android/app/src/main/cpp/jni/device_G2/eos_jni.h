//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_EOS_JNI_H
#define JUBSDKANDROID_EOS_JNI_H


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
native_EOSCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo);

JNIEXPORT jstring JNICALL
native_EOSGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_EOSShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_EOSSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_EOSTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);


#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_EOS_JNI_H
