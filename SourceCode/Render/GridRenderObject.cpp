#include "stdafx.h"
#include "GridRenderObject.h"
#include "RenderBatch.h"
#include "CommonTypes.h"
#include "RenderGroupManager.h"
#include "RenderGroup.h"
#include "Material.h"
#include "RenderState.h"

CGridRenderObject::CGridRenderObject()
    : m_pGridBatch(nullptr)
    , m_nStartGridPosX(0)
    , m_nStartGridPosZ(0)
    , m_uGridWidth(0)
    , m_uGridHeight(0)
    , m_fGridDistance(1.0f)
    , m_fLineWidth(1.0f)
    , m_fPositiveLineLength(0)
    , m_lineColor(0xFFFFFFFF)
    , m_axisXColor(0xFF0000FF)
    , m_axisZColor(0x0000FFFF)
{
    InitGridData();
}

CGridRenderObject::~CGridRenderObject()
{
    BEATS_SAFE_DELETE(m_pGridBatch);
}

void CGridRenderObject::DoRender()
{
    if(m_pGridBatch->GetVertexCount() > 0)
    {
        CRenderGroup *pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_3D);
        pRenderGroup->AddRenderBatch(m_pGridBatch);
    }
}

void CGridRenderObject::GetGridStartPos(int& x, int& y) const
{
    x = m_nStartGridPosX;
    y = m_nStartGridPosZ;
}

size_t CGridRenderObject::GetGridWidth() const
{
    return m_uGridWidth;
}

size_t CGridRenderObject::GetGridHeight() const
{
    return m_uGridHeight;
}

float CGridRenderObject::GetGridDistance() const
{
    return m_fGridDistance;
}

float CGridRenderObject::GetLineWidth() const
{
    return m_fLineWidth;
}

void CGridRenderObject::SetGridStartPos(int x, int y)
{
    if (m_nStartGridPosX != x ||
        m_nStartGridPosZ != y)
    {
        m_nStartGridPosX = x;
        m_nStartGridPosZ = y;
        if (m_pGridBatch != NULL)
        {
            m_pGridBatch->Clear();
            CreateVertexData();
        }
    }
}

void CGridRenderObject::SetGridWidth(size_t uWidth)
{
    if (m_uGridWidth != uWidth)
    {
        m_uGridWidth = uWidth;
        if (m_pGridBatch != NULL)
        {
            m_pGridBatch->Clear();
            CreateVertexData();
        }
    }
}

void CGridRenderObject::SetGridHeight(size_t uHeight)
{
    if (m_uGridHeight != uHeight)
    {
        m_uGridHeight = uHeight;
        if (m_pGridBatch != NULL)
        {
            m_pGridBatch->Clear();
            CreateVertexData();
        }
    }
}

void CGridRenderObject::SetGridDistance(float fDis)
{
    m_fGridDistance = fDis;
    if (m_pGridBatch != NULL)
    {
        m_pGridBatch->Clear();
        CreateVertexData();
    }
}

void CGridRenderObject::SetLineWidth(float fLineWidth)
{
    BEATS_ASSERT(fLineWidth > 0, _T("Line width must be greater than 0"));
    m_fLineWidth = fLineWidth;
    if (m_pGridBatch != NULL)
    {
        m_pMaterial->GetRenderState()->SetLineWidth(m_fLineWidth);
    }
}

void CGridRenderObject::SetPositiveLineLength(float fLineLength)
{
    m_fPositiveLineLength = fLineLength;
    if (m_pGridBatch != NULL)
    {
        m_pGridBatch->Clear();
        CreateVertexData();
    }
}

void CGridRenderObject::SetLineColor(const CColor& color)
{
    m_lineColor = color;
    if (m_pGridBatch != NULL)
    {
        m_pGridBatch->Clear();
        CreateVertexData();
    }
}

void CGridRenderObject::SetAxisXColor(const CColor& color)
{
    m_axisXColor = color;
    if (m_pGridBatch != NULL)
    {
        m_pGridBatch->Clear();
        CreateVertexData();
    }
}

void CGridRenderObject::SetAxisZColor(const CColor& color)
{
    m_axisZColor = color;
    if (m_pGridBatch != NULL)
    {
        m_pGridBatch->Clear();
        CreateVertexData();
    }
}

void CGridRenderObject::InitGridData()
{
    m_pMaterial = new CMaterial();
    m_pMaterial->SetSharders( _T("PointColorShader.vs"), _T("PointColorShader.ps"));
    m_pMaterial->SetDepthTest(true);
    m_pMaterial->SetBlendEnable(true);
    m_pMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pMaterial->SetBlendEquation(GL_FUNC_ADD);
    m_pMaterial->GetRenderState()->SetLineWidth(m_fLineWidth);
    m_pMaterial->GetRenderState()->SetPointSize(5.f);
    m_pMaterial->SetTexture(0, nullptr );

    m_pGridBatch = new CRenderBatch(
        VERTEX_FORMAT(CVertexPC), m_pMaterial, GL_LINES, false);
    m_pGridBatch->SetStatic(true);
    CreateVertexData();
}

void CGridRenderObject::CreateVertexData()
{
    CVertexPC vertex;
    BEATS_ASSERT(m_fGridDistance > 0, _T("GridCount can't be less than 1"));
    // draw grid at x-z pane
    if (m_uGridHeight > 0 && m_uGridWidth > 0)
    {
        for (int i = 0; i <= (int)m_uGridWidth; ++i)
        {
            kmVec3Fill(&vertex.position, (m_nStartGridPosX + i) * m_fGridDistance, 0, m_nStartGridPosZ * m_fGridDistance);
            vertex.color = (m_nStartGridPosX + i) == 0 ? m_axisZColor : m_lineColor; // Axis Z
            m_pGridBatch->AddVertices(&vertex, 1);
            kmVec3Fill(&vertex.position, (m_nStartGridPosX + i) * m_fGridDistance, 0, (m_nStartGridPosZ + (int)m_uGridHeight) * m_fGridDistance);
            m_pGridBatch->AddVertices(&vertex, 1);
        }
        for (int j = 0; j <= (int)m_uGridHeight; ++j)
        {
            kmVec3Fill(&vertex.position, m_nStartGridPosX * m_fGridDistance, 0, (m_nStartGridPosZ + j) * m_fGridDistance);
            vertex.color = (m_nStartGridPosZ + j) == 0 ? m_axisXColor : m_lineColor; // Axis X
            m_pGridBatch->AddVertices(&vertex, 1);
            kmVec3Fill(&vertex.position, (m_nStartGridPosX + (int)m_uGridWidth) * m_fGridDistance, 0, (m_nStartGridPosZ + j) * m_fGridDistance);
            m_pGridBatch->AddVertices(&vertex, 1);
        }
    }

    if (!BEATS_FLOAT_EQUAL(m_fPositiveLineLength, 0))
    {
        CVertexPC originVertex;
        originVertex.color = 0xFFFF00FF;
        kmVec3Zero(&originVertex.position);
        m_pGridBatch->AddVertices(&originVertex, 1);

        CVertexPC axisPositive;
        axisPositive.color = 0xFFFF00FF;
        float fLineLength = MIN(m_fPositiveLineLength, m_fGridDistance * m_uGridWidth * 0.5f);
        kmVec3Fill(&axisPositive.position, fLineLength, fLineLength, fLineLength);
        m_pGridBatch->AddVertices(&axisPositive, 1);
    }
}
