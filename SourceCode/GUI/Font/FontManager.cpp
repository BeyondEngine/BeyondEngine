#include "stdafx.h"
#include "FontManager.h"
#include "FontFace.h"
#include "Render/Texture.h"
#include "Render/RenderManager.h"
#include "Render/Material.h"
#include "Render/RenderGroup.h"
#include "Render/Shader.h"
#include "Render/RenderState.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderWindow.h"
#include "Resource/ResourceManager.h"
#include "FreetypeFontFace.h"
#include "BitmapFontFace.h"

CFontManager *CFontManager::m_pInstance = nullptr;

CFontManager::CFontManager()
    : m_pRenderGroupGUI(nullptr)
    , m_pRenderGroup2D(nullptr)
    , m_pProgramFreetypeText(nullptr)
    , m_pProgramBitmapText(nullptr)
    , m_pDefaultFontFace(nullptr)
{
//     CRenderManager::GetInstance()->SubscribeEvent(CRenderManager::eET_MAIN_RENDER_WINDOW_CHANGED,
//         &CFontManager::OnMainRenderWindowChanged, this);
// 
//     if(CRenderManager::GetInstance()->GetMainRenderWindow())
//     {
//         m_slot = CRenderManager::GetInstance()->GetMainRenderWindow()->EventDispatcher()->
//             SubscribeEvent(CRenderWindow::eET_EVENT_SIZE_CHANGED,
//             &CFontManager::OnMainRenderWindowSizeChanged, this);
//     }
}

CFontManager::~CFontManager()
{
    for(auto face : m_faceMap)
    {
        BEATS_SAFE_DELETE(face.second);
    }
}

CFontFace *CFontManager::CreateFreetypeFontFace( const TString &name, const TString &file, int sizeInPt, int dpi, bool bRegister )
{
    BEATS_ASSERT(!bRegister || m_faceMap.find(name) == m_faceMap.end(),
        _T("Font face:%s already existed"), name.c_str());

    CFreetypeFontFace *face = new CFreetypeFontFace(name, file, sizeInPt, dpi);
    if (bRegister)
    {
        m_faceMap[name] = face;
    }
    return face;
}

CFontFace *CFontManager::CreateBitmapFontFace(const TString &filename)
{
    CBitmapFontFace *face = new CBitmapFontFace(filename);
    BEATS_ASSERT(m_faceMap.find(face->GetName()) == m_faceMap.end(),
        _T("Font face:%s already existed"), face->GetName().c_str());
    m_faceMap[face->GetName()] = face;
    return face;
}

CFontFace *CFontManager::GetFace( const TString &name )
{
    auto itr = m_faceMap.find(name);
    return itr != m_faceMap.end() ? itr->second : nullptr;
}

void CFontManager::RenderText(const TString &text, kmScalar x, kmScalar y, 
                             const TString &faceName, CColor color)
{
    CFontFace *face = GetFace(faceName);
    BEATS_ASSERT(face);

    face->RenderText(text, x, y, 1.0f, color);
}

SharePtr<CMaterial> CFontManager::CreateDefaultMaterial(bool bBitmapFont)
{
    if(!m_pProgramFreetypeText)
    {
        SharePtr<CShader> pVS = CResourceManager::GetInstance()->
            GetResource<CShader>(_T("TextShader.vs"));
        SharePtr<CShader> pPS = CResourceManager::GetInstance()->
            GetResource<CShader>(_T("TextShader.ps"));
        BEATS_ASSERT(pVS && pPS, _T("Load Shader Failed!"));
        m_pProgramFreetypeText = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
    }
    if(!m_pProgramBitmapText)
    {
        SharePtr<CShader> pVS = CResourceManager::GetInstance()->
            GetResource<CShader>(_T("PointTexShader.vs"));
        SharePtr<CShader> pPS = CResourceManager::GetInstance()->
            GetResource<CShader>(_T("PointTexShader.ps"));
        BEATS_ASSERT(pVS && pPS, _T("Load Shader Failed!"));
        m_pProgramBitmapText = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
    }
    SharePtr<CMaterial> material = new CMaterial();
    material->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_ScissorTest, false);
    material->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_Blend, true);
    material->GetRenderState()->SetBlendFuncSrcFactor(GL_SRC_ALPHA);
    material->GetRenderState()->SetBlendFuncTargetFactor(GL_ONE_MINUS_SRC_ALPHA);
    material->GetRenderState()->SetShaderProgram(bBitmapFont ? m_pProgramBitmapText->ID() : m_pProgramFreetypeText->ID());

    return material;
}

CRenderGroup *CFontManager::GetRenderGroupGUI()
{
    if(!m_pRenderGroupGUI)
    {
        m_pRenderGroupGUI = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_GUI);
        BEATS_ASSERT(m_pRenderGroupGUI);
    }
    return m_pRenderGroupGUI;
}

CRenderGroup *CFontManager::GetRenderGroup2D()
{
    if(!m_pRenderGroup2D)
    {
        m_pRenderGroup2D = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_2D);
        BEATS_ASSERT(m_pRenderGroup2D);
    }
    return m_pRenderGroup2D;
}

void CFontManager::Clear()
{
    for(auto facePair : m_faceMap)
    {
        facePair.second->Clear();
    }
}

const std::map<TString, CFontFace *>& CFontManager::GetFontFaceMap() const
{
    return m_faceMap;
}

CFontFace* CFontManager::GetDefaultFontFace() const
{
    return m_pDefaultFontFace;
}

void CFontManager::SetDefaultFontFace(CFontFace* pFontFace)
{
    BEATS_ASSERT(m_pDefaultFontFace == NULL);
    m_pDefaultFontFace = pFontFace;
}

void CFontManager::OnMainRenderWindowChanged(CBaseEvent *event)
{
    if(CRenderManager::GetInstance()->GetCurrentRenderTarget())
    {
//         m_slot = CRenderManager::GetInstance()->GetMainRenderWindow()->EventDispatcher()->
//             SubscribeEvent(CRenderWindow::eET_EVENT_SIZE_CHANGED,
//             &CFontManager::OnMainRenderWindowSizeChanged, this);
        Clear();
    }
}

void CFontManager::OnMainRenderWindowSizeChanged(CBaseEvent *event)
{
    Clear();
}
