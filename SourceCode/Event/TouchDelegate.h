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
    typedef CEventDispatcher super;
    BEATS_DECLARE_SINGLETON(CTouchDelegate);
public:

    void OnTouchBegan(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint);
    void OnTouchMoved(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint);
    void OnTouchEnded(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint);
    void OnTouchCancelled(unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint);
    void OnPinched(EGestureState state, float fPinchScale, CTouch* pTouch1 = nullptr, CTouch* pTouch2 = nullptr);

    template <typename MemberFunc, typename ObjectType>
    CEventConnection SubscribeEvent(int type, const MemberFunc &func, ObjectType *obj,
        CEventDispatcher::EHandlerPriority priority = CEventDispatcher::eHP_DEFAULT);

    void UnsubscribeEvent(const CEventSubscription &subscription);
    void UnsubscribeEvent(int type, int id);

    void SetEnable( bool bEnable );
    bool GetEnable() const;

    const std::map< size_t, int >& GetTouchMap() const;
    CTouch* GetTouch(uint32_t uIndex) const;

    virtual void DispatchEvent(CBaseEvent *event) override;
    const CVec2& GetLastTapScreenPos() const;

private:
    int GetUnUsedIndex();
    void RemoveUsedIndex( int index );
    void OnTouchEndedOrCancelled(EEventType type, unsigned int num, size_t* pTouchIdByDevice, float* xTouchPoint, float* yTouchPoint);

private:
    bool m_bEnableTouch;
    CTouch* m_pTouches[ TOUCH_MAX_NUM ];
    unsigned int m_indexBitsUsed;
    uint32_t m_uLastTapTime = 0;
    int m_nLastTapTouchId = -1;
    static const uint32_t TAP_INTERVAL = 250;
    static const uint32_t TAP_DEVIATION = 35;
#ifdef DEVELOP_VERSION
    uint32_t m_uLogScrollRecord = 0xFFFFFFFF;
#endif
    CVec2 m_lastTapScreenPos;
    std::map< size_t, int > m_currentTouchMap;  //key for touch id, value for index
};

template <typename MemberFunc, typename ObjectType>
CEventConnection CTouchDelegate::SubscribeEvent(int type, const MemberFunc &func, ObjectType *obj,
                                                CEventDispatcher::EHandlerPriority priority)
{
    return CEventDispatcher::SubscribeEvent(type, func, obj, priority);
}

#endif//TOUCHRECEIVERPOOH_H_INCLUDE