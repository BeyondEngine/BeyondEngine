#include "stdafx.h"
#include "JniHelper.h"
#include <string.h>
#include <pthread.h>

static pthread_key_t g_key;
JavaVM* JniHelper::_psJavaVM = nullptr;
jmethodID JniHelper::loadclassMethod_methodID = nullptr;
jobject JniHelper::classloader = nullptr;

extern "C"
{
    jclass _getClassID(const char *className)
    {
        if (nullptr == className)
        {
            return nullptr;
        }

        JNIEnv* env = JniHelper::getEnv();

        jstring _jstrClassName = env->NewStringUTF(className);
        BEATS_ASSERT(JniHelper::classloader != nullptr, _T("class loader can't be empty!"));
        BEATS_ASSERT(JniHelper::loadclassMethod_methodID != nullptr, _T("loadclassMethod_methodID can't be empty!"));
        jclass _clazz = (jclass) env->CallObjectMethod(JniHelper::classloader,
            JniHelper::loadclassMethod_methodID,
            _jstrClassName);

        if (nullptr == _clazz)
        {
            BEATS_ASSERT(false, "Classloader failed to find class of %s", className);
            env->ExceptionClear();
        }
        env->DeleteLocalRef(_jstrClassName);

        return _clazz;
    }
};
JavaVM* JniHelper::getJavaVM()
{
    pthread_t thisthread = pthread_self();
    BEATS_PRINT("JniHelper::getJavaVM(), pthread_self() = %ld", thisthread);
    return _psJavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM)
{
    BEATS_ASSERT(javaVM != NULL, _T("Can't set java vm to null") );
    pthread_t thisthread = pthread_self();
    BEATS_PRINT("JniHelper::setJavaVM(%p), pthread_self() = %ld", javaVM, thisthread);
    _psJavaVM = javaVM;

    pthread_key_create(&g_key, nullptr);
}

JNIEnv* JniHelper::cacheEnv(JavaVM* jvm)
{
    JNIEnv* _env = nullptr;
    // get jni environment
    jint ret = jvm->GetEnv((void**)&_env, JNI_VERSION_1_4);

    switch (ret) 
    {
    case JNI_OK :
        // Success!
        pthread_setspecific(g_key, _env);
        return _env;

    case JNI_EDETACHED :
        // Thread not attached

        // TODO : If calling AttachCurrentThread() on a native thread
        // must call DetachCurrentThread() in future.
        // see: http://developer.android.com/guide/practices/design/jni.html

        if (jvm->AttachCurrentThread(&_env, nullptr) < 0)
        {
            BEATS_ASSERT(false, "Failed to get the environment using AttachCurrentThread()");

            return nullptr;
        }
        else
        {
            // Success : Attached and obtained JNIEnv!
            pthread_setspecific(g_key, _env);
            return _env;
        }

    case JNI_EVERSION :
        // Cannot recover from this error
        BEATS_ASSERT(false, "JNI interface version 1.4 not supported");
    default :
        BEATS_ASSERT(false, "Failed to get the environment using GetEnv()");
        return nullptr;
    }
}

JNIEnv* JniHelper::getEnv()
{
    JNIEnv *_env = (JNIEnv *)pthread_getspecific(g_key);
    if (_env == nullptr)
    {
        BEATS_ASSERT(_psJavaVM != NULL, _T("_psJavaVM can't be NULL"));
        _env = JniHelper::cacheEnv(_psJavaVM);
    }
    BEATS_ASSERT(_env != nullptr, _T("env can't be null!"));
    return _env;
}

bool JniHelper::setClassLoaderFrom(jobject activityinstance)
{
    JniMethodInfo _getclassloaderMethod;
    if (!JniHelper::getMethodInfo_DefaultClassLoader(_getclassloaderMethod,
        "android/content/Context",
        "getClassLoader",
        "()Ljava/lang/ClassLoader;"))
    {
            return false;
    }

    JNIEnv* pEnv = JniHelper::getEnv();
    BEATS_ASSERT(pEnv != NULL, _T("Env can't be null!"));
    jobject _c = pEnv->CallObjectMethod(activityinstance, _getclassloaderMethod.methodID);

    if (nullptr == _c) {
        return false;
    }

    JniMethodInfo _m;
    if (!JniHelper::getMethodInfo_DefaultClassLoader(_m,
        "java/lang/ClassLoader",
        "loadClass",
        "(Ljava/lang/String;)Ljava/lang/Class;"))
    {
            return false;
    }

    JniHelper::classloader = JniHelper::getEnv()->NewGlobalRef(_c);
    JniHelper::loadclassMethod_methodID = _m.methodID;

    return true;
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo &methodinfo,
                                    const char *className, 
                                    const char *methodName,
                                    const char *paramCode) 
{
    BEATS_ASSERT(methodinfo.env == nullptr && methodinfo.classID == nullptr && methodinfo.methodID == 0);
    if ((nullptr == className) ||
        (nullptr == methodName) ||
        (nullptr == paramCode)) 
    {
        return false;
    }

    JNIEnv *env = JniHelper::getEnv();
    if (!env)
    {
        BEATS_ASSERT(false, "Failed to get JNIEnv");
        return false;
    }

    jclass classID = _getClassID(className);
    if (! classID)
    {
        BEATS_ASSERT(false, "Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }
    BEATS_ASSERT(methodName != NULL, _T("methodName can't be null!"));
    BEATS_ASSERT(paramCode != NULL, _T("paramCode can't be null!"));
    jmethodID methodID = env->GetStaticMethodID(classID, methodName, paramCode);
    if (! methodID)
    {
        BEATS_ASSERT(false, "Failed to find static method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = env;
    methodinfo.methodID = methodID;
    return true;
}

bool JniHelper::getMethodInfo_DefaultClassLoader(JniMethodInfo &methodinfo,
                                                 const char *className,
                                                 const char *methodName,
                                                 const char *paramCode)
{
    if ((nullptr == className) ||
        (nullptr == methodName) ||
        (nullptr == paramCode))
    {
        return false;
    }

    JNIEnv *env = JniHelper::getEnv();
    if (!env)
    {
        return false;
    }

    jclass classID = env->FindClass(className);
    if (! classID)
    {
        BEATS_ASSERT(false, "Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetMethodID(classID, methodName, paramCode);
    if (! methodID)
    {
        BEATS_ASSERT(false, "Failed to find method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = env;
    methodinfo.methodID = methodID;

    return true;
}

bool JniHelper::getMethodInfo(JniMethodInfo &methodinfo,
                              const char *className,
                              const char *methodName,
                              const char *paramCode)
{
    if ((nullptr == className) ||
        (nullptr == methodName) ||
        (nullptr == paramCode))
    {
        return false;
    }

    JNIEnv *env = JniHelper::getEnv();
    if (!env)
    {
        return false;
    }

    jclass classID = _getClassID(className);
    if (! classID)
    {
        BEATS_ASSERT(false, "Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetMethodID(classID, methodName, paramCode);
    if (! methodID)
    {
        BEATS_ASSERT(false, "Failed to find method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = env;
    methodinfo.methodID = methodID;

    return true;
}

std::string JniHelper::jstring2string(jstring jstr)
{
    std::string strRet;
    if (jstr != nullptr)
    {
        JNIEnv *env = JniHelper::getEnv();
        BEATS_ASSERT(env != NULL, _T("env is null!"));
        const char* chars = env->GetStringUTFChars(jstr, nullptr);
        BEATS_ASSERT(chars != NULL, _T("GetStringUTFChars returns a null ptr."));
        strRet.assign(chars);
        env->ReleaseStringUTFChars(jstr, chars);
    }
    return strRet;
}
