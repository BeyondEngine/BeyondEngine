#include "stdafx.h"
#include "SwitchMusic.h"
#include "Action\PlayMusic.h"
#include "Audio\SimpleAudioEngine.h"

CSwitchMusic::CSwitchMusic()
    : m_bInFadeOutPreMusicState(false)
    , m_pInvoker(NULL)
    , m_uElapsedTimeMS(0)
    , m_uFadeInElapsedTimeMS(0)
    , m_fFadeOutStartVolume(0)
    , m_uProgress(0)
{

}

CSwitchMusic::~CSwitchMusic()
{
    BEATS_SAFE_DELETE(m_pInvoker);
}

void CSwitchMusic::SetInvoker(CPlayMusic* pInvoker)
{
    BEATS_ASSERT(m_pInvoker == NULL && pInvoker != NULL);
    m_pInvoker = (CPlayMusic*)pInvoker->CloneInstance();
}

size_t CSwitchMusic::GetProgress()
{
    return m_uProgress;
}

void CSwitchMusic::Execute(float ddt)
{
    BEATS_ASSERT(m_pInvoker != NULL);
    if (m_uProgress < 100)
    {
        if (m_uElapsedTimeMS == 0)
        {
            m_bInFadeOutPreMusicState = SimpleAudioEngine::GetInstance()->isBackgroundMusicPlaying();
            m_fFadeOutStartVolume = SimpleAudioEngine::GetInstance()->getBackgroundMusicVolume();
        }
        m_uElapsedTimeMS += (size_t)(ddt * 1000);
        if (m_bInFadeOutPreMusicState)
        {
            float fCurVolume = 0.0f;
            if (m_uElapsedTimeMS < m_pInvoker->GetFadeInTime())
            {
                float fProgress = (float)m_uElapsedTimeMS / m_pInvoker->GetFadeInTime();
                fCurVolume = (1.0f - fProgress) * m_fFadeOutStartVolume;
                m_uProgress = (size_t)(fProgress * 50);
                SimpleAudioEngine::GetInstance()->setBackgroundMusicVolume(fCurVolume);
            }
            else
            {
                m_bInFadeOutPreMusicState = false;
                m_uProgress = 50;
            }
        }
        else
        {
            bool bStartFadeIn = m_uFadeInElapsedTimeMS == 0;
            m_uFadeInElapsedTimeMS += (size_t)(ddt * 1000);
            float fCurVolume = m_pInvoker->GetVolume();
            size_t uFadeInTime = m_pInvoker->GetFadeInTime();
            if (bStartFadeIn)
            {
                m_pInvoker->SetVolume(0);
                m_pInvoker->SetFadeInTime(0);// Set fade in time to 0, so the invoker won't recursive start.
                m_pInvoker->ExecuteImp(NULL);
                m_pInvoker->SetVolume(fCurVolume);
                m_pInvoker->SetFadeInTime(uFadeInTime);//Restore
            }
            else
            {
                if (m_uFadeInElapsedTimeMS < uFadeInTime)
                {
                    fCurVolume = (float)m_uFadeInElapsedTimeMS / uFadeInTime * m_pInvoker->GetVolume();
                    m_uProgress = (size_t)((float)m_uFadeInElapsedTimeMS / uFadeInTime * 50 + 50);
                }
                else
                {
                    m_uProgress = 100;
                }
                SimpleAudioEngine::GetInstance()->setBackgroundMusicVolume(fCurVolume);
            }
        }
    }
}
