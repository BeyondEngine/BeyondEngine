#ifndef BEYOND_ENGINE_GUI_TOPWINDOW_H__INCLUDE
#define BEYOND_ENGINE_GUI_TOPWINDOW_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "Render/ReflectTextureInfo.h"

class CControl;
class CTouchEvent;
class CBaseEvent;
class CRenderTarget;

class CTopWindow : public CComponentInstance
{
    DECLARE_REFLECT_GUID( CTopWindow, 0x1458a906, CComponentInstance )
public:
    CTopWindow();
    virtual ~CTopWindow();

    virtual void Initialize() override;

    virtual void Uninitialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent) override;

    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet) override;

    virtual void Render();

    virtual void Update( float dtt );

    virtual bool OnTouchEvent( CTouchEvent* event );

    virtual void OnRenderTargetSizeChange( float w, float h );

    void SetVisible( bool bVisible );
    bool IsVisible() const;

    void SetName( const TString& name );

    const TString& GetName() const;

    void OnCloseChildWindow( CBaseEvent* event );

protected:
    const TString& GetDefaultName();

protected:
    CControl* m_pRealControl;

private:
    CColor m_colorBack;
    TString m_strDefaultName;
    TString m_strName;
    bool m_bIsVisible;
    float m_fWidth;
    float m_fHeight;
    static int nNum;
};

#endif