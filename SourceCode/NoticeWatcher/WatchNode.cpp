#include "stdafx.h"
#include "WatchNode.h"
#include "NoticeWatcherDispatcher.h"
#include "Resource/ResourceManager.h"

CWatchNode::CWatchNode()
{

}

CWatchNode::~CWatchNode()
{
    if (m_bWatchered)
    {
        CNoticeWatcherDispatcher::GetInstance()->UnRegisterWatchNode(this);
    }
}

void CWatchNode::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_posOffset, true, 0xFFFFFFFF, _T("红点位置偏移"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fIconScale, true, 0xFFFFFFFF, _T("红点缩放大小"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bShowWatchIcon, true, 0xFFFFFFFF, _T("显示红点"), NULL, NULL, NULL);
}
#ifdef EDITOR_MODE
bool CWatchNode::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool ret = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!ret)
    {
        if (pVariableAddr == &m_posOffset)
        {
            DeserializeVariable(m_posOffset, pSerializer, this);
            if (m_bShowWatchIcon)
            {
                this->AddNoticeIcon();
            }
            ret = true;
        }
        else if (pVariableAddr == &m_fIconScale)
        {
            DeserializeVariable(m_fIconScale, pSerializer, this);
            if (m_bShowWatchIcon)
            {
                this->AddNoticeIcon();
            }
            ret = true;
        }
        else if (pVariableAddr == &m_bShowWatchIcon)
        {
            DeserializeVariable(m_bShowWatchIcon, pSerializer, this);
            if (m_bShowWatchIcon)
            {
                this->AddNoticeIcon();
            }
            else
            {
                this->RemoveNoticeIcon();
            }
            ret = true;
        }
    }

    return ret;
}
#endif
void CWatchNode::SetWatchIconOffset(const CVec3& offset)
{
    m_posOffset = offset;
}

void CWatchNode::SetWatchIconScale(float scale)
{
    m_fIconScale = scale;
}

void CWatchNode::WatchCountChanged(int)
{

}

void CWatchNode::ReceiveNotice(int receiveCount)
{
    m_iWatchCount += receiveCount;
    if (!m_bAddNoticeIcon)
    {
        AddNoticeIcon();
        m_bAddNoticeIcon = true;
    }
    WatchCountChanged(m_iWatchCount);
}

void CWatchNode::CompleteNotice(int receiveCount)
{
    if (m_bAddNoticeIcon && m_iWatchCount > 0)
    {
        m_iWatchCount -= receiveCount;
        if (0 >= m_iWatchCount)
        {
            RemoveNoticeIcon();
            m_bAddNoticeIcon = false;
        }
        WatchCountChanged(m_iWatchCount);
    }
}

int CWatchNode::GetWatchCount()
{
    return m_iWatchCount;
}

void CWatchNode::Watched()
{
    m_bWatchered = true;
}

SharePtr<CTextureFrag> CWatchNode::GetNoticeIcon()
{
    if (m_noticeIcon.GetTextureFrag() == nullptr)
    {
        m_noticeIcon.SetTextureFrag("mainscreen_01.xml", "mainscreen_information_map");
    }
    return m_noticeIcon.GetTextureFrag();
}
