#ifndef BEYOND_ENGINE_GUI_Label_H__INCLUDE
#define BEYOND_ENGINE_GUI_Label_H__INCLUDE

#include "Control.h"
#include "GUI/Font/StyledText.h"

class CLabel : public CControl
{
    DECLARE_REFLECT_GUID( CLabel, 0xcab44df9, CControl )
public:
    CLabel();
    virtual ~CLabel();

    virtual void Initialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;

    virtual void DoRender() override;

    void SetText( const TString& text );

    void SetTextSize( size_t size );

    void SetTextHoriAlign( EHoriAlign hAlign );

    void SetTextVertAlign( EVertAlign vAlign );
private:
    void ParseText();
private:
    TString m_strText;
    TString m_strFontName;
    size_t m_uTextSize;
    float m_fPadding;
    CColor m_textColor;
    CColor m_borderColor;
    CFontFace* m_pFontFace;
    TStyledTextSegmentList m_textSegments;
    EHoriAlign m_hAlign;
    EVertAlign m_vAlign;
    static int nNum;
};

#endif