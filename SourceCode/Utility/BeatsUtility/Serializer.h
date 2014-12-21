#ifndef BEATS_UTILITY_SERIALIZER_SERIALISZER_H__INCLUDE
#define BEATS_UTILITY_SERIALIZER_SERIALISZER_H__INCLUDE

static const uint32_t DefaultSerializerBufferSize = 256;
struct SBufferData
{
    const void* pData;
    uint32_t dataLength;
};

class CSerializer
{
public:
    CSerializer(uint32_t size = 0, void* data = NULL);
    CSerializer(const CSerializer& rhs);
    CSerializer(const TCHAR* pFilePath, const TCHAR* pszMode = _T("rb"), uint32_t uStartPos = 0, uint32_t uDataLength = 0);

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

    void SetReadPos(uint32_t offset);
    void SetWritePos(uint32_t offset);
    uint32_t GetReadPos() const;
    uint32_t GetWritePos() const;
    void* GetReadPtr() const;
    void* GetWritePtr() const;
    uint32_t GetBufferSize() const;
    void Reset();
    const unsigned char* GetBuffer() const;
    void SetBuffer(const unsigned char* buffer, uint32_t uBufferLength, bool bReleaseOldBuffer = true);

    void Serialize(const void* pData, uint32_t size);
    bool Serialize(const TCHAR* pFilePath, const TCHAR* pszMode = _T("rb"), uint32_t uStartPos = 0, uint32_t uDataLength = 0);
    bool Serialize(FILE* pFile, const TString& strFilePath, uint32_t startPos = 0, uint32_t dataLength = 0);
    void Serialize(CSerializer& serializer, uint32_t uDataSize = 0xFFFFFFFF);

    void RewriteData(uint32_t uPos, void* pData, uint32_t uDataSize);

    uint32_t Deserialize(void* pData, uint32_t size);
    bool Deserialize(const TCHAR* pFilePath, const TCHAR* pszMode = _T("wb+"));
    void Deserialize(CSerializer& serializer, uint32_t uDataSize = 0xFFFFFFFF);

    uint32_t ReadToData(const SBufferData& data, bool bReverse = false);
    uint32_t ReadToDataInList(const std::vector<SBufferData>& dataList, bool bReverse = false, int* pMatchDataId = NULL );

    void IncreaseBufferSize();
    void ValidateBuffer(uint32_t size, const TCHAR* pszDetail = nullptr);
    static void UnitTest();

    // You can attach any user data along with this serializer, you have to manage this pointer memory by yourself.
    void* GetUserData();
    void SetUserData(void* pData);
    void Release();

private:
    void Create(uint32_t size = DefaultSerializerBufferSize, void* data = NULL);
    void SerializeCharPtr(const char* data);
    void SerializeWcharPtr(const wchar_t* data);

private:
    uint32_t m_size;
    void* m_pBuffer;
    void* m_pReadPtr;
    void* m_pWritePtr;
    void* m_pUserData;
};

#include "Serializer.hpp"

#endif