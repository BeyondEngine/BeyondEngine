#include "stdafx.h"
#include <arpa/inet.h>
#include "AndroidHandler.h"
#include "Event/TouchDelegate.h"
#include "Event/GestureState.h"
#include "Framework/Application.h"
#include "Render/RenderTarget.h"
#include "Render/RenderManager.h"
#include "external/Configuration.h"
#include "Framework/android/JniHelper.h"
#include "StarRaidersCenter.h"
#include "Resource/ResourceManager.h"
#include "BeyondEngineUI/StarRaidersGUISystem.h"
#include "Audio/include/AudioEngine.h"
#include "EnginePublic/EngineCenter.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "EnginePublic/PlayerPrefs.h"
#include "GUI/IMEManager.h"
#include "Framework/android/AndroidKeyBoardDelegate.h"
#include "Task/ExtractAndroidAssets.h"
#include "PlatformHelper.h"
#include "BeyondEngineUI/UISystemManager.h"

CAndroidHandler* CAndroidHandler::m_pInstance = NULL;
#define ASENSOR_TYPE_GRAVITY 9

extern "C"
{
    jint JNI_OnLoad(JavaVM *vm, void *reserved)
    {
        JniHelper::setJavaVM(vm);

        return JNI_VERSION_1_4;
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_BeyondEngineHelper_nativeSetContext(JNIEnv* env, jobject thiz, jobject context)
    {
        JniHelper::setClassLoaderFrom(context);
        BEATS_PRINT(_T("Java_com_beyondtech_starraiders_BeyondEngineHelper_nativeSetContext\n"));
        JniMethodInfo methodInfo;
        if (JniHelper::getStaticMethodInfo(methodInfo, BEYONDENGINE_HELPER_CLASS_NAME, "GetExternalSDPath", "()Ljava/lang/String;"))
        {
            jstring strRet = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
            TString strExteranlPath = JniHelper::jstring2string(strRet);
            CAndroidHandler::GetInstance()->SetAndroidPath(eAPT_ExternalDirectory, strExteranlPath);
            methodInfo.env->DeleteLocalRef(strRet);
        }
        methodInfo.CleanUp();
        if (JniHelper::getStaticMethodInfo(methodInfo, BEYONDENGINE_HELPER_CLASS_NAME, "GetFilesDirectoryPath", "()Ljava/lang/String;"))
        {
            jstring strRet = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
            TString strExteranlPath = JniHelper::jstring2string(strRet);
            CAndroidHandler::GetInstance()->SetAndroidPath(eAPT_FilesDirectory, strExteranlPath);
            methodInfo.env->DeleteLocalRef(strRet);
        }
        methodInfo.CleanUp();
        if (JniHelper::getStaticMethodInfo(methodInfo, BEYONDENGINE_HELPER_CLASS_NAME, "GetPackageName", "()Ljava/lang/String;"))
        {
            jstring strRet = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
            TString strPackageName = JniHelper::jstring2string(strRet);
            CAndroidHandler::GetInstance()->SetPackageName(strPackageName);
            methodInfo.env->DeleteLocalRef(strRet);
        }
        CPlatformHelper::GetInstance()->InitGameExtrat();
        if (!CPlatformHelper::GetInstance()->IsAssetFileExtracted())
        {
            TString strDirectoryAbsolutePath = CAndroidHandler::GetInstance()->GetAndroidPath(eAPT_FilesDirectory);
            strDirectoryAbsolutePath.append("/assets/resource/font");
            BEATS_PRINT("Start extract basic file to %s\n", strDirectoryAbsolutePath.c_str());
            CFilePathTool::GetInstance()->MakeDirectory(strDirectoryAbsolutePath.c_str());
            SDirectory fontDirectory(nullptr, "resource/font");
            CAndroidHandler::GetInstance()->FillDirectory(fontDirectory);
            uint32_t uExtractSize = 0;
            CExtractAndroidAssets::ExtractAssetsFromDirectory(&fontDirectory, uExtractSize);
            BEATS_PRINT("Extract basic file to %s\n", strDirectoryAbsolutePath.c_str());
        }
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_InitAndroidHandler(JNIEnv * env, jobject thiz)
    {
        //HACK:fix the mult thread bug
        CAndroidHandler::GetInstance();
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_StarRaidersLogin(JNIEnv * env, jobject thiz, jstring account, jstring password)
    {
        std::string strAccount = JniHelper::jstring2string(account);
        std::string strPassword = JniHelper::jstring2string(password);
        CStarRaidersCenter::GetInstance()->SetAccountString(strAccount);
        CStarRaidersCenter::GetInstance()->SetPasswordString(strPassword);
        CApplication::GetInstance()->Resume();
        CPlatformHelper::GetInstance()->SetLoginState(eLoginEnd);
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_ChangeChar(JNIEnv* env, jobject thiz, jstring str)
    {
	    std::string strChar = JniHelper::jstring2string(str);
        CAndroidHandler::GetInstance()->SetTextBoxStr(strChar);
    }

    JNIEXPORT void JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_OnBackwardChar(JNIEnv* env, jobject thiz)
    {
        CIMEManager::GetInstance()->OnKeyBackward();
    }

    JNIEXPORT jstring JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_GetCurrText(JNIEnv* env, jobject thiz)
    {
        std::string strText = CIMEManager::GetInstance()->GetCurrText();
        jstring jstrText = env->NewStringUTF(strText.c_str());
        return jstrText;
    }
    
    JNIEXPORT jfloat JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_GetEditBoxPosYPercent(JNIEnv* env, jobject thiz)
    {
        float fRet = CIMEManager::GetInstance()->GetEditBoxPosYPercent();
        return (jfloat)fRet;
    }

    JNIEXPORT jstring JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_StringFilter(JNIEnv* env, jobject thiz, jstring str)
    {
        std::string strChar = JniHelper::jstring2string(str);
        std::string strText = CIMEManager::GetInstance()->OnStringFilter(strChar);
        jstring jstrText = env->NewStringUTF(strText.c_str());
        return jstrText;
    }
	
	JNIEXPORT jstring JNICALL Java_com_beyondtech_starraiders_StarRaidersActivity_GetLogListString(JNIEnv* env, jobject thiz)
    {
		TString strLogList;
        #ifdef DEVELOP_VERSION
        strLogList = CLogManager::GetInstance()->GetLastLogList(500);
        #endif
        jstring jstrText = env->NewStringUTF(strLogList.c_str());
        return jstrText;
    }
};

CAndroidHandler::CAndroidHandler()
    : m_bHasFocus(false)
    , m_glContext (ndk_helper::GLContext::GetInstance())
    , m_pApp(nullptr)
    , m_pSensorManager(nullptr)
    , m_pAccelerometerSensor(nullptr)
    , m_pGravitySensor(nullptr)
    , m_pSensorEventQueue(nullptr)
{
    m_pKeyBoardDelegate = new CAndroidKeyBoardDelegate();
    CIMEManager::GetInstance()->SetKeyBoardDelegate(m_pKeyBoardDelegate);
}

CAndroidHandler::~CAndroidHandler()
{
    BEATS_SAFE_DELETE(m_pKeyBoardDelegate);
}

bool CAndroidHandler::Initialize(android_app* pApp)
{
    BEATS_ASSERT(pApp != NULL, _T("App can't be null when initialize Android handler!"));
    m_pApp = pApp;
    m_pinchDetector.SetConfiguration( m_pApp->config );

}

bool CAndroidHandler::IsFocus() const
{
    return m_bHasFocus;
}

void CAndroidHandler::InitSensors()
{
    m_pSensorManager = ASensorManager_getInstance();
    BEATS_ASSERT(m_pSensorManager != NULL, _T("SensorManager can't be null!"));
    m_pGravitySensor = ASensorManager_getDefaultSensor( m_pSensorManager, ASENSOR_TYPE_GRAVITY );
    m_pAccelerometerSensor = ASensorManager_getDefaultSensor( m_pSensorManager, ASENSOR_TYPE_ACCELEROMETER );
    m_pSensorEventQueue = ASensorManager_createEventQueue( m_pSensorManager, m_pApp->looper, LOOPER_ID_USER, NULL, NULL );
}

void CAndroidHandler::ProcessSensors( int32_t id )
{
    // If a sensor has data, process it now.
    if( id == LOOPER_ID_USER )
    {
        if( m_pAccelerometerSensor != NULL )
        {
            ASensorEvent event;
            while( ASensorEventQueue_getEvents( m_pSensorEventQueue, &event, 1 ) > 0 )
            {
                if (event.type == ASENSOR_TYPE_GRAVITY)
                {
                    CPlatformHelper::GetInstance()->OnGravity(event.vector.x/ASENSOR_STANDARD_GRAVITY
                    , event.vector.y/ASENSOR_STANDARD_GRAVITY, event.vector.z/ASENSOR_STANDARD_GRAVITY);
                }
                else if (event.type == ASENSOR_TYPE_ACCELEROMETER)
                {
                    CPlatformHelper::GetInstance()->OnAccelerometer(event.acceleration.x/ASENSOR_STANDARD_GRAVITY
                    , event.acceleration.y/ASENSOR_STANDARD_GRAVITY, event.acceleration.z/ASENSOR_STANDARD_GRAVITY);
                }
            }
        }
    }
}

void CAndroidHandler::SuspendAccelerometerSensor()
{
    // When our app loses focus, we stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if( m_pAccelerometerSensor != NULL )
    {
        ASensorEventQueue_disableSensor( m_pSensorEventQueue, m_pAccelerometerSensor );
    }
}

void CAndroidHandler::ResumeAccelerometerSensor()
{
    // When our app gains focus, we start monitoring the accelerometer.
    if( m_pAccelerometerSensor != NULL )
    {
        ASensorEventQueue_enableSensor( m_pSensorEventQueue, m_pAccelerometerSensor );
        ASensorEventQueue_setEventRate( m_pSensorEventQueue, m_pAccelerometerSensor,
            (1000L / 60 * 1000));
    }
}

void CAndroidHandler::SuspendGravitySensors()
{
   if( m_pGravitySensor != NULL )
    {
        ASensorEventQueue_disableSensor( m_pSensorEventQueue, m_pGravitySensor );
    }
}

void CAndroidHandler::ResumeGravitySensors()
{
    if( m_pGravitySensor != NULL )
    {
        ASensorEventQueue_enableSensor( m_pSensorEventQueue, m_pGravitySensor );
        ASensorEventQueue_setEventRate( m_pSensorEventQueue, m_pGravitySensor,
            (1000L / 60 * 1000));
    }
}

void CAndroidHandler::ShowLoginWindow(const TString& strDefaultUserName, const TString& strDefaultPassword)
{
    JniMethodInfo methodInfo;
#ifdef _SUPER_SDK
    if (JniHelper::getStaticMethodInfo(methodInfo, SDK_ACTIVITY_CLASS_NAME, "showLoginWindowSdk", "(Ljava/lang/String;Ljava/lang/String;)V"))
#else 
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, "showLoginWindow", "(Ljava/lang/String;Ljava/lang/String;)V"))
#endif   
    {
        jstring strUserName = methodInfo.env->NewStringUTF(strDefaultUserName.c_str());
        jstring strPassword = methodInfo.env->NewStringUTF(strDefaultPassword.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, strUserName, strPassword);
        methodInfo.env->DeleteLocalRef(strUserName);
        methodInfo.env->DeleteLocalRef(strPassword);
    }
}

void CAndroidHandler::TransformPosition( ndk_helper::Vec2& vec )
{
    vec = ndk_helper::Vec2( 2.0f, 2.0f ) * vec
        / ndk_helper::Vec2( m_glContext->GetScreenWidth(), m_glContext->GetScreenHeight() )
        - ndk_helper::Vec2( 1.f, 1.f );
}

ndk_helper::GLContext* CAndroidHandler::GetGLContext() const
{
    return m_glContext;
}

void CAndroidHandler::HandleCmd( struct android_app* app, int32_t cmd )
{
    CAndroidHandler* pHandler = CAndroidHandler::GetInstance();
    BEATS_PRINT("StartHandle android cmd %d\n", cmd);
    switch( cmd )
    {
    case APP_CMD_SAVE_STATE:
        BEATS_PRINT("APP_CMD_SAVE_STATE %d\n", cmd);
        break;
    case APP_CMD_INIT_WINDOW:
        BEATS_PRINT("APP_CMD_INIT_WINDOW %d\n", cmd);
        // The window is being shown, get it ready.
        if( pHandler->m_pApp->window != NULL )
        {
            pHandler->m_glContext->Resume( pHandler->m_pApp->window );
            if (!CApplication::GetInstance()->IsInitialized())
            {
                pHandler->InitVAOFunction();
                int32_t screenWidth = pHandler->GetGLContext()->GetScreenWidth();
                int32_t screenHeight = pHandler->GetGLContext()->GetScreenHeight();
                CRenderTarget* pRenderWindow = new CRenderTarget(screenWidth, screenHeight);
                CRenderManager::GetInstance()->SetCurrentRenderTarget(pRenderWindow);
                CApplication::GetInstance()->Initialize();
            }
        }
        break;
    case APP_CMD_TERM_WINDOW:
        BEATS_PRINT("APP_CMD_TERM_WINDOW %d\n", cmd);
        pHandler->m_glContext->Suspend();
        pHandler->m_bHasFocus = false;
        break;
    case APP_CMD_STOP:
        BEATS_PRINT("APP_CMD_STOP %d\n", cmd);
        CAudioEngine::Pause(CStarRaidersCenter::GetInstance()->GetBackGroundMusic());
        break;
    case APP_CMD_START:
        BEATS_PRINT("APP_CMD_START %d\n", cmd);
        break;
    case APP_CMD_RESUME:
        BEATS_PRINT("APP_CMD_RESUME %d\n", cmd);
        CApplication::GetInstance()->OnSwitchToForeground();
        break;
    case APP_CMD_PAUSE:
        BEATS_PRINT("APP_CMD_PAUSE %d\n", cmd);
        CApplication::GetInstance()->OnSwitchToBackground();
        break;
    case APP_CMD_GAINED_FOCUS:
        BEATS_PRINT("APP_CMD_GAINED_FOCUS %d\n", cmd);
        pHandler->m_bHasFocus = true;
        break;
    case APP_CMD_LOST_FOCUS:
        BEATS_PRINT("APP_CMD_LOST_FOCUS %d\n", cmd);
        pHandler->m_bHasFocus = false;
        break;
    case APP_CMD_DESTROY:
        BEATS_PRINT("APP_CMD_DESTROY %d\n", cmd);
        break;
    case APP_CMD_CONFIG_CHANGED:
        BEATS_PRINT("APP_CMD_CONFIG_CHANGED %d\n", cmd);
        break;
    case APP_CMD_CONTENT_RECT_CHANGED:
        BEATS_PRINT("APP_CMD_CONTENT_RECT_CHANGED %d\n", cmd);
        break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
        BEATS_PRINT("APP_CMD_WINDOW_REDRAW_NEEDED %d\n", cmd);
        break;
    case APP_CMD_WINDOW_RESIZED:
        BEATS_PRINT("APP_CMD_WINDOW_RESIZED %d\n", cmd);
        break;
    case APP_CMD_LOW_MEMORY:
        CResourceManager::GetInstance()->CleanUp();
        BEATS_ASSERT(false, _T("Memory is too low!"));
        break;
    }
    BEATS_PRINT("Finish Handle android cmd %d\n", cmd);
}

int32_t CAndroidHandler::HandleInput( android_app* app, AInputEvent* event )
{
    int32_t nRet = 0;
    CAndroidHandler* pHandler = CAndroidHandler::GetInstance();
    CTouchDelegate* pTouchDelegate = CTouchDelegate::GetInstance();
    if( AInputEvent_getType( event ) == AINPUT_EVENT_TYPE_MOTION )
    {
        ndk_helper::GESTURE_STATE pinchState = pHandler->m_pinchDetector.Detect(event);

        int32_t action = AMotionEvent_getAction( event );
        int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
            >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
        intptr_t pointerCount = AMotionEvent_getPointerCount( event );
        CRenderTarget* pCurrRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
        BEATS_ASSERT(pCurrRenderTarget != NULL);
        float fScaleFactor = pCurrRenderTarget->GetScaleFactor();
        switch (flags)
        {
        case AMOTION_EVENT_ACTION_DOWN:
            {
                BEATS_ASSERT(pointerCount == 1, _T("There should be only one pointer in AMOTION_EVENT_ACTION_DOWN"));
                BEATS_WARNING(index == 0, _T("index should be 0"));
                size_t pointerId = AMotionEvent_getPointerId(event, 0);
                float x = AMotionEvent_getX( event, 0 );
                float y = AMotionEvent_getY( event, 0 );
                x /= fScaleFactor;
                y /= fScaleFactor;
                pTouchDelegate->OnTouchBegan(1, &pointerId, &x, &y);
            }
            break;
        case AMOTION_EVENT_ACTION_UP:
            {
                BEATS_ASSERT(pointerCount == 1, _T("There should be only one pointer in AMOTION_EVENT_ACTION_UP"));
                BEATS_WARNING(index == 0, _T("index should be 0"));
                size_t pointerId = AMotionEvent_getPointerId(event, 0);
                float x = AMotionEvent_getX( event, 0 );
                float y = AMotionEvent_getY( event, 0 );
                x /= fScaleFactor;
                y /= fScaleFactor;
                size_t id[1] = { pointerId };
                pTouchDelegate->OnTouchEnded(1, id, &x, &y);
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            {
                BEATS_ASSERT(pointerCount > 1, _T("There should be greater than one pointer in AMOTION_EVENT_ACTION_POINTER_DOWN"));
                size_t pointerId = AMotionEvent_getPointerId(event, index);
                float x = AMotionEvent_getX( event, index );
                float y = AMotionEvent_getY( event, index );
                x /= fScaleFactor;
                y /= fScaleFactor;
                pTouchDelegate->OnTouchBegan(1, &pointerId, &x, &y);
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
            {
                BEATS_ASSERT(pointerCount > 1, _T("There should be greater than one pointer in AMOTION_EVENT_ACTION_POINTER_DOWN"));
                size_t pointerId = AMotionEvent_getPointerId(event, index);
                float x = AMotionEvent_getX( event, index );
                float y = AMotionEvent_getY( event, index );
                x /= fScaleFactor;
                y /= fScaleFactor;
                size_t id[1] = { pointerId };
                pTouchDelegate->OnTouchEnded(1, id, &x, &y);
            }
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            {
                size_t pointerIds[TOUCH_MAX_NUM] = { 0 };
                float xs[TOUCH_MAX_NUM] = {0.0f};
                float ys[TOUCH_MAX_NUM] = {0.0f};
                for (uint32_t i = 0; i < pointerCount && i < TOUCH_MAX_NUM; ++i)
                {
                    pointerIds[i] = AMotionEvent_getPointerId( event, i );
                    xs[i] = AMotionEvent_getX(event, i);
                    ys[i] = AMotionEvent_getY(event, i);
                    xs[i] /= fScaleFactor;
                    ys[i] /= fScaleFactor;
                }
                pTouchDelegate->OnTouchMoved(pointerCount, pointerIds, xs, ys);
            }
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            {
                size_t pointerIds[TOUCH_MAX_NUM] = { 0 };
                float xs[TOUCH_MAX_NUM] = {0.0f};
                float ys[TOUCH_MAX_NUM] = {0.0f};
                for (uint32_t i = 0; i < pointerCount && i < TOUCH_MAX_NUM; ++i)
                {
                    pointerIds[i] = AMotionEvent_getPointerId( event, i );
                    xs[i] = AMotionEvent_getX(event, i);
                    ys[i] = AMotionEvent_getY(event, i);
                    xs[i] /= fScaleFactor;
                    ys[i] /= fScaleFactor;
                }
                pTouchDelegate->OnTouchCancelled(pointerCount, pointerIds, xs, ys);
            }
            break;
        default:
            BEATS_ASSERT(false, _T("Unknown flags of motion event %d"), flags);
            break;
        }
        if (pinchState & ndk_helper::GESTURE_STATE_START || pinchState & ndk_helper::GESTURE_STATE_MOVE)
        {
            ndk_helper::Vec2 v1;
            ndk_helper::Vec2 v2;
            pHandler->m_pinchDetector.GetPointers( v1, v2 );
            v1 -= v2;
            float fDistance = v1.Length();
            if (BEATS_FLOAT_GREATER(fDistance, 0.f))
            {
                pTouchDelegate->OnPinched((pinchState & ndk_helper::GESTURE_STATE_START) ? EGestureState::eGS_BEGAN : EGestureState::eGS_CHANGED, fDistance);
            }
        }

        return 1;
    }
    return 0;
}

void CAndroidHandler::FillDirectory(SDirectory& directory)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, BEYONDENGINE_HELPER_CLASS_NAME, _T("GetDirectoryData"), _T("(Ljava/lang/String;)[B")))
    {
        jstring str = methodInfo.env->NewStringUTF(directory.m_strPath.c_str());
        jbyteArray jret = (jbyteArray)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, str);
        methodInfo.env->DeleteLocalRef(str);
        jbyte* pData = methodInfo.env->GetByteArrayElements(jret, 0);
        uint32_t uSize = (uint32_t)methodInfo.env->GetArrayLength(jret);
        CSerializer serializer(uSize, pData);
        FillDirectoryWithData(directory, serializer);
        methodInfo.env->ReleaseByteArrayElements(jret, pData, 0);
        methodInfo.env->DeleteLocalRef(jret);
        BEATS_ASSERT(serializer.GetReadPos() == serializer.GetWritePos());
    }
}

const TString& CAndroidHandler::GetAndroidPath(EAndroidPathType type) const
{
    BEATS_ASSERT(type < eAPT_Count);
    return m_strAndroidPath[type];
}

void CAndroidHandler::SetAndroidPath(EAndroidPathType type, const TString& strPath)
{
    BEATS_ASSERT(type < eAPT_Count);
    m_strAndroidPath[type] = CStringHelper::GetInstance()->ToLower(strPath);
}

const TString& CAndroidHandler::GetPackageName()const
{
    return m_strPackageName;
}

void CAndroidHandler::SetPackageName(const TString& strPackageName)
{
    m_strPackageName = strPackageName;
}

void CAndroidHandler::InitVAOFunction()
{
    bool bNoNeedTry = glGenVertexArrays != NULL && glBindVertexArray != NULL && glDeleteVertexArrays != NULL;
    if (!bNoNeedTry)
    {
        bool bTryVAOExtension = false;
        if (glGenVertexArrays == NULL)
        {
            typedef void(*TGenVertexArrayFunc)(GLsizei,GLuint*);
            glGenVertexArrays = (TGenVertexArrayFunc)eglGetProcAddress("glGenVertexArraysOES");
            bTryVAOExtension = glGenVertexArrays != NULL;
        }
        if (glBindVertexArray == NULL)
        {
            typedef void(*TBindVertexArrayFunc)(GLuint);
            glBindVertexArray = (TBindVertexArrayFunc)eglGetProcAddress("glBindVertexArrayOES");
            bTryVAOExtension = bTryVAOExtension && glBindVertexArray != NULL;
        }
        if (glDeleteVertexArrays == NULL)
        {
            typedef void(*TDeleteVertexArrayFunc)(GLsizei, const GLuint*);
            glDeleteVertexArrays = (TDeleteVertexArrayFunc)eglGetProcAddress("glDeleteVertexArraysOES");
            bTryVAOExtension = bTryVAOExtension && glDeleteVertexArrays != NULL;
        }
        if (bTryVAOExtension)
        {
            BEATS_PRINT(_T("VAO is supported by OES!"));
        }
        else
        {
            CConfiguration::GetInstance()->SetSupportShareableVAO(false);
            BEATS_PRINT(_T("VAO is disabled!"));
        }
    }
}

void CAndroidHandler::FillDirectoryWithData(SDirectory& directory, CSerializer& serializer)
{
    unsigned short uStringLength = 0;
    serializer >> uStringLength;
    uStringLength = (unsigned short)ntohs(uStringLength);//Java use big-endian.
    TCHAR szBuffer[MAX_PATH];
    serializer.Deserialize(szBuffer, uStringLength);
    szBuffer[uStringLength] = 0;
    directory.m_strPath = szBuffer;

    uint32_t uFileListCount = 0;
    serializer >> uFileListCount;
    uFileListCount = (uint32_t)ntohl((long)uFileListCount);//Java use big-endian.
    for (uint32_t i = 0; i < uFileListCount; ++i)
    {
        TFileData* pFileData = new TFileData;
        serializer >> uStringLength;
        uStringLength = (uint32_t)ntohs(uStringLength);//Java use big-endian.
        BEATS_ASSERT(uStringLength < MAX_PATH);
        serializer.Deserialize(pFileData->cFileName, uStringLength);
        pFileData->cFileName[uStringLength] = 0;
        AAssetManager* pAssetManager = CFilePathTool::GetInstance()->GetAssetManager();
        TString strFileFullPath = directory.m_strPath;
        if (!strFileFullPath.empty())
        {
            strFileFullPath.append(_T("/"));
        }
        strFileFullPath.append(pFileData->cFileName);
        AAsset* pFileAsset = AAssetManager_open(pAssetManager, strFileFullPath.c_str(), AASSET_MODE_UNKNOWN);
        pFileData->nFileSizeLow = AAsset_getLength(pFileAsset);
        directory.m_data.nFileSizeLow += pFileData->nFileSizeLow;
        AAsset_close(pFileAsset);
        directory.m_pFileList->push_back(pFileData);
    }

    uint32_t uDirectoryListCount = 0;
    serializer >> uDirectoryListCount;
    uDirectoryListCount = (uint32_t)ntohl((long)uDirectoryListCount);//Java use big-endian.
    for (uint32_t i = 0; i < uDirectoryListCount; ++i)
    {
        SDirectory* pNewDirectory = new SDirectory(&directory, nullptr);
        directory.m_pDirectories->push_back(pNewDirectory);
        FillDirectoryWithData(*pNewDirectory, serializer);
        directory.m_data.nFileSizeLow += pNewDirectory->m_data.nFileSizeLow;
    }
}

int CAndroidHandler::GetSDKVersion()
{
    int ret = -1;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, _T("GetSdkVersion"), _T("()I")))
    {
        ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
    }
    return ret;
}

TString CAndroidHandler::GetOsVersion()
{
    TString ret;
    ret = CallJavaStringMethod(ACTIVITY_CLASS_NAME, _T("GenerateOsVersion"), _T("()Ljava/lang/String;"));
    return ret;
}

TString CAndroidHandler::GetModelInfo()
{
    TString ret;
    ret = CallJavaStringMethod(ACTIVITY_CLASS_NAME, _T("GenerateModelInfo"), _T("()Ljava/lang/String;"));
    return ret;
}

void CAndroidHandler::CallJavaVoidMethodParamString(const TString& classPath, const TString& func, const TString& param, const TString& strParam)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, classPath.c_str(), func.c_str(), param.c_str()))
    {
        jstring jstrParam = methodInfo.env->NewStringUTF(strParam.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrParam);
        methodInfo.env->DeleteLocalRef(jstrParam);
    }
}

void CAndroidHandler::CallJavaVoidMethod(const TString& classPath, const TString& func, const TString& param)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, classPath.c_str(), func.c_str(), param.c_str()))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
}

bool CAndroidHandler::CallJavaBooleanMethod(const TString& classPath, const TString& func, const TString& param)
{
    bool ret = false;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, classPath.c_str(), func.c_str(), param.c_str()))
    {
        ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
    }
    return ret;
}

TString CAndroidHandler::CallJavaStringMethod(const TString& classPath, const TString& func, const TString& param)
{
    TString ret;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, classPath.c_str(), func.c_str(), param.c_str()))
    {
        jobject jobj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        ret = JniHelper::jstring2string((jstring)jobj);
        methodInfo.env->DeleteLocalRef(jobj);
    }
    return ret;
}

float CAndroidHandler::CallJavaFloatMethod(const TString& classPath, const TString& func, const TString& param)
{
    float ret = 0;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, classPath.c_str(), func.c_str(), param.c_str()))
    {
        ret = methodInfo.env->CallStaticFloatMethod(methodInfo.classID, methodInfo.methodID);
    }
    return ret;
}

int CAndroidHandler::GetNetState()
{
    int ret = -1;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, _T("GenerateNetInfo"), _T("()I")))
    {
        ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
    }
    return ret;
}

int CAndroidHandler::GetDeviceType()//1:pad 2:phone
{
    int ret = -1;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, _T("GetDeviceType"), _T("()I")))
    {
        ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
    }
    return ret;
}

void CAndroidHandler::SetTextBoxStr(const TString& str)
{
    m_TextBoxStr = str;
}

void CAndroidHandler::RefreshTextBoxStr()
{
    if( !m_TextBoxStr.empty() )
    {
        CIMEManager::GetInstance()->OnCharReplace(m_TextBoxStr.c_str());
        m_TextBoxStr.clear();
    }
}

void CAndroidHandler::ShowLogin()
{
    const TString& strUserName = CPlayerPrefs::GetInstance()->GetString(PlayerPrefGlobalKey[ePPGK_LastLoginUserName]);
    const TString& strPassword = CPlayerPrefs::GetInstance()->GetString(PlayerPrefGlobalKey[ePPGK_LastLoginPassword]);
    CAndroidHandler::GetInstance()->ShowLoginWindow(strUserName, strPassword);
}

void CAndroidHandler::ExtractAndroidAssets()
{
    // Extract some basic file.
    TString strDirectoryAbsolutePath = CAndroidHandler::GetInstance()->GetAndroidPath(eAPT_FilesDirectory);
    strDirectoryAbsolutePath.append("/assets/resource/font");
    BEATS_PRINT("Start extract basic file to %s\n", strDirectoryAbsolutePath.c_str());
    CFilePathTool::GetInstance()->MakeDirectory(strDirectoryAbsolutePath.c_str());
    SDirectory fontDirectory(nullptr, "resource/font");
    CAndroidHandler::GetInstance()->FillDirectory(fontDirectory);
    uint32_t uExtractSize = 0;
    CExtractAndroidAssets::ExtractAssetsFromDirectory(&fontDirectory, uExtractSize);
    BEATS_PRINT("Extract basic file to %s\n", strDirectoryAbsolutePath.c_str());
}

TString CAndroidHandler::GetVersionName()
{
    TString strVersionName;
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, BEYONDENGINE_HELPER_CLASS_NAME, "getVersionName", "()Ljava/lang/String;"))
    {
        jstring strRet = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        strVersionName = JniHelper::jstring2string(strRet);
        methodInfo.env->DeleteLocalRef(strRet);
    }
    return strVersionName;
}