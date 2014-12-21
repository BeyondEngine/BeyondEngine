#ifndef BEYOND_ENGINE_FRAMEWORK_PLATFORMHELPER_H__INCLUDE
#define BEYOND_ENGINE_FRAMEWORK_PLATFORMHELPER_H__INCLUDE

enum ELoginState
{
    eLoginBegin = 0,
    eLogining,
    eLoginEnd,
};

class CPlatformHelper
{
    BEATS_DECLARE_SINGLETON(CPlatformHelper);
public:
    const char* GetPersistentDataPath();
    std::string GetBinaryPath();
    bool IsAssetFileExtracted() const;
    void SetAssetFileExtracted(bool bExacted);
    bool IsNeedExtract();
    void InitGameExtrat();
    void ShowLoginDialog();
    void ShowMessage(const std::string& strMsg);
    void FadeOutMessage(float fTime = 3.0f);
    const ELoginState& GetLoginState() const;
    void  SetLoginState(const ELoginState& eState);
    void ShowLoadingAnimation(int ntime);
    void CloseLoadingAnimation();
    void DisableScreenSleep(bool bDisable);
    std::string GetDieviceIMEI();
    void SetPlatformFPS(int nFPS);
    int GetFreeSpace();
    void OpenDownloadWeb();
    void ShowAppraise();
    std::string GetResourceRootPath();
    void InitSenorsManager();

    void StartAccelerometerListener();
    void StopAccelerometerListener();
    void OnAccelerometer(float x, float y, float z);
    const CVec3& GetAccelerometer() const;

    void StartGravityListener();
    void StopGravityListener();
    void OnGravity(float x, float y, float z);
    const CVec3& GetGravity() const;
private:
    ELoginState m_eState = eLoginBegin;
    bool m_bExacted = false;
    std::string m_strPersistentPath;
    std::string m_strResourcePath;
    CVec3 m_vecAccelerometer;
    CVec3 m_vecGravity;
};

#endif
