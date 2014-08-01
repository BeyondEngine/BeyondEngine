#include "stdafx.h"
#include "StopEffect.h"
#include "Audio\SimpleAudioEngine.h"

CStopEffect::CStopEffect()
    : m_uEffectId(0xFFFFFFFF)
{

}

CStopEffect::~CStopEffect()
{

}

bool CStopEffect::ExecuteImp(SActionContext* /*pContext*/)
{
    if (m_uEffectId == 0xFFFFFFFF)
    {
        if (m_bStopOrPause)
        {
            SimpleAudioEngine::GetInstance()->stopAllEffects();
        }
        else
        {
            SimpleAudioEngine::GetInstance()->pauseAllEffects();
        }
    }
    else
    {
        if (m_bStopOrPause)
        {
            SimpleAudioEngine::GetInstance()->stopEffect(m_uEffectId);
        }
        else
        {
            SimpleAudioEngine::GetInstance()->pauseEffect(m_uEffectId);
        }
    }
    return true;
}

void CStopEffect::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bStopOrPause, true, 0xFFFFFFFF, _T("Í£Ö¹»òÔÝÍ£"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uEffectId, true, 0xFFFFFFFF, _T("ÉùÐ§ID"), NULL, NULL, NULL);
}

