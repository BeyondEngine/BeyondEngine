#ifndef BEYOND_ENGINE_NOTICEWATCHER_NOTICEWATCHERDISPATCHER_H__INCLUDE
#define BEYOND_ENGINE_NOTICEWATCHER_NOTICEWATCHERDISPATCHER_H__INCLUDE

class CWatchNode;
class CNoticeWatcherDispatcher
{
    BEATS_DECLARE_SINGLETON(CNoticeWatcherDispatcher);
public:
    
    void RegisterWatchNode(CWatchNode* pWatchNode, const std::vector<TString>& strNoticeTags);

    void RegisterWatchNode(CWatchNode* pWatchNode, const TString& strNoticeTag);

    void UnRegisterWatchNode(CWatchNode* pWatchNode);

    void DispatchNotice(const TString& strNoticeTag, uint32_t uDispatchCount, bool bIsOnlyNotice = false );

    void NoticeComplete(const TString& strNoticeTag);

    void ResetNoticeCount(const TString& strNoticeTag);

    void ResetAll();

    uint32_t GetTagCount(const TString& strNoticeTag);
    std::map<TString, uint32_t> GetNodeWatchTarget(CWatchNode* pNode);

    void PrintAllWatchCount();
private:
    std::map<TString, std::vector<CWatchNode*>> m_noticeWatcherPool;

    std::map<TString, uint32_t> m_currentNoticeStack;
};

#endif