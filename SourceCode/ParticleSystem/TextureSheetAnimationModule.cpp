#include "stdafx.h"
#include "TextureSheetAnimationModule.h"

CTextureSheetAnimationModule::CTextureSheetAnimationModule()
{

}

CTextureSheetAnimationModule::~CTextureSheetAnimationModule()
{

}

void CTextureSheetAnimationModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);

    DECLARE_PROPERTY(serializer, m_uRow, true, 0xFFFFFFFF, "����", nullptr, "ָ��������һ���ж�����", "MinValue:1");
    DECLARE_PROPERTY(serializer, m_uCol, true, 0xFFFFFFFF, "����", nullptr, "ָ��������һ���ж�����", "MinValue:1");
    DECLARE_PROPERTY(serializer, m_bWholeSheetOrSingleRow, true, 0xFFFFFFFF, "���岥�Ż��ߵ��в���", nullptr, "���Ϊtrue�������岥�ţ�����ֻ����һ��", nullptr);
    DECLARE_PROPERTY(serializer, m_bRandomRow, true, 0xFFFFFFFF, "�����", nullptr, "���ѡ��һ�в���", "VisibleWhen:m_bWholeSheetOrSingleRow==false");
    DECLARE_PROPERTY(serializer, m_uRowIndex, true, 0xFFFFFFFF, "ָ����", nullptr, "����ָ�����У�����С������", "VisibleWhen:m_bWholeSheetOrSingleRow==false && m_bRandomRow==false, MinValue:0");
    DECLARE_PROPERTY(serializer, m_frameOverTime, true, 0xFFFFFFFF, "֡���������ڵ�ӳ��", nullptr, "ָ��������������Ӧ�ò�����һ֡", nullptr);
    DECLARE_PROPERTY(serializer, m_uCycles, true, 0xFFFFFFFF, "���Ŵ���", nullptr, "�����������в��Ŷ��ٴ�", "MinValue:1");
}

uint32_t CTextureSheetAnimationModule::GetRow() const
{
    return m_uRow;
}

uint32_t CTextureSheetAnimationModule::GetCol() const
{
    return m_uCol;
}

bool CTextureSheetAnimationModule::IsWholeSheetOrSingleRow() const
{
    return m_bWholeSheetOrSingleRow;
}

bool CTextureSheetAnimationModule::IsRandomRow() const
{
    return m_bRandomRow;
}

uint32_t CTextureSheetAnimationModule::GetSpecificRow() const
{
    return m_uRowIndex;
}

const SRandomValue& CTextureSheetAnimationModule::GetFrame() const
{
    return m_frameOverTime;
}

uint32_t CTextureSheetAnimationModule::GetLoopTimes() const
{
    return m_uCycles;
}
