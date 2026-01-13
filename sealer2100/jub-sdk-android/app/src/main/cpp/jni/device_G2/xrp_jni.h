//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_XRP_JNI_H
#define JUBSDKANDROID_XRP_JNI_H


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
native_XRPCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo);

JNIEXPORT jobjectArray JNICALL
native_XRPGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_XRPGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex);

JNIEXPORT jstring JNICALL
native_XRPShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_XRPSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_XRPTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);


#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_XRP_JNI_H
