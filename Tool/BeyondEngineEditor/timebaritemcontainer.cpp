#include "stdafx.h"
#include "timebaritemcontainer.h"
#include "timebarframe.h"
#include "timebarscale.h"
#include "NodeAnimation/NodeAnimationElement.h"
#include "wx/dcbuffer.h"
#include "TimeBarFrameData.h"

#define CELLOFFSET 2

BEGIN_EVENT_TABLE(CTimeBarItemContainer, wxScrolledWindow)
EVT_PAINT(CTimeBarItemContainer::OnPaint)
EVT_MOUSE_EVENTS(CTimeBarItemContainer::OnMouse)
EVT_MOUSE_CAPTURE_LOST(CTimeBarItemContainer::OnMouseCaptureLost)
END_EVENT_TABLE()
CTimeBarItemContainer::CTimeBarItemContainer(wxWindow* parent)
: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL)
, m_iCellWidth(1)
, m_iCellHeight(1)
, m_pSyncScale(NULL)
, m_pTimeBarFrame(NULL)
, m_iCursorPositionX(0)
, m_nDraggingFrameIndex(INVALID_DATA)
{
    m_pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_pMainSizer);
    memset(&m_currentSelect, 0, sizeof(m_currentSelect));
    memset(&m_lastSelect, 0, sizeof(m_lastSelect));
    ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_NEVER);
}

CTimeBarItemContainer::~CTimeBarItemContainer()
{

}

void CTimeBarItemContainer::SyncWith(wxWindow* /*win*/, CTimeBarScale* win2)
{
    m_pSyncScale = win2;
}

void CTimeBarItemContainer::ScrollWindow(int dx, int dy, const wxRect* rect )
{
    wxPoint ptViewStart = GetViewStart();
    bool bHorizontalMove = dx != 0 && dy == 0;
    if (bHorizontalMove)
    {
        wxPoint pt = ptViewStart;
        m_pSyncScale->Refresh(false);
    }
    Refresh(false);
    __super::ScrollWindow(dx, dy, rect);
}

void CTimeBarItemContainer::AddItem(CTimeBarFrameData* pItem)
{
    BEATS_ASSERT(pItem != NULL);
    m_items.push_back(pItem);
}

void CTimeBarItemContainer::SetTimeBarFrameWindow(CTimeBarFrame* pSplitterWindow)
{
    m_pTimeBarFrame = pSplitterWindow;
}

CTimeBarFrameData* CTimeBarItemContainer::GetItem(int nIndex)
{
    BEATS_ASSERT(nIndex < (int)m_items.size())
    return m_items[nIndex];
}

CTimeBarFrame* CTimeBarItemContainer::GetTimeBarFrameWindow()
{
    return m_pTimeBarFrame;
}

void CTimeBarItemContainer::ResetScrollBar()
{
    BEATS_ASSERT(m_pTimeBarFrame != NULL);
    wxPoint viewStart = GetViewStart();
    int nCountX = m_pTimeBarFrame->GetFrameCountForDraw();
    int nCountY = m_items.size();
    SetScrollbars(m_iCellWidth, m_iCellHeight, nCountX, nCountY, viewStart.x, viewStart.y);
}

SSelection& CTimeBarItemContainer::GetCurrentSelect()
{
    return m_currentSelect;
}

void CTimeBarItemContainer::SetCellWidth(int iWidth)
{
    m_iCellWidth = iWidth;
}

void CTimeBarItemContainer::SetCellHeight(int height)
{
    m_iCellHeight = height;
}

void CTimeBarItemContainer::RemoveItem(int index)
{
    BEATS_ASSERT(index < (int)m_items.size());
    std::vector<CTimeBarFrameData*>::iterator itr = m_items.begin() + index;
    m_items.erase(itr);
}

int CTimeBarItemContainer::GetCellWidth() const
{
    return m_iCellWidth;
}

int CTimeBarItemContainer::GetItemCount()
{
    return (int)m_items.size();
}

void CTimeBarItemContainer::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxAutoBufferedPaintDC ScaleDC(this);
    wxSize size = GetSize();
    ScaleDC.SetPen(*wxWHITE);
    ScaleDC.DrawRectangle(wxPoint(0, 0), size);
    if (m_pTimeBarFrame->IsEnableFramePanel())
    {
        DrawSelection(ScaleDC);
    }
    wxPoint ViewStart = GetViewStart();
    int iCount = m_pTimeBarFrame->GetFrameCountForDraw();
    int nPosX = 0;
    int nOffset = m_iCellWidth * 0.5f;
    ScaleDC.SetPen(*wxBLACK);
    for (int i = ViewStart.x; i < iCount; i++)
    {
        nPosX = i * m_iCellWidth + nOffset - ViewStart.x * m_iCellWidth;
        if (i % 5 == 0)
        {
            ScaleDC.DrawLine(nPosX, 0, nPosX, size.y);
        }
    }
    DrawItem(ScaleDC);
    nPosX = m_pTimeBarFrame->GetCursorPos() * m_iCellWidth + nOffset - ViewStart.x * m_iCellWidth;
    wxPen pen = m_pTimeBarFrame->IsEnableFramePanel() ? *wxRED_PEN : *wxGREY_PEN;
    pen.SetWidth(CURSORWIDTH);
    ScaleDC.SetPen(pen);
    ScaleDC.DrawLine(nPosX, 0, nPosX, size.y);
}

void CTimeBarItemContainer::DrawItem(wxDC& ScaleDC)
{
    int nPosX = 0;
    int nPosY = 0;
    int nCount = m_items.size();
    ScaleDC.SetPen(*wxGREEN);
    ScaleDC.SetBrush(*wxGREEN);
    wxPoint pos = GetViewStart();
    for (int i = 0; i < nCount; i++)
    {
        auto keyMap = m_items[i]->GetFrameList();
        nPosY = i * m_iCellHeight - pos.y * m_iCellHeight;
        for (auto itr : keyMap)
        {
            nPosX = itr * m_iCellWidth - pos.x * m_iCellWidth;
            ScaleDC.DrawRectangle(nPosX + CELLOFFSET, nPosY, m_iCellWidth - 2 * CELLOFFSET, m_iCellHeight);
        }
    }
}

void CTimeBarItemContainer::DrawSelection(wxDC& ScaleDC)
{
    int columnstart = m_currentSelect.m_iSelectColumnBegin > m_currentSelect.m_iSelectColumnEnd ? m_currentSelect.m_iSelectColumnEnd : m_currentSelect.m_iSelectColumnBegin;
    int columnend = m_currentSelect.m_iSelectColumnBegin < m_currentSelect.m_iSelectColumnEnd ? m_currentSelect.m_iSelectColumnEnd : m_currentSelect.m_iSelectColumnBegin;
    int rowstart = m_currentSelect.m_iSelectRowBegin >= m_currentSelect.m_iSelectRowEnd ? m_currentSelect.m_iSelectRowEnd : m_currentSelect.m_iSelectRowBegin;
    int rowend = m_currentSelect.m_iSelectRowBegin <= m_currentSelect.m_iSelectRowEnd ? m_currentSelect.m_iSelectRowEnd : m_currentSelect.m_iSelectRowBegin;

    wxPoint point = GetViewStart();
    int nX = columnstart * m_iCellWidth - point.x * m_iCellWidth;
    int nY = rowstart * m_iCellHeight - point.y * m_iCellHeight;
    int nWidth = (columnend - columnstart + 1) * m_iCellWidth;
    int nHeight = (rowend - rowstart + 1) * m_iCellHeight;
    ScaleDC.SetPen(*wxBLUE);
    ScaleDC.SetBrush(*wxBLUE);
    ScaleDC.DrawRectangle(nX, nY, nWidth, nHeight);
}

void CTimeBarItemContainer::OnMouse(wxMouseEvent& event)
{
    if (m_pTimeBarFrame->IsEnableFramePanel())
    {
        if (event.ButtonDown(wxMOUSE_BTN_LEFT))
        {
            if (!HasCapture())
            {
                CaptureMouse();
            }
            int nPosX = event.GetX() / m_iCellWidth + GetViewStart().x;
            int nPosY = event.GetY() / m_iCellHeight + GetViewStart().y;
            m_pTimeBarFrame->SetClickOnScalebar(false);
            m_currentSelect.m_iSelectColumnBegin = nPosX;
            m_currentSelect.m_iSelectColumnEnd = nPosX;
            m_currentSelect.m_iSelectRowBegin = nPosY;
            m_currentSelect.m_iSelectRowEnd = nPosY;
            m_nDraggingFrameIndex = INVALID_DATA;
            if (nPosY < (int)m_items.size())
            {
                m_pTimeBarFrame->SelectItem(m_items[nPosY]);
                m_nDraggingFrameIndex = m_items[nPosY]->HasFrame(nPosX) ? nPosX : INVALID_DATA;
            }
            m_pTimeBarFrame->SetCursorPos(nPosX);
            if (m_lastSelect.m_iSelectColumnBegin != nPosX || m_lastSelect.m_iSelectRowBegin != nPosY)
            {
                wxCommandEvent selectFrameChangeEvent(TIMTBAR_SELECTFRAMECHANGE_EVENT, GetId());
                selectFrameChangeEvent.SetEventObject(this);
                ProcessWindowEvent(selectFrameChangeEvent);
            }
        }
        else if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
        {
            int nPosX = event.GetX() / m_iCellWidth + GetViewStart().x;
            int nPosY = event.GetY() / m_iCellHeight + GetViewStart().y;
            bool bRefresh = false;
            if (m_currentSelect.m_iSelectRowEnd != nPosY)
            {
                bRefresh = true;
                if (m_nDraggingFrameIndex == INVALID_DATA)
                {
                    m_currentSelect.m_iSelectRowEnd = nPosY < 0 ? 0 : nPosY;
                }
            }
            if (m_currentSelect.m_iSelectColumnEnd != nPosX)
            {
                bRefresh = true;
                if (m_nDraggingFrameIndex == INVALID_DATA)
                {
                    m_currentSelect.m_iSelectColumnEnd = nPosX < 0 ? 0 : nPosX;
                }
            }
            if (m_pTimeBarFrame->GetCursorPos() != nPosX)
            {
                m_pTimeBarFrame->SetCursorPos(nPosX, false, m_nDraggingFrameIndex == INVALID_DATA);
            }
            if (bRefresh)
            {
                m_pTimeBarFrame->RefreshControl();
            }
        }
        else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
        {
            if (HasCapture())
            {
                ReleaseMouse();
            }
            m_lastSelect = m_currentSelect;
            if (m_nDraggingFrameIndex != INVALID_DATA && m_nDraggingFrameIndex != m_pTimeBarFrame->GetCursorPos())
            {
                wxCommandEvent draggingFrameEvent(TIMTBAR_DRAGGINGFRAME_EVENT, GetId());
                draggingFrameEvent.SetEventObject(this);
                ProcessWindowEvent(draggingFrameEvent);
                m_nDraggingFrameIndex = INVALID_DATA;
            }
        }
        else if (event.ButtonUp(wxMOUSE_BTN_RIGHT))
        {
            int nPosX = event.GetX() / m_iCellWidth + GetViewStart().x;
            int nPosY = event.GetY() / m_iCellHeight + GetViewStart().y;
            if (nPosY < (int)m_items.size())
            {
                m_pTimeBarFrame->SelectItem(m_items[nPosY]);
                m_pTimeBarFrame->RClickOnFrame(nPosX);
            }
        }
    }
}

void CTimeBarItemContainer::Clear()
{
    m_items.clear();
    Refresh(false);
}

void CTimeBarItemContainer::OnMouseCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    //don't remove this or here cause a warning
}

int CTimeBarItemContainer::GetItemIndex(CTimeBarFrameData* pItem)
{
    int nRet = INVALID_DATA;
    for (size_t i = 0; i < m_items.size(); i++)
    {
        if (m_items[i] == pItem)
        {
            nRet = i;
            break;
        }
    }
    return nRet;
}

