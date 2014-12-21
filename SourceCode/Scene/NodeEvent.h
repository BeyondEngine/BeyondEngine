#ifndef BEYOND_ENGINE_SCENE_NODEEVENT_H__INCLUDE
#define BEYOND_ENGINE_SCENE_NODEEVENT_H__INCLUDE

#include "Event/BaseEvent.h"
#include "EnginePublic/CompWrapper.h"
#include "Event/EventDispatcher.h"

class CNodeEvent : public CBaseEvent
{
public:
    CNodeEvent(EEventType type)
        : CBaseEvent(type)
    {
    }

    CNode *SourceNode() const
    {
        return down_cast<CCompWrapper<CEventDispatcher, CNode> *>(Source())->Owner();
    }
};

#endif