#include "stdafx.h"
#include "CoordinateRenderObject.h"
#include "RenderManager.h"
#include "RenderTarget.h"
#include "ShapeRenderer.h"
#ifdef EDITOR_MODE
#include "BeyondEngineEditor/EditorConfig.h"
#endif

CCoordinateRenderObject::CCoordinateRenderObject()
: m_pNode(nullptr)
, m_translateMode(eCTM_XY)
{
#ifdef EDITOR_MODE
    m_defaultGroupID = LAYER_3D_DEBUG_PRIMITIVE;
    m_axisColorX = CEditorConfig::GetInstance()->GetAxisColorX();
    m_axisColorY = CEditorConfig::GetInstance()->GetAxisColorY();
    m_axisColorZ = CEditorConfig::GetInstance()->GetAxisColorZ();
    m_axisSelectColor = CEditorConfig::GetInstance()->GetAxisSelectColor();
    m_panelSelectColor = CEditorConfig::GetInstance()->GetPanelSelectColor();
    m_fAxisLengthFactor = CEditorConfig::GetInstance()->GetAxisLengthFactor();
    m_fConeHeightFactor = CEditorConfig::GetInstance()->GetConeHeightFactor();
    m_fConeBottomColorFactor = CEditorConfig::GetInstance()->GetConeBottomColorFactor();
    m_fConeAngle = CEditorConfig::GetInstance()->GetConeAngle();
    m_fTranslatePanelStartPosRate = CEditorConfig::GetInstance()->GetTranslatePanelStartPosRate();
#endif
}

CCoordinateRenderObject::~CCoordinateRenderObject()
{

}

void CCoordinateRenderObject::SetRenderNode(CNode* pNode)
{
    m_pNode = pNode;
}

CNode* CCoordinateRenderObject::GetRenderNode() const
{
    return m_pNode;
}

void CCoordinateRenderObject::RenderMatrix(const CMat4& mat, bool b3DOr2D)
{
    CMat4 matTmp = mat;
    matTmp.RemoveScale();
    float fPosW = matTmp[15];
    BEATS_ASSERT(!BEATS_FLOAT_EQUAL(fPosW, 0));
    m_originPos.Fill(matTmp[12] / fPosW, matTmp[13] / fPosW, matTmp[14] / fPosW);

    if (b3DOr2D)
    {
        // To keep the axis length constant in screen, we use the distance between the pMatrix and camera.
        CCamera* pCamera = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D);
        float fFOVRadian = DegreesToRadians(pCamera->GetFOV());
        float fFOVScale = tanf(fFOVRadian * 0.5f);
        CVec3 cameraPos = pCamera->GetViewPos();

        m_fAxisLength = (m_originPos - cameraPos).Length() * m_fAxisLengthFactor * fFOVScale;
    }
    else
    {
        m_fAxisLength = CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetDeviceWidth() * 0.2f;
    }
    float fConeHeight = m_fConeHeightFactor * m_fAxisLength;
    float fConeRadius = fConeHeight * tanf(DegreesToRadians(m_fConeAngle));
    m_fPanelSize = m_fAxisLength * m_fTranslatePanelStartPosRate;

    // Draw X axis
    bool bSelectXAxis = m_translateMode == eCTM_X || m_translateMode == eCTM_XY || m_translateMode == eCTM_XZ;
    CColor XAxisColor = bSelectXAxis ? m_axisSelectColor : m_axisColorX;
    CVertexPC originPos, xPos, yPos, zPos;
    originPos.position = m_originPos;
    originPos.color = XAxisColor;

    xPos.position = matTmp.GetRightVec3() * m_fAxisLength + m_originPos;
    xPos.color = XAxisColor;
    if (b3DOr2D)
    {
        CRenderManager::GetInstance()->RenderLine(originPos, xPos, 1.0f, true);
    }
    else
    {
        CRenderManager::GetInstance()->Render2DLine(originPos, xPos);
    }

    // Draw xy and xz panel.
    CVertexPC xStartPos, xyEndPos, xzEndPos;
    CVec3 xPanelPos = matTmp * CVec3(m_fPanelSize, 0, 0);
    xStartPos.position = xPanelPos;

    CVec3 xyPanelPos = matTmp * CVec3(m_fPanelSize, m_fPanelSize, 0);
    xyEndPos.position = xyPanelPos;
    xyEndPos.color = m_translateMode == eCTM_XY ? m_axisSelectColor : m_axisColorX;
    xStartPos.color = m_translateMode == eCTM_XY ? m_axisSelectColor : m_axisColorX;
    if (b3DOr2D)
    {
        CRenderManager::GetInstance()->RenderLine(xStartPos, xyEndPos, 1.0f, true);
    }
    else
    {
        CRenderManager::GetInstance()->Render2DLine(xStartPos, xyEndPos);
    }

    if (b3DOr2D)
    {
        CVec3 xzPanelPos = matTmp * CVec3(m_fPanelSize, 0, m_fPanelSize);
        xzEndPos.position = xzPanelPos;
        xzEndPos.color = m_translateMode == eCTM_XZ ? m_axisSelectColor : m_axisColorX;
        xStartPos.color = m_translateMode == eCTM_XZ ? m_axisSelectColor : m_axisColorX;
        CRenderManager::GetInstance()->RenderLine(xStartPos, xzEndPos, 1.0f, true);
    }

    //Draw X cone
    CMat4 coneMatX;
    coneMatX.FromPitchYawRoll(0, 0, DegreesToRadians(-90.f));
    coneMatX[12] = m_fAxisLength;
    coneMatX[13] = 0;
    coneMatX[14] = 0;
    coneMatX = matTmp * coneMatX;
    if (b3DOr2D)
    {
        CShapeRenderer::GetInstance()->DrawCone(coneMatX, fConeRadius, 0, fConeHeight, m_axisColorX, m_axisColorX * m_fConeBottomColorFactor, m_axisColorX, true);
    }
    else
    {
        CVertex2DPC pt1, pt2, pt3;
        pt1.color = 0xFF0000FF;
        pt2.color = 0xFF0000FF;
        pt3.color = 0xFF0000FF;
        float fTriangleRadius = m_fAxisLength * 0.2f;
        pt1.position.Fill(xPos.position.X(), xPos.position.Y() + fTriangleRadius * 0.5f);
        pt2.position.Fill(xPos.position.X(), xPos.position.Y() - fTriangleRadius * 0.5f);
        pt3.position.Fill(xPos.position.X() + fTriangleRadius, xPos.position.Y());
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
    }

    //Draw Y axis
    bool bSelectYAxis = m_translateMode == eCTM_Y || m_translateMode == eCTM_XY || m_translateMode == eCTM_YZ;
    CColor YAxisColor = bSelectYAxis ? m_axisSelectColor : m_axisColorY;
    yPos.position = matTmp.GetUpVec3() * m_fAxisLength + m_originPos;
    yPos.color = YAxisColor;
    originPos.color = YAxisColor;
    if (b3DOr2D)
    {
        CRenderManager::GetInstance()->RenderLine(originPos, yPos, 1.0f, true);
    }
    else
    {
        CRenderManager::GetInstance()->Render2DLine(originPos, yPos);
    }

    // Draw yx and yz panel.
    CVertexPC yStartPos, yzEndPos;
    CVec3 yPanelPos = matTmp * CVec3(0, m_fPanelSize, 0);
    yStartPos.position = yPanelPos;
    yStartPos.color = m_translateMode == eCTM_XY ? m_axisSelectColor : m_axisColorY;
    xyEndPos.color = m_translateMode == eCTM_XY ? m_axisSelectColor : m_axisColorY;
    if (b3DOr2D)
    {
        CRenderManager::GetInstance()->RenderLine(yStartPos, xyEndPos, 1.0f, true);
    }
    else
    {
        CRenderManager::GetInstance()->Render2DLine(yStartPos, xyEndPos);
    }
    if (b3DOr2D)
    {
        CVec3 yzPanelPos = matTmp * CVec3(0, m_fPanelSize, m_fPanelSize);
        yzEndPos.position = yzPanelPos;
        yStartPos.color = m_translateMode == eCTM_YZ ? m_axisSelectColor : m_axisColorY;
        yzEndPos.color = m_translateMode == eCTM_YZ ? m_axisSelectColor : m_axisColorY;
        CRenderManager::GetInstance()->RenderLine(yStartPos, yzEndPos, 1.0f, true);
    }
    // Draw Y cone
    if (b3DOr2D)
    {
        CMat4 coneMatY;
        coneMatY[12] = 0;
        coneMatY[13] = m_fAxisLength;
        coneMatY[14] = 0;
        coneMatY = matTmp* coneMatY;
        CShapeRenderer::GetInstance()->DrawCone(coneMatY, fConeRadius, 0, fConeHeight, m_axisColorY, m_axisColorY * m_fConeBottomColorFactor, m_axisColorY, true);
    }
    else
    {
        CVertex2DPC pt1, pt2, pt3;
        pt1.color = 0x00FF00FF;
        pt2.color = 0x00FF00FF;
        pt3.color = 0x00FF00FF;
        float fTriangleRadius = m_fAxisLength * 0.2f;
        pt1.position.Fill(yPos.position.X() + fTriangleRadius * 0.5f, yPos.position.Y());
        pt2.position.Fill(yPos.position.X() - fTriangleRadius * 0.5f, yPos.position.Y());
        pt3.position.Fill(yPos.position.X(), yPos.position.Y() + fTriangleRadius);
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
    }

    // Draw zx and zy panel.
    CVertexPC zStartPos;
    if (b3DOr2D)
    {
        // Draw Z axis
        bool bSelectZAxis = m_translateMode == eCTM_Z || m_translateMode == eCTM_XZ || m_translateMode == eCTM_YZ;
        CColor ZAxisColor = bSelectZAxis ? m_axisSelectColor : m_axisColorZ;
        zPos.position = matTmp.GetForwardVec3() * m_fAxisLength + m_originPos;
        zPos.color = ZAxisColor;
        originPos.color = ZAxisColor;
        CRenderManager::GetInstance()->RenderLine(originPos, zPos, 1.0f, true);

        CVec3 zPanelPos = matTmp * CVec3(0, 0, m_fPanelSize);
        zStartPos.position = zPanelPos;
        zStartPos.color = m_translateMode == eCTM_XZ ? m_axisSelectColor : m_axisColorZ;
        xzEndPos.color = m_translateMode == eCTM_XZ ? m_axisSelectColor : m_axisColorZ;
        CRenderManager::GetInstance()->RenderLine(zStartPos, xzEndPos, 1.0f, true);
        zStartPos.color = m_translateMode == eCTM_YZ ? m_axisSelectColor : m_axisColorZ;
        yzEndPos.color = m_translateMode == eCTM_YZ ? m_axisSelectColor : m_axisColorZ;
        CRenderManager::GetInstance()->RenderLine(zStartPos, yzEndPos, 1.0f, true);

        //Draw Z cone
        CMat4 coneMatZ;
        coneMatZ.FromPitchYawRoll(DegreesToRadians(90.f), 0, 0);
        coneMatZ[12] = 0;
        coneMatZ[13] = 0;
        coneMatZ[14] = m_fAxisLength;
        coneMatZ = matTmp* coneMatZ;
        CShapeRenderer::GetInstance()->DrawCone(coneMatZ, fConeRadius, 0, fConeHeight, m_axisColorZ, m_axisColorZ * m_fConeBottomColorFactor, m_axisColorZ, true);
    }
    if (m_pNode != nullptr &&
        (m_translateMode == eCTM_XY || m_translateMode == eCTM_YZ || m_translateMode == eCTM_XZ))
    {
        // Render select panel
        if (b3DOr2D)
        {
            static std::vector<unsigned short> indices;
            if (indices.size() == 0)
            {
                indices.push_back(0);
                indices.push_back(1);
                indices.push_back(3);
                indices.push_back(0);
                indices.push_back(2);
                indices.push_back(3);
            }
            std::vector<CVertexPC> selectPanelPoints;
            originPos.color = m_panelSelectColor;
            selectPanelPoints.push_back(originPos);
            switch (m_translateMode)
            {
            case eCTM_XY:
            {
                            xStartPos.color = m_panelSelectColor;
                            yStartPos.color = m_panelSelectColor;
                            xyEndPos.color = m_panelSelectColor;
                            selectPanelPoints.push_back(xStartPos);
                            selectPanelPoints.push_back(yStartPos);
                            selectPanelPoints.push_back(xyEndPos);
            }
                break;
            case eCTM_YZ:
            {
                            yStartPos.color = m_panelSelectColor;
                            zStartPos.color = m_panelSelectColor;
                            yzEndPos.color = m_panelSelectColor;
                            selectPanelPoints.push_back(yStartPos);
                            selectPanelPoints.push_back(zStartPos);
                            selectPanelPoints.push_back(yzEndPos);
            }
                break;
            case eCTM_XZ:
            {
                            xStartPos.color = m_panelSelectColor;
                            zStartPos.color = m_panelSelectColor;
                            xzEndPos.color = m_panelSelectColor;
                            selectPanelPoints.push_back(xStartPos);
                            selectPanelPoints.push_back(zStartPos);
                            selectPanelPoints.push_back(xzEndPos);
            }
                break;
            default:
                BEATS_ASSERT(false, "Never reach here!");
                break;
            }
            CRenderManager::GetInstance()->RenderTriangle(selectPanelPoints, indices, true);
        }
        else
        {
            if (m_translateMode == eCTM_XY)
            {
                CVertex2DPC pt1, pt2, pt3, pt4;
                pt1.color = m_panelSelectColor;
                pt2.color = m_panelSelectColor;
                pt3.color = m_panelSelectColor;
                pt4.color = m_panelSelectColor;
                pt1.position.Fill(xStartPos.position.X(), xStartPos.position.Y());
                pt2.position.Fill(yStartPos.position.X(), yStartPos.position.Y());
                pt3.position.Fill(xyEndPos.position.X(), xyEndPos.position.Y());
                pt4.position.Fill(originPos.position.X(), originPos.position.Y());

                CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
                CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt4);
            }
        }
    }
}

bool CCoordinateRenderObject::RefreshTranslateMode(float x, float y)
{
    bool bRet = false;
    float fAxisSelectDistance = m_fAxisLength * 0.1f;
    if (m_pNode->GetType() == eNT_Node3D)
    {
        CRay3 selectRay = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->GetRayFromScreenPos(x, y);
        CPlane xzPlane, xyPlane, yzPlane;
        BEATS_ASSERT(m_pNode != nullptr);
        const CMat4& worldTM = m_pNode->GetWorldTM();
        xzPlane.FromPointNormal(m_originPos, worldTM.GetUpVec3());
        xyPlane.FromPointNormal(m_originPos, worldTM.GetForwardVec3());
        yzPlane.FromPointNormal(m_originPos, worldTM.GetRightVec3());
        CMat4 inverseMat = worldTM;
        inverseMat.RemoveScale();
        inverseMat.Inverse();
        float fMinimalAxisDistance = FLT_MAX;
        float fMinimalPanelValue = -1000.f;
        float fIntersectDistance = 0;
        // Calculate the distance between the intersect pos and each axis
        // 1. If we select panel, we no longer detect axis, set the fMinimalPanelValue to a positive number.
        // 2. If we select multi panel, choose the nearest one with compare value fMinimalPanelValue
        // 3. If no panel is selected, we detect the nearest axis, with fMinimalAxisDistance.
        if (selectRay.IntersectPlane(xzPlane, fIntersectDistance))
        {
            CVec3 intersectPosWorld = selectRay.GetStartPos() + selectRay.GetDirection() * fIntersectDistance;
            CVec3 intersectPos = intersectPosWorld * inverseMat;// Transform to local pos.
            float fDistanceToZ = fabs(intersectPos.X());
            float fDistanceToX = fabs(intersectPos.Z());
            if (fDistanceToX < m_fAxisLength && fDistanceToZ < m_fAxisLength)
            {
                if (intersectPos.X() > 0 && intersectPos.Z() > 0 && intersectPos.X() < m_fPanelSize && intersectPos.Z() < m_fPanelSize)
                {
                    if (fMinimalPanelValue < 0 || fIntersectDistance < fMinimalPanelValue)
                    {
                        fMinimalPanelValue = fIntersectDistance;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_translateMode = eCTM_XZ;
                        bRet = true;
                    }
                }
                else if (fMinimalPanelValue < 0)
                {
                    if (fDistanceToZ < fAxisSelectDistance && fDistanceToZ < fMinimalAxisDistance)
                    {
                        fMinimalAxisDistance = fDistanceToZ;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_pickPosOffset = worldTM.GetForwardVec3() * m_pickPosOffset.Dot(worldTM.GetForwardVec3());
                        m_translateMode = eCTM_Z;
                        bRet = true;
                    }
                    if (fDistanceToX < fAxisSelectDistance && fDistanceToX < fMinimalAxisDistance)
                    {
                        fMinimalAxisDistance = fDistanceToX;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_pickPosOffset = worldTM.GetRightVec3() * m_pickPosOffset.Dot(worldTM.GetRightVec3());
                        m_translateMode = eCTM_X;
                        bRet = true;
                    }
                }
            }
        }
        if (selectRay.IntersectPlane(xyPlane, fIntersectDistance))
        {
            CVec3 intersectPosWorld = selectRay.GetStartPos() + selectRay.GetDirection() * fIntersectDistance;
            CVec3 intersectPos = intersectPosWorld * inverseMat;// Transform to local pos.
            float fDistanceToY = fabs(intersectPos.X());
            float fDistanceToX = fabs(intersectPos.Y());
            if (fDistanceToX < m_fAxisLength && fDistanceToY < m_fAxisLength)
            {
                if (intersectPos.X() > 0 && intersectPos.Y() > 0 && intersectPos.X() < m_fPanelSize && intersectPos.Y() < m_fPanelSize)
                {
                    if (fMinimalPanelValue < 0 || fIntersectDistance < fMinimalPanelValue)
                    {
                        fMinimalPanelValue = fIntersectDistance;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_translateMode = eCTM_XY;
                        bRet = true;
                    }
                }
                else if (fMinimalPanelValue < 0)
                {
                    if (fDistanceToY < fAxisSelectDistance && fDistanceToY < fMinimalAxisDistance)
                    {
                        fMinimalAxisDistance = fDistanceToY;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_pickPosOffset = worldTM.GetUpVec3() * m_pickPosOffset.Dot(worldTM.GetUpVec3());
                        m_translateMode = eCTM_Y;
                        bRet = true;
                    }
                    if (fDistanceToX < fAxisSelectDistance && fDistanceToX < fMinimalAxisDistance)
                    {
                        fMinimalAxisDistance = fDistanceToX;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_pickPosOffset = worldTM.GetRightVec3() * m_pickPosOffset.Dot(worldTM.GetRightVec3());
                        m_translateMode = eCTM_X;
                        bRet = true;
                    }
                }
            }
        }
        if (selectRay.IntersectPlane(yzPlane, fIntersectDistance))
        {
            CVec3 intersectPosWorld = selectRay.GetStartPos() + selectRay.GetDirection() * fIntersectDistance;
            CVec3 intersectPos = intersectPosWorld * inverseMat;// Transform to local pos.
            float fDistanceToY = fabs(intersectPos.Z());
            float fDistanceToZ = fabs(intersectPos.Y());
            if (fDistanceToY < m_fAxisLength && fDistanceToZ < m_fAxisLength)
            {
                if (intersectPos.Y() > 0 && intersectPos.Z() > 0 && intersectPos.Y() < m_fPanelSize && intersectPos.Z() < m_fPanelSize)
                {
                    if (fMinimalPanelValue < 0 || fIntersectDistance < fMinimalPanelValue)
                    {
                        fMinimalPanelValue = fIntersectDistance;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_translateMode = eCTM_YZ;
                        bRet = true;
                    }
                }
                else if (fMinimalPanelValue < 0)
                {
                    if (fDistanceToZ < fAxisSelectDistance && fDistanceToZ < fMinimalAxisDistance)
                    {
                        fMinimalAxisDistance = fDistanceToZ;
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_pickPosOffset = worldTM.GetForwardVec3() * m_pickPosOffset.Dot(worldTM.GetForwardVec3());
                        m_translateMode = eCTM_Z;
                        bRet = true;
                    }
                    if (fDistanceToY < fAxisSelectDistance && fDistanceToY < fMinimalAxisDistance)
                    {
                        m_pickPosOffset = intersectPosWorld - m_originPos;
                        m_pickPosOffset = worldTM.GetUpVec3() * m_pickPosOffset.Dot(worldTM.GetUpVec3());
                        m_translateMode = eCTM_Y;
                        bRet = true;
                    }
                }
            }
        }
    }
    else
    {
        CVec3 pos = m_pNode->GetWorldPosition();
        if (fabs(x - pos.X()) < m_fPanelSize && fabs(y - pos.Y()) < m_fPanelSize)
        {
            m_translateMode = eCTM_XY;
            m_pickPosOffset = CVec3(x - pos.X(), y - pos.Y(), 0);
            bRet = true;
        }
        else if (fabs(pos.X() - x) < fAxisSelectDistance &&
            fabs(pos.Y() - y) < m_fAxisLength)
        {
            m_translateMode = eCTM_Y;
            m_pickPosOffset = CVec3(x - pos.X(), y - pos.Y(), 0);
            bRet = true;
        }
        else if (fabs(pos.Y() - y) < fAxisSelectDistance &&
            fabs(pos.X() - x) < m_fAxisLength)
        {
            m_translateMode = eCTM_X;
            m_pickPosOffset = CVec3(x - pos.X(), y - pos.Y(), 0);
            bRet = true;
        }
    }
    return bRet;
}

bool CCoordinateRenderObject::GetOperateFlag() const
{
    return m_bCanOperate;
}

void CCoordinateRenderObject::SetOperateFlag(bool bFlag)
{
    if (m_bVisible)
    {
        m_bCanOperate = bFlag;
    }
}

ECoordinateTranslateMode CCoordinateRenderObject::GetTranslateMode() const
{
    return m_translateMode;
}

void CCoordinateRenderObject::TranslateObject(float x, float y)
{
    if (m_pNode != nullptr && m_bCanOperate)
    {
        if (m_pNode->GetType() == eNT_Node3D)
        {
            CRay3 detectRay = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->GetRayFromScreenPos(x, y);
            CPlane plane;
            const CMat4& worldTM = m_pNode->GetWorldTM();
            switch (m_translateMode)
            {
            case eCTM_Y:
            case eCTM_XY:
                plane.FromPointNormal(m_originPos, worldTM.GetForwardVec3());
                break;
            case eCTM_X:
            case eCTM_Z:
            case eCTM_XZ:
                plane.FromPointNormal(m_originPos, worldTM.GetUpVec3());
                break;
            case eCTM_YZ:
                plane.FromPointNormal(m_originPos, worldTM.GetRightVec3());
                break;
            default:
                BEATS_ASSERT(false, "Never reach here!");
                break;
            }
            float fIntersectDistance = 0;
            if (detectRay.IntersectPlane(plane, fIntersectDistance))
            {
                CVec3 intersectPos = detectRay.GetDirection() * fIntersectDistance + detectRay.GetStartPos();
                intersectPos -= m_pickPosOffset;
                switch (m_translateMode)
                {
                case eCTM_X:
                    intersectPos -= m_originPos;
                    intersectPos = worldTM.GetRightVec3() * intersectPos.Dot(worldTM.GetRightVec3()) + worldTM.GetTranslate();
                    break;
                case eCTM_Y:
                    intersectPos -= m_originPos;
                    intersectPos = worldTM.GetUpVec3() * intersectPos.Dot(worldTM.GetUpVec3()) + worldTM.GetTranslate();
                    break;
                case eCTM_Z:
                    intersectPos -= m_originPos;
                    intersectPos = worldTM.GetForwardVec3() * intersectPos.Dot(worldTM.GetForwardVec3()) + worldTM.GetTranslate();
                    break;
                default:
                    break;
                }
                if (m_pNode->GetParentNode() != nullptr)
                {
                    CMat4 parentMatInverse = m_pNode->GetParentNode()->GetWorldTM();
                    parentMatInverse.Inverse();
                    intersectPos *= parentMatInverse;
                }
                m_pNode->SetPosition(intersectPos);
            }
        }
    }
}

void CCoordinateRenderObject::DoRender()
{
    if (m_pNode != nullptr && m_bVisible)
    {
        bool b3DOr2D = m_pNode->GetType() == eNT_Node3D;
        switch (m_renderType)
        {
        case ECoordinateRenderType::eCRT_Local:
        {
            CMat4 mat = m_pNode->GetLocalTM();
            mat.SetTranslate(m_pNode->GetWorldTM().GetTranslate());
            RenderMatrix(mat, b3DOr2D);
        }
            break;
        case ECoordinateRenderType::eCRT_Parent:
            RenderMatrix(m_pNode->GetWorldTM(), b3DOr2D);
            break;
        case ECoordinateRenderType::eCRT_World:
        {
            CMat4 identityMat;
            identityMat.SetTranslate(m_pNode->GetWorldTM().GetTranslate());
            RenderMatrix(identityMat, b3DOr2D);
        }
            break;
        default:
            break;
        }
    }
}

CColor CCoordinateRenderObject::GetAxisColorX() const
{
    return m_axisColorX;
}

CColor CCoordinateRenderObject::GetAxisColorY() const
{
    return m_axisColorY;
}

CColor CCoordinateRenderObject::GetAxisColorZ() const
{
    return m_axisColorZ;
}

CColor CCoordinateRenderObject::GetAxisSelectColor() const
{
    return m_axisSelectColor;
}

CColor CCoordinateRenderObject::GetPanelSelectColor() const
{
    return m_panelSelectColor;
}

float CCoordinateRenderObject::GetAxisLengthFactor() const
{
    return m_fAxisLengthFactor;
}

float CCoordinateRenderObject::GetConeHeightFactor() const
{
    return m_fConeHeightFactor;
}

float CCoordinateRenderObject::GetConeBottomColorFactor() const
{
    return m_fConeBottomColorFactor;
}

float CCoordinateRenderObject::GetConeAngle() const
{
    return m_fConeAngle;
}

float CCoordinateRenderObject::GetTranslatePanelStartPosRate() const
{
    return m_fTranslatePanelStartPosRate;
}

void CCoordinateRenderObject::SetAxisColorX(CColor color)
{
    m_axisColorX = color;
}

void CCoordinateRenderObject::SetAxisColorY(CColor color)
{
    m_axisColorY = color;
}

void CCoordinateRenderObject::SetAxisColorZ(CColor color)
{
    m_axisColorZ = color;
}

void CCoordinateRenderObject::SetAxisSelectColor(CColor color)
{
    m_axisSelectColor = color;
}

void CCoordinateRenderObject::SetPanelSelectColor(CColor color)
{
    m_panelSelectColor = color;
}

void CCoordinateRenderObject::SetAxisLengthFactor(float factor)
{
    m_fAxisLengthFactor = factor;
}

void CCoordinateRenderObject::SetConeHeightFactor(float factor)
{
    m_fConeHeightFactor = factor;
}

void CCoordinateRenderObject::SetConeBottomColorFactor(float factor)
{
    m_fConeBottomColorFactor = factor;
}

void CCoordinateRenderObject::SetConeAngle(float angle)
{
    m_fConeAngle = angle;
}

void CCoordinateRenderObject::SetTranslatePanelStartPosRate(float rate)
{
    m_fTranslatePanelStartPosRate = rate;
}

void CCoordinateRenderObject::SetRenderType(ECoordinateRenderType type)
{
    m_renderType = type;
}