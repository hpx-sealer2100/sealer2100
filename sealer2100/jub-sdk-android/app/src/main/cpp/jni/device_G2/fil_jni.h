//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_HC_JNI_H
#define JUBSDKANDROID_HC_JNI_H


#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
native_FILCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
                        jstring jJSON, jlong deviceInfo);

JNIEXPORT jobjectArray JNICALL
native_FILGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_FILGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex);

JNIEXPORT jstring JNICALL
native_FILShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_FILSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_FILTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);




#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_HC_JNI_H
