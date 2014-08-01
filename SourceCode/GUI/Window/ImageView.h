#ifndef BEYOND_ENGINE_GUI_IMAGEVIEW_H__INCLUDE
#define BEYOND_ENGINE_GUI_IMAGEVIEW_H__INCLUDE

#include "TextureControl.h"

class CImageView : public CTextureControl
{
    DECLARE_REFLECT_GUID( CImageView, 0xb5ade61c, CTextureControl )
public:
    CImageView();
    virtual ~CImageView();

private:
    static int nNum;
};

#endif