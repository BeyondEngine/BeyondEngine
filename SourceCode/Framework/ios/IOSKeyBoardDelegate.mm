#include "stdafx.h"
#include "IOSKeyBoardDelegate.h"

CIOSKeyBoardDelegate::CIOSKeyBoardDelegate()
: m_pView(nullptr)
{

}

CIOSKeyBoardDelegate::~CIOSKeyBoardDelegate()
{

}

void CIOSKeyBoardDelegate::ShowKeyBoard()
{
    [m_pView ShowKeyboard];
}

void CIOSKeyBoardDelegate::HideKeyBoard()
{
    [m_pView HideKeyboard];
}


void CIOSKeyBoardDelegate::SetUIView(BeyondView *pView)
{
    m_pView = pView;
}