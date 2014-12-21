#include "stdafx.h"
#include "Mat3.h"
#include "gtc/type_ptr.inl"

CMat3::CMat3()
{

}

CMat3::CMat3(float* pData)
{
    memcpy(Mat3ValuePtr(), pData, sizeof(float) * 9);
}

float* CMat3::Mat3ValuePtr() const
{
    return (float*)glm::value_ptr(m_data);
}

CMat3& CMat3::operator = (const CMat3& rhs)
{
    m_data = rhs.m_data;
    return *this;
}

bool CMat3::IsIdentity() const
{
    return m_data == glm::mat3();
}
void CMat3::Identity()
{
    m_data = glm::mat3();
}