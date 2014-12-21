#ifndef FRAMEWORK_ANDROID_ANDROIDHANDLER_H__INCLUDE
#define FRAMEWORK_ANDROID_ANDROIDHANDLER_H__INCLUDE

namespace ndk_helper
{
    class GLContext;
    class PinchDetector;
    class DragDetector;
    class Vec2;
}

enum EAndroidPathType
{
    eAPT_ExternalDirectory,
    eAPT_FilesDirectory,

    eAPT_Count,
    eAPT_Force32Bit = 0xFFFFFFFF
};

static const TCHAR* pszAndroidPathString[] =
{
    _T("eAPT_ExternalDirectory"),
    _T("eAPT_FilesDirectory"),
};

struct android_app;
class ASensorManager;
class ASensor;
class ASensorEventQueue;
class AInputEvent;
class CAndroidKeyBoardDelegate;
class CAndroidHandler
{
    BEATS_DECLARE_SINGLETON(CAndroidHandler);

public:
    bool Initialize(android_app* pApp);
    bool IsFocus() const;
    // SensorStuff
    void InitSensors();
    void ProcessSensors( int32_t id );
    void SuspendAccelerometerSensor();
    void ResumeAccelerometerSensor();
    void SuspendGravitySensors();
    void ResumeGravitySensors();

    void ShowLoginWindow(const TString& strDefaultUserName, const TString& strDefaultPassword);
    void TransformPosition( ndk_helper::Vec2& vec );
    int GetNetState();
    int GetDeviceType();
    int GetSDKVersion();
    TString GetOsVersion();
    TString GetModelInfo();
    void CallJavaVoidMethodParamString(const TString& classPath, const TString& func, const TString& param, const TString& strParam);
    void CallJavaVoidMethod(const TString& classPath, const TString& func, const TString& param);
    bool CallJavaBooleanMethod(const TString& classPath, const TString& func, const TString& param);
    TString CallJavaStringMethod(const TString& classPath, const TString& func, const TString& param);
    float CallJavaFloatMethod(const TString& classPath, const TString& func, const TString& param);
    ndk_helper::GLContext* GetGLContext() const;
    static void HandleCmd( struct android_app* app, int32_t cmd );
    static int32_t HandleInput( android_app* app, AInputEvent* event );
    void FillDirectory(SDirectory& directory);
    const TString& GetAndroidPath(EAndroidPathType type) const;
    void SetAndroidPath(EAndroidPathType type, const TString& strPath);
    const TString& GetPackageName()const;
    void SetPackageName(const TString& strPackageName);

    void SetTextBoxStr(const TString& str);

    void RefreshTextBoxStr();
    void ShowLogin();
    void ExtractAndroidAssets();
    TString GetVersionName();
private:
    void InitVAOFunction();
    void FillDirectoryWithData(SDirectory& directory, CSerializer& data);

private:
    bool m_bHasFocus;
    ndk_helper::GLContext* m_glContext;
    android_app* m_pApp;
    ASensorManager* m_pSensorManager;
    const ASensor* m_pAccelerometerSensor;
    const ASensor* m_pGravitySensor;
    ASensorEventQueue* m_pSensorEventQueue;
    ndk_helper::PinchDetector m_pinchDetector;
    TString m_strPackageName;
    TString m_strAndroidPath[eAPT_Count];

    TString m_TextBoxStr;

    CAndroidKeyBoardDelegate* m_pKeyBoardDelegate;
};

#endif