#include "stdafx.h"
#include "ScrollItem.h"
#include "GUI/WindowManager.h"
#include "ScrollView.h"
#include "Render/TextureFrag.h"
#include "Render/Texture.h"

int CScrollItem::nNum = 0;

CScrollItem::CScrollItem()
    : m_bScrollViewInit( false )
{
    TCHAR szName[25];
    _stprintf( szName, _T("%s_%d"), _T("ScrollItem"), nNum );
    m_strDefaultName = szName;
    nNum++;
}

CScrollItem::~CScrollItem()
{

}

void CScrollItem::Initialize()
{
    super::Initialize();
    if ( m_bScrollViewInit && GetRootFlag() )
    {
         CWindowManager::GetInstance()->RemoveFromRoot( this );
    }
}

void CScrollItem::SetScrollViewInitialzeFlag( bool bInit )
{
    m_bScrollViewInit = bInit;
}
