// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "ClassDescFactor.h"

static ClassDescFactor    desc_main;
HINSTANCE                g_hInstance = NULL;

const TCHAR* LibDescription() 
{
#ifdef    _DEBUG
    return _T("BeyondEnginePLUGIN[d]");
#else
    return _T("BeyondEnginePLUGIN[r]");
#endif
}

INT LibNumberClasses()
{
    return 1;
}

ClassDesc* LibClassDesc(INT classIndex) 
{
    switch(classIndex)
    {
    case 0:
        return  &desc_main;;

    default:
        return NULL;
    }
}

ULONG LibVersion() 
{
    return VERSION_3DSMAX;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID /*lpvReserved*/) 
{
    // ����ȫ��ʵ�����
    if(fdwReason == DLL_PROCESS_ATTACH || fdwReason == DLL_THREAD_ATTACH)
    {
        g_hInstance = hinstDLL;
    }
    else if(fdwReason == DLL_PROCESS_DETACH || fdwReason == DLL_THREAD_DETACH)
    {
        g_hInstance = hinstDLL;
    }
    return TRUE;
}


