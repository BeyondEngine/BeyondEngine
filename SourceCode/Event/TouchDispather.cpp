#include "stdafx.h"
#include "TouchDispather.h"
#include "Event/TouchEvent.h"

CTouchDispather::CTouchDispather()
{

}

CTouchDispather::~CTouchDispather()
{

}

void CTouchDispather::AddTouch( CTouch* pTouch )
{
    m_vecTouch.push_back( pTouch );
}

void CTouchDispather::DispatchEvent( EEventType type )
{
    CTouchEvent touchEvent(type);
    for ( auto iter : m_vecTouch )
    {
        if ( iter )
        {
            touchEvent.AddTouch( iter );
        }
    }

    CEventDispatcher::DispatchEvent( &touchEvent );
}


void CTouchDispather::Remove(CTouch *pTouch)
{
    auto iter = m_vecTouch.begin();
    for ( ; iter != m_vecTouch.end(); ++iter )
    {
        if ( *iter == pTouch )
        {
            m_vecTouch.erase( iter );
            break;
        }
    }
}