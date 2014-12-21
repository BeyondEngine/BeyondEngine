#ifndef BEYOND_ENGINE_RENDER_REFLECTTEXTUREINFO_H__INCLUDE
#define BEYOND_ENGINE_RENDER_REFLECTTEXTUREINFO_H__INCLUDE

#include "Render/TextureFrag.h"
#define DELAY_LOAD_TEXTURE
struct SReflectTextureInfo
{
    SReflectTextureInfo();
    ~SReflectTextureInfo();

    SReflectTextureInfo& operator = (const SReflectTextureInfo& rhs);
    void SetTextureFrag(SharePtr<CTextureFrag> pFrag);
    bool SetTextureFrag(const TString &strAtlasName, const TString &strFragName);
    SharePtr<CTextureFrag> GetTextureFrag() const;
    void Deserialize(CSerializer *pSerializer, CComponentInstance* pOwner);
    void Serialize(CSerializer *pSerializer) const;
private:
    mutable SharePtr<CTextureFrag> m_pFrag;
#ifdef DELAY_LOAD_TEXTURE
    TString m_strAtlasName;
    TString m_strFragName;
#endif
#ifdef _DEBUG
    TString m_strValue;
#endif
};

template<>
inline void DeserializeVariable(SReflectTextureInfo& value, CSerializer* pSerializer, CComponentInstance* pOwner)
{
    value.Deserialize(pSerializer, pOwner);
}

template<>
inline EReflectPropertyType GetEnumType(SReflectTextureInfo& value, CSerializer* pSerializer)
{
    *pSerializer << eRPT_Texture;
    value.Serialize(pSerializer);
    return eRPT_Texture;
}


#endif