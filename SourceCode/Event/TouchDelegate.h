#ifndef BEYOND_ENGINE_EVENT_TOUCHDELEGATE_H_INCLUDE
#define BEYOND_ENGINE_EVENT_TOUCHDELEGATE_H_INCLUDE

#include "EventType.h"
#include "Event/TouchDispather.h"
#include "Event/GestureState.h"

class CTouch;
class CTouchEvent;
class CEventDispatcher;
class CRenderTarget;

class CTouchDelegate : public CEventDispatcher
{
    BEATS_DECLARE_SINGLETON(CTouchDelegate);
public:

    void OnTouchBegan( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int *tapCount );
    void OnTouchMoved( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint );
    void OnTouchEnded( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int *tapCount );
    void OnTouchCancelled( unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int *tapCount);
    void OnTapped( intptr_t touchIdByDevice, float xTouchPoint, float yTouchPoint, unsigned int tapCount );
    void OnPinched(EGestureState state, float fPinchScale);

    template <typename MemberFunc, typename ObjectType>
    CEventConnection SubscribeEvent(int type, const MemberFunc &func, ObjectType *obj,
        CEventDispatcher::EHandlerPriority priority = CEventDispatcher::eHP_DEFAULT);

    void UnsubscribeEvent(const CEventSubscription &subscription);
    void UnsubscribeEvent(int type, int id);

    void SetEnable( bool bEnable );
    bool GetEnable() const;

    const std::map< intptr_t, int >& GetTouchMap() const;
    CTouch* GetTouch(size_t uIndex) const;
private:
    int GetUnUsedIndex();
    void RemoveUsedIndex( int index );
    void OnTouchEndedOrCancelled( EEventType type, unsigned int num, intptr_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint, unsigned int* tapCount );

private:
    bool m_bEnableTouch;
    CTouch* m_pTouches[ TOUCH_MAX_NUM ];
    unsigned int m_indexBitsUsed;
    std::map< intptr_t, int > m_currentTouchMap;
};

template <typename MemberFunc, typename ObjectType>
CEventConnection CTouchDelegate::SubscribeEvent(int type, const MemberFunc &func, ObjectType *obj,
                                                CEventDispatcher::EHandlerPriority priority)
{
    return CEventDispatcher::SubscribeEvent(type, func, obj, priority);
}

#endif//TOUCHRECEIVERPOOH_H_INCLUDE