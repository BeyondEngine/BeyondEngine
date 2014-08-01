#include "stdafx.h"
#include "GradientCtrl.h"
#include "GradientColorBoard.h"
#include "GradientCursor.h"
#include "PublicDef.h"
#include "TransparentText.h"

#include <algorithm>

#define Borader 20
#define DefaultBoardSize 60
#define CursorIconSize 15
#define PositionYAdjust 2
#define SelectedCursorRectWidth 2
#define MaxPosTextLength 5
#define MinLength 1

const static int nHalfIconSize = CursorIconSize / 2;

BEGIN_EVENT_TABLE(CGradientCtrl, wxPanel)
    EVT_PAINT(CGradientCtrl::OnPaint)
    EVT_SIZE(CGradientCtrl::OnSize)
    EVT_MOUSE_EVENTS(CGradientCtrl::OnMouse)
    EVT_TEXT(wxID_ANY, CGradientCtrl::OnInputPos)
    EVT_TEXT_ENTER(wxID_ANY, CGradientCtrl::OnSetPos)
    EVT_COLOURPICKER_CHANGED(wxID_ANY, CGradientCtrl::OnChooseColor)
    EVT_COMMAND_SCROLL(wxID_ANY, CGradientCtrl::OnSliderScroll)
END_EVENT_TABLE()

bool lessmark(const CGradientCursor* s1, const CGradientCursor* s2)
{
    return s1->GetPosPercent() < s2->GetPosPercent();
}

CGradientCtrl::CGradientCtrl(wxWindow *pParent, wxWindowID id , const wxPoint& pos , const wxSize& size , long style , const wxString& name)
    : wxPanel(pParent, id, pos, size, style, name)
    , m_pSelectedCursor(NULL)
{
    InitCtrl();
}

CGradientCtrl::~CGradientCtrl()
{
    for ( auto iter : m_colorCursorList )
    {
        BEATS_SAFE_DELETE( iter );
    }
    for ( auto iter : m_maskCursorList )
    {
        BEATS_SAFE_DELETE( iter );
    }

    m_colorCursorList.clear();
    m_maskCursorList.clear();
}

void CGradientCtrl::InitCtrl()
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pCtrlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pGradientColorBoard = new CGradientColorBoard(this, wxID_ANY, wxDefaultPosition, wxSize(DefaultBoardSize, DefaultBoardSize));
    m_pCtrlPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(300, 60));

    CTransparentText* pStaticText = new CTransparentText(m_pCtrlPanel, wxID_ANY, wxT("%"));
    m_pSetPosButton = new wxButton(m_pCtrlPanel, wxID_ANY, wxT("Set Pos"));
    m_pColoutPicker = new wxColourPickerCtrl(m_pCtrlPanel, wxID_ANY);
    m_pMaskSlider = new wxSlider(m_pCtrlPanel, wxID_ANY, 255, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
    m_pPosEdit = new wxTextCtrl(m_pCtrlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, 20), wxTE_PROCESS_ENTER);
    m_pPosEdit->SetMaxLength(MaxPosTextLength);

    pCtrlSizer->Add(m_pColoutPicker, 0, wxALIGN_CENTER|wxALL, 0);
    pCtrlSizer->Add(m_pMaskSlider, 0, wxALIGN_CENTER|wxALL, 0);
    pCtrlSizer->AddStretchSpacer(1);
    pCtrlSizer->Add(m_pSetPosButton, 0, wxALIGN_CENTER|wxALL, 5);
    pCtrlSizer->Add(m_pPosEdit, 0, wxALIGN_CENTER|wxALL, 5);
    pCtrlSizer->Add(pStaticText, 0, wxALIGN_CENTER|wxALL, 0);

    pSizer->AddStretchSpacer(1);
    pSizer->Add(m_pGradientColorBoard, 0, wxGROW|wxALL, 20);
    pSizer->Add(m_pCtrlPanel, 0, wxGROW|wxALL, 20);
    pSizer->AddStretchSpacer(1);
    m_pCtrlPanel->SetSizer(pCtrlSizer);
    SetSizer(pSizer);

    m_pColoutPicker->Hide();
    m_pMaskSlider->Hide();
    wxColor cWhite = *wxWHITE;
    wxColor c2 = *wxYELLOW;
    wxColor c3 = *wxBLACK;
    m_colorCursorList.push_back(new CGradientCursor(0.0, cWhite, eCT_Color));
    m_colorCursorList.push_back(new CGradientCursor(1.0, c2, eCT_Color));
    m_maskCursorList.push_back(new CGradientCursor(0.0, cWhite, eCT_Alpha));
    m_maskCursorList.push_back(new CGradientCursor(1.0, c3, eCT_Alpha));
}

void CGradientCtrl::AddCursor(CGradientCursor* pCursor)
{
    ECursorType eType = pCursor->GetType();
    BEATS_ASSERT(eType != eCT_Invalid)
    if (eType == eCT_Color)
    {
        m_colorCursorList.push_back(pCursor);
        std::sort(m_colorCursorList.begin(), m_colorCursorList.end(), lessmark);
    }
    else if (eType == eCT_Alpha)
    {
        m_maskCursorList.push_back(pCursor);
        std::sort(m_maskCursorList.begin(), m_maskCursorList.end(), lessmark);
    }
    
    Refresh(true);
}

void CGradientCtrl::DeleteCursor(CGradientCursor* pCursor)
{
    ECursorType eType = pCursor->GetType();
    std::vector<CGradientCursor*>* plist = NULL;
    if (eType == eCT_Color)
    {
        plist = &m_colorCursorList;
    }
    else if (eType == eCT_Alpha)
    {
        plist = &m_maskCursorList;
    }

    BEATS_ASSERT(plist != NULL);
    for (auto itr = plist->begin(); itr != plist->end(); itr++)
    {
        if (*itr == pCursor)
        {
            BEATS_SAFE_DELETE(pCursor);
            plist->erase(itr);
            break;
        }
    }
    Refresh(true);
}

void CGradientCtrl::DrawColorBoard()
{
    wxSize boardSize = m_pGradientColorBoard->GetSize();
    wxBitmap colorBmp(boardSize);
    wxBitmap maskBmp(boardSize);
    
    DrawCursorDataToBmp(m_colorCursorList, colorBmp);
    DrawCursorDataToBmp(m_maskCursorList, maskBmp);

    m_pGradientColorBoard->SetBmp(colorBmp, maskBmp);
    m_pGradientColorBoard->Refresh(false);
}

void CGradientCtrl::DrawCursorDataToBmp(std::vector<CGradientCursor*>& list, wxBitmap& bmp)
{
    CGradientCursor* pPreCursor = *list.begin();
    CGradientCursor* pLastCursor = *list.rbegin();
    wxSize size = bmp.GetSize();
    BEATS_ASSERT(pPreCursor != nullptr);
    BEATS_ASSERT(pLastCursor != nullptr);

    {
        wxMemoryDC dc(bmp);
        int nWidth = INVALID_DATA;
        for (auto itr : list)
        {
            if (pPreCursor == itr)
            {
                nWidth = itr->GetPosPercent() * size.x;
                dc.SetBrush(wxBrush(itr->GetColor()));
                dc.SetPen(wxPen(itr->GetColor()));
                dc.DrawRectangle(wxRect(0, 0, nWidth + 1 , size.y));
            }
            else if (pLastCursor == itr)
            {
                nWidth = (1.0 - itr->GetPosPercent()) * size.x;
                dc.SetBrush(wxBrush(itr->GetColor()));
                dc.SetPen(wxPen(itr->GetColor()));
                dc.DrawRectangle(wxRect(itr->GetPosPercent() * size.x, 0, nWidth + 1 , size.y));
            }
            nWidth = (itr->GetPosPercent() - pPreCursor->GetPosPercent()) * size.x;
            wxRect rect = wxRect(pPreCursor->GetPosPercent() * size.x, 0, nWidth + 1 , size.y);
            dc.GradientFillLinear(rect, pPreCursor->GetColor(), itr->GetColor());
            pPreCursor = itr;
        }
    }
}

void CGradientCtrl::DrawCursorIcons()
{
    wxRect rect = m_pGradientColorBoard->GetRect();
    m_colorIconRect = wxRect(rect.x - nHalfIconSize, rect.GetBottom(), rect.width + CursorIconSize, CursorIconSize);
    m_maskIconRect = wxRect(rect.x - nHalfIconSize, rect.GetTop() - CursorIconSize, rect.width + CursorIconSize,  CursorIconSize);
    wxPaintDC dc(this);
//     //show mouse click response range
//     dc.DrawRectangle(m_colorIconRect);
//     dc.DrawRectangle(m_maskIconRect);
    wxBrush brush;
    wxPen pen = *wxBLUE_PEN;
    pen.SetWidth(SelectedCursorRectWidth);
    for (auto itr : m_colorCursorList)
    {
        brush.SetColour(itr->GetColor());
        dc.SetBrush(brush);
        dc.SetPen(itr->IsSelected() ? pen : *wxBLACK_PEN);
        int nPositionX = itr->GetPosPercent() * rect.width - nHalfIconSize + rect.x;
        dc.DrawRectangle(nPositionX, rect.GetBottom() + PositionYAdjust, CursorIconSize, CursorIconSize);
    }

    for (auto itr : m_maskCursorList)
    {
        brush.SetColour(itr->GetColor());
        dc.SetBrush(brush);
        dc.SetPen(itr->IsSelected() ? pen : *wxBLACK_PEN);
        int nPositionX = itr->GetPosPercent() * rect.width - nHalfIconSize + rect.x;
        dc.DrawRectangle(nPositionX, rect.GetTop() - CursorIconSize - 1, CursorIconSize, CursorIconSize);
    }
}


void CGradientCtrl::OnPaint(wxPaintEvent& /*event*/)
{
    DrawCursorIcons();
    DrawColorBoard();
}

void CGradientCtrl::OnMouse(wxMouseEvent& event)
{
    ECursorType eType = eCT_Invalid;
    wxPoint pos = event.GetPosition();
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        if (m_pSelectedCursor)
        {
            m_pSelectedCursor->Select(false);
        }
        m_pSelectedCursor = GetGradientCursor(pos);
        if (m_pSelectedCursor)
        {
            eType = m_pSelectedCursor->GetType();
            if (eType == eCT_Color)
            {
                m_pColoutPicker->SetColour(m_pSelectedCursor->GetColor());
            }
            else
            {
                m_pMaskSlider->SetValue(m_pSelectedCursor->GetColor().Red());
            }
            m_pSelectedCursor->Select(true);
        }
        ShowCtrl(m_pSelectedCursor == NULL ? eCT_Invalid : m_pSelectedCursor->GetType());
        Refresh(true);
        UpdateSelectedCursorPos();
    }
    else if (event.Dragging())
    {
        if (event.LeftIsDown())
        {
            if (m_pSelectedCursor)
            {
                SetSelectedCursorPos(pos);
                UpdateSelectedCursorPos();
                float fPos = GetNearestCursorPos(m_pSelectedCursor);
                if (fPos >= 0)
                {
                    m_pSelectedCursor->SetPos(fPos);
                }
                Refresh(true);
            }
        }
    }
    else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        if (m_pSelectedCursor)
        {
            if (GetNearestCursorPos(m_pSelectedCursor) > 0)
            {
                DeleteCursor(m_pSelectedCursor);
                m_pSelectedCursor = NULL;
            }
        }
    }
    
}

CGradientCursor* CGradientCtrl::GetGradientCursor(wxPoint point)
{
    CGradientCursor* pRet = NULL;
    ECursorType eType = eCT_Invalid;
    std::vector<CGradientCursor*>* pList = NULL;
    wxRect rect = m_pGradientColorBoard->GetRect();
    if (m_colorIconRect.Contains(point))
    {
        eType = eCT_Color;
        pList = &m_colorCursorList;
    }
    if (m_maskIconRect.Contains(point))
    {
        eType = eCT_Alpha;
        pList = &m_maskCursorList;
    }
    if (eType != eCT_Invalid)
    {
        BEATS_ASSERT(pList != NULL);
        for (auto itr : *pList)
        {
            wxRect iconRect;
            int nPositionX = itr->GetPosPercent() * rect.width - nHalfIconSize + rect.x;
            if (eType == eCT_Color)
            {
                iconRect = wxRect(nPositionX, rect.GetBottom(), CursorIconSize, CursorIconSize);
            }
            else if (eType == eCT_Alpha)
            {
                iconRect = wxRect(nPositionX, rect.GetTop() - CursorIconSize, CursorIconSize, CursorIconSize);
            }

            if (iconRect.Contains(point))
            {
                pRet = itr;
                break;
            }
        }

        if (pRet == NULL)
        {
            float fPos = CalCursorPos(point);
            pRet = new CGradientCursor(fPos, m_pGradientColorBoard->GetColor(rect.width * fPos, eType), eType);
            AddCursor(pRet);
        }
    }
    return pRet;
}

void CGradientCtrl::SetSelectedCursorPos(wxPoint point)
{
    if (m_pSelectedCursor)
    {
        float fPos = CalCursorPos(point);
        m_pSelectedCursor->SetPos(fPos);
    }
}

float CGradientCtrl::CalCursorPos(wxPoint point)
{
    wxRect rect = m_pGradientColorBoard->GetRect();
    float fPos = (point.x - rect.x) / (float)rect.width;
    fPos = fPos < 0.0 ? 0.0 : fPos;
    fPos = fPos > 1.0 ? 1.0 : fPos;
    return fPos;
}

void CGradientCtrl::ShowCtrl(ECursorType eType)
{
    bool bShowPicker = false;
    bool bShowSlider = false;
    if (eType == eCT_Color)
    {
        bShowPicker = true;
    }
    if (eType == eCT_Alpha)
    {
        bShowSlider = true;
    }
    m_pColoutPicker->Show(bShowPicker);
    m_pMaskSlider->Show(bShowSlider);
    m_pCtrlPanel->GetSizer()->RecalcSizes();
}

void CGradientCtrl::OnSize(wxSizeEvent& event)
{
    Refresh(true);
    event.Skip();
}

void CGradientCtrl::OnChooseColor(wxColourPickerEvent& event)
{
    BEATS_ASSERT(m_pSelectedCursor != nullptr);
    BEATS_ASSERT(m_pSelectedCursor->GetType() == eCT_Color);
    m_pSelectedCursor->SetColor(event.GetColour());
    Refresh(true);
}

void CGradientCtrl::OnSliderScroll(wxScrollEvent& event)
{
    BEATS_ASSERT(m_pSelectedCursor != nullptr);
    BEATS_ASSERT(m_pSelectedCursor->GetType() == eCT_Alpha);
    int value = event.GetPosition();
    m_pSelectedCursor->SetColor(wxColor(value, value, value));
    Refresh(true);
}

void CGradientCtrl::OnInputPos(wxCommandEvent& /*event*/)
{
    double val;
    m_pSetPosButton->Enable(m_pPosEdit->GetValue().ToDouble(&val) && val >= 0.0);
}

void CGradientCtrl::OnSetPos(wxCommandEvent& /*event*/)
{
    if (m_pSetPosButton->IsEnabled())
    {
        double val;
        BEATS_ASSERT(m_pSelectedCursor != nullptr);
        m_pPosEdit->GetValue().ToDouble(&val);
        val /= 100.0;
        m_pSelectedCursor->SetPos(val);
        Refresh(true);
    }
}

void CGradientCtrl::UpdateSelectedCursorPos()
{
    if (m_pSelectedCursor)
    {
        wxString textPos = wxString::Format(_T("%3.2f"), m_pSelectedCursor->GetPosPercent() * 100);
        m_pPosEdit->SetLabelText(textPos.Left(MaxPosTextLength));
    }
}

float CGradientCtrl::GetNearestCursorPos(CGradientCursor* pCursor)
{
    BEATS_ASSERT(pCursor);
    float fRet = INVALID_DATA;
    float fCurrentCursorPos = pCursor->GetPosPercent();
    std::vector<CGradientCursor*>* pList = NULL;
    if (pCursor->GetType() == eCT_Color)
    {
        pList = &m_colorCursorList;
    }
    else
    {
        pList = &m_maskCursorList;
    }

    for (auto itr : *pList)
    {
        if (itr != pCursor)
        {
            float fPosAbs = fabs(fCurrentCursorPos - itr->GetPosPercent()) * 100.0f ;
            if (fPosAbs < MinLength)
            {
                fRet = itr->GetPosPercent();
            }
        }
    }
    return fRet;
}

std::vector<CGradientCursor*>& CGradientCtrl::GetColorList()
{
    return m_colorCursorList;
}

std::vector<CGradientCursor*>& CGradientCtrl::GetMaskList()
{
    return m_maskCursorList;
}

wxColor CGradientCtrl::GetColorByPos(float fPos)
{
    static int nPositionY = 1;
    BEATS_ASSERT(fPos <= 1.0f);
    wxRect rect = m_pGradientColorBoard->GetRect();
    return m_pGradientColorBoard->GetColorWithAlpha(wxPoint(rect.width * fPos, nPositionY));
}
