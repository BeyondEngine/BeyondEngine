
#include "stdafx.h"
#include "XGSdkImplement.h"
#include "StarRaidersCenter.h"
#include "Utility/BeatsUtility/md5.h"
#include "Utility/VerifyToken.h"
#include "Application.h"
#include "JniHelper.h"
#include "AndroidHandler.h"
#include "external/SDKManager.h"

CXGSdkImplement* CXGSdkImplement::m_pInstance = NULL;

CXGSdkImplement::CXGSdkImplement()
{
}

CXGSdkImplement::~CXGSdkImplement()
{
}

void CXGSdkImplement::RegistXGByAccount(const std::string& strAccount)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "RegisterPushByAccount", "(Ljava/lang/String;)V", strAccount);
}

void CXGSdkImplement::UnRegistXGByAccount()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "UnRegisterAccountPush", "()V");
}

void CXGSdkImplement::UnRegistPush()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "UnRegisterPush", "()V");
}


void CXGSdkImplement::AddLocalPush(const std::string& strBody, const std::string& strKey, const std::string& strValue, const uint32_t& uInterval)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, SDK_ACTIVITY_CLASS_NAME, "setLocalPush", "(Ljava/lang/String;I)V"))
    {
        jstring jstrBody = methodInfo.env->NewStringUTF(strBody.c_str());
        jint nInterval = (jint)uInterval;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrBody, nInterval);
        methodInfo.env->DeleteLocalRef(jstrBody);
    }
}

void CXGSdkImplement::ClearLocalPushes()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "ClearLocalPushes", "()V");
}