#include "stdafx.h"
#include "GLKRenderWindow.h"

GLKRenderWindow::GLKRenderWindow(int nWidth, int nHeight)
    : CRenderTarget(nWidth, nHeight)
    , m_pContext(nullptr)
{
}

GLKRenderWindow::~GLKRenderWindow()
{
    if ([EAGLContext currentContext] == m_pContext) {
        [EAGLContext setCurrentContext:nil];
    }
}

void GLKRenderWindow::SetContext(EAGLContext *pContext)
{
    m_pContext = pContext;
}

void GLKRenderWindow::SetToCurrent()
{
    [EAGLContext setCurrentContext:m_pContext];
}
