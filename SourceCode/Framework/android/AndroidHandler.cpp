#include "stdafx.h"
#include "AndroidHandler.h"
#include "Event/TouchDelegate.h"
#include "Event/GestureState.h"
#include "Framework/Application.h"
#include "Render/RenderWindow.h"
#include "Render/Viewport.h"
#include "Render/RenderManager.h"
#include "external/Configuration.h"
#include "Framework/android/JniHelper.h"
#include "Audio/SimpleAudioEngine.h"

CAndroidHandler* CAndroidHandler::m_pInstance = NULL;

extern "C"
{
    jint JNI_OnLoad(JavaVM *vm, void *reserved)
    {
        JniHelper::setJavaVM(vm);

        return JNI_VERSION_1_4;
    }

    JNIEXPORT void JNICALL Java_com_sample_Sample_Cocos2dxHelper_nativeSetContext(JNIEnv* env, jobject thiz, jobject context)
    {
        JniHelper::setClassLoaderFrom(context);
    }
};

CAndroidHandler::CAndroidHandler()
    : m_bHasFocus(false)
    , m_glContext (ndk_helper::GLContext::GetInstance())
    , m_pApp(nullptr)
    , m_pSensorManager(nullptr)
    , m_pAccelerometerSensor(nullptr)
    , m_pSensorEventQueue(nullptr)
{

}

CAndroidHandler::~CAndroidHandler()
{

}

bool CAndroidHandler::Initialize(android_app* pApp)
{
    BEATS_ASSERT(pApp != NULL, _T("App can't be null when initialize Android handler!"));
    m_pApp = pApp;
    m_doubleTapDetector.SetConfiguration( m_pApp->config );
    m_pinchDetector.SetConfiguration( m_pApp->config );

    InitSensors();
}

bool CAndroidHandler::IsFocus() const
{
    return m_bHasFocus;
}

void CAndroidHandler::InitSensors()
{
    m_pSensorManager = ASensorManager_getInstance();
    BEATS_ASSERT(m_pSensorManager != NULL, _T("SensorManager can't be null!"));
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
            }
        }
    }
}

void CAndroidHandler::ResumeSensors()
{
    // When our app gains focus, we start monitoring the accelerometer.
    if( m_pAccelerometerSensor != NULL )
    {
        ASensorEventQueue_enableSensor( m_pSensorEventQueue, m_pAccelerometerSensor );
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate( m_pSensorEventQueue, m_pAccelerometerSensor,
            (1000L / 60) * 1000 );
    }
}

void CAndroidHandler::SuspendSensors()
{
    // When our app loses focus, we stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if( m_pAccelerometerSensor != NULL )
    {
        ASensorEventQueue_disableSensor( m_pSensorEventQueue, m_pAccelerometerSensor );
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
    switch( cmd )
    {
    case APP_CMD_SAVE_STATE:
        break;
    case APP_CMD_INIT_WINDOW:
        // The window is being shown, get it ready.
        if( pHandler->m_pApp->window != NULL )
        {
            pHandler->m_glContext->Resume( pHandler->m_pApp->window );
            if (!CApplication::GetInstance()->IsInitialized())
            {
                pHandler->InitVAOFunction();
                int32_t screenWidth = pHandler->GetGLContext()->GetScreenWidth();
                int32_t screenHeight = pHandler->GetGLContext()->GetScreenHeight();
                CRenderWindow* pRenderWindow = new CRenderWindow(screenWidth, screenHeight, true);
                CRenderManager::GetInstance()->SetCurrentRenderTarget(pRenderWindow);
                CApplication::GetInstance()->Initialize();
            }
        }
        break;
    case APP_CMD_TERM_WINDOW:
        pHandler->m_glContext->Suspend();
        SimpleAudioEngine::GetInstance()->end();
        pHandler->m_bHasFocus = false;
        break;
    case APP_CMD_STOP:
        SimpleAudioEngine::GetInstance()->pauseBackgroundMusic();
        break;
    case APP_CMD_START:
        SimpleAudioEngine::GetInstance()->resumeBackgroundMusic();
        break;
    case APP_CMD_GAINED_FOCUS:
        pHandler->ResumeSensors();
        pHandler->m_bHasFocus = true;
        break;
    case APP_CMD_LOST_FOCUS:
        pHandler->SuspendSensors();
        pHandler->m_bHasFocus = false;
        break;
    case APP_CMD_LOW_MEMORY:
        BEATS_ASSERT(false, _T("Memory is too low!"));
        break;
    }
}

int32_t CAndroidHandler::HandleInput( android_app* app, AInputEvent* event )
{
    int32_t nRet = 0;
    CAndroidHandler* pHandler = CAndroidHandler::GetInstance();
    CTouchDelegate* pTouchDelegate = CTouchDelegate::GetInstance();
    if( AInputEvent_getType( event ) == AINPUT_EVENT_TYPE_MOTION )
    {
        ndk_helper::GESTURE_STATE doubleTapState = pHandler->m_doubleTapDetector.Detect(event);
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
                intptr_t pointerId = AMotionEvent_getPointerId( event, 0 );
                float x = AMotionEvent_getX( event, 0 );
                float y = AMotionEvent_getY( event, 0 );
                x /= fScaleFactor;
                y /= fScaleFactor;
                unsigned int nTapCount = doubleTapState == ndk_helper::GESTURE_STATE_ACTION ? 2 : 1;
                pTouchDelegate->OnTouchBegan(1, &pointerId, &x, &y, &nTapCount);
                if (nTapCount == 2)
                {
                    pTouchDelegate->OnTapped(pointerId, x, y, nTapCount);
                }
            }
            break;
        case AMOTION_EVENT_ACTION_UP:
            {
                BEATS_ASSERT(pointerCount == 1, _T("There should be only one pointer in AMOTION_EVENT_ACTION_UP"));
                BEATS_WARNING(index == 0, _T("index should be 0"));
                intptr_t pointerId = AMotionEvent_getPointerId( event, 0 );
                float x = AMotionEvent_getX( event, 0 );
                float y = AMotionEvent_getY( event, 0 );
                x /= fScaleFactor;
                y /= fScaleFactor;
                unsigned int nTapCount = 1;
                pTouchDelegate->OnTouchEnded(1, &pointerId, &x, &y, &nTapCount);
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            {
                BEATS_ASSERT(pointerCount > 1, _T("There should be greater than one pointer in AMOTION_EVENT_ACTION_POINTER_DOWN"));
                intptr_t pointerId = AMotionEvent_getPointerId( event, index );
                float x = AMotionEvent_getX( event, index );
                float y = AMotionEvent_getY( event, index );
                x /= fScaleFactor;
                y /= fScaleFactor;
                unsigned int nTapCount = AKeyEvent_getRepeatCount(event);
                pTouchDelegate->OnTouchBegan(1, &pointerId, &x, &y, &nTapCount);
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
            {
                BEATS_ASSERT(pointerCount > 1, _T("There should be greater than one pointer in AMOTION_EVENT_ACTION_POINTER_DOWN"));
                intptr_t pointerId = AMotionEvent_getPointerId( event, index );
                float x = AMotionEvent_getX( event, index );
                float y = AMotionEvent_getY( event, index );
                x /= fScaleFactor;
                y /= fScaleFactor;
                unsigned int nTapCount = AKeyEvent_getRepeatCount(event);
                pTouchDelegate->OnTouchEnded(1, &pointerId, &x, &y, &nTapCount);
            }
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            {
                intptr_t pointerIds[TOUCH_MAX_NUM] = {0};
                float xs[TOUCH_MAX_NUM] = {0.0f};
                float ys[TOUCH_MAX_NUM] = {0.0f};
                for (size_t i = 0; i < pointerCount && i < TOUCH_MAX_NUM; ++i)
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
                intptr_t pointerIds[TOUCH_MAX_NUM] = {0};
                float xs[TOUCH_MAX_NUM] = {0.0f};
                float ys[TOUCH_MAX_NUM] = {0.0f};
                unsigned int nTapCount = 1;
                for (size_t i = 0; i < pointerCount && i < TOUCH_MAX_NUM; ++i)
                {
                    pointerIds[i] = AMotionEvent_getPointerId( event, i );
                    xs[i] = AMotionEvent_getX(event, i);
                    ys[i] = AMotionEvent_getY(event, i);
                    xs[i] /= fScaleFactor;
                    ys[i] /= fScaleFactor;
                }
                pTouchDelegate->OnTouchCancelled(pointerCount, pointerIds, xs, ys, &nTapCount);
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
            BEATS_ASSERT(!BEATS_FLOAT_EQUAL(fDistance, 0), _T("pinch Distance can't be 0!"));
            pTouchDelegate->OnPinched((pinchState & ndk_helper::GESTURE_STATE_START) ? EGestureState::eGS_BEGAN : EGestureState::eGS_CHANGED, fDistance);
        }

        return 1;
    }
    return 0;
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
