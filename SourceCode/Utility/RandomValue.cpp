#include "stdafx.h"
#include "RandomValue.h"

float SCurveData::GetCurveValue(float fProgress, float fStartTime, float fEndTime, const SCurveKey* pStartKey, const SCurveKey* pEndKey) const
{
    BEATS_ASSERT(BEATS_FLOAT_GREATER_EQUAL_EPSILON(fProgress, fStartTime, 0.001f) && BEATS_FLOAT_LESS_EQUAL_EPSILON(fProgress, fEndTime, 0.001f));
    BEATS_ASSERT(BEATS_FLOAT_GREATER_EQUAL_EPSILON(fStartTime, 0, 0.001f) && BEATS_FLOAT_LESS_EQUAL_EPSILON(fEndTime, 1.0f, 0.001f));
    static const float UNITY_YX_RATE = 0.625f; // The curve value depends on the coordinate rate: height / width
    CVec2 startPos(fStartTime, pStartKey->m_fValue * UNITY_YX_RATE);
    CVec2 endPos(fEndTime, pEndKey->m_fValue * UNITY_YX_RATE);
    static const float UNITY_MAGIC_FACTOR = 0.333333f;
    float tangLengthX = fabs(fStartTime - fEndTime) * UNITY_MAGIC_FACTOR;
    float tangLengthY = tangLengthX;
    float tgOut = pStartKey->m_fOutSlope * UNITY_YX_RATE;
    float tgIn = pEndKey->m_fInSlope * UNITY_YX_RATE;
    CVec2 startCtrlPos = startPos;
    CVec2 endCtrlPos = endPos;
    startCtrlPos.X() += tangLengthX;
    startCtrlPos.Y() += tangLengthY * tgOut;
    endCtrlPos.X() -= tangLengthX;
    endCtrlPos.Y() -= tangLengthY * tgIn;
    float s = (fProgress - fStartTime) / (fEndTime - fStartTime);
    float s2 = s * s;
    float s3 = s2 * s;
    float h1 = -s3 + 3 * s2 - 3 * s + 1;
    float h2 = s3;
    float h3 = 3 * s3 - 6 * s2 + 3 * s;
    float h4 = -3 * s3 + 3 * s2;
    CVec2 ret = startPos * h1 + endPos * h2 + startCtrlPos * h3 + endCtrlPos * h4;

    BEATS_ASSERT(fabs(fProgress - ret.X()) < 0.0001f);
    return ret.Y() / UNITY_YX_RATE;
}

float SCurveData::Evaluate(float fProgress) const
{
    float fRet = 0;
    BEATS_CLIP_VALUE(fProgress, 0, 1);
    const SCurveKey* pStartKey = nullptr;
    const SCurveKey* pEndKey = nullptr;
    float fStartTime = 0;
    float fEndTime = 0;
    BEATS_ASSERT(m_keyList.size() > 0);
    for (auto iter = m_keyList.begin(); iter != m_keyList.end(); ++iter)
    {
        if (iter->first >= fProgress)
        {
            pEndKey = &iter->second;
            fEndTime = iter->first;
            break;
        }
        pStartKey = &iter->second;
        fStartTime = iter->first;
    }
    if (pStartKey == nullptr)
    {
        BEATS_ASSERT(pEndKey != nullptr);
        fRet = pEndKey->m_fValue;
    }
    else if (pEndKey == nullptr)
    {
        BEATS_ASSERT(pStartKey != nullptr);
        fRet = pStartKey->m_fValue;
    }
    else
    {
        fRet = GetCurveValue(fProgress, fStartTime, fEndTime, pStartKey, pEndKey);
    }
    return fRet;
}

void SCurveData::Deserialize(CSerializer* pSerializer)
{
    DeserializeVariable(m_keyList, pSerializer, nullptr);
}

void SCurveData::Serialize(CSerializer* pSerializer) const
{
    SerializeVariable(m_keyList, pSerializer);
}

SRandomValue::SRandomValue()
{
}

SRandomValue::SRandomValue(float fConstantValue)
    : m_fFactor(fConstantValue)
    , m_type(ENewRandomValueType::eRVT_Constant)
{
}

SRandomValue::SRandomValue(const CVec2& randomRange)
{
    m_fFactor = randomRange.X();
    m_minCurve.m_keyList[0].m_fValue = 1.0f;
    m_maxCurve.m_keyList[0].m_fValue = randomRange.Y() / m_fFactor;
    m_type = ENewRandomValueType::eRVT_RandomConstant;
}

SRandomValue::SRandomValue(const SRandomValue& value)
    : m_type(value.m_type)
    , m_fFactor(value.m_fFactor)
    , m_maxCurve(value.m_maxCurve)
    , m_minCurve(value.m_minCurve)
{
}

SRandomValue::~SRandomValue()
{

}

void SRandomValue::Deserialize(CSerializer *pSerializer)
{
    (*pSerializer) >> m_type;
    switch (m_type)
    {
    case ENewRandomValueType::eRVT_Constant:
        (*pSerializer) >> m_fFactor;
        break;
    case ENewRandomValueType::eRVT_RandomConstant:
        (*pSerializer) >> m_fFactor;
        (*pSerializer) >> m_minCurve.m_keyList[0].m_fValue;
        (*pSerializer) >> m_maxCurve.m_keyList[0].m_fValue;
        break;
    case ENewRandomValueType::eRVT_Curve:
        (*pSerializer) >> m_fFactor;
        m_maxCurve.Deserialize(pSerializer);
        break;
    case ENewRandomValueType::eRVT_RandomCurve:
        (*pSerializer) >> m_fFactor;
        m_maxCurve.Deserialize(pSerializer);
        m_minCurve.Deserialize(pSerializer);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

void SRandomValue::Serialize(CSerializer *pSerializer) const
{
    (*pSerializer) << m_type;
    switch (m_type)
    {
    case ENewRandomValueType::eRVT_Constant:
        (*pSerializer) << m_fFactor;
        break;
    case ENewRandomValueType::eRVT_RandomConstant:
        (*pSerializer) << m_fFactor;
        (*pSerializer) << m_minCurve.m_keyList.begin()->second.m_fValue;
        (*pSerializer) << m_maxCurve.m_keyList.begin()->second.m_fValue;
        break;
    case ENewRandomValueType::eRVT_Curve:
        (*pSerializer) << m_fFactor;
        m_maxCurve.Serialize(pSerializer);
        break;
    case ENewRandomValueType::eRVT_RandomCurve:
        (*pSerializer) << m_fFactor;
        m_maxCurve.Serialize(pSerializer);
        m_minCurve.Serialize(pSerializer);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

float SRandomValue::GetValue(float fProgressForCurve, float fRandomFactor) const
{
    float fRet = 0;
    BEATS_CLIP_VALUE(fProgressForCurve, 0, 1);
    BEATS_CLIP_VALUE(fRandomFactor, 0, 1);
    switch (m_type)
    {
    case ENewRandomValueType::eRVT_Constant:
        fRet = m_fFactor;
        break;
    case ENewRandomValueType::eRVT_RandomConstant:
    {
        float fMinValue = m_minCurve.m_keyList.begin()->second.m_fValue * m_fFactor;
        float fMaxValue = m_maxCurve.m_keyList.begin()->second.m_fValue * m_fFactor;
        fRet = fMinValue + (fMaxValue - fMinValue) * fRandomFactor;
    }
        break;
    case ENewRandomValueType::eRVT_Curve:
    {
        fRet = m_maxCurve.Evaluate(fProgressForCurve) * m_fFactor;
    }
        break;
    case ENewRandomValueType::eRVT_RandomCurve:
    {
        float fMinValue = m_minCurve.Evaluate(fProgressForCurve) * m_fFactor;
        float fMaxValue = m_maxCurve.Evaluate(fProgressForCurve) * m_fFactor;
        fRet = fMinValue + (fMaxValue - fMinValue) * fRandomFactor;
    }
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
    return fRet;
}

ENewRandomValueType SRandomValue::GetType() const
{
    return m_type;
}

void SRandomValue::SetType(ENewRandomValueType type)
{
    m_type = type;
}

SCurveData& SRandomValue::GetMaxCurve()
{
    return m_maxCurve;
}

SCurveData& SRandomValue::GetMinCurve()
{
    return m_minCurve;
}

float SRandomValue::GetFactor() const
{
    return m_fFactor;
}

void SRandomValue::SetFactor(float factor)
{
    m_fFactor = factor;
}

bool SRandomValue::IsConstant() const
{
    return m_type == ENewRandomValueType::eRVT_Constant || m_type == ENewRandomValueType::eRVT_Curve;
}

