#ifndef BEATS_UTILITY_SERIALIZER_SERIALISZER_H__INCLUDE
#define BEATS_UTILITY_SERIALIZER_SERIALISZER_H__INCLUDE

#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
#include <string.h>
#endif

static const size_t DefaultSerializerBufferSize = 256;
struct SBufferData
{
    const void* pData;
    size_t dataLength;
};

class CSerializer
{
public:
    CSerializer(size_t size = DefaultSerializerBufferSize, void* data = NULL);
    CSerializer(const CSerializer& rhs);
    CSerializer(const TCHAR* pFilePath, const TCHAR* pszMode = _T("rb"), size_t uStartPos = 0, size_t uDataLength = 0);
    CSerializer(FILE* pFile, size_t startPos = 0, size_t dataLength = 0);

    ~CSerializer();

    template<typename T>
    CSerializer& operator << (const T& data);
    template<typename T>
    CSerializer& operator >> (T& data);
    template<typename T>
    CSerializer& operator >> (const T& data);
    template<typename T>
    CSerializer& operator << (T* data);
    template<typename T>
    CSerializer& operator >> (T* data);
    template<typename T>
    CSerializer& operator >> (const T* data);


    template<typename T>
    CSerializer& Read(T& pOut);

    CSerializer& operator = (const CSerializer& rhs);

    void SetReadPos(size_t offset);
    void SetWritePos(size_t offset);
    size_t GetReadPos() const;
    size_t GetWritePos() const;
    void* GetReadPtr() const;
    void* GetWritePtr() const;
    size_t GetBufferSize() const;
    void Reset();
    const unsigned char* GetBuffer() const;
    void SetBuffer(const unsigned char* buffer, size_t uBufferLength, bool bReleaseOldBuffer = true);

    void Serialize(const void* pData, size_t size);
    bool Serialize(const TCHAR* pFilePath, const TCHAR* pszMode = _T("rb"), size_t uStartPos = 0, size_t uDataLength = 0);
    bool Serialize(FILE* pFile, size_t startPos = 0, size_t dataLength = 0);
    void Serialize(CSerializer& serializer, size_t uDataSize = 0xFFFFFFFF);

    void RewriteData(size_t uPos, void* pData, size_t uDataSize);

    size_t Deserialize(void* pData, size_t size);
    bool Deserialize(const TCHAR* pFilePath, const TCHAR* pszMode = _T("wb+"));
    void Deserialize(CSerializer& serializer, size_t uDataSize = 0xFFFFFFFF);

    size_t ReadToData(const SBufferData& data, bool bReverse = false);
    size_t ReadToDataInList(const std::vector<SBufferData>& dataList, bool bReverse = false, int* pMatchDataId = NULL );

    void IncreaseBufferSize();
    void ValidateBuffer(size_t size);
    static void UnitTest();

    // You can attach any user data along with this serializer, you have to manage this pointer memory by yourself.
    void* GetUserData();
    void SetUserData(void* pData);

private:
    void Create(size_t size = DefaultSerializerBufferSize, void* data = NULL);
    void Release();
    void SerializeCharPtr(const char* data);
    void SerializeWcharPtr(const wchar_t* data);

private:
    size_t m_size;
    void* m_pBuffer;
    void* m_pReadPtr;
    void* m_pWritePtr;
    void* m_pUserData;
};

#include "Serializer.hpp"

#endif