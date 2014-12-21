#ifndef BEYOND_ENGINE_RENDER_TEXTUREATLAS_H__INCLUDE
#define BEYOND_ENGINE_RENDER_TEXTUREATLAS_H__INCLUDE

#include "Resource/Resource.h"

class CTexture;
class CTextureFrag;

class CTextureAtlas : public CResource
{
    DECLARE_REFLECT_GUID(CTextureAtlas, 0x3B9CD4E1, CResource)

    DECLARE_RESOURCE_TYPE(eRT_TextureAtlas);
public:
    CTextureAtlas();
    virtual ~CTextureAtlas();

    virtual bool Load() override;
    virtual void Initialize() override;
    virtual bool Unload() override;
    virtual bool ShouldClean() const override;
    SharePtr<CTexture> Texture() const;
    SharePtr<CTextureFrag> GetTextureFrag(const TString &name) const;

#ifdef EDITOR_MODE
    virtual bool NeedReload() const override;
    virtual void Reload() override;
    virtual void LoadCheckList();
    static std::map<TString, std::map<TString, char>> m_fragCheckList; // key is the atlasName, second key is frag name, value indicate 0: un-invoke, 1: invoked, -1: missing
    static std::map<TString, std::set<TString>> CTextureAtlas::m_fragMissingInfo;
#endif

private:
    SharePtr<CTextureFrag> CreateTextureFrag(const TString &name, CVec2 point, CVec2 size);

private:
    SharePtr<CTexture> m_pTexture;
    std::map<TString, SharePtr<CTextureFrag> > m_textureFrags;
};

#endif 
