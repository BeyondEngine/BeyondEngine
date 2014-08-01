#include "stdafx.h"
#include "Framework/Application.h"
#include "Render/win32/GlfwRenderWindow.h"
#include "Render/Viewport.h"
#include "Render/RenderManager.h"

static size_t iWidth = 960;
static size_t iHeight = 640;

int CApplication::Run(void* pData)
{
    if(pData)
    {
        char *strParam = reinterpret_cast<char *>(pData);
        int ret = _stscanf_s(strParam, _T("width:%d, height:%d"), &iWidth, &iHeight);
        BEATS_ASSERT(ret == 2, _T("Parameter passed to CApplication::Run has invalid format!\n"));
        BEYONDENGINE_UNUSED_PARAM(ret);
    }
    CGlfwRenderWindow *pRenderWindow = new CGlfwRenderWindow(iWidth, iHeight, true);
    CRenderManager::GetInstance()->SetCurrentRenderTarget(pRenderWindow);
    CApplication::GetInstance()->Initialize();

    MSG msg;
    ::ZeroMemory(&msg, sizeof(MSG));
    // Main message loop:
    while (!glfwWindowShouldClose(pRenderWindow->GetMainWindow()))
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        if(CEngineCenter::GetInstance()->ShouldUpdateThisFrame())
        {
            CApplication::GetInstance()->Update();
            CApplication::GetInstance()->Render();
        }
        else
        {
            Sleep(5);
        }
    }

    CApplication::GetInstance()->Uninitialize();
    BEATS_SAFE_DELETE(pRenderWindow);
    return 0;
}