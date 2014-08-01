#include "stdafx.h"
#include "WindowManager.h"
#include "Render/ShaderProgram.h"
#include "Render/ShaderUniform.h"
#include "Render/RenderTarget.h"
#include "GUI/Window/Control.h"
#include "GUI/Window/TextureControl.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Render/Shader.h"
#include "Render/TextureFrag.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderGroup.h"
#include "Render/CommonTypes.h"
#include "Render/RenderBatch.h"
#include "CompWrapper.h"
#include "Font/FontManager.h"
#include "Font/FontFace.h"
#include "Window/TopWindow.h"
#include "Event/TouchEvent.h"
#include "Window/TextBox.h"
#include "Render/RenderState.h"
#include "Render/Material.h"

CWindowManager *CWindowManager::m_pInstance = nullptr;

CWindowManager::CWindowManager()
    : m_pDefaultMateial(NULL)
    , m_pBaseMaterial(NULL)
    , m_pColorMelody(NULL)
    , m_pActiveTopWindow(NULL)
    , m_pFocusControl(NULL)
{
}

CWindowManager::~CWindowManager()
{
    BEATS_SAFE_DELETE( m_pColorMelody );
    for ( auto control : m_vecControlByUserCreate )
    {
        control->Uninitialize();
        BEATS_SAFE_DELETE( control );
    }
    m_vecControlByUserCreate.clear();
}

void CWindowManager::AddToRoot(CControl* pControl)
{
    BEATS_ASSERT( pControl != NULL, _T("the control can not be null"));
    m_rootControl.push_back(pControl);
    pControl->SetRootFlag( true );
}

void CWindowManager::RemoveFromRoot(CControl* pControl )
{
    BEATS_ASSERT(pControl != NULL);
    auto iter = std::find( m_rootControl.begin(), m_rootControl.end(), pControl );
    BEATS_ASSERT(iter != m_rootControl.end());
    if (iter != m_rootControl.end())
    {
        (*iter)->SetRootFlag( false );
        m_rootControl.erase(iter);
    }
}

void CWindowManager::RegisterControl( CControl* pControl )
{
    const TString& strControlName = pControl->GetName();
    BEATS_ASSERT(strControlName.compare(DEFAULT_CONTROL_NAME) != 0);
    auto check = m_mapControl.find(strControlName);
    BEATS_ASSERT ( check == m_mapControl.end(), _T("have register the name of %s before"), strControlName.c_str() );
    m_mapControl[ pControl->GetName() ] = pControl ;
}

void CWindowManager::UnregisterControl(CControl* pControl)
{
    const TString& strControlName = pControl->GetName();
    if (strControlName.compare(DEFAULT_CONTROL_NAME) != 0)
    {
        auto check = m_mapControl.find(strControlName);
        BEATS_ASSERT ( check != m_mapControl.end(), _T("have not register the name of %s before"), strControlName.c_str() );
        m_mapControl.erase( check );
    }
}

void CWindowManager::LogoutControl( CControl* pControl )
{
    UnregisterControl(pControl);
    if ( pControl->GetRootFlag() )
    {
        RemoveFromRoot( pControl );
    }
}

CControl* CWindowManager::GetControl( const TString& name )
{
    CControl* pRet = nullptr;
    auto iter = m_mapControl.find( name );
    if( iter != m_mapControl.end())
    {
        pRet = iter->second;
    }
    return pRet;
}

CControl* CWindowManager::GetControl( float x, float y )
{
    CControl* pRetControl = nullptr;
    for ( auto control :  m_mapControl )
    {
        pRetControl = RecursiveChildren( control.second , x, y );
        if ( pRetControl )
        {
            break;
        }
    }
    return pRetControl;
}

CControl* CWindowManager::RecursiveChildren( CControl* pControl , float x, float y )
{
    CControl* pRetControl = nullptr;
    if ( pControl->IsVisible() && pControl->HitTest( x, y ) )
    {
        pRetControl = pControl;
        for ( auto child : pControl->GetChildren() )
        {
            if ( child->GetType() == eNT_NodeGUI )
            {
                CControl* pChildControl = RecursiveChildren( (CControl*)child , x, y );
                if ( pChildControl )
                {
                    pRetControl = pChildControl;
                    break;
                }
            }
        }
    }
    return pRetControl;
}

bool CWindowManager::DestoryControl( CControl* pControl )
{
    bool bRet = false;
    for ( auto control = m_vecControlByUserCreate.begin(); 
        control != m_vecControlByUserCreate.end(); ++control )
    {
        if ( *control == pControl )
        {
            CNode* pParent = pControl->GetParentNode();
            if( pParent )
            {
                pParent->RemoveChild( pControl );
            }
            (*control)->Uninitialize();
            BEATS_SAFE_DELETE( *control );
            m_vecControlByUserCreate.erase( control );
            bRet = true;
            break;
        }
    }
    return bRet;
}

void CWindowManager::DispatchEventToRecipient( CControl* pControl, CBaseEvent* pEvent )
{
    auto eventHandlder = m_mapRecipient[ pControl ];
    for ( auto func : eventHandlder )
    {
        func( pEvent );
    }
}

void CWindowManager::Update( float dtt )
{
    for (auto iter = m_rootControl.begin(); iter != m_rootControl.end(); ++iter)
    {
        if ( (*iter)->IsVisible() )
        {
            (*iter)->Update( dtt );
        }
    }
    for (auto iter = m_topWindowList.begin(); iter != m_topWindowList.end(); ++iter)
    {
        if ( (*iter)->IsVisible() )
        {
            (*iter)->Update( dtt );
        }
    }
}

void CWindowManager::Render()
{
    for (auto iter = m_rootControl.begin(); iter != m_rootControl.end(); ++iter)
    {
        if ((*iter)->IsVisible())
        {
            (*iter)->Render();
        }
    }
    for (auto iter = m_topWindowList.begin(); iter != m_topWindowList.end(); ++iter)
    {
        if ( (*iter)->IsVisible() )
        {
            (*iter)->Render();
        }
    }
}

void CWindowManager::OnSizeChange()
{
    CRenderTarget* pTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    if (pTarget != NULL)
    {
        float w = (float)pTarget->GetWidth();
        float h = (float)pTarget->GetHeight();
        for (auto iter = m_rootControl.begin(); iter != m_rootControl.end(); ++iter)
        {
            (*iter)->OnParentSizeChange( w, h );
        }
        for ( const auto& topWindow : m_topWindowList )
        {
            topWindow->OnRenderTargetSizeChange( w, h );
        }
        for( auto contol : m_vecControlAddNode )
        {
            contol->OnParentSizeChange( w, h );
        }
        CFontManager::GetInstance()->Clear();
    }
}

void CWindowManager::OnTouchEvent( CTouchEvent* event )
{
    bool bDisposeEvent = false;
    for ( auto topWindow : m_topWindowList )
    {
        if ( topWindow->IsVisible() )
        {
            bDisposeEvent = topWindow->OnTouchEvent( event );
            if ( bDisposeEvent )
            {
                event->StopPropagation();
            }
            break;
        }
    }
    if ( !bDisposeEvent )
    {
        for ( auto control = m_rootControl.rbegin(); control != m_rootControl.rend(); ++control )
        {
            if ( (*control)->IsVisible() && (*control)->OnTouchEvent( event ))
            {
                bDisposeEvent = true;
                break;
            }
        }
    }
    if ( !bDisposeEvent )
    {
        SetFocusControl( nullptr );
    }
}

SharePtr<CMaterial> CWindowManager::GetDefaultMaterial()
{
    if ( m_pDefaultMateial == nullptr )
    {
        SharePtr<CMaterial> pMaterial = new CMaterial();
        pMaterial->SetDepthTest( false );
        pMaterial->SetSharders( _T("GUIShader.vs"), _T("GUIShader.ps") );
        pMaterial->SetBlendEnable( true );
        pMaterial->SetBlendDest( GL_ONE_MINUS_SRC_ALPHA );
        pMaterial->SetBlendSource( GL_SRC_ALPHA );
        m_pColorMelody = new CShaderUniform(_T("u_melody"), eSUT_4f);
        BEATS_ASSERT(m_pColorMelody->GetData().size() == 4);
        pMaterial->AddUniform(m_pColorMelody);
        m_pDefaultMateial = pMaterial;
    }
    return m_pDefaultMateial;
}

SharePtr<CMaterial> CWindowManager::GetScissorMaterial( GLuint texID, const CRect& rect )
{
    SharePtr<CMaterial> pMaterial = nullptr;
    bool bFindRect = false;
    size_t findIndex = 0;
    for ( size_t i = 0; i < m_scissorRect.size(); ++i )
    {
        if ( m_scissorRect[ i ] == rect )
        {
            findIndex = i;
            bFindRect = true;
            break;
        }
    }
    if ( bFindRect )
    {
        auto scissorIter = m_mapScissorMaterial.find( texID );
        if ( scissorIter != m_mapScissorMaterial.end() )
        {
            auto materialIter = scissorIter->second.find( findIndex );
            if ( materialIter != scissorIter->second.end() )
            {
                pMaterial = materialIter->second;
            }
            else
            {
                pMaterial = CreateScissorMaterial( rect );
                scissorIter->second[ findIndex ] = pMaterial;
            }
        }
        else
        {
            pMaterial = CreateScissorMaterial( rect );
            TScissorMaterial secissorMaterial;
            secissorMaterial[ findIndex ] = pMaterial;
            m_mapScissorMaterial[ texID] = secissorMaterial;
        }
    }
    else
    {
        pMaterial = CreateScissorMaterial( rect );
        m_scissorRect.push_back( rect );
        TScissorMaterial secissorMaterial;
        secissorMaterial[ m_scissorRect.size() - 1 ] = pMaterial;
        m_mapScissorMaterial[ texID] = secissorMaterial;
    }

    BEATS_ASSERT( pMaterial );
    return pMaterial;
}

SharePtr<CMaterial> CWindowManager::CreateScissorMaterial( const CRect& rect )
{
    SharePtr<CMaterial> pMaterial = GetDefaultMaterial()->Clone();
    pMaterial->SetScissorTest( true );
    pMaterial->SetScssorRect( (int)rect.position.x, (int)rect.position.y, (int)rect.size.x, (int)rect.size.y );
    return pMaterial;
}

SharePtr<CMaterial> CWindowManager::GetTextrueMateial( GLuint id )
{
    SharePtr<CMaterial> pMaterial = nullptr;
    auto iter = m_mapTextureMaterial.find( id );
    if ( iter != m_mapTextureMaterial.end() )
    {
        pMaterial = iter->second;
    }
    else
    {
        pMaterial = GetDefaultMaterial()->Clone();
        m_mapTextureMaterial[ id ] = pMaterial;
    }
    return pMaterial;
}

SharePtr<CMaterial> CWindowManager::GetBaseMaterial()
{
    if ( m_pBaseMaterial == nullptr )
    {
        SharePtr<CMaterial> pMaterial = GetDefaultMaterial()->Clone();
        m_pBaseMaterial = pMaterial;
    }
    return m_pBaseMaterial;
}

SharePtr<CMaterial> CWindowManager::GetColorMaterial()
{
    if ( m_pColorMaterial == nullptr )
    {
        SharePtr<CMaterial> pMaterial = GetDefaultMaterial()->Clone();
        m_pColorMaterial = pMaterial;
    }
    return m_pColorMaterial;
}

SharePtr<CMaterial> CWindowManager::GetEditorMaterial()
{
    if(!m_pEditorMaterial)
    {
        m_pEditorMaterial = new CMaterial();
        m_pEditorMaterial->SetSharders( _T("PointColorShader.vs"), _T("PointColorShader.ps") );
        m_pEditorMaterial->SetDepthTest(false);
        m_pEditorMaterial->SetBlendEnable( true);
        m_pEditorMaterial->SetBlendEquation( GL_FUNC_ADD );
        m_pEditorMaterial->SetBlendSource( GL_SRC_ALPHA );
        m_pEditorMaterial->SetBlendDest( GL_ONE_MINUS_SRC_ALPHA );
        m_pEditorMaterial->GetRenderState()->SetLineWidth(1.f);
        m_pEditorMaterial->GetRenderState()->SetPointSize(6.f);
    }
    return m_pEditorMaterial;
}

void CWindowManager::RenderBaseControl( const CQuadP* quadp, const CColor& color , bool isClip , const CRect& rect /*= CRect()*/, const kmMat4* pTransform /*= NULL */ )
{
    SharePtr< CMaterial > pMaterial = nullptr;
    pMaterial = isClip ? CWindowManager::GetInstance()->GetScissorMaterial( 0, rect ) : CWindowManager::GetInstance()->GetBaseMaterial();
    BEATS_ASSERT( pMaterial, _T("the material can't be null"));
    CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_GUI );
    BEATS_ASSERT(renderGroup);
    //render window
    CRenderBatch *batch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPC), pMaterial, GL_TRIANGLES, true );
    batch->AddQuad( quadp, color, pTransform );
}

void CWindowManager::RenderTextureControl( const CTextureFrag* pFrag, bool bGrid9, const CVec2& grid9Offset, const CQuadP* quadp, const CQuadT* quadt, const CColor& color
                                          , bool isClip, bool bDisable /*= false*/, const CRect& rect /*= CRect() */, const kmMat4* pTransform )
{
    BEATS_ASSERT( pFrag, _T("the texture frag can't be null"));
    if (pFrag && pFrag->Texture())
    {
        CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_GUI );
        BEATS_ASSERT(renderGroup);
        //render window
        CRenderBatch *batch = nullptr;
        SharePtr<CMaterial> pMaterial = nullptr;
        pMaterial = isClip ? CWindowManager::GetInstance()->GetScissorMaterial( pFrag->Texture()->ID(), rect ) : CWindowManager::GetInstance()->GetTextrueMateial( pFrag->Texture()->ID());
        BEATS_ASSERT( pMaterial, _T("the material can't be null"));
        batch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPTC), pMaterial, GL_TRIANGLES, true );
        if (bDisable)
        {
            CShaderUniform* pColorMelodyUniform = pMaterial->GetUniform(_T("u_melody"));
            std::vector<float>& data = pColorMelodyUniform->GetData();
            data[0] = 1.0f;
            data[1] = 1.0f;
            data[2] = 0.0f;
            data[3] = 1.0f;
        }
        BEATS_ASSERT( pMaterial, _T("the material can't be null"));
        pMaterial->SetTexture(0, pFrag->Texture());
        BEATS_ASSERT( batch, _T("the render batch can't be null") );
        if ( bGrid9 )
        {
            CVertexPTC vertices[GRID9_VERTICES_COUNT];
            unsigned short indices[GRID9_INDICES_COUNT];
            SetGrid9VerticesAndIndices( grid9Offset, quadp, quadt, vertices, indices, color );
            batch->AddIndexedVertices(vertices, GRID9_VERTICES_COUNT,
                indices, GRID9_INDICES_COUNT, pTransform);
        }
        else
        {
            batch->AddQuad( quadp, quadt, color ,pTransform );
        }
    }
}

void CWindowManager::RenderEditorSelect( const CQuadP* quadp , const kmMat4& transform)
{
     //highlight
    CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_GUI_EDITOR );
    CRenderBatch *pBatch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPC), GetEditorMaterial(), GL_TRIANGLES, true);
    pBatch->AddQuad( quadp, 0xFFFFFF33, &transform);

    //outline
    pBatch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPC), GetEditorMaterial(), GL_LINES, true);
    CQuadPC quadLine;
    quadLine.tl.position = quadp->tl;
    quadLine.tr.position = quadp->tr;
    quadLine.bl.position = quadp->bl;
    quadLine.br.position = quadp->br;
    CColor lineColor = 0xFFFFFF7F;
    quadLine.tl.color = lineColor;
    quadLine.tr.color = lineColor;
    quadLine.bl.color = lineColor;
    quadLine.br.color = lineColor;
    unsigned short indices[] = {0, 1, 1, 3, 3, 2, 2, 0};
    pBatch->AddIndexedVertices((const CVertexPC *)&quadLine, 4, indices, 8, &transform);
    //rotation bar line
    CVertexPC linePt[2];
    linePt[0].color = lineColor;
    linePt[1].position.x = 30.f;
    linePt[1].color = lineColor;
    pBatch->AddIndexedVertices(linePt, 2, indices, 2, nullptr);

    //control point
    pBatch = renderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPC), GetEditorMaterial(), GL_POINTS, false);
    //  corner
    CColor pointColor = 0xFFFFFFFF;
    quadLine.tl.color = pointColor;
    quadLine.tr.color = pointColor;
    quadLine.bl.color = pointColor;
    quadLine.br.color = pointColor;
    pBatch->AddVertices((const CVertexPC *)&quadLine, 4, &transform);
    //  edge
    CVertexPC pt[6];
    pt[0].position.x = (quadp->tl.x + quadp->tr.x) / 2;   //top
    pt[0].position.y = (quadp->tl.y + quadp->tr.y) / 2;
    pt[0].color = pointColor;
    pt[1].position.x = (quadp->bl.x + quadp->br.x) / 2;   //bottom
    pt[1].position.y = (quadp->bl.y + quadp->br.y) / 2;
    pt[1].color = pointColor;
    pt[2].position.x = (quadp->tl.x + quadp->bl.x) / 2;   //left
    pt[2].position.y = (quadp->tl.y + quadp->bl.y) / 2;
    pt[2].color = pointColor;
    pt[3].position.x = (quadp->tr.x + quadp->br.x) / 2;   //right
    pt[3].position.y = (quadp->tr.y + quadp->br.y) / 2;
    pt[3].color = pointColor;
    //  anchor
    pt[4].color = 0xFF0000FF;
    //  rotation bar
    pt[5].position.x = 30.f;
    pt[5].color = 0xFFFF00FF;
    pBatch->AddVertices(pt, 6, &transform );
}

void CWindowManager::InitFontFace()
{
    CFontFace* pDefaultFace = CFontManager::GetInstance()->CreateFreetypeFontFace( _T("Default"), _T("Droid Sans Fallback.ttf"), 24);
    BEATS_ASSERT( pDefaultFace, _T("can't create the font face by Droid Sans Fallback.ttf") );
    CFontManager::GetInstance()->SetDefaultFontFace( pDefaultFace );
    CFontManager::GetInstance()->CreateFreetypeFontFace(_T("ipagp"), _T("ipagp.ttf"), 24);
    CFontManager::GetInstance()->CreateBitmapFontFace(_T("NumberL.font"));
    CFontManager::GetInstance()->CreateBitmapFontFace(_T("NumberS.font"));
}

void CWindowManager::Initialize()
{
    InitFontFace();
}

void CWindowManager::RegisterTopWindow( CTopWindow* pTopWindow )
{
    auto check = m_mapNameTopWindow.find( pTopWindow->GetName() );
    BEATS_ASSERT ( check == m_mapNameTopWindow.end(), _T("have registored the name of %s before"),pTopWindow->GetName().c_str() );
    m_mapNameTopWindow[ pTopWindow->GetName() ] = pTopWindow ;
}

void CWindowManager::UnregisterTopWindow( CTopWindow* pTopWindow )
{
    auto check = m_mapNameTopWindow.find( pTopWindow->GetName() );
    BEATS_ASSERT ( check != m_mapNameTopWindow.end(), _T("have not registored the name of %s before"),pTopWindow->GetName().c_str() );
    m_mapNameTopWindow.erase( check );
}

void CWindowManager::LogoutTopWindow( CTopWindow* pTopWindow )
{
    UnregisterTopWindow(pTopWindow);
    RemoveFromRootTopWindow( pTopWindow );
    if ( m_pActiveTopWindow == pTopWindow )
    {
        m_pActiveTopWindow = nullptr;
    }
}

void CWindowManager::AddToRootTopWindow( CTopWindow* pTopWindow )
{
    BEATS_ASSERT( pTopWindow, _T("the top window can not be null"));
    BEATS_ASSERT( std::find(m_topWindowList.begin(), m_topWindowList.end(), pTopWindow) == m_topWindowList.end());
    m_topWindowList.push_back(pTopWindow);
}

void CWindowManager::RemoveFromRootTopWindow( CTopWindow* pTopWindow )
{
    BEATS_ASSERT( pTopWindow, _T("the top window can not be null"));
    auto iter = std::find(m_topWindowList.begin(), m_topWindowList.end(), pTopWindow);
    BEATS_ASSERT( iter != m_topWindowList.end());
    m_topWindowList.erase(iter);
}

CTopWindow* CWindowManager::GetTopWindowByName( const TString& name )
{
    return m_mapNameTopWindow[ name ];
}

void CWindowManager::SetActiveTopWindow( const TString& name )
{
    CTopWindow* pTopWindow = GetTopWindowByName( name );
    BEATS_ASSERT( pTopWindow );
    SetActiveTopWindow( pTopWindow );
}

void CWindowManager::SetActiveTopWindow( CTopWindow* pTopWindow )
{
    if ( m_pActiveTopWindow )
    {
        m_pActiveTopWindow->SetVisible( false );
    }
    m_pActiveTopWindow = pTopWindow;
    m_pActiveTopWindow->SetVisible( true );
}

void CWindowManager::SetFocusControl( CControl* pControl )
{
    if ( pControl )
    {
        pControl->SetFocus( true );
    }
    if ( m_pFocusControl != pControl )
    {
        if ( m_pFocusControl )
        {
            m_pFocusControl->SetFocus( false );
        }
        m_pFocusControl = pControl;
        if ( m_pFocusControl && m_pFocusControl->GetGuid() == CTextBox::REFLECT_GUID )
        {
            //call keybord
        }
        else
        {
            //hide keybord
        }
    }
}

void CWindowManager::OnCharEvent( const TCHAR* data, int num )
{
    if ( m_pFocusControl && m_pFocusControl->GetGuid() == CTextBox::REFLECT_GUID )
    {
        CTextBox* pTextBox = (CTextBox*)m_pFocusControl;
        pTextBox->OnInsertText( data, num );
    }
}

void CWindowManager::SetGrid9VerticesAndIndices( const CVec2& grid9Offset, const CQuadP* quadp,const CQuadT* quadt, CVertexPTC (&vertices)[GRID9_VERTICES_COUNT],
    unsigned short (&indices)[GRID9_INDICES_COUNT],
    const CColor& color) const
{
    float width = (quadp->br - quadp->bl).Length();
    float height = (quadp->br - quadp->tr).Length();
    float texWidth = quadt->br.u - quadt->bl.u;
    float texHeight = quadt->br.v - quadt->tr.v;
    CQuadPTC *quadOutter = (CQuadPTC*)&vertices[0];   // 0, 1, 2, 3
    CQuadPTC *quadInner = (CQuadPTC*)&vertices[4];    // 4, 5, 6, 7
    CQuadPTC *quadWider = (CQuadPTC*)&vertices[8];    // 8, 9, 10, 11
    CQuadPTC *quadHigher = (CQuadPTC*)&vertices[12];  // 12, 13, 14, 15

    quadOutter->tl.position = quadp->tl;
    quadOutter->bl.position = quadp->bl;
    quadOutter->tr.position = quadp->tr;
    quadOutter->br.position = quadp->br;
    quadOutter->tl.tex = quadt->tl;
    quadOutter->bl.tex = quadt->bl;
    quadOutter->tr.tex = quadt->tr;
    quadOutter->br.tex = quadt->br;
    quadOutter->tl.color = color;
    quadOutter->bl.color = color;
    quadOutter->tr.color = color;
    quadOutter->br.color = color;

    quadInner->tl.position.x = quadp->tl.x + grid9Offset.x * width;
    quadInner->tl.position.y = quadp->tl.y + grid9Offset.y * height;
    quadInner->br.position.x = quadp->br.x - grid9Offset.x * width;
    quadInner->br.position.y = quadp->br.y - grid9Offset.y * height;
    quadInner->tr.position.x = quadInner->br.position.x;
    quadInner->tr.position.y = quadInner->tl.position.y;
    quadInner->bl.position.x = quadInner->tl.position.x;
    quadInner->bl.position.y = quadInner->br.position.y;
    quadInner->tl.tex.u = quadt->tl.u + 
        0.3f * texWidth;
    quadInner->tl.tex.v = quadt->tl.v + 
        0.3f * texHeight;
    quadInner->br.tex.u = quadt->br.u - 
        0.3f * texWidth;
    quadInner->br.tex.v = quadt->br.v - 
        0.3f * texHeight;
    quadInner->tr.tex.u = quadInner->br.tex.u;
    quadInner->tr.tex.v = quadInner->tl.tex.v;
    quadInner->bl.tex.u = quadInner->tl.tex.u;
    quadInner->bl.tex.v = quadInner->br.tex.v;
    quadInner->tl.color = color;
    quadInner->bl.color = color;
    quadInner->tr.color = color;
    quadInner->br.color = color;

    quadWider->tl.position.x = quadOutter->tl.position.x;
    quadWider->tl.position.y = quadInner->tl.position.y;
    quadWider->bl.position.x = quadOutter->bl.position.x;
    quadWider->bl.position.y = quadInner->bl.position.y;
    quadWider->tr.position.x = quadOutter->tr.position.x;
    quadWider->tr.position.y = quadInner->tr.position.y;
    quadWider->br.position.x = quadOutter->br.position.x;
    quadWider->br.position.y = quadInner->br.position.y;
    quadWider->tl.tex.u = quadOutter->tl.tex.u;
    quadWider->tl.tex.v = quadInner->tl.tex.v;
    quadWider->bl.tex.u = quadOutter->bl.tex.u;
    quadWider->bl.tex.v = quadInner->bl.tex.v;
    quadWider->tr.tex.u = quadOutter->tr.tex.u;
    quadWider->tr.tex.v = quadInner->tr.tex.v;
    quadWider->br.tex.u = quadOutter->br.tex.u;
    quadWider->br.tex.v = quadInner->br.tex.v;
    quadWider->tl.color = color;
    quadWider->bl.color = color;
    quadWider->tr.color = color;
    quadWider->br.color = color;

    quadHigher->tl.position.x = quadInner->tl.position.x;
    quadHigher->tl.position.y = quadOutter->tl.position.y;
    quadHigher->bl.position.x = quadInner->bl.position.x;
    quadHigher->bl.position.y = quadOutter->bl.position.y;
    quadHigher->tr.position.x = quadInner->tr.position.x;
    quadHigher->tr.position.y = quadOutter->tr.position.y;
    quadHigher->br.position.x = quadInner->br.position.x;
    quadHigher->br.position.y = quadOutter->br.position.y;
    quadHigher->tl.tex.u = quadInner->tl.tex.u;
    quadHigher->tl.tex.v = quadOutter->tl.tex.v;
    quadHigher->bl.tex.u = quadInner->bl.tex.u;
    quadHigher->bl.tex.v = quadOutter->bl.tex.v;
    quadHigher->tr.tex.u = quadInner->tr.tex.u;
    quadHigher->tr.tex.v = quadOutter->tr.tex.v;
    quadHigher->br.tex.u = quadInner->br.tex.u;
    quadHigher->br.tex.v = quadOutter->br.tex.v;
    quadHigher->tl.color = color;
    quadHigher->bl.color = color;
    quadHigher->tr.color = color;
    quadHigher->br.color = color;

    static const unsigned short indexArray[] = {
        0, 8, 12,
        12, 8, 4,
        12, 4, 14,
        14, 4, 6,
        14, 6, 2,
        2, 6, 10,
        6, 7, 10,
        10, 7, 11,
        7, 15, 11,
        11, 15, 3,
        5, 13, 7,
        7, 13, 15,
        5, 1, 13,
        9, 1, 5,
        4, 9, 5,
        8, 9, 4,
        4, 5, 6,
        6, 5, 7,
    };
    memcpy(indices, indexArray, sizeof(indices));
}

void CWindowManager::AttachToNode( CControl* pControl )
{
    auto iter = std::find( m_vecControlAddNode.begin(), m_vecControlAddNode.end(), pControl );
    BEATS_ASSERT( iter == m_vecControlAddNode.end() );
    m_vecControlAddNode.push_back( pControl );
}

void CWindowManager::DetachToNode( CControl* pControl )
{
    auto iter = std::find( m_vecControlAddNode.begin(), m_vecControlAddNode.end(), pControl );
    if( iter != m_vecControlAddNode.end() )
    {
        m_vecControlAddNode.erase( iter );
    }
}
