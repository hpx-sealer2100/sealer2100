//
// Created by FT on 2018/4/16.
//

#include <jni.h>
#include <log_utils.h>
#include <vector>
#include <main_jni.h>

#include "common_jni.h"
#include "jni_registry.h"


// 保存 JavaVM
JavaVM *g_vm = NULL;
int errorCode = 0;

// 是否是多重签名
bool globalMultiSig = false;


/**
 * 自动注册所有 JNI 模块
 * @param env
 * @return
 */
static jint RegisterAllModules(JNIEnv* env) {
    for (auto fn : GetJniModuleRegistry()) {
        if (fn(env) != JNI_OK) {
            LOG_ERR("[JNI] Register module failed");
            return JNI_ERR;
        }
    }
    return JNI_OK;
}


/**
 * JNI_OnLoad 默认会在 System.loadLibrary 过程中自动调用到，因而可利用此函数，进行动态注册
 * JNI 版本的返回视对应 JDK 版本而定
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint ret = JNI_FALSE;

    // 获取 env 指针
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOG_ERR(">>>> get env fail");
        return ret;
    }

    // 保存全局 JVM 以便在动态注册的皆空中使用 env 环境
    ret = env->GetJavaVM(&g_vm);
    if (ret != JNI_OK) {
        LOG_ERR(">>>> GetJavaVM fail");
        return ret;
    }

    LOG_ERR(">>>> JNI_OnLoad Success");

    // 注册所有模块
    return RegisterAllModules(env) == JNI_OK ? JNI_VERSION_1_6 : JNI_ERR;
}

/**
 * 动态注销
 * @param vm
 */
void JNI_OnUnload(JavaVM*, void*) {
    g_vm = nullptr;
    // 释放全局引用

}