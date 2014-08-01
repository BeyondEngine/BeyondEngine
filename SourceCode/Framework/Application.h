#ifndef BEYOND_ENGINE_FRAMEWORK_APPLICATION_H__INCLUDE
#define BEYOND_ENGINE_FRAMEWORK_APPLICATION_H__INCLUDE

class CApplication
{
public:
    CApplication();
    virtual ~CApplication();

    int Run(void* pData = nullptr);
    bool IsInitialized() const;
    virtual void Initialize();
    virtual void Update();
    virtual void Render();
    virtual void Uninitialize();
    bool ShouldUpdateThisFrame();

    virtual bool Start();
    virtual void Pause();
    virtual void Resume();
    virtual void Stop();
    bool IsRunning() const;

    static CApplication *GetInstance();
    static void Destroy();
    
private:
    bool m_bInitialized;
    bool m_bRunning;
    static CApplication *m_sMainApplication;
};

#endif /* defined(__BEYONDENGINE__Application__) */
