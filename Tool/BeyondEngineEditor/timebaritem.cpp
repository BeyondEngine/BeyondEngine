#include "stdafx.h"
#include <wx/dcbuffer.h>
#include <wx/popupwin.h>
#include "timebaritem.h"
#include "timebaritemcontainer.h"
#include "timebarframe.h"
#include "EditAnimationDialog.h"
#include "Render/AnimationController.h"
#include "NodeAnimation/NodeAnimationElement.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "MapPropertyDescription.h"
#include "Vec3fPropertyDescription.h"
#include "NodeAnimation/NodeAnimationData.h"
#include "UIntPropertyDescription.h"
#include "NodeAnimation/NodeAnimation.h"

#define DATACELLHIGHT 18
#define NODATACELLHIGHT 24
#define DATACELLOFFSET 3
#define WIDTHADJUSTMENT 1
#define POSITIONADJUSTMENT -1
enum ETimeBarMenuOrder
{
    eTBMO_AddKey,
    eTBMO_RemoveKey,

    eTBMO_Count,
    eTBMO_Force32Bit = 0xFFFFFFFF
};
ArrayOfTimeBarItem::~ArrayOfTimeBarItem()
{

}

CTimeBarItem::CTimeBarItem( wxWindow *parent, wxWindowID id, 
                           const wxPoint &pos, const wxSize &size, 
                           long style, const wxString &name )
                           : wxPanel(parent, id, pos, size, style , name),
                           m_iDataBegin(INVALID_DATA),
                           m_iDataEnd(INVALID_DATA)
                           , m_pElement(NULL)
{
    SetMinSize(wxSize(0, CELLHIGHT));
    m_pPopUpWindow = new wxPopupTransientWindow(this);
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    m_pPopUpText = new wxStaticText(m_pPopUpWindow, wxID_ANY, wxEmptyString);
    topSizer->Add( m_pPopUpText, 0, wxALL, 5 );
    m_pPopUpWindow->SetSizer(topSizer);
    topSizer->Fit(m_pPopUpWindow);
}

CTimeBarItem::~CTimeBarItem()
{
}

BEGIN_EVENT_TABLE(CTimeBarItem, wxPanel)
    EVT_PAINT(CTimeBarItem::OnPaint)
    EVT_MOUSE_EVENTS(CTimeBarItem::OnMouse)
    EVT_MOUSE_CAPTURE_LOST(CTimeBarItem::OnMouseCaptureLost)
    END_EVENT_TABLE()

    void CTimeBarItem::OnMouseCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    UnSelecteAll();
}

void CTimeBarItem::OnPaint(wxPaintEvent& /*event*/)
{
    static const size_t GrayCellOffset = 5;
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxAutoBufferedPaintDC ScaleDC(this); 
    wxRect ItemRect = GetRect();
    CTimeBarItemContainer* pContainer = (CTimeBarItemContainer*)GetParent(); 
    wxPoint ViewStart = pContainer->GetViewStart();
    wxSize size = pContainer->GetSize();
    int cursorpositionx = pContainer->GetTimeBarFrameWindow()->GetCursorPositionX();

    int iWidith = pContainer->GetCellWidth();
    int iCount = pContainer->GetTimeBarFrameWindow()->GetFrameCount();
    int iDrawRectWdith = iWidith +1;
    int iPositionX = -1;
    int iDefaultCellHeight = ItemRect.height + 1;
    int iCellDataHeight = ItemRect.height - 2 * DATACELLOFFSET;

    ScaleDC.SetPen(*wxLIGHT_GREY);
    ScaleDC.DrawRectangle(ItemRect);
    for(int i = ViewStart.x; i < iCount; i++)
    {
        iPositionX = i * iWidith;
        CVec3 data;
        bool bHasData = GetData(i, data);
        bool bHasSelected = HasSelected(i);
        if (!bHasData)
        {
            if (bHasSelected)
            {
                ScaleDC.SetBrush(*wxBLUE);
                ScaleDC.DrawRectangle(iPositionX, 0, iDrawRectWdith, iDefaultCellHeight);
                ScaleDC.SetBrush(*wxWHITE);
            }
            else
            {
                DrawDefaultItem(&ScaleDC, i, iPositionX, iDrawRectWdith, iDefaultCellHeight);
            }
        }
        else
        {
            DrawDefaultItem(&ScaleDC, i, iPositionX, iDrawRectWdith, iDefaultCellHeight);
            ScaleDC.SetBrush(bHasSelected ? *wxBLUE : *wxGREEN);
            ScaleDC.DrawRectangle(iPositionX, DATACELLOFFSET, iDrawRectWdith, iCellDataHeight);
        }
    }
    wxPen pen = *wxRED_PEN;
    pen.SetWidth(CURSORWIDTH);
    ScaleDC.SetPen(pen);
    ScaleDC.DrawLine(cursorpositionx, 0, cursorpositionx, ItemRect.height);
}

int CTimeBarItem::GetRowID()
{
    int iRet = -1;
    CTimeBarItemContainer* pParent = (CTimeBarItemContainer*)GetParent();
    int num = pParent->GetItemCount();
    for (int i = 0; i < num; i++)
    {
        if (pParent->GetItem(i) == this)
        {
            iRet = i;
            break;
        }
    }
    return iRet;
}

void CTimeBarItem::OnMouse(wxMouseEvent& event)
{
    CTimeBarItemContainer* pParent = (CTimeBarItemContainer*)GetParent();
    CTimeBarFrame* pFrame = pParent->GetTimeBarFrameWindow();
    wxPoint pstn= event.GetPosition(); 
    int index = PointToCell(pstn.x);
    int iCellWidth = pParent->GetCellWidth();
    int CursorPosition = pstn.x / iCellWidth;
    CursorPosition *= iCellWidth;
    CursorPosition += iCellWidth * 0.5F;
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        pFrame->SetClickOnScalebar(false);
        pParent->GetCurrentSelect().m_iSelectRowBegin = index;
        pParent->GetCurrentSelect().m_iSelectColumnBegin = GetRowID();
        pParent->GetCurrentSelect().m_iSelectRowEnd = index;
        pParent->GetCurrentSelect().m_iSelectColumnEnd = GetRowID();
        pFrame->SetCursorPositionX(CursorPosition);
        pFrame->ClickOnScaleBar();
        CNodeAnimation* pAnimation = pFrame->GetAnimation();
        if (pAnimation)
        {
            pAnimation->SetCurrentFrame(index);
            index = pAnimation->GetCurrentFrame(); // index will be adjusted in SetCurrentFrame.
        }
        CNodeAnimationData* pAnimationData = pFrame->GetAnimationData();
        if (pAnimationData && pAnimation && pAnimation->GetOwner())
        {
            pAnimationData->Apply(pAnimation->GetOwner(), index);
        }
    }
    else if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {   
        if (PointToCell(pFrame->GetCursorPositionX()) != index)
        {
            if (!pFrame->IsClickOnScalebar())
            {
                pParent->GetCurrentSelect().m_iSelectRowEnd = index;
                pParent->GetCurrentSelect().m_iSelectColumnEnd = GetRowID();
            }
            pFrame->SetCursorPositionX(CursorPosition);
            pFrame->ClickOnScaleBar();
        }
    }
    else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        if (event.ControlDown())
        {
            CVec3 data;
            if (GetData(index, data))
            {
                BEATS_ASSERT(m_dataMap.find(index) != m_dataMap.end());
                TCHAR szBuffer[64];
                _stprintf(szBuffer, _T("%g, %g, %g"), data.x, data.y, data.z);
                wxString strNewValue = wxGetTextFromUser(_T("请输入新的值，格式为三个浮点数以逗号分隔"), _T("输入X,Y,Z"), szBuffer);
                if (!strNewValue.empty())
                {
                    std::vector<TString> result;
                    CStringHelper::GetInstance()->SplitString(strNewValue, _T(","), result);
                    if (result.size() != 3)
                    {
                        wxMessageBox(_T("格式有误！"));
                    }
                    else
                    {
                        // Update the proxy and host.
                        CComponentProxy* pProxy = m_pElement->GetProxyComponent();
                        BEATS_ASSERT(pProxy != NULL);
                        CMapPropertyDescription* pMapProperty = dynamic_cast<CMapPropertyDescription*>(pProxy->GetPropertyDescription(_T("m_keyFrames")));
                        size_t uChildrenCount = pMapProperty->GetChildrenCount();
                        bool bFound = false;
                        for (size_t i = 0; i < uChildrenCount; ++i)
                        {
                            CPropertyDescriptionBase* pPropertyChild = pMapProperty->GetChild(i);
                            CUIntPropertyDescription* pKeyProperty = dynamic_cast<CUIntPropertyDescription*>(pPropertyChild->GetChild(0));
                            BEATS_ASSERT(pKeyProperty != NULL);
                            size_t* pValue = (size_t*)(pKeyProperty->GetValue(eVT_CurrentValue));
                            if (*pValue == (size_t)index)
                            {
                                CVec3PropertyDescription* pValueProperty = dynamic_cast<CVec3PropertyDescription* >(pPropertyChild->GetChild(1));
                                pValueProperty->GetChild(0)->GetValueByTChar(result[0].c_str(), pValueProperty->GetChild(0)->GetValue(eVT_CurrentValue));
                                pValueProperty->GetChild(1)->GetValueByTChar(result[1].c_str(), pValueProperty->GetChild(1)->GetValue(eVT_CurrentValue));
                                pValueProperty->GetChild(2)->GetValueByTChar(result[2].c_str(), pValueProperty->GetChild(2)->GetValue(eVT_CurrentValue));
                                CVec3* pData = (CVec3*)&m_dataMap[index];
                                BEATS_ASSERT(pData != NULL);
                                float* pX = (float*)pValueProperty->GetChild(0)->GetValue(eVT_CurrentValue);
                                float* pY = (float*)pValueProperty->GetChild(1)->GetValue(eVT_CurrentValue);
                                float* pZ = (float*)pValueProperty->GetChild(2)->GetValue(eVT_CurrentValue);
                                pData->x = *pX;
                                pData->y = *pY;
                                pData->z = *pZ;
                                bFound = true;
                                break;
                            }
                        }
                        BEATS_ASSERT(bFound);
                        pProxy->UpdateHostComponent();
                    }
                }
            }
        }
        else
        {
            int id = GetRowID();
            pFrame->SetSelectedRow(id);
            CVec3 data;
            if (GetData(index, data))
            {
                m_pPopUpText->SetLabel(wxString::Format(_T("X: %g, Y: %g, Z: %g"), data.x, data.y, data.z));
                m_pPopUpWindow->GetSizer()->Fit(m_pPopUpWindow);
                wxPoint pt = wxGetMousePosition();
                pt.x += 25;// little offset.
                m_pPopUpWindow->SetPosition(pt);
                m_pPopUpWindow->Popup();
            }
        }
    }
    else if (event.ButtonUp(wxMOUSE_BTN_RIGHT))
    {
        pParent->GetCurrentSelect().m_iSelectRowBegin = index;
        pParent->GetCurrentSelect().m_iSelectColumnBegin = GetRowID();
        pParent->GetCurrentSelect().m_iSelectRowEnd = index;
        pParent->GetCurrentSelect().m_iSelectColumnEnd = GetRowID();
        pFrame->SetCursorPositionX(CursorPosition);
        SelectCells(index, index);
        BEATS_ASSERT(m_pElement != NULL);
        CVec3 keyFrameData;
        GetData(index, keyFrameData);
        wxMenu menu;
        if (GetData(index, keyFrameData))
        {
            menu.Append(eTBMO_RemoveKey, _T("移除关键帧"));
        }
        else
        {
            menu.Append(eTBMO_AddKey, _T("插入关键帧"));
        }
        const int selection = GetPopupMenuSelectionFromUser(menu, wxDefaultPosition);
        switch (selection)
        {
        case eTBMO_AddKey:
            {
                m_pElement->AddKeyFrame(index, CVec3());
                // Update the proxy and host.
                CComponentProxy* pProxy = m_pElement->GetProxyComponent();
                CMapPropertyDescription* pMapProperty = dynamic_cast<CMapPropertyDescription*>(pProxy->GetPropertyDescription(_T("m_keyFrames")));
                CPropertyDescriptionBase* pNewProperty = pMapProperty->AddChild(NULL);
                CPropertyDescriptionBase* pKeyProperty = pNewProperty->GetChild(0);
                BEATS_ASSERT(pKeyProperty != NULL);
                pKeyProperty->CopyValue(&index, pKeyProperty->GetValue(eVT_CurrentValue));
                CVec3PropertyDescription* pValueProperty = dynamic_cast<CVec3PropertyDescription* >(pNewProperty->GetChild(1));
                BEATS_ASSERT(pValueProperty != NULL);
                CSerializer serializer;
                serializer << 0.f << 0.f << 0.f;
                pValueProperty->Deserialize(serializer);
                pProxy->UpdateHostComponent();
                AddData(index, m_pElement->GetKeyFrames().find(index)->second);
                Refresh();
            }
            break;
        case eTBMO_RemoveKey:
            {
                m_pElement->RemoveKeyFrame(index);
                // Update the proxy and host.
                CComponentProxy* pProxy = m_pElement->GetProxyComponent();
                BEATS_ASSERT(pProxy != NULL);
                CMapPropertyDescription* pMapProperty = dynamic_cast<CMapPropertyDescription*>(pProxy->GetPropertyDescription(_T("m_keyFrames")));
                size_t uChildrenCount = pMapProperty->GetChildrenCount();
                bool bFound = false;
                for (size_t i = 0; i < uChildrenCount; ++i)
                {
                    CPropertyDescriptionBase* pPropertyChild = pMapProperty->GetChild(i);
                    CUIntPropertyDescription* pKeyProperty = dynamic_cast<CUIntPropertyDescription*>(pPropertyChild->GetChild(0));
                    BEATS_ASSERT(pKeyProperty != NULL);
                    size_t* pValue = (size_t*)(pKeyProperty->GetValue(eVT_CurrentValue));
                    if (*pValue == (size_t)index)
                    {
                        pMapProperty->DeleteChild(pPropertyChild);
                        bFound = true;
                        break;
                    }
                }
                BEATS_ASSERT(bFound);
                pProxy->UpdateHostComponent();
                RemoveData(index);
                Refresh();
            }
            break;
        default:
            break;
        }
    }
    event.Skip();
};

int CTimeBarItem::PointToCell(int pos)
{
    CTimeBarItemContainer* pParent = (CTimeBarItemContainer*)GetParent();
    return pos / pParent->GetCellWidth();
}

wxPoint CTimeBarItem::CellToPoint(int index)
{
    CTimeBarItemContainer* pParent = (CTimeBarItemContainer*)GetParent();
    return wxPoint(index * pParent->GetCellWidth(), 0);
}

void CTimeBarItem::UnSelecteAll()
{
    m_iSelectionEnd = INVALID_DATA;
}

void CTimeBarItem::SelectCell( int index, bool /*bselect*/ )
{
    if (HasSelected(index))
    {
        SelectCells(index, index);
    }
}

void CTimeBarItem::SelectCells(int start, int end)
{
    m_iSelectionBegin = start;
    m_iSelectionEnd = end;
}

wxColour CTimeBarItem::GetColour(int ColourID)
{
    wxColour color = *wxWHITE;
    switch (ColourID)
    {
    case eIC_BACKGROUNDCOLOUR2:
        color = *wxLIGHT_GREY;
        break;

    case eIC_DATACELLCOLOUR:
        color = *wxGREEN;
        break;

    case eIC_SELECTEDCOLOUR:
        color = *wxBLUE;
        break;

    case eIC_BORDERCOLOUR:
        color = *wxLIGHT_GREY;
        break;

    case eIC_BACKGROUNDCOLOUR1:
    default:
        break;
    }

    return color;
}

void CTimeBarItem::AddData(size_t uFrame, const CVec3& data)
{
    BEATS_ASSERT(m_dataMap.find(uFrame) == m_dataMap.end());
    m_dataMap[uFrame] = data;
}

void CTimeBarItem::RemoveData(size_t uFrame)
{
    BEATS_ASSERT(m_dataMap.find(uFrame) != m_dataMap.end());
    m_dataMap.erase(uFrame);
}

bool CTimeBarItem::GetData(size_t uFrame, CVec3& data)
{
    bool bRet = false;
    auto iter = m_dataMap.find(uFrame);
    if (iter != m_dataMap.end())
    {
        data = iter->second;
        bRet = true;
    }
    return bRet;
}

void CTimeBarItem::SetAnimationElement(CNodeAnimationElement* pElement)
{
    m_pElement = pElement;
}

CNodeAnimationElement* CTimeBarItem::GetAnimationElement() const
{
    return m_pElement;
}

bool CTimeBarItem::HasSelected( int iFrameIndex )
{
    bool bRet = iFrameIndex >= m_iSelectionBegin && iFrameIndex <= m_iSelectionEnd;
    return bRet;
}

void CTimeBarItem::SetCellsDataRange( int iDataBegin, int iDataEnd )
{
    m_iDataBegin = iDataBegin;
    m_iDataEnd = iDataEnd;
    BEATS_ASSERT(iDataEnd >= iDataBegin);
}

void CTimeBarItem::DrawDefaultItem( wxDC* pDC, int iIndex, int iPositionX, int iWidth, int iHeight )
{
    static const size_t GrayCellOffset = 5;
    pDC->SetBrush(iIndex % GrayCellOffset == 0 ? *wxLIGHT_GREY : *wxWHITE);
    pDC->DrawRectangle(iPositionX, 0, iWidth, iHeight);
}
