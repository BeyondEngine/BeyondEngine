#include "stdafx.h"
#include "AniViewAgent.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "NodeAnimation/NodeAnimationData.h"
#include "timebarframe.h"
#include "NodeAnimation/NodeAnimationElement.h"
#include "NodeAnimation/NodeAnimation.h"
#include "Render/RenderManager.h"
#include "Render/Camera.h"
#include "WxGLRenderWindow.h"
#include "Render/AtlasSprite.h"
#include "TimeBarFrameData.h"
#include "wx/msw/private.h"
#include "Render/SkeletonAnimation.h"
#include "Render/AnimationController.h"
#include "MapPropertyDescription.h"
#include "EditorSceneWindow.h"
#include "MapElementPropertyDescription.h"
#include "Render/CoordinateRenderObject.h"

CAniViewAgent* CAniViewAgent::m_pInstance = nullptr;
CAniViewAgent::CAniViewAgent()
: m_pActiveSprite(NULL)
, m_pParentOfActiveSprite(NULL)
, m_pModel(NULL)
, m_pAnimation(NULL)
, m_pCopyFramePropertyDescription(NULL)
{
}

CAniViewAgent::~CAniViewAgent()
{

}

void CAniViewAgent::InView()
{
    m_spriteDataMap.clear();
    m_pActiveSprite = NULL;
    m_pModel = NULL;
    m_pAnimation = NULL;
    m_pMainFrame->m_pTimeBar->Clear();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pBottomPanel).Show();
    m_pMainFrame->m_Manager.Update();
    m_pMainFrame->m_pTimeBar->SetFps(16);
    m_pParentOfActiveSprite = nullptr;
}

void CAniViewAgent::OutView()
{
    m_spriteDataMap.clear();
    m_animationDataMap.clear();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pBottomPanel).Hide();
    m_pMainFrame->m_Manager.Update();
    if (m_pParentOfActiveSprite)
    {
        m_pParentOfActiveSprite->m_bRenderUserdefinePos = false;
    }
}

void CAniViewAgent::ProcessMouseEvent(wxMouseEvent& event)
{
    CommonHandleMouse(event);
    if (!event.GetSkipped())
    {
        CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
        BEATS_ASSERT(pRenderTarget != NULL);
        wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
        BEATS_ASSERT(pSceneRenderWnd != NULL);
        float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetLogicWidth() : pSceneRenderWnd->GetScaleFactor();
        // Simulate the scene move logic
        if (event.LeftIsDown())
        {
            const CVec2 curPos = CVec2(event.GetX() / fScaleFactor, event.GetY() / fScaleFactor);
            CCamera *pCamera = m_pGLWindow->GetCamera();
            CVec3 newCameraPos = pCamera->QueryCameraPos(m_startMoveWorldPos, curPos, pCamera->GetViewMatrix(),
                pCamera->GetNear(), pCamera->GetFOV(), pCamera->GetViewPos().Y());
            pCamera->SetViewPos(newCameraPos);
        }

        if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
        {
            if (m_pActiveSprite != NULL)
            {
                CSprite* pSprite = m_pActiveSprite->GetCurrentSprite();
                if (pSprite != NULL)
                {
                    CCamera* pCamera = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D);
                    CVec3 vec3Pos = pSprite->GetWorldPosition();
                    CVec2 pos = pCamera->WorldToScreen(vec3Pos);
                    m_mouseOffset = wxPoint(pos.X() - event.GetPosition().x, pos.Y() - event.GetPosition().y);
                }
            }
        }
        else if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_RIGHT))
        {
            if (m_pActiveSprite != NULL)
            {
                CSprite* pSprite = m_pActiveSprite->GetCurrentSprite();
                if (pSprite != NULL)
                {
                    CCamera* pCamera = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D);
                    CVec3 pos = pCamera->RayCast(event.GetPosition().x + m_mouseOffset.x, event.GetPosition().y + m_mouseOffset.y);
                    CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(pSprite->GetProxyComponent()->GetProperty(_T("m_pos")));
                    BEATS_ASSERT(pPropertyDescription != NULL);
                    CVec3 vec3Pos;
                    vec3Pos.X() = pos.X();
                    vec3Pos.Y() = pos.Y();
                    vec3Pos.Z() = pos.Z();
                    UPDATE_PROXY_PROPERTY(pPropertyDescription, vec3Pos);
                }
            }
        }
        else if (event.GetEventType() == wxEVT_MOUSEWHEEL)
        {
            CCamera* pCamera = m_pMainFrame->GetSceneWindow()->GetCamera();
            const CVec3& pos = pCamera->GetViewPos();
            int nLengthPerAction = event.GetWheelRotation() / -12 * event.GetLinesPerAction();
            pCamera->SetViewPos(CVec3(pos.X(), pos.Y() + nLengthPerAction, pos.Z()));
        }
        if (event.LeftUp() && wxIsAltDown())
        {
            wxPGProperty* pSelectPGProperty = m_pMainFrame->GetPropGridManager()->GetSelectedProperty();
            if (pSelectPGProperty != NULL)
            {
                CPropertyDescriptionBase* pPropertyDesc = reinterpret_cast<CPropertyDescriptionBase*>(pSelectPGProperty->GetClientData());
                BEATS_ASSERT(pPropertyDesc != NULL);
                if (pPropertyDesc->GetType() == eRPT_Vec3F)
                {
                    BEATS_ASSERT(pPropertyDesc->GetChildren().size() == 3);
                    CComponentProxy* pProxy = pPropertyDesc->GetOwner();
                    BEATS_ASSERT(pProxy != NULL);
                    CComponentInstance* pComponentInstance = pProxy->GetHostComponent();
                    BEATS_ASSERT(pComponentInstance != NULL);
                    if (pComponentInstance != NULL)
                    {
                        CSprite* pSprite = dynamic_cast<CSprite*>(pComponentInstance);
                        if (pSprite != NULL)
                        {
                            float fX = event.GetX();
                            float fY = event.GetY();
                            fX /= fScaleFactor;
                            fY /= fScaleFactor;
                            CCamera* pCurrCamera = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D);
                            BEATS_ASSERT(pCurrCamera->GetType() == CCamera::eCT_3D);
                            pPropertyDesc->GetParent();
                            CVec3 retPos = pCurrCamera->RayCast(fX, fY);
                            const CMat4& worldTM = pSprite->GetWorldTM();
                            CMat4 inverseTM = worldTM;
                            inverseTM.Inverse();
                            retPos *= inverseTM;
                            UPDATE_PROXY_PROPERTY(pPropertyDesc, retPos);
                        }
                    }
                }
            }
        }
    }
}

void CAniViewAgent::SelectComponent(CComponentProxy* pComponentProxy)
{
    super::SelectComponent(pComponentProxy);
    m_pMainFrame->m_pTimeBar->Clear();
    m_spriteDataMap.clear();
    m_animationDataMap.clear();
    m_pActiveSprite = NULL;
    m_pModel = NULL;
    if (pComponentProxy != NULL && pComponentProxy->GetHostComponent() != NULL)
    {
        CNodeAnimationData* pData = dynamic_cast<CNodeAnimationData*>(pComponentProxy->GetHostComponent());
        if (pData != NULL)
        {
            const std::vector<CNodeAnimationElement*>& elements = pData->GetElements();
            for (uint32_t i = 0; i < elements.size(); ++i)
            {
                CNodeAnimationElement* pElement = elements[i];
                if (pElement != NULL)
                {
                    const std::map<uint32_t, CVec3>& keyFrames = pElement->GetKeyFrames();
                    int nStartPos = 0;
                    int nLength = 0;
                    if (keyFrames.size() > 0)
                    {
                        nStartPos = keyFrames.begin()->first;
                        int nEndPos = keyFrames.rbegin()->first;
                        nLength = nEndPos - nStartPos;
                    }
                    std::vector<int> frameList;
                    for (auto itr : keyFrames)
                    {
                        frameList.push_back(itr.first);
                    }
                    m_pMainFrame->m_pTimeBar->AddItem(strNodeAnimationElementType[pElement->GetType()], frameList);
                }
            }
        }
    }
    m_pMainFrame->m_pTimeBar->UpdateFrameContainer();
}

void CAniViewAgent::InsertAtlasSpriteData(CAtlasSprite* pSprite, CTimeBarFrameData* pParentData, wxString strName)
{
    BEATS_ASSERT(pSprite != NULL);
    std::vector<int> frameList;
    auto& frameMap = pSprite->GetSpriteFrameMap();
    for (auto iter : frameMap)
    {
        frameList.push_back(iter.first);
    }
    CTimeBarFrameData* pData = m_pMainFrame->m_pTimeBar->AddItem(strName.empty() ? _T("sprite") : strName, frameList, pParentData);
    m_spriteDataMap[pData] = pSprite;
    for (auto child : pSprite->GetChildAtlasSprite())
    {
        if (child != NULL)
        {
            InsertAtlasSpriteData(child, pData, child->GetName());
        }
    }
}

void CAniViewAgent::OnPropertyChanged(wxPropertyGridEvent& event)
{
    CComponentProxy* pComponentProxy = m_pMainFrame->GetSelectedComponent();
    BEATS_ASSERT(pComponentProxy != NULL, _T("It's impossible to change property when you don't select a component proxy first."));
    if (pComponentProxy->GetGuid() == CNodeAnimationData::REFLECT_GUID)
    {
        wxString propertyName = event.GetPropertyName();
        if (propertyName == _T("m_uTotalTimeMS"))
        {
            m_pMainFrame->m_pTimeBar->RefreshControl();
        }
    }
}

void CAniViewAgent::OnTimeBarCurSorChange(int nCursorPos)
{
    if (m_pActiveSprite != NULL)
    {
        m_pActiveSprite->SetCurrFrame(nCursorPos);
        CSprite* pSprite = m_pActiveSprite->GetCurrentSprite();
        if (pSprite != NULL)
        {
            InsertSpriteFrame(pSprite, m_pActiveSprite);
        }
    }
    if (m_pAnimation != NULL && m_pModel != NULL)
    {
        int maxFrameCount = m_pAnimation->GetFrameCount();
        nCursorPos = maxFrameCount > nCursorPos ? nCursorPos : maxFrameCount - 1;
        m_pAnimation->SetCurFrame(nCursorPos);
        m_pModel->GetAnimationController()->GoToFrame(nCursorPos);
        m_pModel->SendDataToGraphics();
    }
}

void CAniViewAgent::OnTimeBarTreeItemSelect()
{
    CTimeBarFrameData* pItem = m_pMainFrame->m_pTimeBar->GetSelectedItem();
    if (m_pModel != NULL)
    {
        BEATS_ASSERT(m_animationDataMap.find(pItem) != m_animationDataMap.end());
        m_pAnimation = m_animationDataMap[pItem];
        int maxFrameCount = m_pAnimation->GetFrameCount();
        int nCursorPos = m_pMainFrame->m_pTimeBar->GetCursorPos();
        nCursorPos = maxFrameCount > nCursorPos ? nCursorPos : maxFrameCount - 1;
        m_pAnimation->SetCurFrame(nCursorPos);
        m_pModel->GetAnimationController()->GoToFrame(nCursorPos);
        m_pModel->SendDataToGraphics();
    }
    m_pMainFrame->m_pPropGridManager->ExpandAll(false);
}

void CAniViewAgent::ProcessKeyboardEvent(wxKeyEvent& event)
{
    if (m_pActiveSprite != NULL)
    {
        if (event.GetKeyCode() == WXK_HOME || event.GetKeyCode() == WXK_END)
        {
            bool bPrev = event.GetKeyCode() == WXK_HOME;
            CSprite* pParent = m_pActiveSprite->GetParentSprite();
            if (pParent != NULL && pParent->GetGuid() == CAtlasSprite::REFLECT_GUID)
            {
                CListPropertyDescription* pListProperty = down_cast<CListPropertyDescription*>(pParent->GetProxyComponent()->GetProperty(_T("m_childList")));
                BEATS_ASSERT(pListProperty != NULL);
                const std::vector<CPropertyDescriptionBase*>& childList = pListProperty->GetChildren();
                if (childList.size() > 1)
                {
                    CPropertyDescriptionBase* pGoalProperty = NULL;
                    uint32_t uIndex = 0;
                    for (auto iter : childList)
                    {
                        if (iter->GetInstanceComponent() == m_pActiveSprite->GetProxyComponent())
                        {
                            pGoalProperty = iter;
                            break;
                        }
                        uIndex++;
                    }
                    if ((bPrev && uIndex != 0) || (!bPrev && uIndex != childList.size() - 1))
                    {
                        uIndex = bPrev ? uIndex - 1 : uIndex + 1;
                        uIndex = uIndex == childList.size() - 1 ? 0xFFFFFFFF : uIndex;
                        pListProperty->ChangeOrder(pGoalProperty, uIndex);
                        CScrollableTreeCtrl* pTreeCtrl = m_pMainFrame->m_pTimeBar->GetItemTreeCtrl();
                        wxTreeItemId id = pTreeCtrl->GetSelection();
                        SwitchItem(id, bPrev);
                    }
                }
            }
        }
        else
        {
            uint32_t uCursorPos = m_pMainFrame->m_pTimeBar->GetCursorPos();
            uCursorPos = m_pActiveSprite->GetFrameCount() > uCursorPos ? uCursorPos : m_pActiveSprite->GetFrameCount() - 1;
            CSprite* pSprite = m_pActiveSprite->GetSpriteFrame(uCursorPos);
            if (pSprite != NULL)
            {
                CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(pSprite->GetProxyComponent()->GetProperty(_T("m_pos")));
                BEATS_ASSERT(pPropertyDescription != NULL);
                float step = 0.01f;
                if (event.ShiftDown())
                {
                    step = 0.5f;
                }
                if (ISKEYDOWN('W'))
                {
                    CVec3 vec3Pos = pSprite->GetPosition();
                    vec3Pos.Z() -= step;
                    UPDATE_PROXY_PROPERTY(pPropertyDescription, vec3Pos);
                }
                else if (ISKEYDOWN('A'))
                {
                    CVec3 vec3Pos = pSprite->GetPosition();
                    vec3Pos.X() = pSprite->GetPosition().X() - step;
                    vec3Pos.Y() = pSprite->GetPosition().Y();
                    vec3Pos.Z() = pSprite->GetPosition().Z();
                    UPDATE_PROXY_PROPERTY(pPropertyDescription, vec3Pos);
                }
                else if (ISKEYDOWN('S'))
                {
                    CVec3 vec3Pos = pSprite->GetPosition();
                    vec3Pos.Z() = pSprite->GetPosition().Z() + step;
                    UPDATE_PROXY_PROPERTY(pPropertyDescription, vec3Pos);
                }
                else if (ISKEYDOWN('D'))
                {
                    CVec3 vec3Pos = pSprite->GetPosition();
                    vec3Pos.X() = pSprite->GetPosition().X() + step;
                    UPDATE_PROXY_PROPERTY(pPropertyDescription, vec3Pos);
                }
                else if (ISKEYDOWN('J'))
                {
                    m_pMainFrame->m_pTimeBar->SetCursorPos(m_pMainFrame->m_pTimeBar->GetCursorPos() - 1);
                }
                else if (ISKEYDOWN('K'))
                {
                    m_pMainFrame->m_pTimeBar->SetCursorPos(m_pMainFrame->m_pTimeBar->GetCursorPos() + 1);
                }
                else if (ISKEYDOWN(' '))
                {
                    m_pMainFrame->m_pTimeBar->Play();
                }
            }
        }
    }
    event.Skip(false);
}

void CAniViewAgent::OnTimeBarAddButtonClick()
{
}

void CAniViewAgent::InsertModelData(CModel* pModel)
{
    m_pMainFrame->m_pTimeBar->Clear();
    std::map<TString, CSkeletonAnimation* > animations = pModel->GetAnimations();
    std::map<TString, CSkeletonAnimation* >::iterator it;
    for (it = animations.begin(); it != animations.end(); ++it)
    {
        CSkeletonAnimation* pAnimation = it->second;
        if (pAnimation != NULL)
        {
            std::vector<int> frameList;
            for (size_t i = 0; i < pAnimation->GetFrameCount(); i++)
            {
                frameList.push_back(i);
            }
            CTimeBarFrameData* pData = m_pMainFrame->m_pTimeBar->AddItem(pAnimation->GetAniName(), frameList);
            m_animationDataMap[pData] = pAnimation;
        }
    }
    m_pMainFrame->m_pTimeBar->UpdateFrameContainer();
}

void CAniViewAgent::InsertSpriteFrame(CSprite* pSprite, CAtlasSprite* pParentSprite)
{
    CComponentProxy* pComponent = pParentSprite->GetProxyComponent();
    CPropertyDescriptionBase* pPropertyBase = pComponent->GetProperty("m_spriteFramesMap");
    BEATS_ASSERT(pPropertyBase != NULL);
    CMapPropertyDescription* pMapProperty = (CMapPropertyDescription*)(pPropertyBase);
    for (size_t i = 0; i < pMapProperty->GetChildren().size(); i++)
    {
        CPropertyDescriptionBase* pProperty = pMapProperty->GetChildren()[i];
        CPtrPropertyDescription* pPtrProperty = (CPtrPropertyDescription*)pProperty->GetChildren()[1];
        if (down_cast<CSprite*>(pPtrProperty->GetInstanceComponent()->GetHostComponent()) == pSprite)
        {
            m_pMainFrame->m_pPropGridManager->ClearGrid();
            std::vector<CPropertyDescriptionBase*> vec = { pProperty };
            m_pMainFrame->m_pPropGridManager->InsertInPropertyGrid(vec);
            m_pMainFrame->m_pPropGridManager->ExpandAll(true);
            break;
        }
    }
}

void CAniViewAgent::OnTimeBarSelectFrameChange()
{
    if (m_pActiveSprite != NULL && m_pActiveSprite->GetFrameCount() > 0)
    {
        CSprite* pSprite = m_pActiveSprite->GetCurrentSprite();
        if (pSprite != NULL)
        {
            InsertSpriteFrame(pSprite, m_pActiveSprite);
        }
    }
}

void CAniViewAgent::OnTimeBarDraggingFrame()
{
    SSelection& selection = m_pMainFrame->m_pTimeBar->GetItemContainer()->GetCurrentSelect();
    BEATS_ASSERT(selection.m_iSelectColumnBegin == selection.m_iSelectColumnEnd);
    int nDraggingFrameIndex = selection.m_iSelectColumnBegin;
    if (m_pActiveSprite != NULL && m_pActiveSprite->GetFrameCount() > 0)
    {
        auto& spriteFrameMap = m_pActiveSprite->GetSpriteFrameMap();
        BEATS_ASSERT(spriteFrameMap.find(nDraggingFrameIndex) != spriteFrameMap.end());
        int nCursorPos = m_pMainFrame->m_pTimeBar->GetCursorPos();
        if (spriteFrameMap.find(nCursorPos) == spriteFrameMap.end())
        {
            CComponentProxy* pComponent = m_pActiveSprite->GetProxyComponent();
            CPropertyDescriptionBase* pPropertyBase = pComponent->GetProperty("m_spriteFramesMap");
            BEATS_ASSERT(pPropertyBase != NULL);
            CMapPropertyDescription* pMapProperty = (CMapPropertyDescription*)(pPropertyBase);
            CSprite* pSprite = m_pActiveSprite->GetCurrentSprite();
            for (size_t i = 0; i < pMapProperty->GetChildren().size(); i++)
            {
                CPropertyDescriptionBase* pPropertyDescription = pMapProperty->GetChildren()[i];
                CPtrPropertyDescription* pPtrProperty = (CPtrPropertyDescription*)pPropertyDescription->GetChildren()[1];
                if (down_cast<CSprite*>(pPtrProperty->GetInstanceComponent()->GetHostComponent()) == pSprite)
                {
                    wxVariant xValue = nCursorPos;
                    CWxwidgetsPropertyBase* pKeyProperty = (CWxwidgetsPropertyBase*)(pPropertyDescription->GetChildren()[0]);
                    pKeyProperty->SetValue(xValue, false);
                    wxPGProperty* pPGProperty = m_pMainFrame->m_pPropGridManager->GetPGPropertyByBase(pKeyProperty);
                    BEATS_ASSERT(pPGProperty);
                    pPGProperty->SetValue(xValue);
                    m_pMainFrame->m_pPropGridManager->OnComponentPropertyChangedImpl(pPGProperty);
                    CTimeBarFrameData* pData = m_pMainFrame->m_pTimeBar->GetSelectedItem();
                    pData->RemoveFrame(nDraggingFrameIndex);
                    pData->AddFrame(nCursorPos);
                    pData->Sort();
                    m_pMainFrame->m_pTimeBar->GetItemContainer()->Refresh(false);
                    break;
                }
            }
        }
        else
        {
            wxMessageBox(_T("当前位置已存在关键帧"), _T("警告"), wxOK);
        }
    }
}

void CAniViewAgent::OnTimeBarItemContainerRClick(wxCommandEvent& event)
{
    if (m_pActiveSprite != NULL && m_pActiveSprite->GetFrameCount() > 0)
    {
        wxMenu menu;
        int nFrameIndex = event.GetInt();
        CTimeBarFrameData* pItem = m_pMainFrame->m_pTimeBar->GetSelectedItem();
        if (pItem->HasFrame(nFrameIndex))
        {
            menu.Append(0, _T("复制帧"));
        }
        else
        {
            CComponentProxy* pComponent = m_pActiveSprite->GetProxyComponent();
            CPropertyDescriptionBase* pPropertyBase = pComponent->GetProperty("m_spriteFramesMap");
            BEATS_ASSERT(pPropertyBase != NULL);
            CMapPropertyDescription* pMapProperty = (CMapPropertyDescription*)(pPropertyBase);
            //check if the property is existed, because it may have been deleted
            for (size_t i = 0; i < pMapProperty->GetChildren().size(); i++)
            {
                CPropertyDescriptionBase* pPropertyDescription = pMapProperty->GetChildren()[i];
                if (m_pCopyFramePropertyDescription == pPropertyDescription)
                {
                    menu.Append(1, _T("粘贴帧"));
                    break;
                }
            }
        }
        const int selection = m_pMainFrame->m_pTimeBar->GetPopupMenuSelectionFromUser(menu, wxDefaultPosition);
        switch (selection)
        {
        case 0:
            CopyFrame(nFrameIndex);
            break;
        case 1:
            PasteFrame(nFrameIndex);
            break;
        }
    }
}

void CAniViewAgent::OnPropertyGridSelect(wxPropertyGridEvent& event)
{
    wxPGProperty* pSelectProperty = event.GetProperty();
    if (pSelectProperty != nullptr)
    {
        CPropertyDescriptionBase* pPropertyBase = (CPropertyDescriptionBase*)(pSelectProperty->GetClientData());
        if (pPropertyBase->GetType() == eRPT_Ptr)
        {
            CComponentProxy* pProxy = pPropertyBase->GetInstanceComponent();
            if (pProxy != nullptr && pProxy->GetHostComponent() != nullptr)
            {
                CNode3D* pNode = dynamic_cast<CNode3D*>(pProxy->GetHostComponent());
                CRenderManager::GetInstance()->GetCoordinateRenderObject()->SetRenderNode(pNode);
            }
        }
        CComponentProxy* pProxy = pPropertyBase->GetOwner();
        if (pProxy->GetGuid() == CAnimatableSprite::REFLECT_GUID || pProxy->GetGuid() == CAtlasSprite::REFLECT_GUID)
        {
            CPropertyDescriptionBase* pParentProperty = pPropertyBase;
            while (pParentProperty->GetOwner() != nullptr && pParentProperty->GetOwner() == pProxy)
            {
                pParentProperty = pParentProperty->GetParent();
            }
        }
    }
}

void CAniViewAgent::PasteFrame(int nFramePos)
{
    BEATS_ASSERT(m_pActiveSprite != NULL && m_pActiveSprite->GetFrameCount() > 0);
    CMapElementPropertyDescription* pPaste = down_cast<CMapElementPropertyDescription*>(m_pCopyFramePropertyDescription->Clone(true));
    wxVariant xValue = nFramePos;
    ((CWxwidgetsPropertyBase*)(pPaste->GetChildren()[0]))->SetValue(xValue, false);
    m_pCopyFramePropertyDescription->GetParent()->InsertChild(pPaste);
    m_pMainFrame->m_pPropGridManager->ClearPage(0);
    std::vector<CPropertyDescriptionBase*> vec = { pPaste };
    m_pMainFrame->m_pPropGridManager->InsertInPropertyGrid(vec);
    m_pMainFrame->m_pPropGridManager->ExpandAll(true);

    CTimeBarFrameData* pData = m_pMainFrame->m_pTimeBar->GetSelectedItem();
    pData->AddFrame(nFramePos);
    pData->Sort();
    m_pMainFrame->m_pTimeBar->GetItemContainer()->Refresh(false);
}

void CAniViewAgent::CopyFrame(int nFramePos)
{
    CComponentProxy* pComponent = m_pActiveSprite->GetProxyComponent();
    CPropertyDescriptionBase* pPropertyBase = pComponent->GetProperty("m_spriteFramesMap");
    BEATS_ASSERT(pPropertyBase != NULL);
    CMapPropertyDescription* pMapProperty = (CMapPropertyDescription*)(pPropertyBase);
    for (size_t i = 0; i < pMapProperty->GetChildren().size(); i++)
    {
        CPropertyDescriptionBase* pPropertyDescription = pMapProperty->GetChildren()[i];
        int nValue = *(int*)pPropertyDescription->GetChildren()[0]->GetValue(eVT_CurrentValue);
        if (nFramePos == nValue)
        {
            m_pCopyFramePropertyDescription = pPropertyDescription;
            break;
        }
    }
}

void CAniViewAgent::SwitchItem(wxTreeItemId id, bool bPrev)
{
    CScrollableTreeCtrl* pTreeCtrl = m_pMainFrame->m_pTimeBar->GetItemTreeCtrl();
    wxTreeItemId siblingId = bPrev ? pTreeCtrl->GetPrevSibling(id) : pTreeCtrl->GetNextSibling(id);
    wxString strName = pTreeCtrl->GetItemText(id);
    wxString strPrevName = pTreeCtrl->GetItemText(siblingId);
    wxTreeItemData* pData = pTreeCtrl->GetItemData(id);
    wxTreeItemData* pPrevData = pTreeCtrl->GetItemData(siblingId);
    pTreeCtrl->SetItemData(id, pPrevData);
    pTreeCtrl->SetItemData(siblingId, pData);
    pTreeCtrl->SetItemText(id, strPrevName);
    pTreeCtrl->SetItemText(siblingId, strName);
    m_pMainFrame->m_pTimeBar->UpdateFrameContainer();
    pTreeCtrl->SelectItem(siblingId);
}
