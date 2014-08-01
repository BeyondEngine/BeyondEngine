#include "stdafx.h"
#include "AniViewAgent.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "NodeAnimation\NodeAnimationData.h"
#include "timebarframe.h"
#include "NodeAnimation\NodeAnimationElement.h"
#include "NodeAnimation\NodeAnimation.h"

CAniViewAgent* CAniViewAgent::m_pInstance = nullptr;
CAniViewAgent::CAniViewAgent()
{
}

CAniViewAgent::~CAniViewAgent()
{

}

void CAniViewAgent::InView()
{
    if (!m_pMainFrame->m_pTimeBar->GetAnimation()->IsPlaying())
    {
        m_pMainFrame->m_pTimeBar->Clear();
    }

    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pBottomPanel).Show();
    m_pMainFrame->m_Manager.Update();
}

void CAniViewAgent::OutView()
{
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pBottomPanel).Hide();
    m_pMainFrame->m_Manager.Update();
}

void CAniViewAgent::OnCommandEvent(wxCommandEvent& event)
{
    switch (event.GetId())
    {
    case Ctrl_TimeBar_Button_Add:
        break;
    default:
        break;
    }
}

void CAniViewAgent::SelectComponent(CComponentProxy* pComponentProxy)
{
    if (pComponentProxy != NULL && pComponentProxy->GetHostComponent() != NULL)
    {
        CNodeAnimationData* pData = dynamic_cast<CNodeAnimationData*>(pComponentProxy->GetHostComponent());
        if (pData != NULL)
        {
            m_pMainFrame->m_pTimeBar->Clear();
            const std::vector<CNodeAnimationElement*>& elements = pData->GetElements();
            for (size_t i = 0; i < elements.size(); ++i)
            {
                CNodeAnimationElement* pElement = elements[i];
                BEATS_ASSERT(pElement != NULL);
                const std::map<size_t, CVec3>& keyFrames = pElement->GetKeyFrames();
                int nStartPos = 0;
                int nLength = 0;
                if (keyFrames.size() > 0)
                {
                    nStartPos = keyFrames.begin()->first;
                    int nEndPos = keyFrames.rbegin()->first;
                    nLength = nEndPos - nStartPos;
                }
                CTimeBarItem* pItem = m_pMainFrame->m_pTimeBar->AddItem(strNodeAnimationElementType[pElement->GetType()], (wxUIntPtr)pElement, nStartPos, nLength);
                if (pItem != NULL)
                {
                    pItem->SetAnimationElement(pElement);
                    for (auto iter = keyFrames.begin(); iter != keyFrames.end(); ++iter)
                    {
                        pItem->AddData(iter->first, iter->second);
                    }
                }
            }
            m_pMainFrame->m_pTimeBar->SetAnimationData(pData);
        }
        else
        {
            CNode* pNode = dynamic_cast<CNode*>(pComponentProxy->GetHostComponent());
            if (pNode != NULL)
            {
                CNodeAnimation* pAnimation = m_pMainFrame->m_pTimeBar->GetAnimation();
                if (pAnimation->GetOwner() != NULL)
                {
                    pAnimation->ResetNode();
                }
                pAnimation->SetOwner(pNode);
            }
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
        if (propertyName == _T("m_uFrameCount"))
        {
            m_pMainFrame->m_pTimeBar->GetScalebar()->Refresh();
            m_pMainFrame->m_pTimeBar->GetItemContainer()->ResetScrollBar();
        }
    }
}

void CAniViewAgent::Update()
{
    CNodeAnimation* pAnimation = m_pMainFrame->m_pTimeBar->GetAnimation();
    m_pMainFrame->m_pTimeBar->SetCurrentCursor(pAnimation->GetCurrentFrame());
}
