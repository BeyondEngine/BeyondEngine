#ifndef BEYOND_ENGINE_UTILITY_MATHEXT_MAT3_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_MATHEXT_MAT3_H__INCLUDE

class CMat3
{
public:
    CMat3();
    CMat3(float* pData);
    float* Mat3ValuePtr() const;
    CMat3& operator = (const CMat3& rhs);
    bool IsIdentity() const;
    void Identity();
private:
    glm::mat3 m_data;
};
#endif