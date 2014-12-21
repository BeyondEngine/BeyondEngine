#include "stdafx.h"
#include "CircleProgressSprite.h"
#include "ShaderUniform.h"
#include "Material.h"
#include "RenderBatch.h"
#include "RenderGroup.h"
#include "Texture.h"

CCircleProgressSprite::CCircleProgressSprite()
{

}

CCircleProgressSprite::~CCircleProgressSprite()
{
}

void CCircleProgressSprite::DoRender()
{
    if (IsVisible())
    {
        SharePtr<CTextureFrag> pFrag = GetTextureFrag();
        if (pFrag)
        {
            CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup();
            BEATS_ASSERT(renderGroup);
            std::map<unsigned char, SharePtr<CTexture> > textureMap;
            textureMap[0] = pFrag->GetTexture();
            if (m_fTotleCount != 0)
            {
                CShaderUniform* pInfoUniform = m_pCircleProgressSpriteMaterial->GetUniform("info");
                BEATS_ASSERT(pInfoUniform != nullptr);
                pInfoUniform->GetData()[0] = m_fCurrentCount / m_fTotleCount;
            }
            CRenderBatch* batch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPT), m_pCircleProgressSpriteMaterial, GL_TRIANGLES, true, true, &textureMap);
            batch->AddQuad(&m_quad, &GetQuadT(), &GetWorldTM());
        }
    }
}

void CCircleProgressSprite::SetTotleCount(float count)
{
    if (m_fTotleCount != count && count != 0)
    {
        m_fTotleCount = count;
    }
}

void CCircleProgressSprite::SetCurrentCount(float count)
{
    m_fCurrentCount = count > m_fTotleCount ? m_fTotleCount : count;
}

void CCircleProgressSprite::Initialize()
{
    super::Initialize();
    if (nullptr == m_pCircleProgressSpriteMaterial)
    {
        m_pCircleProgressSpriteMaterial = new CMaterial();
        m_pCircleProgressSpriteMaterial->SetDepthTestEnable(false);
        m_pCircleProgressSpriteMaterial->SetSharders(_T("circelprogressspriteshader.vs"), _T("circelprogressspriteshader.ps"));
        CShaderUniform* pInfoUniform = new CShaderUniform(_T("info"), eSUT_4f);
        pInfoUniform->GetData()[1] = 0.6f;
        const CQuadT& quadt = GetQuadT();
        CVec2 centerUV = CVec2((quadt.tl.u + quadt.br.u) * 0.5f, (quadt.tl.v + quadt.br.v) * 0.5f);
        pInfoUniform->GetData()[2] = centerUV.X();
        pInfoUniform->GetData()[3] = centerUV.Y();
        m_pCircleProgressSpriteMaterial->AddUniform(pInfoUniform);
        m_pCircleProgressSpriteMaterial->Initialize();
    }
}
