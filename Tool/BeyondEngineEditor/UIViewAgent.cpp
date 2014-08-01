#include "stdafx.h"
#include "UIViewAgent.h"
#include "EditorMainFrame.h"
#include "wxWidgetsPropertyBase.h"
#include "GUI/WindowManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "GUI/Window/Control.h"
#include "Render/RenderManager.h"
#include "WxGLRenderWindow.h"

CUIViewAgent* CUIViewAgent::m_pInstance = nullptr;
CUIViewAgent::CUIViewAgent()
{

}

CUIViewAgent::~CUIViewAgent()
{

}

void CUIViewAgent::ProcessMouseEvent( wxMouseEvent& event )
{
    __super::ProcessMouseEvent(event);
    
    wxPoint MousePos = event.GetPosition();
    CRenderTarget* pRenderWnd = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderWnd);
    BEATS_ASSERT(pSceneRenderWnd != NULL);
    float fScale = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetWidth() : pSceneRenderWnd->GetScaleFactor();

    MousePos.x /= fScale;
    MousePos.y /= fScale;
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        if (m_pMainFrame->m_nCursorIconID == wxCURSOR_ARROW)
        {
            CComponentBase* pSelectedComponent = CWindowManager::GetInstance()->GetControl(MousePos.x, MousePos.y);
            CComponentProxy* pProxy = NULL;
            if (pSelectedComponent)
            {
                pProxy = (CComponentProxy*)CComponentProxyManager::GetInstance()->GetComponentInstance(pSelectedComponent->GetId(), pSelectedComponent->GetGuid());
            }

            if (m_pMainFrame->m_pSelectedComponentProxy != pProxy)
            {
                m_pMainFrame->SelectComponent(pProxy);
            }
        }

        if (m_pMainFrame->m_pSelectedComponentProxy != NULL)
        {
            kmVec2 kmVec2Pos;
            kmVec2Fill(&kmVec2Pos, 0, 0);
            ((CControl*)m_pMainFrame->m_pSelectedComponentProxy->GetHostComponent())->LocalToWorld(kmVec2Pos.x, kmVec2Pos.y);
            m_pMainFrame->m_activeGUIPosOffset = wxPoint(kmVec2Pos.x - MousePos.x, kmVec2Pos.y - MousePos.y);
        }
        UpdateCursorIcon(MousePos);
        ((wxWindow*)m_pMainFrame->m_pViewScreen)->SetCursor(m_pMainFrame->m_nCursorIconID);
    }
    else if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        m_pMainFrame->SelectComponent(NULL);
        UpdateCursorIcon(MousePos);
        ((wxWindow*)m_pMainFrame->m_pViewScreen)->SetCursor(m_pMainFrame->m_nCursorIconID);
    }
    else if(event.Dragging())
    {
        if (m_pMainFrame->m_pSelectedComponentProxy)
        {
            wxString propertyName;
            int nHitTestRet = m_pMainFrame->m_nHitTestRet;
            if (nHitTestRet == CControl::eHTR_HIT_CONTENT || nHitTestRet == CControl::eHTR_HIT_EDGE)
            {
                propertyName = _T("m_pos");
            }
            else if (nHitTestRet > CControl::eHTR_HIT_CONTENT && nHitTestRet < CControl::eHTR_HIT_ANCHOR)
            {
                propertyName = _T("m_vecSize");
            }
            else
            {
                propertyName = _T("");
            }

            if (!propertyName.IsEmpty())
            {
                CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(m_pMainFrame->m_pSelectedComponentProxy->GetPropertyDescription(propertyName));
                BEATS_ASSERT(pPropertyDescription != NULL);
                CControl* pWindow = dynamic_cast<CControl*>(m_pMainFrame->m_pSelectedComponentProxy->GetHostComponent());
                BEATS_ASSERT(pWindow != NULL);
                if (nHitTestRet == CControl::eHTR_HIT_CONTENT || nHitTestRet == CControl::eHTR_HIT_EDGE)
                {
                    kmScalar posX = MousePos.x + m_pMainFrame->m_activeGUIPosOffset.x;
                    kmScalar posY = MousePos.y + m_pMainFrame->m_activeGUIPosOffset.y;
                    kmVec2 pos = pWindow->CalcPosFromMouse(posX, posY);
                    SetkmVec2PropertyValue(pPropertyDescription, pos);
                }
                else if (nHitTestRet > CControl::eHTR_HIT_CONTENT && nHitTestRet < CControl::eHTR_HIT_ANCHOR)
                {
                    kmVec2 size = pWindow->CalcSizeFromMouse(MousePos.x, MousePos.y, (CControl::EHitTestResult)nHitTestRet);
                    SetkmVec2PropertyValue(pPropertyDescription, size);
                }
            }
        }
    }
    else if(event.Moving())
    {
        UpdateCursorIcon(MousePos);
    }
}

void CUIViewAgent::SetkmVec2PropertyValue(CWxwidgetsPropertyBase* pPropertyDescription, kmVec2& value)
{
    wxPGProperty* pProperty = m_pMainFrame->m_pPropGridManager->GetGrid()->GetProperty(pPropertyDescription->GetBasicInfo()->m_variableName);
    BEATS_ASSERT(pProperty->GetChildCount() == 3 || pProperty->GetChildCount() == 2);
    wxVariant xValue = value.x;
    wxVariant yValue = value.y;
    // 1. Update the value in the property grid.
    pProperty->Item(0)->SetValue(xValue);
    pProperty->Item(1)->SetValue(yValue);
    // 2. Update the value in description.
    ((CWxwidgetsPropertyBase*)(pPropertyDescription->GetChild(0)))->SetValue(xValue, false);
    ((CWxwidgetsPropertyBase*)(pPropertyDescription->GetChild(1)))->SetValue(yValue, false);
    m_pMainFrame->m_pPropGridManager->OnComponentPropertyChangedImpl(pProperty);
}

void CUIViewAgent::UpdateCursorIcon(wxPoint& mousePos)
{
    wxStockCursor nCursorIconID = wxCURSOR_ARROW;
    if (m_pMainFrame->m_pSelectedComponentProxy)
    {
        
        CComponentBase* pCurrentHitedComponent = CWindowManager::GetInstance()->GetControl(mousePos.x, mousePos.y);
        CComponentBase* pSelectedComponent = m_pMainFrame->m_pSelectedComponentProxy->GetHostComponent();
        if (pCurrentHitedComponent == pSelectedComponent)
        {
            m_pMainFrame->m_nHitTestRet = down_cast<CControl*>(pSelectedComponent)->GetHitResult(mousePos.x, mousePos.y);
            switch (m_pMainFrame->m_nHitTestRet)
            {
            case CControl::eHTR_HIT_CONTENT:
            case CControl::eHTR_HIT_ANCHOR:
            case CControl::eHTR_HIT_EDGE:
                nCursorIconID = wxCURSOR_CROSS;
                break;
            case CControl::eHTR_HIT_LEFT_CENTER:
            case CControl::eHTR_HIT_RIGHT_CENTER:
                nCursorIconID = wxCURSOR_SIZEWE;
                break;
            case CControl::eHTR_HIT_TOP_CENTER:
            case CControl::eHTR_HIT_BOTTOM_CENTER:
                nCursorIconID = wxCURSOR_SIZENS;
                break;
            case CControl::eHTR_HIT_TOP_LEFT:
            case CControl::eHTR_HIT_BOTTOM_RIGHT:
                nCursorIconID = wxCURSOR_SIZENWSE;
                break;
            case CControl::eHTR_HIT_TOP_RIGHT:
            case CControl::eHTR_HIT_BOTTOM_LEFT:
                nCursorIconID = wxCURSOR_SIZENESW;
                break;
            case CControl::eHTR_HIT_ROTATION_BAR:
                nCursorIconID = wxCURSOR_HAND;
                break;
            default:
                nCursorIconID = wxCURSOR_ARROW;
                break;
            }
        }
    }
    m_pMainFrame->m_nCursorIconID = nCursorIconID;
}

void CUIViewAgent::InView()
{
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Show();
    m_pMainFrame->SelectComponent(NULL);
}

void CUIViewAgent::OutView()
{
    m_pMainFrame->m_pToolBook->DeleteAllPages();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Hide();
    m_pMainFrame->m_Manager.Update();
    if(m_pMainFrame->m_pSelectedComponentProxy)
    {
        CControl *pWindow = dynamic_cast<CControl*>(
            m_pMainFrame->m_pSelectedComponentProxy->GetHostComponent());
        if(pWindow)
        {
            pWindow->SetEditorSelect(false);
        }
    }
}

void CUIViewAgent::SelectComponent( CComponentProxy* pComponentInstance )
{
    if (m_pMainFrame->m_pSelectedComponentProxy)
    {
        CControl* pWindow = dynamic_cast<CControl*>(m_pMainFrame->m_pSelectedComponentProxy->GetHostComponent());
        if (pWindow != NULL)
        {
            pWindow->SetEditorSelect(false);
        }
    }
    if (pComponentInstance != NULL)
    {
        CControl* pWindow = dynamic_cast<CControl*>(pComponentInstance->GetHostComponent());
        if (pWindow != NULL)
        {
            pWindow->SetEditorSelect(true);
        }
    }
}
