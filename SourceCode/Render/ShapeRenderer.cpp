#include "stdafx.h"
#include "ShapeRenderer.h"
#include "RenderManager.h"
#include "RenderBatch.h"
#include "RenderGroup.h"
#include "Material.h"
#include "RandomValue.h"
#include "MathExt/AABBBox.h"

CShapeRenderer* CShapeRenderer::m_pInstance = NULL;

CShapeRenderer::CShapeRenderer()
{

}

CShapeRenderer::~CShapeRenderer()
{

}

void CShapeRenderer::DrawCircle(const CMat4& mat, float fRadius, CColor color, bool bSolid, float fRate) const
{
    static const float fStepRadians = DegreesToRadians(15);
    if (bSolid)
    {
        std::vector<CVertexPC> vertexData;
        std::vector<unsigned short> indicesData;
        CVertexPC point;
        point.position = CVec3(mat[12], mat[13], mat[14]);
        point.color = color;
        vertexData.push_back(point);
        for (float fRadian = 0; fRadian <= MATH_PI_DOUBLE * fRate; fRadian += fStepRadians)
        {
            CVec3 pos(fRadius * sinf(fRadian), 0, fRadius * cosf(fRadian));
            pos *= mat;
            point.position = pos;
            vertexData.push_back(point);
            if (vertexData.size() >= 3)
            {
                unsigned short index = (unsigned short)vertexData.size();
                indicesData.push_back(0);
                indicesData.push_back(index - 2);
                indicesData.push_back(index - 1);
            }
        }
        CRenderManager::GetInstance()->RenderTriangle(vertexData, indicesData, true);
    }
    else
    {
        CVertexPC startPos;
        CVertexPC endPos;
        startPos.color = color;
        endPos.color = color;
        startPos.position = CVec3(0, 0, fRadius);
        startPos.position *= mat;
        float fEndRadian = MATH_PI_DOUBLE * fRate;
        for (float fRadian = fStepRadians; fRadian <= fEndRadian; fRadian += fStepRadians)
        {
            endPos.position = CVec3(fRadius * sinf(fRadian), 0, fRadius * cosf(fRadian));
            endPos.position *= mat;
            CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
            startPos.position = endPos.position;
        }
        //Close the circle to avoid the error condition.
        endPos.position = CVec3(fRadius * sinf(fEndRadian), 0, fRadius * cosf(fEndRadian));
        endPos.position *= mat;
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    }
}

void CShapeRenderer::DrawCone(const CMat4& mat, float fButtomRadius, float fTopRadius, float fHeight, CColor coneColor, CColor bottomColor, CColor topColor, bool bSolid) const
{
    DrawCircle(mat, fButtomRadius, bottomColor, bSolid);
    CMat4 translateMat;
    translateMat.SetTranslate(CVec3(0, fHeight, 0));
    CMat4 topMat = mat * translateMat;
    DrawCircle(topMat, fTopRadius, topColor, bSolid);
    if (bSolid)
    {
        CVec3 center(mat[12], mat[13], mat[14]);
        CVec3 upDirection = mat.GetUpVec3();
        upDirection = upDirection * fHeight;
        CVec3 topCenter = center + upDirection;
        CVertexPC point;
        point.position = topCenter;
        point.color = coneColor;
        std::vector<unsigned short> indicesData;
        std::vector<CVertexPC> vertexData;
        vertexData.push_back(point);
        static const float fStepRadians = DegreesToRadians(15);
        for (float fRadian = 0; fRadian <= MATH_PI_DOUBLE; fRadian += fStepRadians)
        {
            CVec3 pos(fButtomRadius * sinf(fRadian), 0, fButtomRadius * cosf(fRadian));
            pos *= mat;
            point.position = pos;
            vertexData.push_back(point);
            if (vertexData.size() >= 3)
            {
                unsigned short index = (unsigned short)vertexData.size();
                indicesData.push_back(0);
                indicesData.push_back(index - 2);
                indicesData.push_back(index - 1);
            }
        }
        CRenderManager::GetInstance()->RenderTriangle(vertexData, indicesData, true);
    }
    else
    {
        CVertexPC startPos, endPos;
        startPos.color = coneColor;
        endPos.color = coneColor;
        CVec3 topPoint(fTopRadius, fHeight, 0);
        CVec3 buttomPoint(fButtomRadius, 0, 0);
        startPos.position = topPoint * mat;
        endPos.position = buttomPoint * mat;
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
        topPoint.X() *= -1;
        buttomPoint.X() *= -1;
        startPos.position = topPoint * mat;
        endPos.position = buttomPoint * mat;
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
        topPoint = CVec3(0, fHeight, fTopRadius);
        buttomPoint = CVec3(0, 0, fButtomRadius);
        startPos.position = topPoint * mat;
        endPos.position = buttomPoint * mat;
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
        topPoint.Z() *= -1;
        buttomPoint.Z() *= -1;
        startPos.position = topPoint * mat;
        endPos.position = buttomPoint * mat;
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    }
}

void CShapeRenderer::DrawSphere(const CVec3& pos, float fRadius, float fElliptical, CColor color, uint32_t uGridCnt, bool bWireFrame) const
{
    if (BEATS_FLOAT_GREATER(fRadius, 0.0F))
    {
        const uint32_t uVertexCnt = uGridCnt + 1;

        std::vector<CVertexPC> vertices;
        std::vector<unsigned short> indices;
        // Generate vertex.
        for (uint32_t y = 0; y < uVertexCnt; y++)
        {
            for (uint32_t x = 0; x < uVertexCnt; x++)
            {
                CVertexPC vertex;
                vertex.color = color;
                vertex.position.X() = sinf(DegreesToRadians((float)y / (float)(uVertexCnt - 1) * 180.0F)) *
                    cosf(DegreesToRadians((float)x / (float)(uVertexCnt - 1) * 360.0F)) *
                    fRadius;
                vertex.position.Y() =
                    cosf(DegreesToRadians((float)y / (float)(uVertexCnt - 1) * 180.0F)) *
                    fRadius *
                    fElliptical;

                vertex.position.Z() =
                    sinf(DegreesToRadians((float)y / (float)(uVertexCnt - 1) * 180.0F)) *
                    sinf(DegreesToRadians((float)x / (float)(uVertexCnt - 1) * 360.0F)) *
                    fRadius;
                vertex.position += pos;
                vertices.push_back(vertex);
            }
        }

        // Generate index.
        for (uint32_t z = 0; z < uGridCnt; z++)
        {
            for (uint32_t x = 0; x < uGridCnt; x++)
            {
                indices.push_back((unsigned short)(z * uVertexCnt + x));
                indices.push_back((unsigned short)((z + 1) * uVertexCnt + x + 1));
                indices.push_back((unsigned short)((z + 1) * uVertexCnt + x));

                indices.push_back((unsigned short)(z * uVertexCnt + x));
                indices.push_back((unsigned short)(z * uVertexCnt + x + 1));
                indices.push_back((unsigned short)((z + 1) * uVertexCnt + x + 1));
            }
        }
        if (bWireFrame)
        {
            CRenderManager::GetInstance()->RenderLines(vertices, indices, 1.0f, true);
        }
        else
        {
            CRenderManager::GetInstance()->RenderTriangle(vertices, indices, true);
        }
    }
}

void CShapeRenderer::DrawBox(const CMat4& mat, float fX, float fY, float fZ, CColor color, bool bSolid)
{
    if (!bSolid)
    {
        CVertexPC top[4];
        CVertexPC buttom[4];
        float fHalfX = fX * 0.5f;
        float fHalfZ = fZ * 0.5f;
        top[0].position = CVec3(-fHalfX, fY, -fHalfZ) * mat;
        top[0].color = color;
        top[1].position = CVec3(fHalfX, fY, -fHalfZ) * mat;
        top[1].color = color;
        top[2].position = CVec3(fHalfX, fY, fHalfZ) * mat;
        top[2].color = color;
        top[3].position = CVec3(-fHalfX, fY, fHalfZ) * mat;
        top[3].color = color;
        CRenderManager::GetInstance()->RenderLine(top[0], top[1], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(top[1], top[2], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(top[2], top[3], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(top[3], top[0], 1.0f, true);
        buttom[0].position = CVec3(-fHalfX, 0, -fHalfZ) * mat;
        buttom[0].color = color;
        buttom[1].position = CVec3(fHalfX, 0, -fHalfZ) * mat;
        buttom[1].color = color;
        buttom[2].position = CVec3(fHalfX, 0, fHalfZ) * mat;
        buttom[2].color = color;
        buttom[3].position = CVec3(-fHalfX, 0, fHalfZ) * mat;
        buttom[3].color = color;
        CRenderManager::GetInstance()->RenderLine(buttom[0], buttom[1], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(buttom[1], buttom[2], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(buttom[2], buttom[3], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(buttom[3], buttom[0], 1.0f, true);

        CRenderManager::GetInstance()->RenderLine(top[0], buttom[0], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(top[1], buttom[1], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(top[2], buttom[2], 1.0f, true);
        CRenderManager::GetInstance()->RenderLine(top[3], buttom[3], 1.0f, true);
    }
}

void CShapeRenderer::DrawCurve(const SCurveData& curveData, CColor color)
{
    CVertexPC startVertex, endVertex;
    startVertex.color = color;
    endVertex.color = color;
    static const uint32_t uDrawStep = 25;
    for (auto iter = curveData.m_keyList.begin(); iter != curveData.m_keyList.end();++iter)
    {
        float fStartTime = iter->first;
        if (iter != curveData.m_keyList.end())
        {
            float fEndTime = iter->first;
            for (uint32_t i = 0; i < uDrawStep; ++i)
            {
                float fCurTime = fStartTime + (fEndTime - fStartTime) * ((float)i / uDrawStep);
                startVertex.position.Fill(fCurTime, 0, curveData.Evaluate(fCurTime));
                float fNexTime = fStartTime + (fEndTime - fStartTime) * ((float)(i+1) / uDrawStep);
                endVertex.position.Fill(fNexTime, 0, curveData.Evaluate(fNexTime));
                CRenderManager::GetInstance()->RenderLine(startVertex, endVertex);
            }
        }
    }
}

void CShapeRenderer::DrawAABB(const CAABBBox& aabb, CColor color)
{
    CVec3 tmin = aabb.m_minPos;
    CVec3 tmax = aabb.m_maxPos;
    CVec3 sub = tmax - tmin;
    CVertexPC startPos, endPos;
    startPos.color = color;
    endPos.color = color;
    startPos.position = tmin;
    endPos.position = CVec3(tmin.X(), tmin.Y(), tmin.Z() + sub.Z() * 0.2F);// Min
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X(), tmin.Y() + sub.Y() * 0.2F, tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X() + sub.X() * 0.2F, tmin.Y(), tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);

    startPos.position = tmax;
    endPos.position = CVec3(tmax.X(), tmax.Y(), tmax.Z() - sub.Z() * 0.2F);// Max
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X(), tmax.Y() - sub.Y() * 0.2F, tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X() - sub.X() * 0.2F, tmax.Y(), tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);

    startPos.position = CVec3(tmin.X(), tmin.Y(), tmax.Z());// x min, y min, z max
    endPos.position = CVec3(tmin.X(), tmin.Y(), tmax.Z() - sub.Z() * 0.2F);
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X(), tmin.Y() + sub.Y() * 0.2F, tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X() + sub.X() * 0.2F, tmin.Y(), tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);

    startPos.position = CVec3(tmin.X(), tmax.Y(), tmin.Z());// x min, y max, z min
    endPos.position = CVec3(tmin.X(), tmax.Y(), tmin.Z() + sub.Z() * 0.2F);
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X(), tmax.Y() - sub.Y() * 0.2F, tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X() + sub.X() * 0.2F, tmax.Y(), tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);

    startPos.position = CVec3(tmax.X(), tmin.Y(), tmin.Z());// x max, y min, z min
    endPos.position = CVec3(tmax.X(), tmin.Y(), tmin.Z() + sub.Z() * 0.2F);
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X(), tmin.Y() + sub.Y() * 0.2F, tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X() - sub.X() * 0.2F, tmin.Y(), tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);

    startPos.position = CVec3(tmin.X(), tmax.Y(), tmax.Z());// x min, y max, z max
    endPos.position = CVec3(tmin.X(), tmax.Y(), tmax.Z() - sub.Z() * 0.2F);
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X(), tmax.Y() - sub.Y() * 0.2F, tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmin.X() + sub.X() * 0.2F, tmax.Y(), tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);

    startPos.position = CVec3(tmax.X(), tmax.Y(), tmin.Z());// x min, y max, z max
    endPos.position = CVec3(tmax.X(), tmax.Y(), tmin.Z() + sub.Z() * 0.2F);
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X(), tmax.Y() - sub.Y() * 0.2F, tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X() - sub.X() * 0.2F, tmax.Y(), tmin.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);

    startPos.position = CVec3(tmax.X(), tmin.Y(), tmax.Z());// x max, y min, z max
    endPos.position = CVec3(tmax.X(), tmin.Y(), tmax.Z() - sub.Z() * 0.2F);
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X(), tmin.Y() + sub.Y() * 0.2F, tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
    endPos.position = CVec3(tmax.X() - sub.X() * 0.2F, tmin.Y(), tmax.Z());
    CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f, true);
}
