#ifndef BEYOND_ENGINE_RENDER_REFLECTTEXTUREINFO_H__INCLUDE
#define BEYOND_ENGINE_RENDER_REFLECTTEXTUREINFO_H__INCLUDE

class CTextureFrag;
class CTextureAtlas;
struct SReflectTextureInfo
{
    SReflectTextureInfo();
    ~SReflectTextureInfo();
    SReflectTextureInfo( const SReflectTextureInfo& other );

    void SetTextureFrag(const TString &strAtlasName, const TString &strFragName);
    CTextureFrag *GetTextureFrag() const;
    SharePtr<CTextureAtlas> GetAtlas() const;
    void Deserialize(CSerializer *pSerializer);
    void Serialize(CSerializer *pSerializer) const;

    CTextureFrag *m_pFrag;
#ifdef _DEBUG
    TString m_strValue;
#endif
};

template<>
inline void DeserializeVariable(SReflectTextureInfo& value, CSerializer* pSerializer)
{
    value.Deserialize(pSerializer);
}

template<>
inline EReflectPropertyType GetEnumType(SReflectTextureInfo& value, CSerializer* pSerializer)
{
    *pSerializer << eRPT_Texture;
    value.Serialize(pSerializer);
    return eRPT_Texture;
}


#endif