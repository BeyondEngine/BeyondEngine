#include "stdafx.h"
#include "ImageView.h"
#include "GUI/Event/WindowEvent.h"
#include "GUI/WindowManager.h"
#include "Render/TextureFrag.h"

int CImageView::nNum = 0;

CImageView::CImageView()
{
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("ImageView"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CImageView::~CImageView()
{

}