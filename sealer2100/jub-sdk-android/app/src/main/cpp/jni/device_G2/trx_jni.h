//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_TRX_JNI_H
#define JUBSDKANDROID_TRX_JNI_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
native_TRXCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo);

JNIEXPORT jobjectArray JNICALL
native_TRXGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_TRXGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex);

JNIEXPORT jstring JNICALL
native_TRXShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_TRXSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_TRXTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON,
                      jbyteArray jPackedContractInPb);

JNIEXPORT jint JNICALL
native_TRXSetTRC20Token(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_TRXBuildTRC20TransferAbi(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jint JNICALL
native_TRXSetTRC10Asset(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jbyteArray JNICALL
native_TRXPackContract(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);


#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_TRX_JNI_H
