#include <cstring>
#include <unistd.h>
#include <sys/system_properties.h>
#include <xhook/xhook.h>
#include <jni.h>

#include "riru.h"
#include "logging.h"
#include "android.h"
#include "hook.h"
#include "config.h"

#define XHOOK_REGISTER(NAME) \
    if (xhook_register(".*", #NAME, (void*) new_##NAME, (void **) &old_##NAME) != 0) { \
        LOGE("failed to register hook " #NAME "."); \
    }

#define NEW_FUNC_DEF(ret, func, ...) \
    static ret (*old_##func)(__VA_ARGS__); \
    static ret new_##func(__VA_ARGS__)

NEW_FUNC_DEF(int, __system_property_get, const char *key, char *value) {
    int res = old___system_property_get(key, value);
    auto prop = Config::Properties::Find(key);
    if (prop) {
        LOGI("system_property_get: %s=%s -> %s", key, value, prop->value.c_str());
        strcpy(value, prop->value.c_str());
        res = (int) prop->value.length();
    }
    return res;
}

using callback_func = void(void *cookie, const char *name, const char *value, uint32_t serial);

thread_local callback_func *saved_callback = nullptr;

static void my_callback(void *cookie, const char *name, const char *value, uint32_t serial) {
    if (!saved_callback) return;

    auto prop = Config::Properties::Find(name);
    if (!prop) {
        saved_callback(cookie, name, value, serial);
        return;
    }

    LOGI("system_property_read_callback: %s=%s -> %s", name, value, prop->value.c_str());
    saved_callback(cookie, name, prop->value.c_str(), serial);
}

NEW_FUNC_DEF(void, __system_property_read_callback, const prop_info *pi, callback_func *callback, void *cookie) {
    saved_callback = callback;
    old___system_property_read_callback(pi, my_callback, cookie);
}

void inject_build(JNIEnv *env) {
    if (env == nullptr) {
        LOGW("failed to inject android.os.Build due to env is null");
        return;
    }

    jclass build_class = env->FindClass("android/os/Build");
    if (build_class == nullptr) {
        LOGW("failed to inject android.os.Build due to build is null");
        return;
    }

    for (auto it = Config::Builds::Begin(); it != Config::Builds::End(); ++it) {
        jfieldID id = env->GetStaticFieldID(build_class, it->first.c_str(), "Ljava/lang/String;");
        if (id != nullptr) {
            jstring new_value = env->NewStringUTF(it->second->value.c_str());
            env->SetStaticObjectField(build_class, id, new_value);

            if(env->ExceptionCheck())
            {
                env->ExceptionClear();
            }

            env->DeleteLocalRef(new_value);

            LOGI("Inject build: android.os.Build.%s -> %s", it->first.c_str(), it->second->value.c_str());
        }
    }
}


void Hook::install(JNIEnv *env) {
    inject_build(env);
    XHOOK_REGISTER(__system_property_get);

    if (android::GetApiLevel() >= 26) {
        XHOOK_REGISTER(__system_property_read_callback);
    }

    if (xhook_refresh(0) == 0)
        xhook_clear();
    else
        LOGE("failed to refresh hook");
}