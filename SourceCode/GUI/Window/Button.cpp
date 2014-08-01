#include "stdafx.h"
#include "Button.h"
#include "GUI/Event/WindowEvent.h"
#include "GUI/WindowManager.h"
#include "Render/TextureFrag.h"

int CButton::nIndex = 0;

CButton::CButton()
{
    TCHAR szNname[25];
    _stprintf( szNname, _T("%s_%d"),  _T("Button"), nIndex );
    m_strDefaultName = szNname;
    nIndex++;
}

CButton::~CButton()
{

}

