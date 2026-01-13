//
// Created by feng on 2025/8/10.
//

#ifndef JUBSDKANDROID_SOL_JNI_H
#define JUBSDKANDROID_SOL_JNI_H


#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
native_SOLCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo);

JNIEXPORT jobjectArray JNICALL
native_SOLGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jobjectArray JNICALL
native_SOLShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_SOLGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex);

JNIEXPORT jstring JNICALL native_SOLGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID);

JNIEXPORT jstring JNICALL native_SOLTransaction(JNIEnv *env, jclass obj, jlong contextID,
                                                jstring jJSON);

#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_SOL_JNI_H
