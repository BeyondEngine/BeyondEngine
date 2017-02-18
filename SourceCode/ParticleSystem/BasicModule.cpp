#include "stdafx.h"
#include "BasicModule.h"

CBasicModule::CBasicModule()
{

}

CBasicModule::~CBasicModule()
{

}

void CBasicModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_b3D, true, 0xFFFFFFFF, "3D����", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_fDuration, true, 0xFFFFFFFF, "����ʱ�����룩", nullptr, "���������������ʱ�䣬���Ϊѭ�����䣬��ô��ָһ�����ڵ�ʱ��", nullptr);
    DECLARE_PROPERTY(serializer, m_bLoop, true, 0xFFFFFFFF, "ѭ��", nullptr, "���Ϊ�棬��һ����������֮����������", nullptr);
    DECLARE_PROPERTY(serializer, m_bStopWhenDeactive, true, 0xFFFFFFFF, "����ֹͣ", nullptr, nullptr, "VisibleWhen:m_bLoop == true");
    DECLARE_PROPERTY(serializer, m_bPrewarm, true, 0xFFFFFFFF, "Ԥ����", nullptr, "���Ϊ�棬��������ֱ�Ӵ�һ�����������Ժ��״̬��ʼ", "VisibleWhen:m_bLoop==true");
    DECLARE_PROPERTY(serializer, m_fStartDelay, true, 0xFFFFFFFF, "�ӳٷ���ʱ�䣨�룩", nullptr, "��ʼ���������ϵͳ���ȴ���ʱ������������", "VisibleWhen:m_bPrewarm==false");
    DECLARE_PROPERTY(serializer, m_fGravityModifier, true, 0xFFFFFFFF, "��������", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bEnableDepth, true, 0xFFFFFFFF, "������ȼ��", nullptr, nullptr, nullptr);

    DECLARE_PROPERTY(serializer, m_startLifeTime, true, 0xFFFFFFFF, "�����������ڣ��룩", nullptr, "���ӵ��������ڣ�������ʱ������ӽ�����", nullptr);
    DECLARE_PROPERTY(serializer, m_startSpeed, true, 0xFFFFFFFF, "��ʼ����", nullptr, "���ӵĳ�ʼ����", nullptr);
    DECLARE_PROPERTY(serializer, m_startSize, true, 0xFFFFFFFF, "��ʼ�ߴ�", nullptr, "���ӵĳ�ʼ�ߴ�", "MinValue:0");
    DECLARE_PROPERTY(serializer, m_startRotation, true, 0xFFFFFFFF, "��ʼ��ת���Ƕȣ�", nullptr, "���ӵĳ�ʼ��ת", nullptr);
    DECLARE_PROPERTY(serializer, m_startColor, true, 0xFFFFFFFF, "��ʼ��ɫ", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bAttachToEmitter, true, 0xFFFFFFFF, "�ֲ�����", nullptr, "���Ϊ�棬��ʾ�ھֲ��ռ�����ϵ������Ϊ��������ռ�ϵ", nullptr);
    DECLARE_PROPERTY(serializer, m_bPlayOnAwake, true, 0xFFFFFFFF, "�Զ���ʼ����", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_uMaxParticle, true, 0xFFFFFFFF, "�����������", nullptr, "�����ǰ���������������ֵ, ��������ֹͣ����", nullptr);
}

bool CBasicModule::IsParticleAttach() const
{
    return m_bAttachToEmitter;
}

bool CBasicModule::IsLoop() const
{
    return m_bLoop;
}

bool CBasicModule::IsPlayOnAwake() const
{
    return m_bPlayOnAwake;
}

bool CBasicModule::IsStopWhenDeactive() const
{
    return m_bStopWhenDeactive;
}

void CBasicModule::SetDepthTest(bool bEnable)
{
    m_bEnableDepth = bEnable;
}

bool CBasicModule::IsDepthTestEnable() const
{
    return m_bEnableDepth;
}

bool CBasicModule::Is3DEmitter() const
{
    return m_b3D;
}

void CBasicModule::Set3DEmitterFlag(bool b3DOr2D)
{
    m_b3D = b3DOr2D;
}

float CBasicModule::GetDuration() const
{
    return m_fDuration;
}

float CBasicModule::GetStartDelay() const
{
    return m_fStartDelay;
}

float CBasicModule::GetGravityModifier() const
{
    return m_fGravityModifier;
}

uint32_t CBasicModule::GetMaxParticleCount() const
{
    return m_uMaxParticle;
}

const SRandomValue& CBasicModule::GetInitLifeTime() const
{
    return m_startLifeTime;
}

const SRandomValue& CBasicModule::GetInitSpeed() const
{
    return m_startSpeed;
}

const SRandomValue& CBasicModule::GetInitSize() const
{
    return m_startSize;
}

const SRandomValue& CBasicModule::GetInitRotation() const
{
    return m_startRotation;
}

const CRandomColor& CBasicModule::GetInitColor() const
{
    return m_startColor;
}

