#ifndef BEYOND_ENGINE_TASK_SWITCHMUSIC_H__INCLUDE
#define BEYOND_ENGINE_TASK_SWITCHMUSIC_H__INCLUDE

#include "TaskBase.h"
class CPlayMusic;
class CSwitchMusic : public CTaskBase
{
    DECLARE_REFLECT_GUID(CSwitchMusic, 0x70EA1017, CTaskBase)
public:
    CSwitchMusic();
    virtual ~CSwitchMusic();

    void SetInvoker(CPlayMusic* pInvoker);

    virtual size_t GetProgress() override;
    virtual void Execute(float ddt) override;

private:
    bool m_bInFadeOutPreMusicState;
    CPlayMusic* m_pInvoker;
    size_t m_uElapsedTimeMS;
    size_t m_uFadeInElapsedTimeMS;
    float m_fFadeOutStartVolume;
    size_t m_uProgress;
};

#endif