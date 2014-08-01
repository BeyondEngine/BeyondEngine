#ifndef _3DSCENEEXPORT_H_
#define _3DSCENEEXPORT_H_

#include "IGame\IGame.h"
#include "IGame\IGameModifier.h"
#include "impexp.h"
#include "DataExporter.h"

class CModelExporter;
class C3DSceneExport : public SceneExport
{
public:
    C3DSceneExport();
    ~C3DSceneExport();

	virtual int				ExtCount();
	virtual const TCHAR*	Ext(int n);
	virtual const TCHAR*	LongDesc();
	virtual const TCHAR*	ShortDesc();
	virtual const TCHAR*	AuthorName();
	virtual const TCHAR*	CopyrightMessage();
	virtual const TCHAR*	OtherMessage1();
	virtual const TCHAR*	OtherMessage2();
	virtual unsigned int	Version();
	virtual void			ShowAbout(HWND hWnd);
	virtual BOOL			SupportsOptions(int ext, DWORD options);
	virtual int				DoExport(const TCHAR* pszName,
                                    ExpInterface* pExpInterface,
                                    Interface* pInterface, 
                                    BOOL bSuppressPrompts = FALSE,
                                    DWORD options = 0);

private:
	CDataExporter*  m_pDataExporter;
    CModelExporter* m_pModelExporter;
};

#endif//_3DSCENEEXPORT_H_