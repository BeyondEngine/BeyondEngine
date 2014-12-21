#include "stdafx.h"
#include "Component/ComponentPublic.h"
#include "TimebarframeData.h"
#include <algorithm>
CTimeBarFrameData::CTimeBarFrameData()
{
}

CTimeBarFrameData::~CTimeBarFrameData()
{

}

void CTimeBarFrameData::SetName(const wxString& strName)
{
    m_strName = strName;
}

void CTimeBarFrameData::AddFrame(int nFrame)
{
    BEATS_ASSERT(!HasFrame(nFrame));
    m_frameList.push_back(nFrame);
}

void CTimeBarFrameData::RemoveFrame(int nFrame)
{
    for (auto itr = m_frameList.begin(); itr != m_frameList.end(); itr++)
    {
        if (*itr == nFrame)
        {
            m_frameList.erase(itr);
            break;
        }
    }
}

const std::vector<int>& CTimeBarFrameData::GetFrameList() const
{
    return m_frameList;
}

const wxString& CTimeBarFrameData::GetName() const
{
    return m_strName;
}

bool CTimeBarFrameData::HasFrame(int nFrameIndex) const
{
    bool bRet = false;
    for (size_t i = 0; i < m_frameList.size(); i++)
    {
        if (m_frameList[i] == nFrameIndex)
        {
            bRet = true;
        }
    }
    return bRet;
}

void CTimeBarFrameData::Sort()
{
    std::sort(m_frameList.begin(), m_frameList.end(), [](const int v1, const int v2){return v1 < v2; });
}

void CTimeBarFrameData::Clear()
{
    m_frameList.clear();
}
