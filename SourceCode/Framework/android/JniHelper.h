#ifndef __ANDROID_JNI_HELPER_H__INCLUDE
#define __ANDROID_JNI_HELPER_H__INCLUDE

#include <jni.h>
#include <string>

#define SDK_ACTIVITY_CLASS_NAME "com/beyondtech/starraiders/SdkActivity"
#define ACTIVITY_CLASS_NAME "com/beyondtech/starraiders/StarRaidersActivity"
#define BEYONDENGINE_HELPER_CLASS_NAME "com/beyondtech/starraiders/BeyondEngineHelper"

typedef struct JniMethodInfo_
{
    JNIEnv *    env = nullptr;
    jclass      classID = nullptr;
    jmethodID   methodID = 0;
    void CleanUp()
    {
        if (classID && env)
        {
            env->DeleteLocalRef(classID);
        }
        env = nullptr;
        classID = nullptr;
        methodID = 0;
    }
    ~JniMethodInfo_()
    {
        CleanUp();
    }
} JniMethodInfo;

class JniHelper
{
public:
    static void setJavaVM(JavaVM *javaVM);
    static JavaVM* getJavaVM();
    static JNIEnv* getEnv();

    static bool setClassLoaderFrom(jobject activityInstance);
    static bool getStaticMethodInfo(JniMethodInfo &methodinfo,
                                    const char *className,
                                    const char *methodName,
                                    const char *paramCode);
    static bool getMethodInfo(JniMethodInfo &methodinfo,
                              const char *className,
                              const char *methodName,
                              const char *paramCode);

    static std::string jstring2string(jstring str);

    static jmethodID loadclassMethod_methodID;
    static jobject classloader;
private:
    static JNIEnv* cacheEnv(JavaVM* jvm);

    static bool getMethodInfo_DefaultClassLoader(JniMethodInfo &methodinfo,
                                                 const char *className,
                                                 const char *methodName,
                                                 const char *paramCode);

    static JavaVM* _psJavaVM;
};

#endif // __ANDROID_JNI_HELPER_H__
