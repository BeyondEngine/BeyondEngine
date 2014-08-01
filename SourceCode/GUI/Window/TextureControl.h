#ifndef BEYOND_ENGINE_GUI_TEXTRUECONTROL_H__INCLUDE
#define BEYOND_ENGINE_GUI_TEXTRUECONTROL_H__INCLUDE

#include "Control.h"
#include "Render\ReflectTextureInfo.h"

class CTexture;

enum ERatioFixMode
{
    eRFM_NO_FIX,
    eRFM_FOLLOW_CONTENT,
    eRFM_HEIGHT_FOLLOW_WIDTH,
    eRFM_WIDTH_FOLLOW_HEIGHT,
};

class CTextureControl : public CControl
{
    DECLARE_REFLECT_GUID( CTextureControl, 0x1458a910, CControl )
public:
    CTextureControl();
    virtual ~CTextureControl();

    virtual void Initialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;
    virtual void DoRender() override;

    void SetTextrueFrag( const SReflectTextureInfo& frag );
    void SetRatioFixMode(ERatioFixMode mode);
    
    const CQuadT& GetQuadT() const;
    SharePtr<CTexture> GetTextrue() const;

    bool IsGrid9Scale() const;
    const CVec2& GetGrid9ScaleOffset() const;
protected:
    virtual void CalcRealSize() override;

    virtual void UpdateTextrue();

protected:
    CQuadT m_quadt;
    bool m_bGrid9Enabled;
    CVec2 m_vec2Grid9Offset;
    SReflectTextureInfo m_fragImage;
private:
    ERatioFixMode m_eFixMode;
};

#endif