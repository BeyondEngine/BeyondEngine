#include "stdafx.h"
#include "RandomColor.h"

CRandomColor::CRandomColor()
{

}

CRandomColor::~CRandomColor()
{

}

void CRandomColor::Serialize(CSerializer* pSerializer)
{
    (*pSerializer) << m_type;
    switch (m_type)
    {
    case ERandomColorType::eRCT_ConstantColor:
        m_maxColorSpline.m_mapColors[0] = 0xFFFFFFFF;
        (*pSerializer) << m_maxColorSpline.m_mapColors[0];
        break;
    case ERandomColorType::eRCT_RandomColor:
        (*pSerializer) << m_maxColorSpline.m_mapColors[0];
        (*pSerializer) << m_minColorSpline.m_mapColors[0];
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        (*pSerializer) << m_maxColorSpline.m_mapColors.size();
        for (auto iter = m_maxColorSpline.m_mapColors.begin(); iter != m_maxColorSpline.m_mapColors.end(); ++iter)
        {
            (*pSerializer) << iter->first;
            (*pSerializer) << iter->second;
        }
        break;
    case ERandomColorType::eRCT_RandomSpline:
        (*pSerializer) << m_maxColorSpline.m_mapColors.size();
        for (auto iter = m_maxColorSpline.m_mapColors.begin(); iter != m_maxColorSpline.m_mapColors.end(); ++iter)
        {
            (*pSerializer) << iter->first;
            (*pSerializer) << iter->second;
        }
        (*pSerializer) << m_minColorSpline.m_mapColors.size();
        for (auto iter = m_minColorSpline.m_mapColors.begin(); iter != m_minColorSpline.m_mapColors.end(); ++iter)
        {
            (*pSerializer) << iter->first;
            (*pSerializer) << iter->second;
        }
        break;
    default:
        break;
    }
}

CColor CRandomColor::GetColor(float fProgress, float fRandomRate) const
{
    CColor ret;
    switch (m_type)
    {
    case ERandomColorType::eRCT_ConstantColor:
        BEATS_ASSERT(m_maxColorSpline.m_mapColors.size() > 0);
        ret = m_maxColorSpline.m_mapColors.begin()->second;
        break;
    case ERandomColorType::eRCT_RandomColor:
    {
        BEATS_ASSERT(m_minColorSpline.m_mapColors.size() > 0);
        BEATS_ASSERT(m_maxColorSpline.m_mapColors.size() > 0);
        CColor minColor = m_minColorSpline.m_mapColors.begin()->second;
        CColor maxColor = m_maxColorSpline.m_mapColors.begin()->second;
        ret = minColor.Interpolate(maxColor, fRandomRate);
    }
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        ret = m_maxColorSpline.GetValue(fProgress);
        break;
    case ERandomColorType::eRCT_RandomSpline:
    {
        CColor minColor = m_minColorSpline.GetValue(fProgress);
        CColor maxColor = m_maxColorSpline.GetValue(fProgress);
        ret = minColor.Interpolate(maxColor, fRandomRate);
    }
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
    return ret;
}

