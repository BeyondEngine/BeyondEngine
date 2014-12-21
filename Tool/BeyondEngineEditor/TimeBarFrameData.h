#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMEBARFRAMEDATA_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMEBARFRAMEDATA_H_INCLUDE

#include <wx/treectrl.h>
class CUINode;
class CTimeBarFrameData : public wxTreeItemData
{
public:
    CTimeBarFrameData();
    virtual ~CTimeBarFrameData();
    void SetName(const wxString& strName);
    const wxString& GetName() const;
    void AddFrame(int nFrame);
    void RemoveFrame(int nFrame);
    const std::vector<int>& GetFrameList() const;
    bool HasFrame(int nFrameIndex) const;
    void Sort();
    void Clear();

private:
    wxString m_strName;
    std::vector<int> m_frameList;
};

#endif