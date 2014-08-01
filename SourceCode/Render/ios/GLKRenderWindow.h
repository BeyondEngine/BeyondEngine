#ifndef __FCEngine__GLKRenderWindow__
#define __FCEngine__GLKRenderWindow__

#include "Render/RenderTarget.h"
#include <GLKit/GLKit.h>

class GLKRenderWindow : public CRenderTarget
{
public:
    GLKRenderWindow(int nWidth, int nHeight);
    virtual ~GLKRenderWindow();
    
    void SetContext(EAGLContext *pContext);
    
    virtual void SetToCurrent();
    
private:
    EAGLContext *m_pContext;
};

#endif
