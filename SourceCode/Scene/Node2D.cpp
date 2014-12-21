#include "stdafx.h"
#include "Node2D.h"

CNode2D::CNode2D()
{
    m_defaultGroupID = LAYER_2D;
}

CNode2D::~CNode2D()
{
}

ENodeType CNode2D::GetType() const
{
    return eNT_Node2D;
}
