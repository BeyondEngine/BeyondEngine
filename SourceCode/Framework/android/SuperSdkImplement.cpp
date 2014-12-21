
#include "stdafx.h"
#include "SuperSdkImplement.h"
#include "StarRaidersCenter.h"
#include "Utility/BeatsUtility/md5.h"
#include "Utility/VerifyToken.h"
#include "Application.h"
#include "JniHelper.h"
#include "AndroidHandler.h"
#include "external/SDKManager.h"
#include "Language/LanguageManager.h"

CSuperSdkImplement* CSuperSdkImplement::m_pInstance = NULL;
extern "C"
{
    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_SetTicket(JNIEnv* env, jobject thiz, jstring str)
    {
        std::string strChar = JniHelper::jstring2string(str);
        CSDKManager::GetInstance()->SetTicket(strChar);
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnSuperSdkInitSuccess(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnSuperSdkInitSuccess();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnSuperSdkInitFailed(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnSuperSdkInitFailed();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnCheckWithNewVersion(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnCheckWithNewVersion();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnCheckWithOutNewVersion(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnCheckWithoutNewVersion();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnCheckWithOutCheckVersion(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnCheckWithoutCheckVersion();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnLoginSuccess(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnLoginSuccess();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnLoginFailed(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnLoginFailed();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnLogoutSuccess(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnLogoutSuccess();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnLogoutFailed(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnLogoutFailed();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnGetOrderSuccess(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnGetOrderIdSuccess(0);
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnPaySuccess(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnPaySuccess();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_OnPayFailed(JNIEnv* env, jobject thiz, jstring str)
    {
        std::string strChar = JniHelper::jstring2string(str);
        CSDKManager::GetInstance()->OnPayFailed(strChar);
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_SdkActivity_SetHaveLoginDialog(JNIEnv* env, jobject thiz, jboolean bHave)
    {
        if (bHave == JNI_TRUE)
        {
            CSDKManager::GetInstance()->SetHaveLoginDialog(true);
        }
        else
        {
            CSDKManager::GetInstance()->SetHaveLoginDialog(false);
        }
    }
    
};

CSuperSdkImplement::CSuperSdkImplement()
{
}

CSuperSdkImplement::~CSuperSdkImplement()
{
}

void CSuperSdkImplement::OnInitPlatform()
{
    //must init in OnCreate
    //CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "initSuperSdk", "()V");
}

void CSuperSdkImplement::OnInitPlatformSuccess()
{
}

void CSuperSdkImplement::OnInitPlatformFailed()
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "showMessage", "(Ljava/lang/String;)V"))
    {
        std::string strMsg = L10N(eLTT_Games_Alert_LoseConnect);
        jstring jstrMsg = methodInfo.env->NewStringUTF(strMsg.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrMsg);
        methodInfo.env->DeleteLocalRef(jstrMsg);
    }
}

void CSuperSdkImplement::OnCheckWithNewVersion()
{
}

void CSuperSdkImplement::OnCheckWithoutNewVersion()
{
}

void CSuperSdkImplement::OnCheckWithoutCheckVersion()
{
}

void CSuperSdkImplement::ShowPlatformLogin()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "showLogin", "()V");
}

void CSuperSdkImplement::OnLoginPlatformSuccess()
{
}

void CSuperSdkImplement::OnLoginPlatformFailed()
{
    
}

void CSuperSdkImplement::ShowFloatWindow(const int& nX, const int& nY)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "showFloatWindow", "()V");
}

void CSuperSdkImplement::CloseFloatWindow()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "closeFloatWindow", "()V");
}

void CSuperSdkImplement::OnShowLogoutAlert()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "showLogoutAlert", "()V");
}

void CSuperSdkImplement::LogoutGame()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "logoutGame", "()V");
}

void CSuperSdkImplement::OnLogoutSuccess()
{
    CStarRaidersCenter::GetInstance()->SetNeedRestart(true);
}

void CSuperSdkImplement::OnLogoutFailed()
{
}

void CSuperSdkImplement::PayProduct(const int& nPrice, const std::string& strProductId, const std::string& strProductName, const std::string& strProductDesc, const std::string& strPointRate, const std::string& strPointName, const std::string& strOrderTitle, const std::string& strCustomData)
{
    BEATS_PRINT("PAY_PRODUCT CALL IN SDK IMPLEMENT");
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, SDK_ACTIVITY_CLASS_NAME, "payProducet", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    {
        BEATS_PRINT("PAY_PRODUCT HAVE JAVA API");
        jstring jstrProductId = methodInfo.env->NewStringUTF(strProductId.c_str());
        jstring jstrProductName = methodInfo.env->NewStringUTF(strProductName.c_str());
        jstring jstrProductDesc = methodInfo.env->NewStringUTF(strProductDesc.c_str());
        jstring jstrPointRate = methodInfo.env->NewStringUTF(strPointRate.c_str());
        jstring jstrPointName = methodInfo.env->NewStringUTF(strPointName.c_str());
        jstring jstrOrderTitle = methodInfo.env->NewStringUTF(strOrderTitle.c_str());
        jstring jstrCustomData = methodInfo.env->NewStringUTF(strCustomData.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, nPrice, jstrProductId, jstrProductName, jstrProductDesc, jstrPointRate, jstrPointName, jstrOrderTitle, jstrCustomData);
        BEATS_PRINT("PAY_PRODUCT CALL JAVA API");
        methodInfo.env->DeleteLocalRef(jstrProductId);
        methodInfo.env->DeleteLocalRef(jstrProductName);
        methodInfo.env->DeleteLocalRef(jstrProductDesc);
        methodInfo.env->DeleteLocalRef(jstrPointRate);
        methodInfo.env->DeleteLocalRef(jstrPointName);
        methodInfo.env->DeleteLocalRef(jstrOrderTitle);
        methodInfo.env->DeleteLocalRef(jstrCustomData);
    }
}

void CSuperSdkImplement::OnGetOrderIdSuccess(const int& nOrderId)
{
}

void CSuperSdkImplement::OnPaySuccess()
{
}

void CSuperSdkImplement::OnPayFailed(const std::string& strParam)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "showMessage", "(Ljava/lang/String;)V"))
    {
        jstring jstrMsg = methodInfo.env->NewStringUTF(strParam.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrMsg);
        methodInfo.env->DeleteLocalRef(jstrMsg);
    }
}

void CSuperSdkImplement::PerformGameEventOfGameInitEnd()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventInitEnd", "()V");
}

void CSuperSdkImplement::PerformGameEventOfGameCheckNewVersionBegin()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventCheckNewVersionBegin", "()V");
}

void CSuperSdkImplement::PerformGameEventOfGameCheckNewVersionEnd(bool bSuccess)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventCheckNewVersionEnd", "()V");
}

void CSuperSdkImplement::PerformGameEventOfLoginView()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventShowLoginView", "()V");
}

void CSuperSdkImplement::PerformGameEventOfEnterGame()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventEnterGame", "()V");
}

void CSuperSdkImplement::PerformGameEventOfCreateRole()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventCreatRole", "()V");
}

void CSuperSdkImplement::PerformGameEventOfOpenMainPage()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventOpenMainPage", "()V");
}

void CSuperSdkImplement::PerformGameEventOfLevelUp()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "gameEventLevelUp", "()V");
}

void CSuperSdkImplement::SetGameDataServerId(const std::string& strServerId)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "SetGameDataServerId", "(Ljava/lang/String;)V", strServerId);
}

void CSuperSdkImplement::SetGameDataServerName(const std::string& strServerName)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "SetGameDataServerName", "(Ljava/lang/String;)V", strServerName);
}

void CSuperSdkImplement::SetGameDataAccountId(const std::string& strAccountId)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "SetGameDataAccountId", "(Ljava/lang/String;)V", strAccountId);
}

void CSuperSdkImplement::SetGameDataRoleId(const std::string& strRoleId)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "SetGameDataRoleId", "(Ljava/lang/String;)V", strRoleId);
}

void CSuperSdkImplement::SetGameDataRoleName(const std::string& strName)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "SetGameDataRoleName", "(Ljava/lang/String;)V", strName);
}

void CSuperSdkImplement::SetGameDataRoleLevel(const std::string& strLevel)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "SetGameDataRoleLevel", "(Ljava/lang/String;)V", strLevel);
}

void CSuperSdkImplement::SetGameDataLoginData(const std::string& strLoginData)
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethodParamString(SDK_ACTIVITY_CLASS_NAME, "SetGameDataLoginData", "(Ljava/lang/String;)V", strLoginData);
}

void CSuperSdkImplement::SetGameDataExByKey(const std::string& strKey,const std::string& strValue)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, SDK_ACTIVITY_CLASS_NAME, "SetGameDataExByKey", "(Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jstrKey = methodInfo.env->NewStringUTF(strKey.c_str());
        jstring jstrValue = methodInfo.env->NewStringUTF(strValue.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrKey, jstrValue);
        methodInfo.env->DeleteLocalRef(jstrKey);
        methodInfo.env->DeleteLocalRef(jstrValue);
    }
}

const std::string CSuperSdkImplement::GetGameDataExByKey(const std::string& strKey,const std::string& strDefaultValue)
{
    std::string strRet;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, SDK_ACTIVITY_CLASS_NAME, "GetGameDataExByKey", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
    {
        jstring jstrKey = methodInfo.env->NewStringUTF(strKey.c_str());
        jstring jstrValue = methodInfo.env->NewStringUTF(strDefaultValue.c_str());
        jobject jobj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jstrKey, jstrValue);
        strRet = JniHelper::jstring2string((jstring)jobj);
        methodInfo.env->DeleteLocalRef(jstrKey);
        methodInfo.env->DeleteLocalRef(jstrValue);
        methodInfo.env->DeleteLocalRef(jobj);
    }
    return strRet;
}

bool CSuperSdkImplement::HasForum() const
{
    return CAndroidHandler::GetInstance()->CallJavaBooleanMethod(SDK_ACTIVITY_CLASS_NAME, "HasForum", "()Z");
}

void CSuperSdkImplement::OpenForum()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "OpenForum", "()V");
}

bool CSuperSdkImplement::HasPersonalCenter() const
{
    return CAndroidHandler::GetInstance()->CallJavaBooleanMethod(SDK_ACTIVITY_CLASS_NAME, "HasPersonalCenter", "()Z");
}

void CSuperSdkImplement::OpenPersonalCenter()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "OpenPersonalCenter", "()V");
}

bool CSuperSdkImplement::HasCustomerService() const
{
    return CAndroidHandler::GetInstance()->CallJavaBooleanMethod(SDK_ACTIVITY_CLASS_NAME, "HasCustomerService", "()Z");
}

void CSuperSdkImplement::OpenCustomerService()
{
    CAndroidHandler::GetInstance()->CallJavaVoidMethod(SDK_ACTIVITY_CLASS_NAME, "OpenCustomerService", "()V");
}

const std::string CSuperSdkImplement::GetDeviceID()
{
    std::string strRet = CAndroidHandler::GetInstance()->CallJavaStringMethod(SDK_ACTIVITY_CLASS_NAME,"GetDeviceID","()Ljava/lang/String;");
    return strRet;
}

void CSuperSdkImplement::SetState(int nState)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, SDK_ACTIVITY_CLASS_NAME, "SetState", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, nState);
    }
}

TString CSuperSdkImplement::GetPlatformExtend()
{
    std::string strRet = CAndroidHandler::GetInstance()->CallJavaStringMethod(SDK_ACTIVITY_CLASS_NAME, "GetPlatExtend", "()Ljava/lang/String;");
    return strRet;
}