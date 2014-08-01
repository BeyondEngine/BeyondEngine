#ifndef BEYOND_ENGINE_GUI_TIPWINDOW_H__INCLUDE
#define BEYOND_ENGINE_GUI_TIPWINDOW_H__INCLUDE

#include "TextureControl.h"
#include "Render/ReflectTextureInfo.h"

class CTipWindow : public CTextureControl
{
    DECLARE_REFLECT_GUID( CTipWindow, 0x1458a90a, CTextureControl )
public:
    CTipWindow();
    virtual ~CTipWindow();

    virtual void Initialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;

    virtual bool OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent) override;

    virtual void OnParentSizeChange( float width, float height ) override;

    virtual void DoRender() override;

    virtual void PostRender() override;

    virtual void Update( float dtt );

    void SetAttachControl( CControl* pControl );

    virtual bool OnTouchEvent( CTouchEvent* event ) override;

    virtual void UpdateQuadP() override;

    virtual void UpdateTextrue() override;

private:
    void ResetTip();

    bool CheckRectInRenderTarget( float& x, float& y );

    void AligningTheControl();

private:
    enum EHeadDeriction
    {
        eHD_Left = 0,
        eHD_Bottom,
        eHD_Right,
        eHD_Top,
    };
    CVec2 m_vec2PosOffset;
    float m_fHeadLength;

    EHeadDeriction m_eHeadDeriction;
    float xOffset;
    float yOffset;
    CQuadP m_headQuadp;
    CQuadT m_headQuadt;
    CControl* m_pAttachControl;

    bool m_bUpdateFinish;

    static int nNum;
};
#endif