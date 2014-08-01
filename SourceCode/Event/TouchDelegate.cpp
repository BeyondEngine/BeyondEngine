
#include "stdafx.h"
#include "TouchDelegate.h"
#include "TouchEvent.h"
#include "Touch.h"
#include "Event/TouchEvent.h"
#include "Event/PinchEvent.h"
#include "GUI/WindowManager.h"
#include "Render/RenderTarget.h"

CTouchDelegate* CTouchDelegate::m_pInstance = nullptr;

CTouchDelegate::CTouchDelegate()
    : m_bEnableTouch(true)
    , m_indexBitsUsed(0)
{
}
CTouchDelegate::~CTouchDelegate()
{
}

void CTouchDelegate::OnTouchBegan( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int *tapCount )
{
    if ( m_bEnableTouch )
    {
        CTouchEvent event(eET_EVENT_TOUCH_BEGAN);

        for ( unsigned int index = 0; index < num ; ++index )
        {
            intptr_t id = pTouchIdByDevice[ index ];
            auto iter = m_currentTouchMap.find( id );
            bool bFind = iter != m_currentTouchMap.end();
            BEATS_ASSERT(!bFind, _T("OnTouchBegan: touch id %d already exists!"), id);
            if ( !bFind )
            {
                int useIndex = 0;
                useIndex = GetUnUsedIndex();
                BEATS_ASSERT(useIndex != -1);
                if ( -1 != useIndex )
                {
                    CTouch* pTouch = new CTouch( id );
                    m_pTouches[ useIndex ] = pTouch;
                    pTouch->UpdateTouchInfo( id, xTouchPoint[ index ], yTouchPoint[ index ] );
                    pTouch->SetTapCount(tapCount[index]);
                    event.AddTouch(pTouch);
                    m_currentTouchMap.insert( std::make_pair( id, useIndex ));
                }
            }
        }
        if ( event.GetTouchNum() > 0 )
        {
            CWindowManager::GetInstance()->OnTouchEvent( &event );
            DispatchEvent(&event);
        }
    }
}

void CTouchDelegate::OnTouchMoved( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint )
{
    if ( m_bEnableTouch )
    {
        CTouchEvent event(EEventType::eET_EVENT_TOUCH_MOVED);
        for ( unsigned int index = 0; index < num ; ++index )
        {
            intptr_t id = pTouchIdByDevice[ index ];
            auto iter = m_currentTouchMap.find( id );
            BEATS_ASSERT(iter != m_currentTouchMap.end(), _T("OnTouchMoved: touch id %d doesn't exists! have you called began?"), id);
            if ( iter != m_currentTouchMap.end() )
            {
                CTouch* pTouch = m_pTouches[ iter->second ];
                pTouch->UpdateTouchInfo( id, xTouchPoint[ index ], yTouchPoint[ index ] );
                event.AddTouch(pTouch);
            }
        }
        if ( event.GetTouchNum() > 0 )
        {
            CWindowManager::GetInstance()->OnTouchEvent( &event );
            DispatchEvent(&event);
        }
    }
}

void CTouchDelegate::OnTouchEnded( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int* tapCount )
{
    OnTouchEndedOrCancelled( EEventType::eET_EVENT_TOUCH_ENDED, num, pTouchIdByDevice, xTouchPoint, yTouchPoint, tapCount );
}

void CTouchDelegate::OnTouchCancelled( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int *tapCount )
{
    OnTouchEndedOrCancelled( EEventType::eET_EVENT_TOUCH_CANCELLED, num, pTouchIdByDevice, xTouchPoint, yTouchPoint, tapCount );
}

void CTouchDelegate::OnTapped( intptr_t touchIdByDevice, float xTouchPoint, float yTouchPoint, unsigned int tapCount )
{
    BEATS_ASSERT(tapCount == 1 || tapCount == 2);
    auto iter = m_currentTouchMap.find( touchIdByDevice );
    BEATS_ASSERT(iter != m_currentTouchMap.end());
    if ( iter != m_currentTouchMap.end() )
    {
        CTouchEvent event(tapCount == 1 ?
                          EEventType::eET_EVENT_TOUCH_SINGLE_TAPPED :
                          EEventType::eET_EVENT_TOUCH_DOUBLE_TAPPED );
        CTouch *pTouch = m_pTouches[iter->second];
        BEATS_ASSERT(pTouch);
        pTouch->UpdateTouchInfo(touchIdByDevice, xTouchPoint, yTouchPoint);
        pTouch->SetTapCount(tapCount);
        event.AddTouch(pTouch);
        CWindowManager::GetInstance()->OnTouchEvent( &event );
        DispatchEvent(&event);
    }
}

void CTouchDelegate::OnPinched( EGestureState state, float fPinchScale)
{
    CPinchEvent event(EEventType::eET_EVENT_TOUCH_PINCHED, state, fPinchScale);
#if(BEYONDENGINE_PLATFORM != PLATFORM_WIN32)
    BEATS_ASSERT(m_currentTouchMap.size() >= 2);
    CTouch* pTouches[2];
    int i = 0;
    for (std::map< intptr_t, int >::iterator iter= m_currentTouchMap.begin(); iter != m_currentTouchMap.end(); ++iter)
    {
        CTouch* pTouch = m_pTouches[ iter->second ];
        pTouches[i++] = pTouch;
        if (i == 2)
        {
            break;
        }
    }

    event.SetTouch(pTouches[0], pTouches[1]);
#endif
    DispatchEvent(&event);
}

int CTouchDelegate::GetUnUsedIndex()
{
    int i = -1;
    int temp = m_indexBitsUsed;

    for (i = 0; i < TOUCH_MAX_NUM; i++)
    {
        if (! (temp & 0x00000001))
        {
            m_indexBitsUsed |= (1 <<  i);
            break;
        }
        temp >>= 1;
    }

    // all bits are used
    return i;
}

void CTouchDelegate::RemoveUsedIndex( int index )
{
    if (index < 0 || index >= TOUCH_MAX_NUM )
    {
        return;
    }

    unsigned int temp = 1 << index;
    temp = ~temp;
    m_indexBitsUsed &= temp;
}

void CTouchDelegate::OnTouchEndedOrCancelled( EEventType type, unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int* tapCount )
{
    if ( m_bEnableTouch )
    {
        CTouchEvent event(type);
        for ( unsigned int index = 0; index < num ; ++index )
        {
            intptr_t id = pTouchIdByDevice[ index ];
            auto iter = m_currentTouchMap.find( id );
            BEATS_ASSERT(iter != m_currentTouchMap.end(), _T("touch id %d doesn't exists"), id);
            if ( iter != m_currentTouchMap.end() )
            {
                CTouch* pTouch = m_pTouches[ iter->second ];
                pTouch->UpdateTouchInfo( id, xTouchPoint[ index ], yTouchPoint[ index ] );
                pTouch->SetTapCount(tapCount[index]);
                event.AddTouch(pTouch);
            }
        }
        if ( event.GetTouchNum() > 0 )
        {
            CWindowManager::GetInstance()->OnTouchEvent( &event );
            DispatchEvent(&event);
        }

        for ( unsigned int index = 0; index < num ; ++index )
        {
            intptr_t id = pTouchIdByDevice[ index ];
            auto iter = m_currentTouchMap.find( id );
            BEATS_ASSERT(iter != m_currentTouchMap.end());
            if ( iter != m_currentTouchMap.end() )
            {
                CTouch* pTouch = m_pTouches[ iter->second ];
                RemoveUsedIndex( iter->second );
                m_currentTouchMap.erase( iter );
                BEATS_SAFE_DELETE( pTouch );
            }
        }
    }
}

void CTouchDelegate::SetEnable( bool bEnable )
{
    m_bEnableTouch = bEnable;
}

bool CTouchDelegate::GetEnable() const
{
    return m_bEnableTouch;
}

const std::map< intptr_t, int >& CTouchDelegate::GetTouchMap() const
{
    return m_currentTouchMap;
}

CTouch* CTouchDelegate::GetTouch(size_t uIndex) const
{
    BEATS_ASSERT(uIndex < TOUCH_MAX_NUM);
    return m_pTouches[uIndex];
}

void CTouchDelegate::UnsubscribeEvent(const CEventSubscription &subscription)
{
    CEventDispatcher::UnsubscribeEvent(subscription);
}

void CTouchDelegate::UnsubscribeEvent(int type, int id)
{
    CEventDispatcher::UnsubscribeEvent(type, id);
}

