#ifndef BEYOND_ENGINE_GUI_FONT_FONT_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_FONT_H__INCLUDE

#include "Resource/Resource.h"
#include "Resource/ResourcePublic.h"

class CFont : public CResource
{
    DECLARE_REFLECT_GUID(CFont, 0xC32B11CB, CResource)
    DECLARE_RESOURCE_TYPE(eRT_Font);
public:
    CFont();
    virtual ~CFont();

    virtual bool Load();

    virtual bool Unload();

    FT_Face GetFontFace() const;

    static FT_Library GetLibrary();

private:
    FT_Face m_pFace;
    static FT_Library m_library;
    static int m_nLibRefCount;
};

#endif