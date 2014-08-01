#ifndef BEYOND_ENGINE_EVENT_KEYBOARDEVENT_H__INCLUDE
#define BEYOND_ENGINE_EVENT_KEYBOARDEVENT_H__INCLUDE

#include "Event/BaseEvent.h"

class CKeyboardEvent : public CBaseEvent
{
public:
    CKeyboardEvent(EEventType type, int key, int mods = 0)
        : CBaseEvent(type)
        , m_nKey(key)
        , m_nMods(mods)
    {}

    int Key() const
    {
        return m_nKey;
    }

    bool IsControlKeyDown(int ctrlKey) const
    {
        return (m_nMods & ctrlKey) != 0;
    }

    TCHAR TChar() const
    {
        return static_cast<TCHAR>(m_nKey);
    }

private:
    int m_nKey;
    int m_nMods;
};

#endif