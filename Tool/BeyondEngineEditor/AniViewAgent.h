#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_ANIVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_ANIVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

class CDirectionSprite;
class CAtlasSprite;
class CTimeBarFrameData;
class CWxwidgetsPropertyBase;
class CSprite;
class CModel;
class CSkeletonAnimation;
class CPropertyDescriptionBase;
class wxTreeItemId;
class CAniViewAgent : public CViewAgentBase
{
    typedef CViewAgentBase super;
    BEATS_DECLARE_SINGLETON(CAniViewAgent);
public:
    virtual void InView() override;
    virtual void OutView() override;
    virtual void ProcessMouseEvent(wxMouseEvent& event) override;
    virtual void ProcessKeyboardEvent(wxKeyEvent& event);
    virtual void SelectComponent(CComponentProxy* pComponentProxy) override;
    virtual void OnPropertyChanged(wxPropertyGridEvent& event) override;
    virtual void OnTimeBarTreeItemSelect() override;
    virtual void OnTimeBarCurSorChange(int nCursorPos) override;
    virtual void OnTimeBarAddButtonClick() override;
    virtual void OnTimeBarSelectFrameChange() override;
    virtual void OnTimeBarDraggingFrame() override;
    virtual void OnTimeBarItemContainerRClick(wxCommandEvent& event) override;
    virtual void OnPropertyGridSelect(wxPropertyGridEvent& event) override;
    void InsertAtlasSpriteData(CAtlasSprite* pSprite, CTimeBarFrameData* pParentData, wxString strName);
    void InsertModelData(CModel* pModel);
    void InsertSpriteFrame(CSprite* pSprite, CAtlasSprite* pParentSprite);
    void SwitchItem(wxTreeItemId id, bool bPrev);
private:
    void CopyFrame(int nFramePos);
    void PasteFrame(int nFramePos);
private:
    CAtlasSprite* m_pActiveSprite;
    CAtlasSprite* m_pParentOfActiveSprite;
    CSkeletonAnimation* m_pAnimation;
    CPropertyDescriptionBase* m_pCopyFramePropertyDescription;
    CModel* m_pModel;
    wxPoint m_mouseOffset;
    std::map<CTimeBarFrameData*, CAtlasSprite*> m_spriteDataMap;
    std::map<CTimeBarFrameData*, CSkeletonAnimation*> m_animationDataMap;
};
#endif