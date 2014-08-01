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
    void RenderText(const TString &text, kmScalar x, kmScalar y, const TString &faceName, CColor color = 0x000000FF);
    SharePtr<CMaterial> CreateDefaultMaterial(bool bBitmapFont);
    CRenderGroup *GetRenderGroupGUI();
    CRenderGroup *GetRenderGroup2D();
    void Clear();
    const std::map<TString, CFontFace *>& GetFontFaceMap() const;
    CFontFace* GetDefaultFontFace() const;
    void SetDefaultFontFace(CFontFace* pFontFace);

private:
    void OnMainRenderWindowChanged(CBaseEvent *event);
    void OnMainRenderWindowSizeChanged(CBaseEvent *event);
private:
    CFontFace* m_pDefaultFontFace;
    std::map<TString, CFontFace *> m_faceMap;
    CShaderProgram *m_pProgramFreetypeText;
    CShaderProgram *m_pProgramBitmapText;
    CRenderGroup *m_pRenderGroupGUI;
    CRenderGroup *m_pRenderGroup2D;
    CEventSlot m_slot;
};

#endif