#include "stdafx.h"
#include "ComponentGraphics_GL.h"
#include "Component/Component/ComponentProxy.h"
#include "Component/Component/DependencyDescription.h"
#include "Component/Component/DependencyDescriptionLine.h"
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

#include <shlobj.h>

static const float Font_Render_X_Offset = 8.0f;
static const float Font_Render_Y_Offset = 0.5f;
static const float DefaultCellSize = 15.0f;
CFontFace* CComponentGraphic_GL::m_pFont = NULL;
SharePtr<CMaterial> CComponentGraphic_GL::m_pMaterial;
SharePtr<CTextureAtlas> CComponentGraphic_GL::m_pAtlas;
SharePtr<CTextureFrag> CComponentGraphic_GL::m_pFrags[eCT_Count];
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
    // HACK: make sure this graphics's depth value is less than the grid.
    m_gridPosZ = -90;
}

CComponentGraphic_GL::~CComponentGraphic_GL()
{

}

void CComponentGraphic_GL::CreateMaterials()
{
    if (m_pFont == NULL)
    {
        TString strFontPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Font);
        strFontPath.append("/msyh.ttf");
        if (!CFilePathTool::GetInstance()->Exists(strFontPath.c_str()))
        {
            char szBuf[MAX_PATH] = { 0 };
            SHGetSpecialFolderPath(NULL, szBuf, CSIDL_FONTS, FALSE);
            strFontPath = szBuf;
            BEATS_ASSERT(!strFontPath.empty(), _T("Get font directory failed!"));
            strFontPath.append(_T("\\msyh.ttf"));
            BEATS_ASSERT(CFilePathTool::GetInstance()->Exists(strFontPath.c_str()), "Get file msyh.ttf falied!");
        }
        m_pFont = CFontManager::GetInstance()->CreateFreetypeFontFace(_T("ComponentGraphicFont"), strFontPath, 12, 0, false);
        BEATS_ASSERT(m_pFont != NULL, _T("You need to install msyh.ttf in your OS!"));
        down_cast<CFreetypeFontFace *>(m_pFont)->SetRelatedRenderTarget(CRenderManager::GetInstance()->GetCurrentRenderTarget());
    }
    BEATS_ASSERT(m_pFont != NULL);

    if (m_pFrags[eCT_RectBG] == NULL)
    {
        m_pAtlas = CResourceManager::GetInstance()->GetResource<CTextureAtlas>(_T("component.xml"));
        m_pMaterial = CreateMaterial();
        m_pMaterial->Initialize();
        m_pFrags[eCT_RectBG] = m_pAtlas->GetTextureFrag(_T("rectbg"));
        m_pFrags[eCT_RefRectBG] = m_pAtlas->GetTextureFrag(_T("refrectbg"));
        m_pFrags[eCT_ConnectRect] = m_pAtlas->GetTextureFrag(_T("connectrect"));
        m_pFrags[eCT_SelectedRectBG] = m_pAtlas->GetTextureFrag(_T("selectedrect"));
        m_pFrags[eCT_NormalLine] = m_pAtlas->GetTextureFrag(_T("normalline"));
        m_pFrags[eCT_SelectedLine] = m_pAtlas->GetTextureFrag(_T("selectedline"));
        m_pFrags[eCT_NormalArrow] = m_pAtlas->GetTextureFrag(_T("normalarrow"));
        m_pFrags[eCT_SelectedArrow] = m_pAtlas->GetTextureFrag(_T("selectedarrow"));
        m_pFrags[eCT_ConnectedDependency] = m_pAtlas->GetTextureFrag(_T("connecteddependency"));
        m_pFrags[eCT_ConnectedDependencyList] = m_pAtlas->GetTextureFrag(_T("connecteddependencylist"));
        m_pFrags[eCT_WeakDependency] = m_pAtlas->GetTextureFrag(_T("weakdependency"));
        m_pFrags[eCT_WeakDependencyList] = m_pAtlas->GetTextureFrag(_T("weakdependencylist"));
        m_pFrags[eCT_StrongDependency] = m_pAtlas->GetTextureFrag(_T("strongdependency"));
        m_pFrags[eCT_StrongDependencyList] = m_pAtlas->GetTextureFrag(_T("strongdependencylist"));
#ifdef _DEBUG
        for (int i = eCT_RectBG; i < eCT_Count; ++i)
        {
            BEATS_ASSERT(m_pFrags[i] != nullptr);
        }
#endif
        m_pRenderBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPT), m_pMaterial, GL_TRIANGLES, true, nullptr);
    }
    SharePtr<CTexture> texture = CResourceManager::GetInstance()->GetResource<CTexture>(_T("Component.png"));
    BEATS_ASSERT(texture, _T("Create Component Texture Failed!"));
    m_pRenderBatch->SetTexture(0, texture);
}

SharePtr<CMaterial> CComponentGraphic_GL::CreateMaterial()
{
    SharePtr<CMaterial> material = new CMaterial();
    material->SetDepthTestEnable(true);
    material->SetBlendEnable(true);
    material->SetAlphaTest(true);
    material->SetAlphaFunc(GL_GREATER);
    material->SetAlphaRef(0);
    material->SetBlendSource(GL_SRC_ALPHA);
    material->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    material->SetSharders( _T("pointtexshader.vs"), _T("pointtexshader.ps"));
    return material;
}

void CComponentGraphic_GL::GetDependencyPosition(uint32_t uDependencyIndex, int* pOutX, int* pOutY)
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

    CQuadPT head;
    head.tl.position.X() = (m_gridPosX + CONNECTION_WIDTH) * cellSize;// Left top
    head.tl.position.Y() = m_gridPosY * cellSize;
    head.tl.position.Z() = (float)m_gridPosZ;
    head.tl.tex = m_pFrags[eCT_RectBG]->GetQuadT().tl;

    head.bl.position.X() = (m_gridPosX + CONNECTION_WIDTH) * cellSize;// Left down
    head.bl.position.Y() = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    head.bl.position.Z() = (float)m_gridPosZ;
    head.bl.tex = m_pFrags[eCT_RectBG]->GetQuadT().bl;

    head.tr.position.X() = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right top
    head.tr.position.Y() = m_gridPosY * cellSize;
    head.tr.position.Z() = (float)m_gridPosZ;
    head.tr.tex = m_pFrags[eCT_RectBG]->GetQuadT().tr;

    head.br.position.X() = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right down
    head.br.position.Y() = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    head.br.position.Z() = (float)m_gridPosZ;
    head.br.tex = m_pFrags[eCT_RectBG]->GetQuadT().br;
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_2_2)
    m_pRenderBatch->AddQuad(&head);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_2)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_3)
    // 2. Draw title text.
    const TString& strDisplayString = m_pOwner->GetUserDefineDisplayName().empty() ? m_pOwner->GetDisplayName() : m_pOwner->GetUserDefineDisplayName();
    m_pFont->RenderText(strDisplayString, head.tl.position.X() + Font_Render_X_Offset * cellSize / DefaultCellSize, head.tl.position.Y() + Font_Render_Y_Offset * cellSize / DefaultCellSize,  LAYER_GUI_EDITOR, 1.0f, 0x000000FF, 0, nullptr, false);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ComponentWindow_InstanceHead_3)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_ComponentWindow_InstanceHead_4)
    // 3. Draw Connect rect.
    CQuadPT connectRect;

    connectRect.tl.position.X() = m_gridPosX * cellSize;    // Left top
    connectRect.tl.position.Y() = m_gridPosY * cellSize;
    connectRect.tl.position.Z() = (float)m_gridPosZ;
    connectRect.tl.tex = m_pFrags[eCT_ConnectRect]->GetQuadT().tl;

    connectRect.bl.position.X() = m_gridPosX * cellSize;    // Left down
    connectRect.bl.position.Y() = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    connectRect.bl.position.Z() = (float)m_gridPosZ;
    connectRect.bl.tex = m_pFrags[eCT_ConnectRect]->GetQuadT().bl;

    connectRect.tr.position.X() = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right top
    connectRect.tr.position.Y() = m_gridPosY * cellSize;
    connectRect.tr.position.Z() = (float)m_gridPosZ;    
    connectRect.tr.tex = m_pFrags[eCT_ConnectRect]->GetQuadT().tr;

    connectRect.br.position.X() = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right down
    connectRect.br.position.Y() = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    connectRect.br.position.Z() = (float)m_gridPosZ;    
    connectRect.br.tex = m_pFrags[eCT_ConnectRect]->GetQuadT().br;
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

    const std::vector<CDependencyDescription*>* pDependencies = m_pOwner->GetDependencies();
    BEATS_ASSERT(pDependencies != NULL);
    uint32_t uDependencyCount = pDependencies->size();
    for (int i = 0; i < (int)uDependencyCount; ++i)
    {
        // 1. Draw background.
        CQuadPT dependencyPoint;
        dependencyPoint.tl.position.X() = m_gridPosX * cellSize;    // Left top
        dependencyPoint.tl.position.Y() = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.tl.position.Z() = (float)m_gridPosZ;
        dependencyPoint.tl.tex = m_pFrags[eCT_RectBG]->GetQuadT().tl;

        dependencyPoint.bl.position.X() = m_gridPosX * cellSize;    // Left down
        dependencyPoint.bl.position.Y() = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.bl.position.Z() = (float)m_gridPosZ;
        dependencyPoint.bl.tex = m_pFrags[eCT_RectBG]->GetQuadT().bl;

        dependencyPoint.tr.position.X() = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right top
        dependencyPoint.tr.position.Y() = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.tr.position.Z() = (float)m_gridPosZ;
        dependencyPoint.tr.tex = m_pFrags[eCT_RectBG]->GetQuadT().tr;

        dependencyPoint.br.position.X() = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right down
        dependencyPoint.br.position.Y() = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.br.position.Z() = (float)m_gridPosZ;
        dependencyPoint.br.tex = m_pFrags[eCT_RectBG]->GetQuadT().br;
        m_pRenderBatch->AddQuad(&dependencyPoint);

        // 2. Draw dependency name.
        m_pFont->RenderText(m_pOwner->GetDependency(i)->GetDisplayName(), dependencyPoint.tl.position.X() + Font_Render_X_Offset * cellSize / DefaultCellSize, dependencyPoint.tl.position.Y() + Font_Render_Y_Offset * cellSize / DefaultCellSize, LAYER_GUI_EDITOR, 1.0f, 0x000000FF, 0, nullptr, false);

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

        dependencyPoint.tl.position.X() = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left top
        dependencyPoint.tl.position.Y() = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.tl.position.Z() = (float)m_gridPosZ;
        dependencyPoint.tl.tex = m_pFrags[textureType]->GetQuadT().tl;

        dependencyPoint.bl.position.X() = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left down
        dependencyPoint.bl.position.Y() = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.bl.position.Z() = (float)m_gridPosZ;
        dependencyPoint.bl.tex = m_pFrags[textureType]->GetQuadT().bl;

        dependencyPoint.tr.position.X() = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right top
        dependencyPoint.tr.position.Y() = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.tr.position.Z() = (float)m_gridPosZ;
        dependencyPoint.tr.tex = m_pFrags[textureType]->GetQuadT().tr;

        dependencyPoint.br.position.X() = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right down
        dependencyPoint.br.position.Y() = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.br.position.Z() = (float)m_gridPosZ;
        dependencyPoint.br.tex = m_pFrags[textureType]->GetQuadT().br;

        m_pRenderBatch->AddQuad(&dependencyPoint);

        // 4. Draw the line.
        DrawDependencyLine(cellSize, m_pOwner->GetDependency(i));
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
        uint32_t uDependencyLineCount = pDependency->GetDependencyLineCount();
        for (uint32_t i = 0; i < uDependencyLineCount; ++i)
        {
            CDependencyDescriptionLine* pDependencyLine = pDependency->GetDependencyLine(i);
            EComponentTexture textureLine = pDependencyLine->IsSelected() ? eCT_SelectedLine : eCT_NormalLine;
            EComponentTexture textureArrow = pDependencyLine->IsSelected() ? eCT_SelectedArrow : eCT_NormalArrow;
            const SVertex* pData = pDependencyLine->GetRectArray();
            static const uint32_t SVERTEX_SIZE = 24;
            CSerializer serializer(SVERTEX_SIZE * 4, (void*)pData);
            DWORD tmpColor = 0;
            CQuadPT dependencyLine;
            serializer >> dependencyLine.br.position.X();
            serializer >> dependencyLine.br.position.Y();
            serializer >> dependencyLine.br.position.Z();
            dependencyLine.br.position.Z() = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.br.tex.u;
            serializer >> dependencyLine.br.tex.v;
            dependencyLine.br.tex = m_pFrags[textureLine]->GetQuadT().br;

            serializer >> dependencyLine.bl.position.X();
            serializer >> dependencyLine.bl.position.Y();
            serializer >> dependencyLine.bl.position.Z();
            dependencyLine.bl.position.Z() = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.bl.tex.u;
            serializer >> dependencyLine.bl.tex.v;
            dependencyLine.bl.tex = m_pFrags[textureLine]->GetQuadT().bl;

            serializer >> dependencyLine.tr.position.X();
            serializer >> dependencyLine.tr.position.Y();
            serializer >> dependencyLine.tr.position.Z();
            dependencyLine.tr.position.Z() = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.tr.tex.u;
            serializer >> dependencyLine.tr.tex.v;
            dependencyLine.tr.tex = m_pFrags[textureLine]->GetQuadT().tr;

            serializer >> dependencyLine.tl.position.X();
            serializer >> dependencyLine.tl.position.Y();
            serializer >> dependencyLine.tl.position.Z();
            dependencyLine.tl.position.Z() = -1.0f;
            serializer >> tmpColor;
            serializer >> dependencyLine.tl.tex.u;
            serializer >> dependencyLine.tl.tex.v;
            dependencyLine.tl.tex = m_pFrags[textureLine]->GetQuadT().tl;

            m_pRenderBatch->AddQuad(&dependencyLine);

            const SVertex* pArrowData = pDependencyLine->GetArrowRectArray();
            CSerializer serializerArrow(SVERTEX_SIZE * 4, (void*)pArrowData);
            CQuadPT arrow;
            serializerArrow >> arrow.br.position.X();
            serializerArrow >> arrow.br.position.Y();
            serializerArrow >> arrow.br.position.Z();
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.br.tex.u;
            serializerArrow >> arrow.br.tex.v;
            arrow.br.tex = m_pFrags[textureArrow]->GetQuadT().br;

            serializerArrow >> arrow.bl.position.X();
            serializerArrow >> arrow.bl.position.Y();
            serializerArrow >> arrow.bl.position.Z();
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.bl.tex.u;
            serializerArrow >> arrow.bl.tex.v;
            arrow.bl.tex = m_pFrags[textureArrow]->GetQuadT().bl;

            serializerArrow >> arrow.tr.position.X();
            serializerArrow >> arrow.tr.position.Y();
            serializerArrow >> arrow.tr.position.Z();
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.tr.tex.u;
            serializerArrow >> arrow.tr.tex.v;
            arrow.tr.tex = m_pFrags[textureArrow]->GetQuadT().tr;

            serializerArrow >> arrow.tl.position.X();
            serializerArrow >> arrow.tl.position.Y();
            serializerArrow >> arrow.tl.position.Z();
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.tl.tex.u;
            serializerArrow >> arrow.tl.tex.v;
            arrow.tl.tex = m_pFrags[textureArrow]->GetQuadT().tl;

            m_pRenderBatch->AddQuad(&arrow);

            //Render index number for dependency list.
            if (pDependency->IsListType())
            {
                CVec3 deltaDirection;
                deltaDirection = dependencyLine.tr.position - dependencyLine.tl.position;
                float fXPos = (dependencyLine.tl.position.X() + deltaDirection.X() * 0.15f);
                static const float fHardCodeOffset = 8;
                float fYPos = (dependencyLine.tl.position.Y() + deltaDirection.Y() * 0.15f - fHardCodeOffset);

                TCHAR szIndex[8];
                _stprintf(szIndex, _T("%d"), pDependencyLine->GetIndex());
                m_pFont->RenderText(szIndex, fXPos, fYPos, LAYER_GUI_EDITOR, 1.0f, 0xffff00ff, 0, nullptr, false);
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
    rect.tl.position.X() = x * fCellSize;
    rect.tl.position.Y() = y * fCellSize;
    rect.tl.position.Z() = (float)m_gridPosZ + eRL_SelectRect;
    rect.tl.tex = m_pFrags[eCT_SelectedRectBG]->GetQuadT().tl;
    // left down
    rect.bl.position.X() = x * fCellSize;
    rect.bl.position.Y() = y * fCellSize + fHeight;
    rect.bl.position.Z() = (float)m_gridPosZ + eRL_SelectRect;
    rect.bl.tex = m_pFrags[eCT_SelectedRectBG]->GetQuadT().bl;
    // right top
    rect.tr.position.X() = x * fCellSize + fWidth;
    rect.tr.position.Y() = y * fCellSize;
    rect.tr.position.Z() = (float)m_gridPosZ + eRL_SelectRect;
    rect.tr.tex = m_pFrags[eCT_SelectedRectBG]->GetQuadT().tr;
    // right down
    rect.br.position.X() = x * fCellSize + fWidth;
    rect.br.position.Y() = y * fCellSize + fHeight;
    rect.br.position.Z() = (float)m_gridPosZ + eRL_SelectRect;
    rect.br.tex = m_pFrags[eCT_SelectedRectBG]->GetQuadT().br;

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