#ifndef BEYOND_ENGINE_GUI_PROCESSBAR_H__INCLUDE
#define BEYOND_ENGINE_GUI_PROCESSBAR_H__INCLUDE

#include "TextureControl.h"
#include "Render/ReflectTextureInfo.h"

class CProgressBar : public CTextureControl
{
    DECLARE_REFLECT_GUID( CProgressBar, 0x496e5dab, CTextureControl )
public:
    CProgressBar();
    virtual ~CProgressBar();

    virtual void Initialize() override;

    virtual void ReflectData( CSerializer& serializer ) override;

    virtual bool OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer ) override;

    virtual void DoRender() override;

    void SetCurrentProgress( size_t process );

    virtual void UpdateQuadP() override;

protected:
    virtual void UpdateTextrue() override;

private:
    void UpdateForegroundQuadP();
    void UpdateForegroundTexture();
private:
    size_t m_uCurrentProgress;
    SReflectTextureInfo m_fragForeGround;

    CQuadP m_foreQuadp;
    CQuadT m_foreQuadt;
    static int nNum;
};

#endif