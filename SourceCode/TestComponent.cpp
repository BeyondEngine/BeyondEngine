#include "stdafx.h"
#include "TestComponent.h"

void CTestComponent::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_enum, true, 0xFFFFFFFF, _T("枚举"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_str, true, 0xFFFFFFFF, _T("字符串"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bool, true, 0xFFFFFFFF, _T("布尔"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_colour, true, 0xFFFFFFFF, _T("颜色"), NULL, NULL, NULL);

    DECLARE_PROPERTY(serializer, m_double, true, 0xFFFFFFFF, _T("double"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_float, true, 0xFFFFFFFF, _T("float"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_int, true, 0xFFFFFFFF, _T("int"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uint, true, 0xFFFFFFFF, _T("uint"), NULL, NULL, NULL);

    DECLARE_PROPERTY(serializer, m_vector, true, 0xFFFFFFFF, _T("vector"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_map, true, 0xFFFFFFFF, _T("map"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_vec2, true, 0xFFFFFFFF, _T("vec2"), NULL, NULL, NULL); 
    DECLARE_PROPERTY(serializer, m_vec3, true, 0xFFFFFFFF, _T("vec3"), NULL, NULL, NULL);

    DECLARE_PROPERTY(serializer, m_vec4, true, 0xFFFFFFFF, _T("vec4"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_ptr, true, 0xFFFFFFFF, _T("指针"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_abstractptr, true, 0xFFFFFFFF, _T("抽象类指针"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_filePath, true, 0xFFFFFFFF, _T("文件"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_texture, true, 0xFFFFFFFF, _T("纹理"), NULL, NULL, NULL);
}

CTestComponent::CTestComponent():
        m_enum(eTE_ENUM_1),
        m_bool(false),
        m_str(_T("string")),
        m_double(0.0),
        m_int(0),
        m_uint(0),
        m_float(0.0)
{
    kmVec2Fill(&m_vec2, 0.0, 0.0);
    kmVec3Fill(&m_vec3, 0.0, 0.0, 0.0);
    kmVec4Fill(&m_vec4, 0.0, 0.0, 0.0, 0.0);
}

CTestComponent::~CTestComponent()
{

}
