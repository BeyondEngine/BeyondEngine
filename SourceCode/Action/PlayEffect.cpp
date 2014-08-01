#include "stdafx.h"
#include "PlayEffect.h"
#include "Audio\SimpleAudioEngine.h"

CPlayEffect::CPlayEffect()
    : m_bLoop(false)
    , m_uEffectId(0)
    , m_fVolume(1.0f)
    , m_fPan(0)
    , m_fPitch(1.0f)
{

}

CPlayEffect::~CPlayEffect()
{
    SimpleAudioEngine::GetInstance()->stopEffect(m_uEffectId);
}

bool CPlayEffect::ExecuteImp(SActionContext* /*pContext*/)
{
    BEATS_ASSERT(!m_strFileName.empty());
    if (!m_strFileName.empty())
    {
        m_uEffectId = SimpleAudioEngine::GetInstance()->playEffect(m_strFileName.c_str(), m_bLoop, m_fPitch, m_fPan, m_fVolume);
    }
    return true;
}

void CPlayEffect::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bLoop, true, 0xFFFFFFFF, _T("�Ƿ�ѭ��"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fVolume, true, 0xFFFFFFFF, _T("����"), NULL, NULL, _T("MinValue:0,MaxValue:1"));
    DECLARE_PROPERTY(serializer, m_fPan, true, 0xFFFFFFFF, _T("����ƫ��"), NULL, _T("-1������������1����������"), _T("MinValue:-1,MaxValue:1"));
    DECLARE_PROPERTY(serializer, m_fPitch, true, 0xFFFFFFFF, _T("Pitch"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strFileName, true, 0xFFFFFFFF, _T("�ļ���"), NULL, NULL, NULL);
}
