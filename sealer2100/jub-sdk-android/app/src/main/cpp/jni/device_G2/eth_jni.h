//
// Created by feng on 2025/6/4.
//

#ifndef JUBSDKANDROID_ETH_G2_JNI_H
#define JUBSDKANDROID_ETH_G2_JNI_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
native_ETHCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo);

JNIEXPORT jstring JNICALL
native_ETH_Transaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jint JNICALL
native_ETH_SetERC20Tokens(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jint JNICALL
native_ETH_SetERC20Token(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jint JNICALL
native_ETH_SetERC20TokenV2(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jint JNICALL
native_ETH_SetNetworkV2(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_ETH_BuildERC20TransferAbi(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_ETH_SignContract(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jint JNICALL
native_ETH_SetContrAddr(JNIEnv *env, jclass obj, jlong contextID, jstring jContrAddr);

JNIEXPORT jstring JNICALL
native_ETH_BuildContractWithAddrAmtDataAbi(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_ETH_BuildContractWithTxIDAbi(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jobjectArray JNICALL
native_ETHGetAddress(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON);

JNIEXPORT jstring JNICALL
native_SetMyAddressETH(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL
native_ETHShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint index);

JNIEXPORT jstring JNICALL
native_SetMyAddressETH(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index);

JNIEXPORT jstring JNICALL native_ETHGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID);

JNIEXPORT jint JNICALL
native_ETH_UploadNFT(JNIEnv *env, jclass obj, jlong contextID, jstring jMeta, jbyteArray image,
                     jbyteArray thumbnail, jbyteArray wallpaper);

#ifdef __cplusplus
}
#endif


#endif //JUBSDKANDROID_ETH_G2_JNI_H
