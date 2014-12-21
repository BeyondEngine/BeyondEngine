#include "stdafx.h"
#include "Framework/Application.h"

#include "Render/Viewport.h"
#include "Render/RenderManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Render/win32/GlfwRenderWindow.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "glfw3native.h"

#ifndef USE_VLD
struct SCrtMemoryDetector
{
    SCrtMemoryDetector() {}
    ~SCrtMemoryDetector()
    {
        _CrtDumpMemoryLeaks();
    }
};
#pragma warning(disable:4074)
#pragma init_seg(compiler)
SCrtMemoryDetector ds;
#endif

HWND BEYONDENGINE_HWND = NULL;

int CApplication::Run(int argc, char * argv[], void* pData)
{

    BEYONDENGINE_UNUSED_PARAM(argc);
    BEYONDENGINE_UNUSED_PARAM(argv);
    uint32_t iWidth = 960;
    uint32_t iHeight = 640;
    if (pData)
    {
        char *strParam = reinterpret_cast<char *>(pData);
        int ret = _stscanf_s(strParam, _T("width:%d, height:%d"), &iWidth, &iHeight);
        BEATS_ASSERT(ret == 2, _T("Parameter passed to CApplication::Run has invalid format!\n"));
        BEYONDENGINE_UNUSED_PARAM(ret);
    }
#ifndef EDITOR_MODE
    TCHAR szModulePath[MAX_PATH];
    GetModuleFileName(NULL, szModulePath, MAX_PATH);
    TString strFilePath = CFilePathTool::GetInstance()->ParentPath(szModulePath);
    strFilePath.append(_T("\\Login.txt"));
    if (CFilePathTool::GetInstance()->Exists(strFilePath.c_str()))
    {
        TString strResolution;
        TString strParam;
        TString strTroopArray;
        std::ifstream stream(strFilePath.c_str());
        int a = stream.get();
        int b = stream.get();
        int c = stream.get();
        if (!(a == 0xEF && b == 0xBB && c == 0xBF)) // detect UTF8 BOM header
        {
            stream.seekg(0);
        }
        std::getline(stream, strResolution);
        std::getline(stream, strParam);
        std::vector<TString> strDataList;
        CStringHelper::GetInstance()->SplitString(strResolution.c_str(), _T("@"), strDataList);
        if (strDataList.size() == 2)
        {
            iWidth = _ttoi(strDataList[0].c_str());
            iHeight = _ttoi(strDataList[1].c_str());
        }
        strDataList.clear();
        CStringHelper::GetInstance()->SplitString(strParam.c_str(), _T("@"), strDataList);
        for (size_t i = 0; i < strDataList.size(); ++i)
        {
            std::vector<TString> params;
            CStringHelper::GetInstance()->SplitString(strDataList[i].c_str(), _T(":"), params);
            BEATS_ASSERT(params.size() == 2);
            if (params[0] == "PlayEngine")
            {
#ifdef DEVELOP_VERSION
                CPlayEngine::GetInstance()->m_bDisable = params[1] == "0";
#endif
            }
            else if (params[0] == "Audio")
            {
                if (params[1] == "0")
                {
                    CStarRaidersCenter::GetInstance()->SetGlobalMusicVolumeScale(0);
                    CStarRaidersCenter::GetInstance()->SetGlobalSfxVolumeScale(0);
                }
            }
        }
    }
#endif
    CGlfwRenderWindow *pRenderWindow = new CGlfwRenderWindow(iWidth, iHeight);
    CRenderManager::GetInstance()->SetCurrentRenderTarget(pRenderWindow);
    CApplication::GetInstance()->Initialize();
    MSG msg;
    ::ZeroMemory(&msg, sizeof(MSG));
    // Main message loop:
    BEYONDENGINE_HWND = glfwGetWin32Window(pRenderWindow->GetMainWindow());
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

        if (CEngineCenter::GetInstance()->ShouldUpdateThisFrame())
        {
            CApplication::GetInstance()->Update();
            CApplication::GetInstance()->Render();
        }
        else
        {
            Sleep(5);
        }
    }
    pRenderWindow->Uninitialize();
    CApplication::GetInstance()->Uninitialize();
    BEATS_SAFE_DELETE(pRenderWindow);
    return 0;
}

void CApplication::OnSwitchToBackground()
{
    Pause();
}

void CApplication::OnSwitchToForeground()
{
    Resume();
}
