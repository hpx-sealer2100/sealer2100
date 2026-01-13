//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_BTC_JNI_H
#define JUBSDKANDROID_BTC_JNI_H


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
native_BTCCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jboolean isMultiSig,
                        jstring jJSON, jlong deviceInfo);

JNIEXPORT jstring JNICALL
native_USDTTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_QRC20Transaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_BTC_ShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                       jboolean useLegacyAddress);

JNIEXPORT jstring JNICALL
native_BTC_SetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                        jboolean useLegacyAddress);

JNIEXPORT jobjectArray JNICALL
native_BTCGetAddress(JNIEnv *env, jclass obj, jlong contextID, jboolean useLegacyAddress,
                     jstring jJSON);

JNIEXPORT jstring JNICALL native_BTCGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID);

JNIEXPORT jstring JNICALL
native_ParseTransactionRaw(JNIEnv *env, jclass obj, jlong contextID, jstring jRaw);

JNIEXPORT jstring JNICALL
native_BTCTransaction(JNIEnv *env, jclass obj, jlong contextID, jboolean useLegacyAddress,
                      jstring jJSON);


#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_BTC_JNI_H
