#include "stdafx.h"
#include "StopMusic.h"
#include "Audio\SimpleAudioEngine.h"

CStopMusic::CStopMusic()
    : m_bStopOrPause(true)
{

}

CStopMusic::~CStopMusic()
{

}

bool CStopMusic::ExecuteImp(SActionContext* /*pContext*/)
{
    if (m_bStopOrPause)
    {
        SimpleAudioEngine::GetInstance()->stopBackgroundMusic(true);
    }
    else
    {
        SimpleAudioEngine::GetInstance()->pauseBackgroundMusic();
    }
    return true;
}

void CStopMusic::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bStopOrPause, true, 0xFFFFFFFF, _T("Õ£÷πªÚ‘›Õ£"), NULL, NULL, NULL);
}
