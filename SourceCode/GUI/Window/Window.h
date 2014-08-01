#ifndef BEYOND_ENGINE_GUI_WINDOW_H__INCLUDE
#define BEYOND_ENGINE_GUI_WINDOW_H__INCLUDE

#include "TextureControl.h"
#include "Render/ReflectTextureInfo.h"
#include "GUI/Event/WindowEvent.h"

class CButton;

class CWindow : public CTextureControl
{
    DECLARE_REFLECT_GUID( CWindow, 0x1458a901, CTextureControl )
public:
    CWindow();
    virtual ~CWindow();

    virtual void Initialize() override;

    virtual void Uninitialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer ) override;

    void OnCloseButtonClicked( CBaseEvent* event );

private:

    void UpdateCloseButton();

private:
    SReflectTextureInfo m_fragCloseButton;
    CVec2 m_vec2CloseButtonSize;
    CVec2 m_vec2CloseButtonPosition;
    CQuadP m_closeQuadp;
    CQuadT m_closeQuadt;
    CButton* m_pCloseButton;
    static int nNum;
};

#endif