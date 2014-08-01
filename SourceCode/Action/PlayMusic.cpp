#include "stdafx.h"
#include "PlayMusic.h"
#include "Audio\SimpleAudioEngine.h"
#include "Task\SwitchMusic.h"
#include "Task\TaskManager.h"

CPlayMusic::CPlayMusic()
    : m_bLoop(false)
    , m_bRewind(false)
    , m_fVolume(1.0f)
    , m_uFadeInTimeMS(0)
{

}

CPlayMusic::~CPlayMusic()
{

}

size_t CPlayMusic::GetFadeInTime() const
{
    return m_uFadeInTimeMS;
}

void CPlayMusic::SetFadeInTime(size_t uTimeMS)
{
    m_uFadeInTimeMS = uTimeMS;
}

float CPlayMusic::GetVolume() const
{
    return m_fVolume;
}

void CPlayMusic::SetVolume(float fVolume)
{
    m_fVolume = fVolume;
}

void CPlayMusic::Initialize()
{
    super::Initialize();
    // HACK: Preload will cause current playing music stop! so I comment here to find a better solution!
    /*if (!m_strFileName.empty())
    {
    SimpleAudioEngine::GetInstance()->preloadBackgroundMusic(m_strFileName.c_str());
    }*/
}

bool CPlayMusic::ExecuteImp(SActionContext* /*pContext*/)
{
    BEATS_ASSERT(!m_strFileName.empty());
    if (!m_strFileName.empty())
    {
        const TString& strCurPlayingMusic = CEngineCenter::GetInstance()->GetCurPlayingMusic();
        if (strCurPlayingMusic.compare(m_strFileName) != 0)
        {
            if (m_uFadeInTimeMS != 0)
            {
                SharePtr<CSwitchMusic> pSwitchTask = new CSwitchMusic();
                pSwitchTask->SetInvoker(this);
                CTaskManager::GetInstance()->AddTask(pSwitchTask, false);
            }
            else
            {
                if (m_bRewind)
                {
                    SimpleAudioEngine::GetInstance()->rewindBackgroundMusic();
                }
                SimpleAudioEngine::GetInstance()->setBackgroundMusicVolume(m_fVolume);
                SimpleAudioEngine::GetInstance()->playBackgroundMusic(m_strFileName.c_str(), m_bLoop);
                CEngineCenter::GetInstance()->SetCurPlayingMusic(m_strFileName);
            }
        }
    }
    return true;
}

void CPlayMusic::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bLoop, true, 0xFFFFFFFF, _T("是否循环"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bRewind, true, 0xFFFFFFFF, _T("从头播放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fVolume, true, 0xFFFFFFFF, _T("音量"), NULL, NULL, _T("MinValue:0,MaxValue:1"));
    DECLARE_PROPERTY(serializer, m_uFadeInTimeMS, true, 0xFFFFFFFF, _T("淡入时间（毫秒）"), NULL, NULL, _T("MinValue:0"));
    DECLARE_PROPERTY(serializer, m_strFileName, true, 0xFFFFFFFF, _T("文件名"), NULL, NULL, NULL);
}