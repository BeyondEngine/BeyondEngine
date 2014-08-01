#ifndef BEYOND_ENGINE_TESECOMPONENT_H__INCLUDE
#define BEYOND_ENGINE_TESECOMPONENT_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "ParticlesSystem/Emitter.h"
#include "Render/ReflectTextureInfo.h"

class CTestAbstractClass : public CComponentInstance
{
    DECLARE_REFLECT_GUID_ABSTRACT(CTestAbstractClass, 0x1A7BB177, CComponentInstance)
public:
    CTestAbstractClass(){};
    ~CTestAbstractClass(){};

    virtual void ReflectData(CSerializer& serializer) override
    {DECLARE_PROPERTY(serializer, a, true, 0xFFFFFFFF, _T("int"), NULL, NULL, NULL);};
protected:
    virtual void fun() = 0;
private:
    int a;
};

class CTestComponent : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CTestComponent, 0x7B1A17B7, CComponentInstance)
public:
    enum ETestEnum
    {
        eTE_ENUM_1,
        eTE_ENUM_2,
        eTE_ENUM_3,
        eTE_ENUM_4
    };

public:
    CTestComponent();
    ~CTestComponent();
    virtual void ReflectData(CSerializer& serializer) override;

private:
    ETestEnum m_enum;
    bool m_bool;
    TString m_str;
    CColor m_colour;
    double m_double;
    int m_int;
    unsigned int m_uint;
    float m_float;
    CVec2 m_vec2;
    CVec3 m_vec3;
    CVec4 m_vec4;
    SReflectFilePath m_filePath;
    SReflectTextureInfo m_texture;
    std::map<int, int> m_map;
    std::vector<int> m_vector;
    CTestComponent* m_ptr;
    CTestAbstractClass* m_abstractptr;
};

#endif