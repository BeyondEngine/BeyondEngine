#include "stdafx.h"
#include "DisplayData.h"

using namespace _2DSkeletalAnimation;

CDisplayData::CDisplayData()
    : m_fX(0.f)
    , m_fY(0.f)
    , m_fScaleX(1.f)
    , m_fScaleY(1.f)
    , m_fSkewX(0.f)
    , m_fSkewY(0.f)
    , m_eType(eST_CS_DISPLAY_SPRITE)
    , m_bChanged(false)
    , m_tstrName(_T("ax2"))
    , m_strName("ax2")
{

}

CDisplayData::~CDisplayData()
{

}

bool CDisplayData::CheckChange()
{
    bool bRet = false;
    if(m_bChanged && !m_tstrName.empty())
    {
        bRet = true;
        m_bChanged = false;
    }
    return bRet;
}

void CDisplayData::SetChange(bool bChange)
{
    m_bChanged = bChange;
}

void CDisplayData::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_tstrName, true, 0xFFFFFFFF, _T("名称"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_eType, true, 0xFFFFFFFF, _T("类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fX, true, 0xFFFFFFFF, _T("X坐标"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fY, true, 0xFFFFFFFF, _T("Y坐标"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fScaleX, true, 0xFFFFFFFF, _T("沿X轴缩放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fScaleY, true, 0xFFFFFFFF, _T("沿Y轴缩放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fSkewX, true, 0xFFFFFFFF, _T("沿X抽旋转"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fSkewY, true, 0xFFFFFFFF, _T("沿Y轴旋转"), NULL, NULL, NULL);
}

bool CDisplayData::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bRet)
    {
        if(pVariableAddr == &m_tstrName)
        {
            DeserializeVariable(m_tstrName, pSerializer);
            size_t len = m_tstrName.size() + 1;
            char *displayName = new char[len];
            CStringHelper::GetInstance()->ConvertToCHAR(m_tstrName.c_str(), displayName, len);
            m_strName = displayName;
            BEATS_SAFE_DELETE_ARRAY(displayName);
            bRet = true;
            m_bChanged = true;
        }
        else if(pVariableAddr == &m_fX)
        {
            DeserializeVariable(m_fX, pSerializer);
            bRet = true;
            m_bChanged = true;
        }
        else if(pVariableAddr == &m_fY)
        {
            DeserializeVariable(m_fY, pSerializer);
            bRet = true;
            m_bChanged = true;
        }
        else if(pVariableAddr == &m_fScaleX)
        {
            DeserializeVariable(m_fScaleX, pSerializer);
            bRet = true;
            m_bChanged = true;
        }
        else if(pVariableAddr == &m_fScaleY)
        {
            DeserializeVariable(m_fScaleY, pSerializer);
            bRet = true;
            m_bChanged = true;
        }
        else if(pVariableAddr == &m_fSkewX)
        {
            DeserializeVariable(m_fSkewX, pSerializer);
            bRet = true;
            m_bChanged = true;
        }
        else if(pVariableAddr == &m_fSkewY)
        {
            DeserializeVariable(m_fSkewY, pSerializer);
            bRet = true;
            m_bChanged = true;
        }
    }
    return bRet;
}

EDisplayType CDisplayData::GetType() const
{
    return m_eType;
}

void CDisplayData::SetType(EDisplayType type)
{
    m_eType = type;
}

const std::string& CDisplayData::GetName() const
{
    return m_strName;
}

void CDisplayData::SetName(const std::string& name)
{
    m_strName = name;
}

float CDisplayData::GetPositionX() const
{
    return m_fX;
}

void CDisplayData::SetPositionX(float x)
{
    m_fX = x;
}

float CDisplayData::GetPositionY() const
{
    return m_fY;
}

void CDisplayData::SetPositionY(float y)
{
    m_fY = y;
}

float CDisplayData::GetSkewX() const
{
    return m_fSkewX;
}

void CDisplayData::SetSkewX(float skewX)
{
    m_fSkewX = skewX;
}

float CDisplayData::GetSkewY() const
{
    return m_fSkewY;
}

void CDisplayData::SetSkewY(float skewY)
{
    m_fSkewY = skewY;
}

float CDisplayData::GetScaleX() const
{
    return m_fScaleX;
}

void CDisplayData::SetScaleX(float scaleX)
{
    m_fScaleX = scaleX;
}

float CDisplayData::GetScaleY() const
{
    return m_fScaleY;
}

void CDisplayData::SetScaleY(float scaleY)
{
    m_fScaleY = scaleY;
}
