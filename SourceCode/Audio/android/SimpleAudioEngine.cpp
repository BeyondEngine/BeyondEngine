#include "stdafx.h"
#include "Audio/SimpleAudioEngine.h"
#include "Framework/android/JniHelper.h"
#include "Resource/ResourcePathManager.h"

#define  CLASS_NAME "com/sample/Sample/Cocos2dxHelper"

SimpleAudioEngine* SimpleAudioEngine::m_pInstance = NULL;

bool getJNIStaticMethodInfo(JniMethodInfo &methodinfo,
    const char *methodName,
    const char *paramCode) 
{
    return JniHelper::getStaticMethodInfo(methodinfo, CLASS_NAME, methodName, paramCode);
}

SimpleAudioEngine::SimpleAudioEngine()
{
}

SimpleAudioEngine::~SimpleAudioEngine()
{
    JniMethodInfo methodInfo;
    if (getJNIStaticMethodInfo(methodInfo, "end", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::end()
{
    //TODO: Cocos doesn't implement this method. I don't know why.
}

void SimpleAudioEngine::preloadBackgroundMusic(const char* pszFilePath)
{
    std::string fullPath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Audio);
    fullPath.append(_T("/")).append(pszFilePath);

    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "preloadBackgroundMusic", "(Ljava/lang/String;)V"))
    {
        jstring stringArg = methodInfo.env->NewStringUTF(fullPath.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);
        methodInfo.env->DeleteLocalRef(stringArg);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::playBackgroundMusic(const char* pszFilePath, bool bLoop)
{
    std::string fullPath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Audio);
    fullPath.append(_T("/")).append(pszFilePath);

    JniMethodInfo methodInfo;
    bool bGetMethod = getJNIStaticMethodInfo(methodInfo, "playBackgroundMusic", "(Ljava/lang/String;Z)V");
    BEATS_ASSERT(bGetMethod, _T("Get Method playBackgroundMusic failed!"));
    if (bGetMethod)
    {
        jstring stringArg = methodInfo.env->NewStringUTF(fullPath.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg, bLoop);
        methodInfo.env->DeleteLocalRef(stringArg);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::stopBackgroundMusic(bool bReleaseData)
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "stopBackgroundMusic", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::pauseBackgroundMusic()
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "pauseBackgroundMusic", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::resumeBackgroundMusic()
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "resumeBackgroundMusic", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::rewindBackgroundMusic()
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "rewindBackgroundMusic", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}


bool SimpleAudioEngine::willPlayBackgroundMusic()
{
    return true;
}

bool SimpleAudioEngine::isBackgroundMusicPlaying()
{
    JniMethodInfo methodInfo;
    jboolean ret = false;

    if (getJNIStaticMethodInfo(methodInfo, "isBackgroundMusicPlaying", "()Z"))
    {
        ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
}

float SimpleAudioEngine::getBackgroundMusicVolume()
{
    JniMethodInfo methodInfo;
    jfloat ret = -1.0;

    if (getJNIStaticMethodInfo(methodInfo, "getBackgroundMusicVolume", "()F"))
    {
        ret = methodInfo.env->CallStaticFloatMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    return ret;
}

void SimpleAudioEngine::setBackgroundMusicVolume(float volume)
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "setBackgroundMusicVolume", "(F)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, volume);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

float SimpleAudioEngine::getEffectsVolume()
{
    JniMethodInfo methodInfo;
    jfloat ret = -1.0;

    if (getJNIStaticMethodInfo(methodInfo, "getEffectsVolume", "()F"))
    {
        ret = methodInfo.env->CallStaticFloatMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    return ret;
}

void SimpleAudioEngine::setEffectsVolume(float volume)
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "setEffectsVolume", "(F)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, volume);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

unsigned int SimpleAudioEngine::playEffect(const char* pszFilePath, bool bLoop,
                                           float pitch, float pan, float gain) 
{
    JniMethodInfo methodInfo;
    int ret = 0;
    std::string fullPath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Audio);
    fullPath.append(_T("/")).append(pszFilePath);

    if (getJNIStaticMethodInfo(methodInfo, "playEffect", "(Ljava/lang/String;ZFFF)I"))
    {
        jstring stringArg = methodInfo.env->NewStringUTF(fullPath.c_str());
        ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID,
            methodInfo.methodID,
            stringArg,
            bLoop,
            pitch, pan, gain);
        methodInfo.env->DeleteLocalRef(stringArg);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return (unsigned int)ret;
}

void SimpleAudioEngine::pauseEffect(unsigned int nSoundId)
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "pauseEffect", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)nSoundId);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::pauseAllEffects()
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "pauseAllEffects", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::resumeEffect(unsigned int nSoundId)
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "resumeEffect", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)nSoundId);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::resumeAllEffects()
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "resumeAllEffects", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::stopEffect(unsigned int nSoundId)
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "stopEffect", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)nSoundId);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::stopAllEffects()
{
    JniMethodInfo methodInfo;

    if (getJNIStaticMethodInfo(methodInfo, "stopAllEffects", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::preloadEffect(const char* pszFilePath)
{
    JniMethodInfo methodInfo;
    std::string fullPath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Audio);
    fullPath.append(_T("/")).append(pszFilePath);

    if (getJNIStaticMethodInfo(methodInfo, "preloadEffect", "(Ljava/lang/String;)V")) 
    {
        jstring stringArg = methodInfo.env->NewStringUTF(fullPath.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);
        methodInfo.env->DeleteLocalRef(stringArg);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void SimpleAudioEngine::unloadEffect(const char* pszFilePath)
{
    JniMethodInfo methodInfo;
    std::string fullPath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Audio);
    fullPath.append(_T("/")).append(pszFilePath);

    if (getJNIStaticMethodInfo(methodInfo, "unloadEffect", "(Ljava/lang/String;)V"))
    {
        jstring stringArg = methodInfo.env->NewStringUTF(fullPath.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);
        methodInfo.env->DeleteLocalRef(stringArg);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}