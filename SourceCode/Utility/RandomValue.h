#ifndef BEYOND_ENGINE_RENDER_RANDOMVALUE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RANDOMVALUE_H__INCLUDE
#include "Component/Property/PropertyPublic.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"

enum class ENewRandomValueType : uint8_t
{
    eRVT_Constant,
    eRVT_Curve,
    eRVT_RandomConstant,
    eRVT_RandomCurve,

    eRVT_Count,
};
struct SCurveKey
{
    float m_fValue = 0;
    float m_fInSlope = 0;
    float m_fOutSlope = 0;
};

struct SCurveData
{
    float Evaluate(float fProgress) const;
    void Deserialize(CSerializer* pSerializer);
    void Serialize(CSerializer* pSerializer) const;
    std::map<float, SCurveKey> m_keyList;
private:
    float GetCurveValue(float fProgress, float fStartTime, float fEndTime, const SCurveKey* pStartKey, const SCurveKey* pEndKey) const;
};

class CComponentInstance;
struct SRandomValue
{
public:
    SRandomValue();
    SRandomValue(float fConstantValue);
    SRandomValue(const CVec2& randomRange);
    SRandomValue(const SRandomValue& value);
    ~SRandomValue();
    void Deserialize(CSerializer* pSerializer);
    void Serialize(CSerializer *pSerializer) const;
    float GetValue(float fProgressForCurve, float fRandomFactor) const;
    ENewRandomValueType GetType() const;
    void SetType(ENewRandomValueType type);
    SCurveData& GetMaxCurve();
    SCurveData& GetMinCurve();
    float GetFactor() const;
    void SetFactor(float factor);
    bool IsConstant() const;
private:
    ENewRandomValueType m_type = ENewRandomValueType::eRVT_Constant;
    float m_fFactor = 1.0f;
    SCurveData m_maxCurve;
    SCurveData m_minCurve;
};

template<>
inline void DeserializeVariable(SRandomValue& value, CSerializer* pSerializer, CComponentInstance* /*pOwner*/)
{
    value.Deserialize(pSerializer);
}

template<>
inline EReflectPropertyType GetEnumType(SRandomValue& value, CSerializer* pSerializer)
{
    *pSerializer << eRPT_RandomValue;
    value.Serialize(pSerializer);
    return eRPT_RandomValue;
}

#endif