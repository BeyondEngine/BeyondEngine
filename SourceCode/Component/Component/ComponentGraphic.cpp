#include "stdafx.h"

#include "ComponentProxy.h"
#include "ComponentGraphic.h"
#include "DependencyDescriptionLine.h"
#include "DependencyDescription.h"

CComponentGraphic::CComponentGraphic()
: m_pOwner(NULL)
, m_width(0)
, m_height(0)
, m_gridPosX(0)
, m_gridPosY(0)
, m_gridPosZ(999)
{
}

CComponentGraphic::~CComponentGraphic()
{
}

void CComponentGraphic::OnRender(float cellSize, bool bSelected)
{
    DrawHead(cellSize);
    DrawDependencies(cellSize);
    if (bSelected)
    {
        DrawSelectedRect(cellSize);
    }
}

void CComponentGraphic::SetPosition( int x, int y )
{
    m_gridPosX = x;
    m_gridPosY = y;
}

void CComponentGraphic::CaculateSize()
{
    BEATS_ASSERT(m_pOwner != NULL);
    uint32_t bodySize = 0;
    if (m_pOwner->GetDependencies() != NULL)
    {
        bodySize = (uint32_t)(m_pOwner->GetDependencies()->size() * DEPENDENCY_HEIGHT);
    }    
    // Notice: we only consider graphics width = body width + connection width, dependency width are ignored.
    m_width = MIN_WIDTH + CONNECTION_WIDTH;
    m_height = HEADER_HEIGHT + bodySize;
}

void CComponentGraphic::GetPosition(int* pOutX, int* pOutY)
{
    *pOutX = m_gridPosX;
    *pOutY = m_gridPosY;
}

void CComponentGraphic::GetDependencyPosition( uint32_t uDependencyIndex, int* pOutX, int* pOutY )
{
    BEATS_ASSERT(m_pOwner->GetDependency(uDependencyIndex) != NULL);
    *pOutX = m_gridPosX + MIN_WIDTH;
    *pOutY = m_gridPosY - HEADER_HEIGHT - (int)uDependencyIndex * DEPENDENCY_HEIGHT;
}

uint32_t CComponentGraphic::GetDependencyWidth()
{
    return DEPENDENCY_WIDTH;
}

uint32_t CComponentGraphic::GetDependencyHeight()
{
    return DEPENDENCY_HEIGHT;
}

uint32_t CComponentGraphic::GetWidth()
{
    return m_width;
}

uint32_t CComponentGraphic::GetHeight()
{
    return m_height;
}

EComponentAeraRectType CComponentGraphic::HitTestForAreaType( int x, int y, void** pReturnData)
{
    EComponentAeraRectType result = eCART_Invalid;
    if (x >= m_gridPosX && x < m_gridPosX + (int)m_width && y <= m_gridPosY && y > m_gridPosY - (int)m_height)
    {
        result = eCART_Body;
        if (y > m_gridPosY - HEADER_HEIGHT && x < m_gridPosX + CONNECTION_WIDTH)
        {
            result = eCART_Connection;
        }
        else if (x >= m_gridPosX + MIN_WIDTH && y <= m_gridPosY - HEADER_HEIGHT)
        {
            result = eCART_Dependency;
            if (pReturnData != NULL)
            {
                (*pReturnData) = m_pOwner->GetDependency((m_gridPosY - HEADER_HEIGHT - y) / DEPENDENCY_HEIGHT);
            }
        }
    }

    return result;
}

void CComponentGraphic::SetOwner(CComponentProxy* pOwner)
{
    m_pOwner = pOwner;
    CaculateSize();
}

CComponentProxy* CComponentGraphic::GetOwner() const
{
    return m_pOwner;
}
uint32_t CComponentGraphic::GetHeaderHeight()
{
    return HEADER_HEIGHT;
}

uint32_t CComponentGraphic::GetConnectWidth()
{
    return CONNECTION_WIDTH;
}

float CComponentGraphic::GetDependencyLineWidth()
{
    return DEPENDENCY_LINE_WIDTH;
}

float CComponentGraphic::GetDependencyLineArrowSize()
{
    return ARROW_SIZE;
}
