// 3DExporter.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "3DSceneExport.h"
#include "ModelExporter.h"

C3DSceneExport::C3DSceneExport( void )
{
	GetConversionManager()->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	m_pDataExporter = new CDataExporter;
    m_pModelExporter = new CModelExporter;
}

C3DSceneExport::~C3DSceneExport( void )
{
    BEATS_SAFE_DELETE(m_pDataExporter);
    BEATS_SAFE_DELETE(m_pModelExporter);
}

int C3DSceneExport::ExtCount()
{
	return 1;
}

const TCHAR* C3DSceneExport::Ext( int /*n*/ )
{
	return _T("sce");
}

const TCHAR* C3DSceneExport::LongDesc()
{
#ifdef	_DEBUG
	return _T("BeyondEngine PLUGIN for 3dsmax 11d");
#else
	return _T("BeyondEngine PLUGIN for 3dsmax 11r");
#endif
}

const TCHAR* C3DSceneExport::ShortDesc()
{
#ifdef	_DEBUG
	return _T("BeyondEngine PLUGINd");
#else
	return _T("BeyondEngine PLUGINr");
#endif
}

const TCHAR* C3DSceneExport::AuthorName()
{
	return _T("Beyond Engine Team");
}

const TCHAR* C3DSceneExport::CopyrightMessage()
{
	return _T("COPYRIGHT(C) ALL RIGHT RESERVED");
}

const TCHAR* C3DSceneExport::OtherMessage1()
{
	return _T("LWM");
}

const TCHAR* C3DSceneExport::OtherMessage2()
{
	return _T("LWM");
}

unsigned int C3DSceneExport::Version()
{
	return 1;
}

void C3DSceneExport::ShowAbout( HWND /*hWnd*/ )
{

}

BOOL C3DSceneExport::SupportsOptions( int /*ext*/, DWORD /*options*/ )
{
	return FALSE;
}

int C3DSceneExport::DoExport( const TCHAR* pszName, ExpInterface* /*pExpInterface*/, Interface* /*pInterface*/, BOOL/* bSuppressPrompts = FALSE*/, DWORD/* options = 0*/)
{
    m_pDataExporter->SetExportFileName(pszName);
    m_pDataExporter->ExportSkeletonAnimation();
    m_pModelExporter->Export(pszName);
    MessageBox(NULL, _T("Export finished!"), _T("Success"), MB_OK);
    return TRUE;
}