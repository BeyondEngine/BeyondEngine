#ifndef BEYOND_ENGINE_SCENE_NODE2D_H__INCLUDE
#define BEYOND_ENGINE_SCENE_NODE2D_H__INCLUDE

#include "Node.h"

class CNode2D : public CNode
{
    DECLARE_REFLECT_GUID( CNode2D, 0x1458a518, CNode )
public:
    CNode2D();
    virtual ~CNode2D();
    virtual ENodeType GetType() const override;

};

#endif

