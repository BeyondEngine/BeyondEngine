#include "stdafx.h"
#include "GradientCursor.h"

CGradientCursor::CGradientCursor(float fPos, wxColor color, ECursorType eType)
    : m_bSelected(false)
{
    m_fPos = fPos;
    m_color = color;
    m_eType = eType;
}

CGradientCursor::~CGradientCursor()
{

}

void CGradientCursor::SetPos(float fPos)
{
    m_fPos = fPos;
}

float CGradientCursor::GetPosPercent() const
{
    return m_fPos;
}

void CGradientCursor::SetColor(wxColor color)
{
    m_color = color;
}

wxColor& CGradientCursor::GetColor()
{
    return m_color;
}

bool CGradientCursor::operator!=(const CGradientCursor& obj) const
{
    return m_fPos != obj.m_fPos;
}

bool CGradientCursor::operator<(const CGradientCursor& obj) const
{
    return m_fPos < obj.m_fPos;
}

bool CGradientCursor::operator==(const CGradientCursor& obj) const
{
    return m_fPos == obj.m_fPos;
}

void CGradientCursor::Select(bool bSelect)
{
    m_bSelected = bSelect;
}

bool CGradientCursor::IsSelected()
{
    return m_bSelected;
}

ECursorType CGradientCursor::GetType() const
{
    return m_eType;
}
