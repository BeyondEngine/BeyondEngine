#include "stdafx.h"
#include "WindowEvent.h"
#include "CompWrapper.h"
#include "Event/EventDispatcher.h"
#include "Scene/Node.h"
#include "GUI/Window/Window.h"



CWindowEvent::CWindowEvent(EEventType type, CControl *operand)
    : CBaseEvent(type)
    , m_pOperand(operand)
{

}

CControl *CWindowEvent::OperandWindow() const
{
    return m_pOperand;
}

CControl *CWindowEvent::SourceWindow() const
{
    return down_cast<CControl*>(down_cast<CCompWrapper<CEventDispatcher, CNode> *>(Source())->Owner());
}

