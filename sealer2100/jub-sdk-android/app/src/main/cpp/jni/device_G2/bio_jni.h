//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_BIO_JNI_H
#define JUBSDKANDROID_BIO_JNI_H


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
native_identityVerifyPIN(JNIEnv *env, jclass obj, jlong deviceID, jint verifyMode, jstring jPin,
                         jlongArray retryTimes);

JNIEXPORT jint JNICALL native_identityShowNineGrids(JNIEnv *env, jclass obj, jlong deviceID);

JNIEXPORT jint JNICALL native_identityCancelNineGrids(JNIEnv *env, jclass obj, jlong deviceID);

JNIEXPORT jstring JNICALL
native_enrollFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut,
                         jbyteArray fgptIndex);

JNIEXPORT jstring JNICALL native_enumFingerprint(JNIEnv *env, jclass obj, jlong deviceID);

JNIEXPORT jint JNICALL
native_eraseFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut);

JNIEXPORT jint JNICALL
native_deleteFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut, jbyte fgptID);

JNIEXPORT jint JNICALL
native_verifyFingerprint(JNIEnv *env, jclass obj, jlong contextID, jlongArray retryTimes);


#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_BIO_JNI_H
