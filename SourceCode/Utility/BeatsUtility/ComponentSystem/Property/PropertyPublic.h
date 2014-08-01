#ifndef BEATS_COMPONENTS_PROPERTY_PROPERTYPUBLIC_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_PROPERTYPUBLIC_H__INCLUDE

enum EReflectPropertyType
{
    eRPT_Invalid,
    eRPT_Bool,
    eRPT_Float,
    eRPT_Double,
    eRPT_Int,
    eRPT_UInt,
    eRPT_Str,
    eRPT_GUID,
    eRPT_List,
    eRPT_Map,
    eRPT_Ptr,
    eRPT_Color,
    eRPT_File,
    eRPT_Enum,
    eRPT_Vec2F,
    eRPT_Vec3F,
    eRPT_Vec4F,
    eRPT_Texture,
    eRPT_GradientColor,

    eRPT_Count,
    eRPT_ForceTo32Bit = 0xFFFFFFFF

};

const static TCHAR* szPropertyTypeStr[] =
{
    _T("ePT_Invalid"),
    _T("ePT_Bool"),
    _T("ePT_Float"),
    _T("ePT_Double"),
    _T("ePT_Int"),
    _T("ePT_UInt"),
    _T("ePT_Str"),
    _T("ePT_GUID"),
    _T("ePT_List"),
    _T("ePT_Map"),
    _T("ePT_Ptr"),
    _T("ePT_Color"),
    _T("ePT_File"),
    _T("ePT_Enum"),
    _T("ePT_Vec2F"),
    _T("ePT_Vec3F"),
    _T("ePT_Vec4F"),

    _T("ePT_Count"),
    _T("ePT_ForceTo32Bit")
};

struct SBasicPropertyInfo
{
    SBasicPropertyInfo(bool bEditable, size_t uColor, const TCHAR* pszDisplayName = NULL, const TCHAR* pszCatalog = NULL, const TCHAR* pszTip = NULL, const TCHAR* pszVariableName = NULL)
        : m_bEditable(bEditable)
        , m_color(uColor)
    {
        if (pszDisplayName)
        {
            m_displayName.assign(pszDisplayName);
        }
        if (pszCatalog)
        {
            m_catalog.assign(pszCatalog);
        }
        if (pszTip)
        {
            m_tip.assign(pszTip);
        }
        if (pszVariableName)
        {
            m_variableName.assign(pszVariableName);
        }
    }
    ~SBasicPropertyInfo()
    {

    }
    SBasicPropertyInfo& operator = (const SBasicPropertyInfo& rRef)
    {
        m_bEditable = rRef.m_bEditable;
        m_color = rRef.m_color;
        m_displayName = rRef.m_displayName;
        m_catalog = rRef.m_catalog;
        m_tip = rRef.m_tip;
        m_variableName = rRef.m_variableName;
        return *this;
    }
    bool m_bEditable;
    size_t m_color;
    TString m_displayName;
    TString m_catalog;
    TString m_tip;
    TString m_variableName; // This member is a key of matching data structure with existing data.
};

enum EUIParameterAttrType
{
    eUIPAT_DefaultValue,
    eUIPAT_MinValue,
    eUIPAT_MaxValue,
    eUIPAT_SpinStep,
    eUIPAT_MaxCount,
    eUIPAT_FixCount,
    eUIPAT_VisibleWhen,
    eUIPAT_EnumStringArray,
    eUIPAT_Count,
    eUIPAT_Force32Bit = 0xffffffff
};

static const TCHAR* UIParameterAttrStr[] = 
{
    _T("DefaultValue"),
    _T("MinValue"),
    _T("MaxValue"),
    _T("SpinStep"),
    _T("MaxCount"),
    _T("FixCount"),
    _T("VisibleWhen"),
    _T("EnumUIString"),
};

enum EValueType
{
    eVT_DefaultValue, //Value comes from code serialization.
    eVT_SavedValue,      //Value comes from current saved data.
    eVT_CurrentValue, //Value comes from editor view.

    eVT_Count,
    eVT_Force32Bit = 0xFFFFFFFF
};
#endif