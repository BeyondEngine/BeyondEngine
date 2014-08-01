#ifndef BEYOND_ENGINE_GUI_SCROLLITEM_H__INCLUDE
#define BEYOND_ENGINE_GUI_SCROLLITEM_H__INCLUDE

#include "TextureControl.h"
#include "Render\ReflectTextureInfo.h"

class CScrollItem : public CTextureControl
{
    DECLARE_REFLECT_GUID_ABSTRACT( CScrollItem, 0x1458a909, CTextureControl )
public:
    CScrollItem();
    virtual ~CScrollItem();
    virtual void Initialize() override;

    virtual CScrollItem* CloneItem() = 0;

    void SetScrollViewInitialzeFlag( bool bInit );

private:
    bool m_bScrollViewInit;
    static int nNum;
};

#endif
