#ifndef BEYOND_ENGINE_GUI_TEXTBOX_H__INCLUDE
#define BEYOND_ENGINE_GUI_TEXTBOX_H__INCLUDE

#include "TextureControl.h"
#include "Render/ReflectTextureInfo.h"
#include "Event/KeyboardEvent.h"
#include "GUI/Font/StyledText.h"

class CTextBox : public CTextureControl
{
    DECLARE_REFLECT_GUID( CTextBox, 0x1458a90b, CTextureControl )
public:
    CTextBox();
    virtual ~CTextBox();

    virtual void Initialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;

    virtual void UpdateQuadP() override;

    void OnBackSpaceWord();

    void OnInsertText( const TCHAR* data, int num );

    virtual void DoRender() override;

    virtual void Update( float dtt ) override;

    const TString& GetText() const;

private:
    void ParseText();
    void UpdateCur();
    void DeleteAChar();

private:
    float m_fXBorderWidth;
    float m_fYBorderWidth;
    size_t m_uTextSize;

    //the time use to flicker the cur
    float m_fCurTime;
    bool m_bCurVIsible;

    TString m_strFontName;
    TString m_strText;
    CQuadP m_quadCur;
    CRect m_rectInnerRect;
    float m_fScaleSize;
    CVec3 m_vec3CurStartPosition;
    CFontFace* m_pFontFace;
    TStyledTextSegmentList m_textSegments;
    TStyledTextSegmentList m_textSegmentsTransform;
    float m_fCurXoffset;
    static int nNum;
};

#endif