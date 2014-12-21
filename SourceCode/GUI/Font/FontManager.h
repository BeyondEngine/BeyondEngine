#ifndef BEYOND_ENGINE_GUI_FONT_FONTMANAGER_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_FONTMANAGER_H__INCLUDE

#include "Event/EventSlot.h"

class CShaderProgram;
class CMaterial;
class CRenderGroup;
class CBaseEvent;

//forward declaration
class CFontFace;
class CFontGlyph;

class CFontManager
{
    BEATS_DECLARE_SINGLETON(CFontManager);
public:
    CFontFace *CreateFreetypeFontFace(const TString &name,
        const TString &file, int sizeInPt, int dpi = -1, bool bRegister = true);
    CFontFace *CreateBitmapFontFace(const TString &filename);
    CFontFace *GetFace(const TString &name);
    void Clear();
    const std::map<TString, CFontFace *>& GetFontFaceMap() const;

private:
    std::map<TString, CFontFace *> m_faceMap;
    CEventSlot m_slot;
};

#endif