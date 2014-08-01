#ifndef BEYOND_ENGINE_GUI_BUTTON_H__INCLUDE 
#define BEYOND_ENGINE_GUI_BUTTON_H__INCLUDE

#include "TextureControl.h"
#include "Render/ReflectTextureInfo.h"
#include "NodeAnimation/NodeAnimation.h"

class CButton : public CTextureControl
{
    DECLARE_REFLECT_GUID( CButton, 0x1458a902, CTextureControl )
public:
    CButton();
    virtual ~CButton();

private:
    static int nIndex;
};
#endif