#ifndef BEYOND_ENGINE_RENDER_RANDOMCOLOR_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RANDOMCOLOR_H__INCLUDE
#include "Component/Property/PropertyPublic.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"

enum class ERandomColorType
{
    eRCT_ConstantColor,
    eRCT_RandomColor,
    eRCT_ConstantSpline,
    eRCT_RandomSpline,

    eRCT_Count,
};

static const TCHAR* pszRandomColorTypeStr[] =
{
    "Color",
    "Random Color",
    "Gradient",
    "Random Gradient",
};

class CRandomColor
{
public:
    CRandomColor();
    ~CRandomColor();
    ERandomColorType m_type = ERandomColorType::eRCT_ConstantColor;
    CColorSpline m_minColorSpline;
    CColorSpline m_maxColorSpline;
    void Serialize(CSerializer* pSerializer);
    CColor GetColor(float fProgress, float fRandomRate) const;
};

template<>
inline void DeserializeVariable(CRandomColor& value, CSerializer* pSerializer, CComponentInstance* pOwner)
{
    (*pSerializer) >> value.m_type;
    switch (value.m_type)
    {
    case ERandomColorType::eRCT_ConstantColor:
        (*pSerializer) >> value.m_maxColorSpline.m_mapColors[0];
        break;
    case ERandomColorType::eRCT_RandomColor:
        (*pSerializer) >> value.m_maxColorSpline.m_mapColors[0];
        (*pSerializer) >> value.m_minColorSpline.m_mapColors[0];
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        DeserializeVariable(value.m_maxColorSpline, pSerializer, pOwner);
        break;
    case ERandomColorType::eRCT_RandomSpline:
        DeserializeVariable(value.m_maxColorSpline, pSerializer, pOwner);
        DeserializeVariable(value.m_minColorSpline, pSerializer, pOwner);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

template<>
inline EReflectPropertyType GetEnumType(CRandomColor& value, CSerializer* pSerializer)
{
    *pSerializer << eRPT_RandomColor;
    value.Serialize(pSerializer);
    return eRPT_RandomColor;
}

#endif