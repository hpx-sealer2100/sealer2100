
#ifndef JUBSDKANDROID_COMMON_JNI_H
#define JUBSDKANDROID_COMMON_JNI_H


#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

// 保存 JavaVM
extern JavaVM *g_vm;
extern int errorCode;

// 是否是多重签名
extern bool globalMultiSig;


#ifdef __cplusplus
}
#endif

#endif //JUBSDKANDROID_COMMON_JNI_H
