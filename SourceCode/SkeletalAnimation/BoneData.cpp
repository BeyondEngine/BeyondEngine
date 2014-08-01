#include "stdafx.h"
#include "BoneData.h"
#include "DisplayData.h"
using namespace _2DSkeletalAnimation;

CBoneData::CBoneData()
{

}
CBoneData::~CBoneData()
{

}

const std::string& CBoneData::GetName() const
{
    return m_strName;
}

void  CBoneData::SetName(const std::string& name)
{
    m_strName = name;
}

const std::string& CBoneData::GetParentName() const
{
    return m_parentName;
}

void  CBoneData::SetParentName(const std::string& name)
{
    m_parentName = name;
}

float CBoneData::GetPositionX() const
{
    return m_fX;
}

void CBoneData::SetPositionX(float x)
{
    m_fX = x;
}

float CBoneData::GetPositionY() const
{
    return m_fY;
}

void CBoneData::SetPositionY(float y)
{
    m_fY = y;
}

float CBoneData::GetSkewX() const
{
    return m_fSkewX;
}

void CBoneData::SetSkewX(float skewX)
{
    m_fSkewX = skewX;
}

float CBoneData::GetSkewY() const
{
    return m_fSkewY;
}

void CBoneData::SetSkewY(float skewY)
{
    m_fSkewY = skewY;
}

float CBoneData::GetScaleX() const
{
    return m_fScaleX;
}

void CBoneData::SetScaleX(float scaleX)
{
    m_fScaleX = scaleX;
}

float CBoneData::GetScaleY() const
{
    return m_fScaleY;
}

void CBoneData::SetScaleY(float scaleY)
{
    m_fScaleY = scaleY;
}

int CBoneData::GetZOrder() const
{
    return m_iZOrder;
}

void CBoneData::SetZOrder(int zOrder)
{
    m_iZOrder = zOrder;
}

const std::vector<CDisplayData>& CBoneData::GetDisplayDataList() const
{
    return m_displayDataList;
}

void CBoneData::AddDisplayData(const CDisplayData& data)
{
    m_displayDataList.push_back(data);
}

CDisplayData CBoneData::ReplaceDisplayData(const CDisplayData& displayData, size_t index /*= 0*/)
{
    BEATS_ASSERT(index < m_displayDataList.size());
    CDisplayData oldDisplayData = m_displayDataList[index];
    m_displayDataList.erase(m_displayDataList.begin() + index);
    m_displayDataList.insert(m_displayDataList.begin() + index, displayData);
    return oldDisplayData;
}
