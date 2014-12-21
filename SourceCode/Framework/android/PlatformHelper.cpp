#include "stdafx.h"
#include "PlatformHelper.h"
#include "JniHelper.h"
#include "AndroidHandler.h"
#include <string.h>
#include "external/SDKManager.h"
#include "StarRaidersCenter.h"
#include "EnginePublic/BeyondEngineVersion.h"
#include "Config/ServerConfig.h"

CPlatformHelper* CPlatformHelper::m_pInstance = NULL;

extern "C"
{
    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_LoadingTimeOut(JNIEnv* env, jobject thiz)
    {
        CSDKManager::GetInstance()->OnGetOrderTimeOut();
    }
}

CPlatformHelper::CPlatformHelper()
{
}

CPlatformHelper::~CPlatformHelper()
{
}

void CPlatformHelper::InitGameExtrat()
{
    TCHAR szDescription[MAX_PATH];
    _stprintf(szDescription, _T("%d.%d.%d"), BEYONDENGINE_VER_MAJOR, BEYONDENGINE_VER_MINOR, BEYONDENGINE_VER_REVISION);
    TString strResVersion = szDescription;
    CStarRaidersCenter::GetInstance()->SetAppVersion(strResVersion);
    SetAssetFileExtracted(!IsNeedExtract());
}

void CPlatformHelper::ShowAppraise()
{
    BEATS_PRINT("show appraise!");
}

std::string CPlatformHelper::GetResourceRootPath()
{
    return _T("assets/");
}

const char* CPlatformHelper::GetPersistentDataPath()
{
    if (m_strPersistentPath.empty())
    {
        m_strPersistentPath = CAndroidHandler::GetInstance()->GetAndroidPath(eAPT_FilesDirectory);
    }
    return m_strPersistentPath.c_str();
}

std::string CPlatformHelper::GetBinaryPath()
{
    return GetResourceRootPath();
}

bool CPlatformHelper::IsAssetFileExtracted() const
{
    return m_bExacted;
}

void CPlatformHelper::SetAssetFileExtracted(bool bExacted)
{
    m_bExacted = bExacted;
}

bool CPlatformHelper::IsNeedExtract()
{
    bool bNeedExact = false;
    TString strVersionFileName = GetPersistentDataPath();
    strVersionFileName.append("/").append("ver.bin");
    FILE* pVerFile = _tfopen(strVersionFileName.c_str(), "rb");
    if (pVerFile != nullptr)//First time run our game.
    {
        char verBuffer[128];
        fgets(verBuffer, 128, pVerFile);
        BEATS_PRINT("Find version file %s version:%s\n", strVersionFileName.c_str(), verBuffer);
        fclose(pVerFile);
        
        TString strVersionName = CStarRaidersCenter::GetInstance()->GetAppVersion();
        if (strcmp(verBuffer, strVersionName.c_str()) != 0)
        {
            bNeedExact = true;
        }
        else
        {
            SetAssetFileExtracted(true);
        }
    }
    else
    {
        bNeedExact = true;
    }
    return bNeedExact;
}

void CPlatformHelper::ShowLoginDialog()
{
   	CAndroidHandler::GetInstance()->ShowLogin();
}

void CPlatformHelper::ShowMessage(const std::string& strMsg)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "showMessage", "(Ljava/lang/String;)V"))
    {
        jstring jstrMsg = methodInfo.env->NewStringUTF(strMsg.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrMsg);
        methodInfo.env->DeleteLocalRef(jstrMsg);
    }
}

void CPlatformHelper::FadeOutMessage(float fTime)
{
}

const ELoginState& CPlatformHelper::GetLoginState() const
{
    return m_eState;
}

void CPlatformHelper::SetLoginState(const ELoginState& eState)
{
    m_eState = eState;
}

void CPlatformHelper::ShowLoadingAnimation(int ntime)
{
    JniMethodInfo methodInfo;
    BEATS_PRINT("show loading");
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "ShowLoadingAnimation", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, ntime);
    }
}

void CPlatformHelper::CloseLoadingAnimation()
{
    JniMethodInfo methodInfo;
    BEATS_PRINT("close loading");
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "CloseLoadingAnimation", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
}

void CPlatformHelper::DisableScreenSleep(bool bDisable)
{
    JniMethodInfo methodInfo;
    BEATS_PRINT("set screen on:%d", bDisable);
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "DisableScreenSleep", "(Z)V"))
    {
        jboolean jdisable = bDisable;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jdisable);
    }    
}

std::string CPlatformHelper::GetDieviceIMEI()
{
    TString ret;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "GetIMEI", "()Ljava/lang/String;"))
    {
        jobject jobj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        ret = JniHelper::jstring2string((jstring)jobj);
        methodInfo.env->DeleteLocalRef(jobj);
    }
    return ret;
}

void CPlatformHelper::SetPlatformFPS(int /*nFPS*/)
{

}

//单位M
int CPlatformHelper::GetFreeSpace()
{
    int nFreeSpace = -1;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, BEYONDENGINE_HELPER_CLASS_NAME, _T("GetSDCardFreeSpace"), _T("()I")))
    {
        nFreeSpace = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
    }
    return nFreeSpace;
}

void CPlatformHelper::OpenDownloadWeb()
{
    auto config = ServerConfig::GetInstance()->GetDefault();
    std::string strWeb = config.strAppUpdateUrl;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "OpenDownloadWeb", "(Ljava/lang/String;)V"))
    {
        jstring jstrWeb = methodInfo.env->NewStringUTF(strWeb.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrWeb);
        methodInfo.env->DeleteLocalRef(jstrWeb);
    }
}

void CPlatformHelper::InitSenorsManager()
{
    CAndroidHandler::GetInstance()->InitSensors();
}

void CPlatformHelper::StartAccelerometerListener()
{
    CAndroidHandler::GetInstance()->ResumeAccelerometerSensor();
}

void CPlatformHelper::StopAccelerometerListener()
{
    CAndroidHandler::GetInstance()->SuspendAccelerometerSensor();
}

void CPlatformHelper::OnAccelerometer(float x, float y, float z)
{
    x = MAX(x, -1.0f);
    x = MIN(x, 1.0f);
    y = MAX(y, -1.0f);
    y = MIN(y, 1.0f);
    z = MAX(z, -1.0f);
    z = MIN(z, 1.0f);
    m_vecAccelerometer = CVec3(x, y, z);
}

const CVec3& CPlatformHelper::GetAccelerometer() const
{
    return m_vecAccelerometer;
}

void CPlatformHelper::StartGravityListener()
{
    CAndroidHandler::GetInstance()->ResumeGravitySensors();
}

void CPlatformHelper::StopGravityListener()
{
    CAndroidHandler::GetInstance()->SuspendGravitySensors();
}

void CPlatformHelper::OnGravity(float x, float y, float z)
{
    x = MAX(x, -1.0f);
    x = MIN(x, 1.0f);
    y = MAX(y, -1.0f);
    y = MIN(y, 1.0f);
    z = MAX(z, -1.0f);
    z = MIN(z, 1.0f);
    m_vecGravity = CVec3(x, y, z);
}

const CVec3& CPlatformHelper::GetGravity() const
{
    return m_vecGravity;
}