
#ifndef JUBSDKANDROID_JNI_REGISTRY_H
#define JUBSDKANDROID_JNI_REGISTRY_H


#include <vector>
#include <jni.h>
#include "log_utils.h"

using ModuleRegisterFn = jint (*)(JNIEnv*);

/**
 * 取得全局注册函数表（懒汉单例，静态局部对象）
 */
inline std::vector<ModuleRegisterFn>& GetJniModuleRegistry() {
    static std::vector<ModuleRegisterFn> registry;
    return registry;
}

/**
 * 任何模块声明一个‘静态对象’把自己的注册函数塞进全局表
 */
class JniModuleRegistrar {
public:
    explicit JniModuleRegistrar(ModuleRegisterFn fn) {
        GetJniModuleRegistry().push_back(fn);
    }
};


//-------------------------------------------------------------------------------------------------


class LocalJClassRef {
public:
    LocalJClassRef(JNIEnv* env, jclass cls) : env(env), cls(cls) {}
    ~LocalJClassRef() {
        if (cls) {
            env->DeleteLocalRef(cls);
        }
    }
    operator jclass() const { return cls; }
private:
    JNIEnv* env;
    jclass cls;
};


#define REGISTER_NATIVES(ENV, CLASS_PATH, METHODS, METHOD_COUNT)          \
    do {                                                                  \
        jclass _c = (ENV)->FindClass(CLASS_PATH);                         \
        LocalJClassRef localCls(ENV, _c);                                 \
        if ((ENV)->ExceptionCheck()) {                                    \
            (ENV)->ExceptionDescribe();                                   \
            (ENV)->ExceptionClear();                                      \
            LOG_ERR("FindClass %s fail", CLASS_PATH);                     \
            return JNI_ERR;                                               \
        }                                                                 \
        if (!localCls) {                                                  \
            LOG_ERR("FindClass %s fail", CLASS_PATH);                     \
            return JNI_ERR;                                               \
        }                                                                 \
        if ((ENV)->RegisterNatives(localCls, METHODS, METHOD_COUNT) < 0) {\
            if ((ENV)->ExceptionCheck()) {                                \
                (ENV)->ExceptionDescribe();                               \
                (ENV)->ExceptionClear();                                  \
            }                                                             \
            LOG_ERR("RegisterNatives %s fail", CLASS_PATH);               \
            return JNI_ERR;                                               \
        }                                                                 \
    } while (0)



#endif //JUBSDKANDROID_JNI_REGISTRY_H
