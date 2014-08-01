#ifndef FRAMEWORK_ANDROID_ANDROIDHANDLER_H__INCLUDE
#define FRAMEWORK_ANDROID_ANDROIDHANDLER_H__INCLUDE

namespace ndk_helper
{
    class GLContext;
    class DoubletapDetector;
    class PinchDetector;
    class DragDetector;
    class Vec2;
}
struct android_app;
class ASensorManager;
class ASensor;
class ASensorEventQueue;
class AInputEvent;
class CAndroidHandler
{
    BEATS_DECLARE_SINGLETON(CAndroidHandler);

public:
    bool Initialize(android_app* pApp);
    bool IsFocus() const;
    // SensorStuff
    void InitSensors();
    void ProcessSensors( int32_t id );
    void SuspendSensors();
    void ResumeSensors();

    void TransformPosition( ndk_helper::Vec2& vec );

    ndk_helper::GLContext* GetGLContext() const;
    static void HandleCmd( struct android_app* app, int32_t cmd );
    static int32_t HandleInput( android_app* app, AInputEvent* event );

private:
    void InitVAOFunction();

private:
    bool m_bHasFocus;
    ndk_helper::GLContext* m_glContext;
    android_app* m_pApp;
    ASensorManager* m_pSensorManager;
    const ASensor* m_pAccelerometerSensor;
    ASensorEventQueue* m_pSensorEventQueue;
    ndk_helper::DoubletapDetector m_doubleTapDetector;
    ndk_helper::PinchDetector m_pinchDetector;
};

#endif