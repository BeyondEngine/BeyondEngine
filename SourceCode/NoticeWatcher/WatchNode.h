#ifndef BEYOND_ENGINE_NOTICEWATCHER_WATCHNODE_H__INCLUDE
#define BEYOND_ENGINE_NOTICEWATCHER_WATCHNODE_H__INCLUDE

#include "Scene/Node2D.h"
#include "Render/TextureFrag.h"
#include "Render/ReflectTextureInfo.h"

class CWatchNode : public CNode2D
{
public:
    DECLARE_REFLECT_GUID(CWatchNode, 0x1311231a, CNode2D)
public:
    CWatchNode();
    virtual ~CWatchNode();

    virtual void ReflectData(CSerializer& serializer) override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    void ReceiveNotice(int receiveCount = 1);

    void CompleteNotice(int receiveCount = 1);

    void Watched();

    void SetWatchIconOffset(const CVec3& offset);

    void SetWatchIconScale(float scale);
    virtual void WatchCountChanged(int count);
    int GetWatchCount();
protected:
    virtual void AddNoticeIcon(){}

    virtual void RemoveNoticeIcon(){}

    SharePtr<CTextureFrag> GetNoticeIcon();

protected:
    int m_iWatchCount = 0;
    bool m_bAddNoticeIcon = false;
    bool m_bWatchered = false;
    SReflectTextureInfo m_noticeIcon;
    CVec3 m_posOffset;
    float m_fIconScale = 1.0f;
    bool m_bShowWatchIcon = false;
};

#endif