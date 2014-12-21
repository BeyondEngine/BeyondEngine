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

    DECLARE_PROPERTY(serializer, m_uRow, true, 0xFFFFFFFF, "行数", nullptr, "指定该纹理一共有多少行", "MinValue:1");
    DECLARE_PROPERTY(serializer, m_uCol, true, 0xFFFFFFFF, "列数", nullptr, "指定该纹理一共有多少列", "MinValue:1");
    DECLARE_PROPERTY(serializer, m_bWholeSheetOrSingleRow, true, 0xFFFFFFFF, "整体播放或者单行播放", nullptr, "如果为true，则整体播放，否则只播放一行", nullptr);
    DECLARE_PROPERTY(serializer, m_bRandomRow, true, 0xFFFFFFFF, "随机行", nullptr, "随机选择一行播放", "VisibleWhen:m_bWholeSheetOrSingleRow==false");
    DECLARE_PROPERTY(serializer, m_uRowIndex, true, 0xFFFFFFFF, "指定行", nullptr, "播放指定的行，必须小于行数", "VisibleWhen:m_bWholeSheetOrSingleRow==false && m_bRandomRow==false, MinValue:0");
    DECLARE_PROPERTY(serializer, m_frameOverTime, true, 0xFFFFFFFF, "帧与生命周期的映射", nullptr, "指定在生命周期中应该播放哪一帧", nullptr);
    DECLARE_PROPERTY(serializer, m_uCycles, true, 0xFFFFFFFF, "播放次数", nullptr, "在生命周期中播放多少次", "MinValue:1");
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
