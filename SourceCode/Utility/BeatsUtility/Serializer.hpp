#ifndef BEATS_UTILITY_SERIALIZER_SERIALISZER_HPP__INCLUDE
#define BEATS_UTILITY_SERIALIZER_SERIALISZER_HPP__INCLUDE

template<typename T>
 CSerializer& CSerializer::operator>>( T& data )
{
    BEATS_ASSERT((static_cast<unsigned char*>(m_pReadPtr) + sizeof(T) <= static_cast<unsigned char*>(m_pWritePtr)), _T("Deserialize overflow!"));
    memcpy(&data, m_pReadPtr, sizeof(T));
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + sizeof(T);
    return (*this);
}

 template<typename T>
 CSerializer& CSerializer::operator >> (const T& /*data*/)
 {
     BEATS_ASSERT(false, _T("Deserialize to a const value is not allowed!"));
     return (*this);
 }

template<typename T>
 CSerializer& CSerializer::operator<<( const T& data )
{
    ValidateBuffer(sizeof(T));
    memcpy(m_pWritePtr, &data, sizeof(T));
    m_pWritePtr = static_cast<unsigned char*>(m_pWritePtr) + sizeof(T);
    return (*this);
}

template<typename T>
CSerializer& CSerializer::operator << (T* /*data*/)
{
     BEATS_ASSERT(false, _T("Not implement yet!"));
     return (*this);
}

template<typename T>
CSerializer& CSerializer::operator >> (T* /*data*/)
{
     BEATS_ASSERT(false, _T("Not implement yet!"));
     return (*this);
}

template<>
inline CSerializer& CSerializer::operator<<( const std::string& str )
{
    const char* data = str.c_str();
    size_t size = ((str.length() + 1) * sizeof(char));
    ValidateBuffer(size);
    memcpy(m_pWritePtr, data, size);
    m_pWritePtr = static_cast<unsigned char*>(m_pWritePtr) + size;
    return (*this);
}

template<>
inline CSerializer& CSerializer::operator>>( std::string& data )
{
    char* pData = static_cast<char*>(m_pReadPtr);
    data.assign(pData);
    size_t size = (data.length() + 1) * sizeof(char);
    BEATS_ASSERT((static_cast<unsigned char*>(m_pReadPtr) + size) <= (static_cast<unsigned char*>(m_pBuffer) + m_size), _T("overflow in reading buffer!"));
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + size;
    return (*this);
}

template<>
inline CSerializer& CSerializer::operator<<( const std::wstring& str )
{
    const wchar_t* data = str.c_str();
    size_t size = ((str.length() + 1) * sizeof(wchar_t));
    ValidateBuffer(size);
    memcpy(m_pWritePtr, data, size);
    m_pWritePtr = static_cast<unsigned char*>(m_pWritePtr) + size;
    return (*this);
}

template<>
inline CSerializer& CSerializer::operator>>( std::wstring& data )
{
    wchar_t* pData = static_cast<wchar_t*>(m_pReadPtr);
    data.assign(pData);
    size_t size = (data.length() + 1) * sizeof(wchar_t);
    BEATS_ASSERT((static_cast<unsigned char*>(m_pReadPtr) + size) <= (static_cast<unsigned char*>(m_pBuffer) + m_size), _T("overflow in reading buffer!"));
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + size;
    return (*this);
}

#define DECLARE_PTR_IMPL(TypeName, FuncName)\
   template<> inline CSerializer& CSerializer::operator << (TypeName data){FuncName(data); return *this;}\
   template<> inline CSerializer& CSerializer::operator << (const TypeName data){FuncName(data); return *this;}

DECLARE_PTR_IMPL(char*, SerializeCharPtr);
DECLARE_PTR_IMPL(wchar_t*, SerializeWcharPtr);

template<>
inline CSerializer& CSerializer::operator >> (char* data)
{
    BEATS_ASSERT(data != NULL, _T("Data pointer is NULL!"));
    size_t size = (strlen(static_cast<char*>(m_pReadPtr)) + 1) * sizeof(char);
    BEATS_ASSERT(static_cast<unsigned char*>(m_pReadPtr) + size <= static_cast<unsigned char*>(m_pWritePtr), _T("Deserialize out of bound!"));
    memcpy(data, m_pReadPtr, size);
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + size;
    return (*this);
}

template<>
inline CSerializer& CSerializer::operator >> (wchar_t* data)
{
    BEATS_ASSERT(data != NULL, _T("Data is null to deserialize."));
    size_t size = (wcslen(static_cast<wchar_t*>(m_pReadPtr)) + 1) * sizeof(wchar_t);
    BEATS_ASSERT(static_cast<unsigned char*>(m_pReadPtr) + size <= static_cast<unsigned char*>(m_pWritePtr), _T("Deserialize out of bound!"));
    memcpy(data, m_pReadPtr, size);
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + size;
    return (*this);
}

//T must be a pointer type, or compile error.
template<typename T>
inline CSerializer& CSerializer::Read(T& pOut)
{
    pOut = (T)m_pReadPtr;
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + sizeof(*pOut);
    return (*this);
}

template<>
inline CSerializer& CSerializer::Read(char**& pOut)
{
    BEATS_ASSERT(pOut != NULL, _T("Out pointer of serializer read is NULL!"));
    *pOut = (char*)m_pReadPtr;
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + sizeof(char) * (strlen(*pOut) + 1);
    return (*this);
}

template<>
inline CSerializer& CSerializer::Read(wchar_t**& pOut)
{
    BEATS_ASSERT(pOut != NULL, _T("Out pointer of serializer read is NULL!"));
    *pOut = (wchar_t*)m_pReadPtr;
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + sizeof(wchar_t) * (wcslen(*pOut) + 1);
    return (*this);
}

#endif