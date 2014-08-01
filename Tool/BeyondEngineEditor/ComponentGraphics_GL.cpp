#include "stdafx.h"
#include "ComponentGraphics_GL.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescriptionLine.h"
#include "utility/BeatsUtility/Serializer.h"
#include "Render/Texture.h"
#include "Render/TextureFrag.h"
#include "Render/Material.h"
#include "render/RenderManager.h"
#include "render/Renderer.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"
#include "GUI/Font/FreetypeFontFace.h"
#include "Resource/ResourceManager.h"
#include "Render/Shader.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderGroup.h"
#include "Render/RenderBatch.h"
#include "Utility/PerformDetector/PerformDetector.h"
#include "Render/TextureAtlas.h"
#include "Utf8String.h"
#include <shlobj.h>

static const float Font_Render_X_Offset = 8.0f;
static const float Font_Render_Y_Offset = 0.5f;
static const float DefaultCellSize = 15.0f;
CFontFace* CComponentGraphic_GL::m_pFont = NULL;
SharePtr<CMaterial> CComponentGraphic_GL::m_pMaterial;
SharePtr<CTextureAtlas> CComponentGraphic_GL::m_pAtlas;
CTextureFrag* CComponentGraphic_GL::m_pFrags[eCT_Count] = {0};
CRenderBatch* CComponentGraphic_GL::m_pRenderBatch = NULL;

enum ERenderLayer
{
    eRL_BackGround,
    eRL_Component,
    eRL_Dependency,
    eRL_SelectRect,

    eRL_Count,
    eRL_Force32Bit = 0xFFFFFFFF
};

CComponentGraphic_GL::CComponentGraphic_GL()
{
    // Base class is dx implemented.
    m_gridPosZ *= -1;
    ++m_gridPosZ;
}

CComponentGraphic_GL::~CComponentGraphic_GL()
{

}

void CComponentGraphic_GL::CreateMaterials()
{
    if (m_pFont == NULL)
    {
        m_pFont = CFontManager::GetInstance()->GetFace(_T("ComponentGraphicFont"));
        if (m_pFont == NULL)
        {
            char szBuf[MAX_PATH] = {0};
            SHGetSpecialFolderPath(NULL , szBuf , CSIDL_FONTS , FALSE);
            TString strFontPath = szBuf;
            BEATS_ASSERT(!strFontPath.empty(), _T("Get font directory failed!"));
            strFontPath.append(_T("\\msyh.ttf"));

            m_pFont = CFontManager::GetInstance()->CreateFreetypeFontFace(_T("ComponentGraphicFont"), strFontPath, 12, 0, false);
            BEATS_ASSERT(m_pFont != NULL, _T("You need to install msyh.ttf in your OS!"));
            down_cast<CFreetypeFontFace *>(m_pFont)->SetRelatedRenderTarget(CRenderManager::GetInstance()->GetCurrentRenderTarget());
        }
    }
    BEATS_ASSERT(m_pFont != NULL);

    if (m_pFrags[eCT_RectBG] == NULL)
    {
        m_pAtlas = CResourceManager::GetInstance()->GetResource<CTextureAtlas>(_T("Component.xml"));
        m_pMaterial = CreateMaterial(_T("Component.png"));
        m_pMaterial->Initialize();
        m_pFrags[eCT_RectBG] = m_pAtlas->GetTextureFrag(_T("RectBG"));
        m_pFrags[eCT_RefRectBG] = m_pAtlas->GetTextureFrag(_T("RefRectBG"));
        m_pFrags[eCT_ConnectRect] = m_pAtlas->GetTextureFrag(_T("ConnectRect"));
        m_pFrags[eCT_SelectedRectBG] = m_pAtlas->GetTextureFrag(_T("SelectedRect"));
        m_pFrags[eCT_NormalLine] = m_pAtlas->GetTextureFrag(_T("NormalLine"));
        m_pFrags[eCT_SelectedLine] = m_pAtlas->GetTextureFrag(_T("SelectedLine"));
        m_pFrags[eCT_NormalArrow] = m_pAtlas->GetTextureFrag(_T("NormalArrow"));
        m_pFrags[eCT_SelectedArrow] = m_pAtlas->GetTextureFrag(_T("SelectedArrow"));
        m_pFrags[eCT_ConnectedDependency] = m_pAtlas->GetTextureFrag(_T("ConnectedDependency"));
        m_pFrags[eCT_ConnectedDependencyList] = m_pAtlas->GetTextureFrag(_T("ConnectedDependencyList"));
        m_pFrags[eCT_WeakDependency] = m_pAtlas->GetTextureFrag(_T("WeakDependency"));
        m_pFrags[eCT_WeakDependencyList] = m_pAtlas->GetTextureFrag(_T("WeakDependencyList"));
        m_pFrags[eCT_StrongDependency] = m_pAtlas->GetTextureFrag(_T("StrongDependency"));
        m_pFrags[eCT_StrongDependencyList] = m_pAtlas->GetTextureFrag(_T("StrongDependencyList"));
        CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_2D);
        BEATS_ASSERT(renderGroup);
        m_pRenderBatch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPT), m_pMaterial, GL_TRIANGLES, true);
    }
}

SharePtr<CMaterial> CComponentGraphic_GL::CreateMaterial(const TString &textureFileName)
{
    SharePtr<CMaterial> material = new CMaterial();
    material->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_DepthTest, true);
    material->GetRenderState()->SetBoolState((CBoolRenderStateParam::EBoolStateParam)GL_ALPHA_TEST, true);
    material->GetRenderState()->SetAlphaFunc(GL_GREATER);
    material->GetRenderState()->SetAlphaRef(0);
    material->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_Blend, true);
    material->GetRenderState()->SetBlendFuncSrcFactor(GL_SRC_ALPHA);
    material->GetRenderState()->SetBlendFuncTargetFactor(GL_ONE_MINUS_SRC_ALPHA);
    material->SetSharders( _T("PointTexShader.vs"), _T("PointTexShader.ps"));
    SharePtr<CTexture> texture = CResourceManager::GetInstance()->GetResource<CTexture>(textureFileName);
    BEATS_ASSERT(texture, _T("Create Component Texture Failed!"));
    material->SetTexture(0, texture);
    return material;
}

void CComponentGraphic_GL::GetDependencyPosition(size_t uDependencyIndex, int* pOutX, int* pOutY)
{
    BEATS_ASSERT(m_pOwner->GetDependency(uDependencyIndex) != NULL);
    *pOutX = m_gridPosX + MIN_WIDTH;
    *pOutY = m_gridPosY + HEADER_HEIGHT + (int)(uDependencyIndex + 1) * DEPENDENCY_HEIGHT;
}

CComponentGraphic* CComponentGraphic_GL::Clone()
{
    return new CComponentGraphic_GL();
}

CFontFace* CComponentGraphic_GL::GetFontFace()
{
    return m_pFont;
}

void CComponentGraphic_GL::DrawHead(float cellSize)
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_1)
    CreateMaterials();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_1)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_2)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_2_2)

    EComponentTexture BGTexture = m_bIsReference ? eCT_RefRectBG : eCT_RectBG;
    CQuadPT head;
    head.tl.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;// Left top
    head.tl.position.y = m_gridPosY * cellSize;
    head.tl.position.z = (float)m_gridPosZ;
    head.tl.tex = m_pFrags[BGTexture]->Quad().tl;

    head.bl.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;// Left down
    head.bl.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    head.bl.position.z = (float)m_gridPosZ;
    head.bl.tex = m_pFrags[BGTexture]->Quad().bl;

    head.tr.position.x = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right top
    head.tr.position.y = m_gridPosY * cellSize;
    head.tr.position.z = (float)m_gridPosZ;
    head.tr.tex = m_pFrags[BGTexture]->Quad().tr;

    head.br.position.x = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right down
    head.br.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    head.br.position.z = (float)m_gridPosZ;
    head.br.tex = m_pFrags[BGTexture]->Quad().br;
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_2_2)
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_2_3)

    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_2_3)
    m_pRenderBatch->AddQuad(&head);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_2)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_3)

    // 2. Draw title text.
    const TString& strDisplayString = m_pOwner->GetUserDefineDisplayName().empty() ? m_pOwner->GetDisplayName() : m_pOwner->GetUserDefineDisplayName();
    m_pFont->RenderText(TStringToUtf8(strDisplayString), head.tl.position.x + Font_Render_X_Offset * cellSize / DefaultCellSize, head.tl.position.y + Font_Render_Y_Offset * cellSize / DefaultCellSize, 1.0f, 0xFFFFFFFF, 0, nullptr, false);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_3)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_4)
    // 3. Draw Connect rect.
    CQuadPT connectRect;

    connectRect.tl.position.x = m_gridPosX * cellSize;    // Left top
    connectRect.tl.position.y = m_gridPosY * cellSize;
    connectRect.tl.position.z = (float)m_gridPosZ;
    connectRect.tl.tex = m_pFrags[eCT_ConnectRect]->Quad().tl;

    connectRect.bl.position.x = m_gridPosX * cellSize;    // Left down
    connectRect.bl.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    connectRect.bl.position.z = (float)m_gridPosZ;
    connectRect.bl.tex = m_pFrags[eCT_ConnectRect]->Quad().bl;

    connectRect.tr.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right top
    connectRect.tr.position.y = m_gridPosY * cellSize;
    connectRect.tr.position.z = (float)m_gridPosZ;    
    connectRect.tr.tex = m_pFrags[eCT_ConnectRect]->Quad().tr;

    connectRect.br.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right down
    connectRect.br.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    connectRect.br.position.z = (float)m_gridPosZ;    
    connectRect.br.tex = m_pFrags[eCT_ConnectRect]->Quad().br;
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_4)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_5)
    m_pRenderBatch->AddQuad(&connectRect);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_5)

    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead)
}

void CComponentGraphic_GL::DrawDependencies( float cellSize )
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_Dependency)
    CreateMaterials();

    if (!m_bIsReference)
    {
        const std::vector<CDependencyDescription*>* pDependencies = m_pOwner->GetDependencies();
        BEATS_ASSERT( pDependencies != NULL);
        size_t uDependencyCount = pDependencies->size();
        for (int i = 0; i < (int)uDependencyCount; ++i)
        {
            // 1. Draw background.
            CQuadPT dependencyPoint;
            dependencyPoint.tl.position.x = m_gridPosX * cellSize;    // Left top
            dependencyPoint.tl.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
            dependencyPoint.tl.position.z = (float)m_gridPosZ;
            dependencyPoint.tl.tex = m_pFrags[eCT_RectBG]->Quad().tl;

            dependencyPoint.bl.position.x = m_gridPosX * cellSize;    // Left down
            dependencyPoint.bl.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
            dependencyPoint.bl.position.z = (float)m_gridPosZ;
            dependencyPoint.bl.tex = m_pFrags[eCT_RectBG]->Quad().bl;

            dependencyPoint.tr.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right top
            dependencyPoint.tr.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize ;
            dependencyPoint.tr.position.z = (float)m_gridPosZ;
            dependencyPoint.tr.tex = m_pFrags[eCT_RectBG]->Quad().tr;

            dependencyPoint.br.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right down
            dependencyPoint.br.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
            dependencyPoint.br.position.z = (float)m_gridPosZ;
            dependencyPoint.br.tex = m_pFrags[eCT_RectBG]->Quad().br;
            m_pRenderBatch->AddQuad(&dependencyPoint);

            // 2. Draw dependency name.
            m_pFont->RenderText(TStringToUtf8(m_pOwner->GetDependency(i)->GetDisplayName()), dependencyPoint.tl.position.x + Font_Render_X_Offset * cellSize / DefaultCellSize, dependencyPoint.tl.position.y + Font_Render_Y_Offset * cellSize / DefaultCellSize, 1.0f, 0xFFFFFFFF, 0, nullptr, false);

            // 3. Draw Connect rect.
            CDependencyDescription* pDescription = m_pOwner->GetDependency(i);
            BEATS_ASSERT(pDescription != NULL);
            bool bConnected = pDescription->GetDependencyLineCount() > 0;
            EComponentTexture textureType = eCT_Count;
            EDependencyType descriptionType = pDescription->GetType();
            bool bIsList = pDescription->IsListType();
            if (bConnected)
            {
                textureType = bIsList ? eCT_ConnectedDependencyList : eCT_ConnectedDependency;
            }
            else
            {
                if (bIsList)
                {
                    textureType = descriptionType == eDT_Strong ? eCT_StrongDependencyList : eCT_WeakDependencyList;
                }
                else
                {
                    textureType = descriptionType == eDT_Strong ? eCT_StrongDependency : eCT_WeakDependency;
                }
            }
            BEATS_ASSERT(textureType != eCT_Count);

            dependencyPoint.tl.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left top
            dependencyPoint.tl.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
            dependencyPoint.tl.position.z = (float)m_gridPosZ;
            dependencyPoint.tl.tex = m_pFrags[textureType]->Quad().tl;

            dependencyPoint.bl.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left down
            dependencyPoint.bl.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
            dependencyPoint.bl.position.z = (float)m_gridPosZ;
            dependencyPoint.bl.tex = m_pFrags[textureType]->Quad().bl;

            dependencyPoint.tr.position.x = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right top
            dependencyPoint.tr.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
            dependencyPoint.tr.position.z = (float)m_gridPosZ;
            dependencyPoint.tr.tex = m_pFrags[textureType]->Quad().tr;

            dependencyPoint.br.position.x = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right down
            dependencyPoint.br.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
            dependencyPoint.br.position.z = (float)m_gridPosZ;
            dependencyPoint.br.tex = m_pFrags[textureType]->Quad().br;

            m_pRenderBatch->AddQuad(&dependencyPoint);

            // 4. Draw the line.
            DrawDependencyLine(cellSize, m_pOwner->GetDependency(i));
        }
    }

    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_Dependency)
}

void CComponentGraphic_GL::DrawDependencyLine( float /*cellSize*/, const CDependencyDescription* pDependency )
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_DependencyLine)

    CreateMaterials();

    BEATS_ASSERT(pDependency != NULL);
    if (pDependency->IsVisible())
    {
        size_t uDependencyLineCount = pDependency->GetDependencyLineCount();
        for (size_t i = 0; i < uDependencyLineCount; ++i)
        {
            CDependencyDescriptionLine* pDependencyLine = pDependency->GetDependencyLine(i);
            EComponentTexture textureLine = pDependencyLine->IsSelected() ? eCT_SelectedLine : eCT_NormalLine;
            EComponentTexture textureArrow = pDependencyLine->IsSelected() ? eCT_SelectedArrow : eCT_NormalArrow;
            const SVertex* pData = pDependencyLine->GetRectArray();
            static const size_t SVERTEX_SIZE = 24;
            CSerializer serializer(SVERTEX_SIZE * 4, (void*)pData);
            DWORD tmpColor = 0;
            CQuadPT dependencyLine;
            serializer >> dependencyLine.br.position.x;
            serializer >> dependencyLine.br.position.y;
            serializer >> dependencyLine.br.position.z;
            dependencyLine.br.position.z = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.br.tex.u;
            serializer >> dependencyLine.br.tex.v;
            dependencyLine.br.tex = m_pFrags[textureLine]->Quad().br;

            serializer >> dependencyLine.bl.position.x;
            serializer >> dependencyLine.bl.position.y;
            serializer >> dependencyLine.bl.position.z;
            dependencyLine.bl.position.z = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.bl.tex.u;
            serializer >> dependencyLine.bl.tex.v;
            dependencyLine.bl.tex = m_pFrags[textureLine]->Quad().bl;

            serializer >> dependencyLine.tr.position.x;
            serializer >> dependencyLine.tr.position.y;
            serializer >> dependencyLine.tr.position.z;
            dependencyLine.tr.position.z = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.tr.tex.u;
            serializer >> dependencyLine.tr.tex.v;
            dependencyLine.tr.tex = m_pFrags[textureLine]->Quad().tr;

            serializer >> dependencyLine.tl.position.x;
            serializer >> dependencyLine.tl.position.y;
            serializer >> dependencyLine.tl.position.z;
            dependencyLine.tl.position.z = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.tl.tex.u;
            serializer >> dependencyLine.tl.tex.v;
            dependencyLine.tl.tex = m_pFrags[textureLine]->Quad().tl;

            m_pRenderBatch->AddQuad(&dependencyLine);

            const SVertex* pArrowData = pDependencyLine->GetArrowRectArray();
            CSerializer serializerArrow(SVERTEX_SIZE * 4, (void*)pArrowData);
            CQuadPT arrow;
            serializerArrow >> arrow.br.position.x;
            serializerArrow >> arrow.br.position.y;
            serializerArrow >> arrow.br.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.br.tex.u;
            serializerArrow >> arrow.br.tex.v;
            arrow.br.tex = m_pFrags[textureArrow]->Quad().br;

            serializerArrow >> arrow.bl.position.x;
            serializerArrow >> arrow.bl.position.y;
            serializerArrow >> arrow.bl.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.bl.tex.u;
            serializerArrow >> arrow.bl.tex.v;
            arrow.bl.tex = m_pFrags[textureArrow]->Quad().bl;

            serializerArrow >> arrow.tr.position.x;
            serializerArrow >> arrow.tr.position.y;
            serializerArrow >> arrow.tr.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.tr.tex.u;
            serializerArrow >> arrow.tr.tex.v;
            arrow.tr.tex = m_pFrags[textureArrow]->Quad().tr;

            serializerArrow >> arrow.tl.position.x;
            serializerArrow >> arrow.tl.position.y;
            serializerArrow >> arrow.tl.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.tl.tex.u;
            serializerArrow >> arrow.tl.tex.v;
            arrow.tl.tex = m_pFrags[textureArrow]->Quad().tl;

            m_pRenderBatch->AddQuad(&arrow);

            //Render index number for dependency list.
            if (pDependency->IsListType())
            {
                kmVec3 deltaDirection;
                kmVec3Subtract(&deltaDirection, &dependencyLine.tr.position, &dependencyLine.tl.position);
                float fXPos = (dependencyLine.tl.position.x + deltaDirection.x * 0.15f);
                static const float fHardCodeOffset = 8;
                float fYPos = (dependencyLine.tl.position.y + deltaDirection.y * 0.15f - fHardCodeOffset);

                TCHAR szIndex[8];
                _stprintf(szIndex, _T("%d"), pDependencyLine->GetIndex());
                m_pFont->RenderText(szIndex, fXPos, fYPos, 1.0f, 0xffff00ff, 0, nullptr, false);
            }
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_DependencyLine)
}

void CComponentGraphic_GL::DrawSelectedRect(float fCellSize)
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_SelectRect)
    CreateMaterials();

    float fWidth = GetWidth() * fCellSize;
    float fHeight = GetHeight() * fCellSize;
    int x, y;
    GetPosition(&x, &y);
    CQuadPT rect;
    // left top
    rect.tl.position.x = x * fCellSize;
    rect.tl.position.y = y * fCellSize;
    rect.tl.position.z = (float)m_gridPosZ + eRL_SelectRect;
    rect.tl.tex = m_pFrags[eCT_SelectedRectBG]->Quad().tl;
    // left down
    rect.bl.position.x = x * fCellSize;
    rect.bl.position.y = y * fCellSize + fHeight;
    rect.bl.position.z = (float)m_gridPosZ + eRL_SelectRect;
    rect.bl.tex = m_pFrags[eCT_SelectedRectBG]->Quad().bl;
    // right top
    rect.tr.position.x = x * fCellSize + fWidth;
    rect.tr.position.y = y * fCellSize;
    rect.tr.position.z = (float)m_gridPosZ + eRL_SelectRect;
    rect.tr.tex = m_pFrags[eCT_SelectedRectBG]->Quad().tr;
    // right down
    rect.br.position.x = x * fCellSize + fWidth;
    rect.br.position.y = y * fCellSize + fHeight;
    rect.br.position.z = (float)m_gridPosZ + eRL_SelectRect;
    rect.br.tex = m_pFrags[eCT_SelectedRectBG]->Quad().br;

    m_pRenderBatch->AddQuad(&rect);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_SelectRect)
}

EComponentAeraRectType CComponentGraphic_GL::HitTestForAreaType( int x, int y, void** pReturnData)
{
    EComponentAeraRectType result = eCART_Invalid;
    if (x >= m_gridPosX && x < m_gridPosX + (int)m_width && y > m_gridPosY && y <= m_gridPosY + (int)m_height)
    {
        result = eCART_Body;
        if (y <= m_gridPosY + HEADER_HEIGHT && x < m_gridPosX + CONNECTION_WIDTH)
        {
            result = eCART_Connection;
        }
        else if (x >= m_gridPosX + MIN_WIDTH && y > m_gridPosY + HEADER_HEIGHT)
        {
            result = eCART_Dependency;
            if (pReturnData != NULL)
            {
                (*pReturnData) = m_pOwner->GetDependency((y - m_gridPosY - HEADER_HEIGHT - 1) / DEPENDENCY_HEIGHT);
            }
        }
    }
    return result;
}