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

    const TString &Name() const;

    SharePtr<CTexture> Texture() const;

    CTextureFrag *GetTextureFrag(const TString &name) const;

    CTextureFrag *CreateTextureFrag(const TString &name, kmVec2 point, kmVec2 size);

private:
    TString m_name;
    SharePtr<CTexture> m_texture;
    std::map<TString, CTextureFrag *> m_textureFrags;
};

#endif 
