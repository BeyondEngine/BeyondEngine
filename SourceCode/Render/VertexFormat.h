#ifndef BEYOND_ENGINE_RENDER_VERTEXFORMAT_H__INCLUDE
#define BEYOND_ENGINE_RENDER_VERTEXFORMAT_H__INCLUDE

class CVertexFormat
{
public:
    struct SCommonAttrib
    {
        SCommonAttrib(GLint size, GLenum type, GLboolean normalized, size_t sizeInBytes)
            : size(size)
            , type(type)
            , normalized(normalized)
            , sizeInBytes(sizeInBytes)
        {}

        GLint size;
        GLenum type;
        GLboolean normalized;
        size_t sizeInBytes;
    };
    struct SAttrib
    {
        SAttrib(GLuint index, const SCommonAttrib &common, GLsizei stride, GLsizei offset)
            : index(index)
            , common(common)
            , stride(stride)
            , offset(offset)
        {}

        GLuint index;
        SCommonAttrib common;
        GLsizei stride;
        GLsizei offset;
    };

    bool HasAttrib(GLuint index) const;
    const SAttrib &GetAttrib(GLuint index) const;
    size_t AttribCount() const;
    size_t Size() const;
    void SetupAttribPointer(GLuint vbo) const;
    void DisableAttribPointer() const;

    bool operator == (const CVertexFormat &another) const;
    bool operator != (const CVertexFormat &another) const;
    bool Compatible(const CVertexFormat &another) const;

    template <typename AttribType>
    static const SCommonAttrib &GetCommonAttrib();

    template <typename VertexType>
    static const CVertexFormat &Get();

    static const size_t MAX_VERTEX_SIZE = sizeof(CVertexPTB);

private:
    CVertexFormat();
    CVertexFormat(const CVertexFormat &);
    ~CVertexFormat();

    void AddAttrib(const SAttrib &attrib);
    void SetSize(size_t size);

private:
    std::map<GLuint, SAttrib> m_attribs;    //<index, SAttrib>
    size_t m_size;
};

template <typename AttribType>
inline const CVertexFormat::SCommonAttrib &CVertexFormat::GetCommonAttrib()
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    static_assert(false, "Unsupported attribute type");
#else
    BEATS_ASSERT(false, "Unsupported attribute type");
#endif
    static SCommonAttrib attrib(0, GL_FLOAT, GL_FALSE, 0);
    return attrib;
}

template <>
inline const CVertexFormat::SCommonAttrib &CVertexFormat::GetCommonAttrib<CVec3>()
{
    static SCommonAttrib attrib(3, GL_FLOAT, GL_FALSE, sizeof(CVec3));
    return attrib;
}

template <>
inline const CVertexFormat::SCommonAttrib &CVertexFormat::GetCommonAttrib<CVec4>()
{
    static SCommonAttrib attrib(4, GL_FLOAT, GL_FALSE, sizeof(CVec4));
    return attrib;
}

template <>
inline const CVertexFormat::SCommonAttrib &CVertexFormat::GetCommonAttrib<CIVec4>()
{
    static SCommonAttrib attrib(4, GL_INT, GL_FALSE, sizeof(CIVec4));
    return attrib;
}

template <>
inline const CVertexFormat::SCommonAttrib &CVertexFormat::GetCommonAttrib<CTex>()
{
    static SCommonAttrib attrib(2, GL_FLOAT, GL_FALSE, sizeof(CTex));
    return attrib;
}

template <>
inline const CVertexFormat::SCommonAttrib &CVertexFormat::GetCommonAttrib<CColor>()
{
    static SCommonAttrib attrib(4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(CColor));
    return attrib;
}

#define MAKE_VERTEX_ATTRIB(attribIndex, vertexType, memberName) \
    SAttrib(attribIndex, GetCommonAttrib<decltype(vertexType::memberName)>(), \
        sizeof(vertexType), offsetof(vertexType, memberName))

#define VERTEX_FORMAT(VertexType) CVertexFormat::Get<VertexType>()

template <typename VertexType>
inline const CVertexFormat &CVertexFormat::Get()
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    static_assert(false, "Unsuported vertex type");
#else
    BEATS_ASSERT(false, "Unsupported vertex type");
#endif
    static CVertexFormat format;
    return format;
}

template <>
inline const CVertexFormat &CVertexFormat::Get<CVertexPTCC>()
{
    typedef CVertexPTCC VertexType;
    static CVertexFormat format;
    if(format.AttribCount() == 0)
    {
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_POSITION, VertexType, position));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_TEXCOORD0, VertexType, tex));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_COLOR, VertexType, color));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_COLOR2, VertexType, color2));
        format.SetSize(sizeof(VertexType));
    }
    return format;
}

template <>
inline const CVertexFormat &CVertexFormat::Get<CVertexPTB>()
{
    typedef CVertexPTB VertexType;
    static CVertexFormat format;
    if(format.AttribCount() == 0)
    {
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_POSITION, VertexType, position));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_TEXCOORD0, VertexType, tex));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_BONE_INDICES, VertexType, bones));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_WEIGHTS, VertexType, weights));
        format.SetSize(sizeof(VertexType));
    }
    return format;
}

template <>
inline const CVertexFormat &CVertexFormat::Get<CVertexPTC>()
{
    typedef CVertexPTC VertexType;
    static CVertexFormat format;
    if(format.AttribCount() == 0)
    {
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_POSITION, VertexType, position));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_TEXCOORD0, VertexType, tex));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_COLOR, VertexType, color));
        format.SetSize(sizeof(VertexType));
    }
    return format;
}

template <>
inline const CVertexFormat &CVertexFormat::Get<CVertexPT>()
{
    typedef CVertexPT VertexType;
    static CVertexFormat format;
    if(format.AttribCount() == 0)
    {
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_POSITION, VertexType, position));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_TEXCOORD0, VertexType, tex));
        format.SetSize(sizeof(VertexType));
    }
    return format;
}

template <>
inline const CVertexFormat &CVertexFormat::Get<CVertexPC>()
{
    typedef CVertexPC VertexType;
    static CVertexFormat format;
    if(format.AttribCount() == 0)
    {
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_POSITION, VertexType, position));
        format.AddAttrib(MAKE_VERTEX_ATTRIB(ATTRIB_INDEX_COLOR, VertexType, color));
        format.SetSize(sizeof(VertexType));
    }
    return format;
}

#endif