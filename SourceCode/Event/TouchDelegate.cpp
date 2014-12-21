#include "stdafx.h"
#include "TouchDelegate.h"
#include "TouchEvent.h"
#include "Touch.h"
#include "Event/TouchEvent.h"
#include "Event/PinchEvent.h"
#include "Render/RenderTarget.h"

CTouchDelegate* CTouchDelegate::m_pInstance = nullptr;

CTouchDelegate::CTouchDelegate()
    : m_bEnableTouch(true)
    , m_indexBitsUsed(0)
{
    for (uint32_t i = 0; i < TOUCH_MAX_NUM; i++)
    {
        CTouch* pTouch = new CTouch(0xFFFFFFFF);
        m_pTouches[i] = pTouch;
    }
}

CTouchDelegate::~CTouchDelegate()
{
    for (uint32_t i = 0; i < TOUCH_MAX_NUM; i++)
    {
        BEATS_SAFE_DELETE(m_pTouches[i]);
    }
    Clear();
}

void CTouchDelegate::OnTouchBegan(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint)
{
#ifdef PRINT_TOUCH_EVENT
    BEATS_PRINT(_T("touchCount:%d, touchId:%lu\n"), num, pTouchIdByDevice[0]);
#endif
#ifndef DEVELOP_VERSION
    if ( m_bEnableTouch )
#endif
    {
        CTouchEvent event(eET_EVENT_TOUCH_BEGAN);
        for ( unsigned int index = 0; index < num ; ++index )
        {
            size_t id = pTouchIdByDevice[index];
            auto iter = m_currentTouchMap.find( id );
            bool bFind = iter != m_currentTouchMap.end();
            BEATS_WARNING(!bFind, _T("OnTouchBegan: touch id %d already exists!"), (uint32_t)id);
            if ( !bFind )
            {
                int useIndex = 0;
                useIndex = GetUnUsedIndex();
                BEATS_ASSERT(useIndex != -1);
                if ( -1 != useIndex )
                {
                    m_pTouches[useIndex]->Reset(id);
                    uint32_t uCurrMSec = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
                    if (uCurrMSec - m_uLastTapTime <= TAP_INTERVAL &&
                        labs((int)(xTouchPoint[index] - m_lastTapScreenPos.X())) <= TAP_DEVIATION &&
                        labs((int)(yTouchPoint[index] - m_lastTapScreenPos.Y())) <= TAP_DEVIATION)
                    {
                        m_pTouches[useIndex]->SetTapCount(m_pTouches[useIndex]->GetTapCount() + 1);
                    }
                    else
                    {
                        m_pTouches[useIndex]->SetTapCount(1);
                    }
                    m_uLastTapTime = uCurrMSec;
                    m_nLastTapTouchId = id;
                    m_pTouches[useIndex]->UpdateTouchInfo(id, xTouchPoint[index], yTouchPoint[index]);
                    event.AddTouch(m_pTouches[useIndex]);
                    m_lastTapScreenPos.Fill(xTouchPoint[index], yTouchPoint[index]);
                    m_currentTouchMap.insert( std::make_pair( id, useIndex ));
#ifdef DEVELOP_VERSION
                    if (m_currentTouchMap.size() == 2)
                    {
                        if (m_pTouches[useIndex]->GetTapCount() == 2)
                        {
                            CLogManager::GetInstance()->Show(!CLogManager::GetInstance()->IsShown());
                        }
                    }
#endif
                }
            }
        }
        if (event.GetTouchNum() > 0 && m_bEnableTouch)
        {
            DispatchEvent(&event);
        }
    }
}

void CTouchDelegate::OnTouchMoved(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint)
{
#ifdef PRINT_TOUCH_EVENT
    BEATS_PRINT(_T("move %d-- id-%lu \n"), num, pTouchIdByDevice[0]);
#endif
#ifdef DEVELOP_VERSION
    bool bNeedScrollLog = CLogManager::GetInstance()->IsShown() && (CLogManager::GetInstance()->GetLogChannel() == ELogChannel::eLC_Log || CLogManager::GetInstance()->GetLogChannel() == ELogChannel::eLC_Resource);
    if (m_bEnableTouch || bNeedScrollLog)
#else
    if ( m_bEnableTouch )
#endif
    {
#ifdef DEVELOP_VERSION
        if (bNeedScrollLog)
        {
            if (num == 1)
            {
                size_t id = pTouchIdByDevice[0];
                auto iter = m_currentTouchMap.find(id);
                if (iter != m_currentTouchMap.end())
                {
                    CTouch* pTouch = m_pTouches[iter->second];
                    if (m_uLogScrollRecord == 0xFFFFFFFF)
                    {
                        m_uLogScrollRecord = (uint32_t)pTouch->GetStartPoint().Y();
                    }
                    float fCurrPos = yTouchPoint[0];
                    uint32_t uFontHeight = CLogManager::GetInstance()->GetLogFontHeight();
                    if (fabs(fCurrPos - m_uLogScrollRecord) > uFontHeight)
                    {
                        if (fCurrPos > m_uLogScrollRecord)
                        {
                            CLogManager::GetInstance()->DecreaseRenderPos();
                        }
                        else
                        {
                            CLogManager::GetInstance()->IncreaseRenderPos();
                        }
                        m_uLogScrollRecord = (uint32_t)fCurrPos;
                    }
                }
            }
        }
        else
        {
#endif
            CTouchEvent event(EEventType::eET_EVENT_TOUCH_MOVED);
            for (unsigned int index = 0; index < num; ++index)
            {
                size_t id = pTouchIdByDevice[index];
                auto iter = m_currentTouchMap.find(id);
                BEATS_WARNING(iter != m_currentTouchMap.end(), _T("OnTouchMoved: touch id %d doesn't exists! have you called began?"), (uint32_t)id);
                if (iter != m_currentTouchMap.end())
                {
                    CTouch* pTouch = m_pTouches[iter->second];
                    if (pTouch->GetCurrentPoint().X() != xTouchPoint[index] || pTouch->GetCurrentPoint().Y() != yTouchPoint[index])
                    {
                        pTouch->UpdateTouchInfo(id, xTouchPoint[index], yTouchPoint[index]);
                        event.AddTouch(pTouch);
                    }
                }
            }
            if (event.GetTouchNum() > 0)
            {
                DispatchEvent(&event);
            }
#ifdef DEVELOP_VERSION
        }
#endif
    }
}

void CTouchDelegate::OnTouchEnded(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint)
{
#ifdef PRINT_TOUCH_EVENT
    BEATS_PRINT(_T("end %d-- id-%lu \n"), num, pTouchIdByDevice[0]);
#endif
    OnTouchEndedOrCancelled(EEventType::eET_EVENT_TOUCH_ENDED, num, pTouchIdByDevice, xTouchPoint, yTouchPoint);
}

void CTouchDelegate::OnTouchCancelled(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint)
{
#ifdef PRINT_TOUCH_EVENT
    BEATS_PRINT(_T("cancel %d-- id-%lu \n"), num, pTouchIdByDevice[0]);
#endif
    OnTouchEndedOrCancelled( EEventType::eET_EVENT_TOUCH_CANCELLED, num, pTouchIdByDevice, xTouchPoint, yTouchPoint);
}

void CTouchDelegate::OnPinched( EGestureState state, float fPinchScale, CTouch* /*pTouch1*/, CTouch* /*pTouch2*/)
{
    if (m_bEnableTouch)
    {
        CPinchEvent event(EEventType::eET_EVENT_TOUCH_PINCHED, state, fPinchScale);
#if(BEYONDENGINE_PLATFORM != PLATFORM_WIN32)
        if (m_currentTouchMap.size() >= 2)
        {
            CTouch* pTouches[2];
            int i = 0;
            for (std::map< size_t, int >::iterator iter= m_currentTouchMap.begin(); iter != m_currentTouchMap.end(); ++iter)
            {
                CTouch* pTouch = m_pTouches[ iter->second ];
                pTouches[i++] = pTouch;
                if (i == 2)
                {
                    break;
                }
            }
            event.SetTouch(pTouches[0], pTouches[1]);
            super::DispatchEvent(&event);
        }
#else
    super::DispatchEvent(&event);
#endif
    }
}

int CTouchDelegate::GetUnUsedIndex()
{
    int nRet = -1;
    int temp = m_indexBitsUsed;

    for (int i = 0; i < TOUCH_MAX_NUM; i++)
    {
        if (! (temp & 0x00000001))
        {
            m_indexBitsUsed |= (1 << i);
            nRet = i;
            break;
        }
        temp >>= 1;
    }

    // all bits are used
    return nRet;
}

void CTouchDelegate::RemoveUsedIndex( int index )
{
    if (index >= 0 && index < TOUCH_MAX_NUM )
    {
        unsigned int temp = 1 << index;
        temp = ~temp;
        m_indexBitsUsed &= temp;
    }
}

void CTouchDelegate::OnTouchEndedOrCancelled(EEventType type, unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint)
{
    if ( m_bEnableTouch )
    {
        CTouchEvent event(type);
        for ( unsigned int index = 0; index < num ; ++index )
        {
            size_t id = pTouchIdByDevice[index];
            auto iter = m_currentTouchMap.find( id );
            BEATS_ASSERT(iter != m_currentTouchMap.end(), _T("touch id %d doesn't exists"), (uint32_t)id);
            if ( iter != m_currentTouchMap.end() )
            {
                CTouch* pTouch = m_pTouches[ iter->second ];
                pTouch->UpdateTouchInfo( id, xTouchPoint[ index ], yTouchPoint[ index ] );
                event.AddTouch(pTouch);
            }
        }
#ifdef DEVELOP_VERSION
        if (!CLogManager::GetInstance()->SwitchChannel(CVec2(*xTouchPoint, *yTouchPoint)))
        {
#endif
            if (event.GetTouchNum() > 0)
            {
                DispatchEvent(&event);
            }
#ifdef DEVELOP_VERSION
        }
#endif

        for ( unsigned int index = 0; index < num ; ++index )
        {
            size_t id = pTouchIdByDevice[index];
            auto iter = m_currentTouchMap.find( id );
            BEATS_ASSERT(iter != m_currentTouchMap.end());
            if ( iter != m_currentTouchMap.end() )
            {
                RemoveUsedIndex( iter->second );
                m_currentTouchMap.erase( iter );
            }
        }
    }
    else
    {
        for (auto iter = m_currentTouchMap.begin(); iter != m_currentTouchMap.end(); iter++)
        {
            RemoveUsedIndex(iter->second);
        }
        m_currentTouchMap.clear();
    }
    if (m_currentTouchMap.size() == 0)
    {
        m_nLastTapTouchId = -1;
#ifdef DEVELOP_VERSION
        m_uLogScrollRecord = 0xFFFFFFFF;
#endif
    }
}

const CVec2& CTouchDelegate::GetLastTapScreenPos() const
{
    return m_lastTapScreenPos;
}

void CTouchDelegate::SetEnable( bool bEnable )
{
    m_bEnableTouch = bEnable;
}

bool CTouchDelegate::GetEnable() const
{
    return m_bEnableTouch;
}

const std::map< size_t, int >& CTouchDelegate::GetTouchMap() const
{
    return m_currentTouchMap;
}

CTouch* CTouchDelegate::GetTouch(uint32_t uIndex) const
{
    BEATS_ASSERT(uIndex < TOUCH_MAX_NUM);
    return m_pTouches[uIndex];
}

void CTouchDelegate::DispatchEvent(CBaseEvent *event)
{
    CTouchEvent* pTouchEvent = down_cast<CTouchEvent*>(event);
    if (!pTouchEvent->Stopped())
    {
        super::DispatchEvent(pTouchEvent);
    }
}

void CTouchDelegate::UnsubscribeEvent(const CEventSubscription &subscription)
{
    CEventDispatcher::UnsubscribeEvent(subscription);
}

void CTouchDelegate::UnsubscribeEvent(int type, int id)
{
    CEventDispatcher::UnsubscribeEvent(type, id);
}

