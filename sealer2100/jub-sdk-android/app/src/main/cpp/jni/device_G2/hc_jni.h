//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_HC_JNI_H
#define JUBSDKANDROID_HC_JNI_H


#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
native_HCCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jboolean isMultiSig,
                       jstring jJSON, jlong deviceInfo);

JNIEXPORT jobjectArray JNICALL
native_HCGetAddress(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_HCShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL native_HCGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID);

JNIEXPORT jstring JNICALL
native_HCTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);


#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_HC_JNI_H
