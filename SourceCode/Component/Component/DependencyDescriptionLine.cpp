#include "stdafx.h"
#include "DependencyDescription.h"
#include "DependencyDescriptionLine.h"
#include "ComponentGraphic.h"
#include "ComponentProxy.h"
#include "Utility/MathExt/Mat4.h"

CDependencyDescriptionLine::CDependencyDescriptionLine( CDependencyDescription* pOwner, uint32_t uIndex, CComponentProxy* pTo )
: m_bIsSelected(false)
, m_uIndex(uIndex)
, m_pOwner(pOwner)
, m_pConnectedComponent(NULL)
{
    m_pRect = new SVertex[4];
    m_pArrowRect = new SVertex[4];
    for (uint32_t i = 0; i < 4; ++i)
    {
        m_pRect[i].m_color = 0xffffffff;
        m_pRect[i].m_uv.X() = i < 2 ? 0.f : 1.f;
        m_pRect[i].m_uv.Y() = (i % 2 == 0) ? 0.f : 1.f;
        m_pArrowRect[i].m_color = 0xffffffff;
        m_pArrowRect[i].m_uv.X() = i < 2 ? 0.f : 1.f;
        m_pArrowRect[i].m_uv.Y() = (i % 2 == 0) ? 0.f : 1.f;
    }
    SetConnectComponent(pTo);
}

CDependencyDescriptionLine::~CDependencyDescriptionLine()
{
    m_pConnectedComponent->RemoveBeConnectedDependencyDescriptionLine(this);
    m_pOwner->RemoveDependencyLine(this);
    BEATS_SAFE_DELETE_ARRAY(m_pRect);
    BEATS_SAFE_DELETE_ARRAY(m_pArrowRect);
}

void CDependencyDescriptionLine::UpdateRect(float cellSize, bool bInverseY /*= false*/)
{
    if (m_pOwner != NULL && m_pConnectedComponent != NULL)
    {
        int iDependencyPosX = 0; 
        int iDependencyPosY = 0;
        uint32_t uIndex = m_pOwner->GetIndex();
        
        CComponentGraphic* pGraphics = static_cast<CComponentProxy*>(m_pOwner->GetOwner())->GetGraphics();
        BEATS_ASSERT(pGraphics != NULL);
        pGraphics->GetDependencyPosition(uIndex, &iDependencyPosX, &iDependencyPosY);
        // 1. Draw line rect
        // a. calculate angle.
        CVec2 point[2];
        point[0].X() = iDependencyPosX * cellSize + cellSize * 0.5f * pGraphics->GetDependencyWidth();
        point[0].Y() = iDependencyPosY * cellSize - cellSize * 0.5f * pGraphics->GetDependencyHeight() * (bInverseY ? -1 : 1);

        int x = 0;
        int y = 0;
        static_cast<CComponentProxy*>(m_pConnectedComponent)->GetGraphics()->GetPosition(&x, &y);
        point[1].X() = x * cellSize + cellSize * 0.5f * pGraphics->GetConnectWidth();
        point[1].Y() = y * cellSize - cellSize * 0.5f * pGraphics->GetHeaderHeight() * (bInverseY ? -1 : 1);

        CVec2 direction(point[1] - point[0]);
        float fAngle = atan2f(direction.X(), direction.Y());
        float fLength = direction.Length();
        CMat4 rotateTransform;
        rotateTransform.FromAxisAngle(CVec3(0, 0, 1),-fAngle);
        // b. found a rect line at the dependency center with direction up.

        // left top.
        CVec2 leftTop(-pGraphics->GetDependencyLineWidth(), fLength);
        CVec2 newLeftTop = rotateTransform * leftTop;
        newLeftTop.X() += point[0].X();
        newLeftTop.Y() += point[0].Y();

        m_pRect[0].m_position.X() = newLeftTop.X();
        m_pRect[0].m_position.Y() = newLeftTop.Y();
        m_pRect[0].m_position.Z() = 0.0f;

        // left down.
        CVec2 leftDown(-pGraphics->GetDependencyLineWidth(), 0);
        CVec2 newLeftDown = rotateTransform * leftDown;
        newLeftDown.X() += point[0].X();
        newLeftDown.Y() += point[0].Y();
        if (bInverseY)
        {
            newLeftDown.Y() -= cellSize * pGraphics->GetHeaderHeight() * 0.5f;
        }

        m_pRect[1].m_position.X() = newLeftDown.X();
        m_pRect[1].m_position.Y() = newLeftDown.Y();
        m_pRect[1].m_position.Z() = 0.0f;

        // right top.
        CVec2 rightTop(pGraphics->GetDependencyLineWidth(), fLength);
        CVec2 newRightTop = rotateTransform * rightTop;
        newRightTop.X() += point[0].X();
        newRightTop.Y() += point[0].Y();

        m_pRect[2].m_position.X() = newRightTop.X();
        m_pRect[2].m_position.Y() = newRightTop.Y();
        m_pRect[2].m_position.Z() = 0.0f;

        // right down.
        CVec2 rightDown(pGraphics->GetDependencyLineWidth(), 0);
        CVec2 newRightDown = rotateTransform * rightDown;
        newRightDown.X() += point[0].X();
        newRightDown.Y() += point[0].Y();
        if (bInverseY)
        {
            newRightDown.Y() -= cellSize * pGraphics->GetHeaderHeight() * 0.5f;
        }

        m_pRect[3].m_position.X() = newRightDown.X();
        m_pRect[3].m_position.Y() = newRightDown.Y();
        m_pRect[3].m_position.Z() = 0.0f;

        //2. Render arrow
        // left top
        float fArrowSize = pGraphics->GetDependencyLineArrowSize();
        CVec2 arrowLeftTop( -fArrowSize, fLength);
        CVec2 newArrowLeftTop = rotateTransform * arrowLeftTop;
        newArrowLeftTop.X() += point[0].X();
        newArrowLeftTop.Y() += point[0].Y();

        m_pArrowRect[0].m_position.X() = newArrowLeftTop.X();
        m_pArrowRect[0].m_position.Y() = newArrowLeftTop.Y();
        m_pArrowRect[0].m_position.Z() = 0.0f;

        //left down
        CVec2 arrowLeftDown(-fArrowSize, fLength - fArrowSize);
        CVec2 newArrowLeftDown = rotateTransform * arrowLeftDown;
        newArrowLeftDown.X() += point[0].X();
        newArrowLeftDown.Y() += point[0].Y();

        m_pArrowRect[1].m_position.X() = newArrowLeftDown.X();
        m_pArrowRect[1].m_position.Y() = newArrowLeftDown.Y();
        m_pArrowRect[1].m_position.Z() = 0.0f;

        //right top
        CVec2 arrowRightTop(fArrowSize, fLength);
        CVec2 newArrowRightTop = rotateTransform * arrowRightTop;
        newArrowRightTop.X() += point[0].X();
        newArrowRightTop.Y() += point[0].Y();

        m_pArrowRect[2].m_position.X() = newArrowRightTop.X();
        m_pArrowRect[2].m_position.Y() = newArrowRightTop.Y();
        m_pArrowRect[2].m_position.Z() = 0.0f;

        //right down
        CVec2 arrowRightDown(fArrowSize, fLength - fArrowSize);
        CVec2 newArrowRightDown = rotateTransform * arrowRightDown;
        newArrowRightDown.X() += point[0].X();
        newArrowRightDown.Y() += point[0].Y();

        m_pArrowRect[3].m_position.X() = newArrowRightDown.X();
        m_pArrowRect[3].m_position.Y() = newArrowRightDown.Y();
        m_pArrowRect[3].m_position.Z() = 0.0f;
    }
}

const SVertex* CDependencyDescriptionLine::GetRectArray()
{
    return m_pRect;
}

CDependencyDescription* CDependencyDescriptionLine::GetOwnerDependency()
{
    return m_pOwner;
}

CComponentProxy* CDependencyDescriptionLine::GetConnectedComponent()
{
    return m_pConnectedComponent;
}

void CDependencyDescriptionLine::SetConnectComponent( CComponentProxy* pComponentTo )
{
    if (m_pConnectedComponent != NULL)
    {
        m_pConnectedComponent->RemoveBeConnectedDependencyDescriptionLine(this);
    }
    m_pConnectedComponent = pComponentTo;
    if (m_pConnectedComponent != NULL)
    {
        m_pConnectedComponent->AddBeConnectedDependencyDescriptionLine(this);
    }
}

bool CDependencyDescriptionLine::IsSelected()
{
    return m_bIsSelected;
}

void CDependencyDescriptionLine::SetSelected(bool bFlag)
{
    m_bIsSelected = bFlag;
}

uint32_t CDependencyDescriptionLine::GetIndex()
{
    return m_uIndex;
}

void CDependencyDescriptionLine::SetIndex( uint32_t uIndex )
{
    m_uIndex = uIndex;
}

const SVertex* CDependencyDescriptionLine::GetArrowRectArray()
{
    return m_pArrowRect;
}

bool CDependencyDescriptionLine::HitTest( float x, float y )
{
    bool bRet = false;
    BEATS_ASSERT(m_pRect != NULL);
    if (m_pRect != NULL)
    {
        float top = (m_pRect[1].m_position.X() - m_pRect[0].m_position.X()) * (y - m_pRect[0].m_position.Y()) - (x - m_pRect[0].m_position.X()) * (m_pRect[1].m_position.Y() - m_pRect[0].m_position.Y());
        float buttom = (m_pRect[3].m_position.X() - m_pRect[2].m_position.X()) * (y - m_pRect[2].m_position.Y()) - (x - m_pRect[2].m_position.X()) * (m_pRect[3].m_position.Y() - m_pRect[2].m_position.Y());
        bool bBetweenHorizontal = top * buttom <= 0;

        float left = (m_pRect[3].m_position.X() - m_pRect[1].m_position.X()) * (y - m_pRect[1].m_position.Y()) - (x - m_pRect[1].m_position.X()) * (m_pRect[3].m_position.Y() - m_pRect[1].m_position.Y());
        float right = (m_pRect[2].m_position.X() - m_pRect[0].m_position.X()) * (y - m_pRect[0].m_position.Y()) - (x - m_pRect[0].m_position.X()) * (m_pRect[2].m_position.Y() - m_pRect[0].m_position.Y());
        bool bBetweenVertical = left * right <= 0;
        bRet = bBetweenHorizontal && bBetweenVertical;
    }
    return bRet;
}
