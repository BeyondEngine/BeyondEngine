#include "stdafx.h"
#include "RenderManager.h"
#include "ShaderProgram.h"
#include "external/Configuration.h"
#include "Texture.h"
#include "Resource/ResourceManager.h"
#include "Render/Material.h"
#include "Render/RenderState.h"
#include "Render/Shader.h"
#include "Render/RenderTarget.h"
#include "Renderer.h"
#include "Event/EventType.h"
#include "Event/KeyboardEvent.h"
#include "Render/RenderGroup.h"
#include "RenderBatch.h"
#include "Event/TouchDelegate.h"
#include "VertexBufferContent.h"
#include "CoordinateRenderObject.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"

CRenderManager* CRenderManager::m_pInstance = NULL;

CRenderManager::CRenderManager()
    : m_pCurrRenderTarget(nullptr)
    , m_p2DCamera(new CCamera(CCamera::eCT_2D))
    , m_p3DCamera(new CCamera(CCamera::eCT_3D))
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    , m_uCurrPolygonMode(GL_FILL)
#endif
    , m_pLineBatch(nullptr)
    , m_pTriangleBatch(nullptr)
    , m_pGlobalColorMultiFactor(nullptr)
{
    BEATS_ASSERT(!CApplication::GetInstance()->IsDestructing(), "Should not create singleton when exit the program!");
    m_pGlobalColorMultiFactor = new CShaderUniform(COMMON_UNIFORM_NAMES[UNIFORM_COLOR_SCALE], eSUT_4f);
    m_pGlobalColorMultiFactor->GetData()[0] = 1.0f;
    m_pGlobalColorMultiFactor->GetData()[1] = 1.0f;
    m_pGlobalColorMultiFactor->GetData()[2] = 1.0f;
    m_pGlobalColorMultiFactor->GetData()[3] = 1.0f;
    m_identityMatrix.Identity();
    m_pCoordinateRenderObject = new CCoordinateRenderObject;
}

CRenderManager::~CRenderManager()
{
    BEATS_ASSERT(m_pCoordinateRenderObject == nullptr);
}

void CRenderManager::SetCurrentRenderTarget(CRenderTarget *pRenderTarget)
{
    if(m_pCurrRenderTarget != pRenderTarget)
    {
        m_pCurrRenderTarget = pRenderTarget;
        if(m_pCurrRenderTarget)
        {
            m_pCurrRenderTarget->SetToCurrent();
        }
    }
}

CRenderTarget *CRenderManager::GetCurrentRenderTarget() const
{
    return m_pCurrRenderTarget;
}

bool CRenderManager::Initialize()
{
    m_pCoordinateRenderObject->Initialize();
    // check OpenGL version at first
#ifdef _DEBUG
    const GLubyte* glVersion = glGetString(GL_VERSION);
    BEATS_ASSERT(glVersion != NULL, _T("Get Opengl version failed!"));
#if(GL_ES_VERSION_2_0)
    BEATS_ASSERT(strncmp((const char*)glVersion, "OpenGL ES ", 10) == 0 && atof((const char*)glVersion+10) >= 2.0, _T("OpenGL ES 2.0 or higher is required (your version is %s)."), (const char*)glVersion);
#else
    BEATS_ASSERT(atof((const char*)glVersion) >= 1.5, _T("OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card."), (const char*)glVersion);
#endif  //GL_ES_VERSION_2_0
#endif  //def _DEBUG

    // Configuration. Gather GPU info
    CConfiguration *conf = CConfiguration::GetInstance();
    conf->GatherGPUInfo();
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();

    m_pDefaultMaterial = new CMaterial();
    m_pDefaultMaterial->SetSharders( _T("pointcolorshader.vs"), _T("pointcolorshader.ps"));
    m_pDefaultMaterial->SetDepthTestEnable(true);
    m_pDefaultMaterial->SetBlendEnable(true);
    m_pDefaultMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pDefaultMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pDefaultMaterial->SetBlendEquation(GL_FUNC_ADD);
    m_pDefaultMaterial->SetLineWidth(1.f);
    m_pDefaultMaterial->SetPointSize(5.f);
    m_pDefaultMaterial->SetCullFaceEnable(false);
    m_pDefaultMaterial->Initialize();

    m_pMapGridMaterial = m_pDefaultMaterial->Clone();
    m_pMapGridMaterial->SetLineWidth(2.0f);
    m_pMapGridMaterial->SetDepthTestEnable(false);
    m_pMapGridMaterial->Initialize();

#ifdef DEVELOP_VERSION
    m_pDebugPrimitiveMaterial = m_pDefaultMaterial->Clone();
    m_pDebugPrimitiveMaterial->SetDepthTestEnable(false);
    m_pDebugPrimitiveMaterial->SetBlendEnable(false);
    m_pDebugPrimitiveMaterial->Initialize();
#endif

    m_pSpriteMaterial = new CMaterial();
    m_pSpriteMaterial->SetSharders(_T("spriteshader.vs"), _T("spriteshader.ps"));
    m_pSpriteMaterial->SetCullFaceEnable(false);
    m_pSpriteMaterial->SetDepthTestEnable(false);
    m_pSpriteMaterial->SetDepthMask(false);
    m_pSpriteMaterial->SetBlendEnable(true);
    m_pSpriteMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pSpriteMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pSpriteMaterial->Initialize();

    m_pSpriteBlendAddMaterial = m_pSpriteMaterial->Clone();
    m_pSpriteBlendAddMaterial->SetBlendDest(GL_ONE);
    m_pSpriteBlendAddMaterial->Initialize();

    m_pSpriteMaterialWithOutColor = new CMaterial();
    m_pSpriteMaterialWithOutColor->SetSharders(_T("spriteshaderwithoutcolor.vs"), _T("spriteshaderwithoutcolor.ps"));
    m_pSpriteMaterialWithOutColor->SetCullFaceEnable(false);
    m_pSpriteMaterialWithOutColor->SetDepthTestEnable(false);
    m_pSpriteMaterialWithOutColor->SetDepthMask(false);
    m_pSpriteMaterialWithOutColor->SetBlendEnable(true);
    m_pSpriteMaterialWithOutColor->SetBlendSource(GL_SRC_ALPHA);
    m_pSpriteMaterialWithOutColor->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pSpriteMaterialWithOutColor->Initialize();

    m_pSpriteBlendAddMaterialWithOutColor = m_pSpriteMaterialWithOutColor->Clone();
    m_pSpriteBlendAddMaterialWithOutColor->SetBlendDest(GL_ONE);
    m_pSpriteBlendAddMaterialWithOutColor->Initialize();

    m_pSkinMaterial = new CMaterial();
    m_pSkinMaterial->SetSharders(_T("skinshader.vs"), _T("skinshader.ps"));
    m_pSkinMaterial->SetDepthTestEnable(true);
    m_pSkinMaterial->SetBlendEnable(true);
    m_pDefaultMaterial->SetCullFaceEnable(true);
    m_pSkinMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pSkinMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pSkinMaterial->SetBlendEquation(GL_FUNC_ADD);
    m_pSkinMaterial->Initialize();

    m_pBackGroundMaterial = new CMaterial();
    m_pBackGroundMaterial->SetSharders(_T("pointtexshader.vs"), _T("pointtexshader.ps"));
    m_pBackGroundMaterial->SetDepthTestEnable(false);
    m_pBackGroundMaterial->SetBlendEnable(false);
    m_pBackGroundMaterial->Initialize();

    m_pParticleAlphaBlendMaterial = new CMaterial();
    m_pParticleAlphaBlendMaterial->SetSharders(_T("newparticle.vs"), _T("newparticle.ps"));
    m_pParticleAlphaBlendMaterial->SetDepthTestEnable(true);
    m_pParticleAlphaBlendMaterial->SetDepthMask(false);
    m_pParticleAlphaBlendMaterial->SetBlendEnable(true);
    m_pParticleAlphaBlendMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pParticleAlphaBlendMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pParticleAlphaBlendMaterial->SetBlendEquation(GL_FUNC_ADD);
    m_pParticleAlphaBlendMaterial->SetCullFaceEnable(false);
    m_pParticleAlphaBlendMaterial->Initialize();

    m_pParticleNoDepthAlphaBlendMaterial = m_pParticleAlphaBlendMaterial->Clone();
    m_pParticleNoDepthAlphaBlendMaterial->SetDepthTestEnable(false);
    m_pParticleNoDepthAlphaBlendMaterial->Initialize();

    m_pParticleAdditiveMaterial = m_pParticleAlphaBlendMaterial->Clone();
    m_pParticleAdditiveMaterial->SetBlendDest(GL_ONE);
    m_pParticleAdditiveMaterial->Initialize();

    m_pParticleNoDepthAdditiveMaterial = m_pParticleAdditiveMaterial->Clone();
    m_pParticleNoDepthAdditiveMaterial->SetDepthTestEnable(false);
    m_pParticleNoDepthAdditiveMaterial->Initialize();

    m_pAttackRangeMaterial = new CMaterial;
    m_pAttackRangeMaterial->SetSharders(_T("attackrange.vs"), _T("attackrange.ps"));
    m_pAttackRangeMaterial->SetCullFaceEnable(false);
    m_pAttackRangeMaterial->SetDepthTestEnable(false);
    m_pAttackRangeMaterial->SetBlendEnable(true);
    m_pAttackRangeMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pAttackRangeMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);

    m_pFreetypeFontMaterial = new CMaterial();
    m_pFreetypeFontMaterial->SetSharders(_T("textshader.vs"), _T("textshader.ps"));
    m_pFreetypeFontMaterial->SetDepthTestEnable(false);
    m_pFreetypeFontMaterial->SetBlendEnable(true);
    m_pFreetypeFontMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pFreetypeFontMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pFreetypeFontMaterial->Initialize();

    m_pBitmapFontMaterial = new CMaterial();
    m_pBitmapFontMaterial->SetSharders(_T("guishader.vs"), _T("guishader.ps"));
    m_pBitmapFontMaterial->SetDepthTestEnable(false);
    m_pBitmapFontMaterial->SetCullFaceEnable(false);
    m_pBitmapFontMaterial->SetBlendEnable(true);
    m_pBitmapFontMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pBitmapFontMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pBitmapFontMaterial->Initialize();

    m_pMeshMaterial = new CMaterial();
    m_pMeshMaterial->SetSharders(_T("pointtexshader.vs"), _T("pointtexshader.ps"));
    m_pMeshMaterial->SetBlendEnable(false);
    m_pMeshMaterial->SetDepthTestEnable(true);
    m_pMeshMaterial->SetCullFaceEnable(true);
    m_pMeshMaterial->SetBlendEquation(GL_FUNC_ADD);
    m_pMeshMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pMeshMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pMeshMaterial->Initialize();

    m_pMeshMaterialBlend = m_pMeshMaterial->Clone();
    m_pMeshMaterialBlend->SetBlendEnable(true);
    m_pMeshMaterialBlend->Initialize();

    m_pMeshMaterialBlendAdd = m_pMeshMaterialBlend->Clone();
    m_pMeshMaterialBlendAdd->SetBlendDest(GL_ONE);
    m_pMeshMaterialBlendAdd->Initialize();

    m_pMeshLightMapMaterial = m_pMeshMaterial->Clone();
    m_pMeshLightMapMaterial->SetSharders(_T("pointtexshaderlightmap.vs"), _T("pointtexshaderlightmap.ps"));
    m_pMeshLightMapMaterial->Initialize();

    m_pMeshLightMapMaterialBlend = m_pMeshLightMapMaterial->Clone();
    m_pMeshLightMapMaterialBlend->SetCullFaceEnable(true);
    m_pMeshLightMapMaterialBlend->SetBlendEnable(true);
    m_pMeshLightMapMaterialBlend->Initialize();

    m_pWaterMaterial = new CMaterial();
    m_pWaterMaterial->SetSharders(_T("water.vs"), _T("water.ps"));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_time"), eSUT_1f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_1DivLevelWidth"), eSUT_1f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_1DivLevelHeight"), eSUT_1f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_Wave_height"), eSUT_1f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_Wave_movement"), eSUT_1f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_Shore_Dark"), eSUT_3f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_Shore_Light"), eSUT_3f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_Sea_Dark"), eSUT_3f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_Sea_Light"), eSUT_3f));
    m_pWaterMaterial->AddUniform(new CShaderUniform(_T("u_lightPos"), eSUT_3f));
    m_pWaterMaterial->SetBlendEnable(true);
    m_pWaterMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pWaterMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pWaterMaterial->SetBlendEquation(GL_FUNC_ADD);
    m_pWaterMaterial->Initialize();

    CShaderUniform* pAttackRangeColorUniform = new CShaderUniform(_T("u_color"), eSUT_4f);
    std::vector<float>& colorData = pAttackRangeColorUniform->GetData();
    BEATS_ASSERT(colorData.size() == 4);
    for (uint32_t i = 0; i < colorData.size(); ++i)
    {
        colorData[i] = 1.0f;
    }
    m_pAttackRangeMaterial->AddUniform(pAttackRangeColorUniform);

    CShaderUniform* pAttackRangeUniform = new CShaderUniform(_T("u_minRangeRate"), eSUT_1f);
    m_pAttackRangeMaterial->AddUniform(pAttackRangeUniform);
    m_pAttackRangeMaterial->Initialize();


    // Init Default uniform value
    CShaderUniform colorScaleUniform(COMMON_UNIFORM_NAMES[UNIFORM_COLOR_SCALE], eSUT_4f);
    colorScaleUniform.GetData()[0] = 1.0f;
    colorScaleUniform.GetData()[1] = 1.0f;
    colorScaleUniform.GetData()[2] = 1.0f;
    colorScaleUniform.GetData()[3] = 1.0f;
    m_defaultUniformMap[COMMON_UNIFORM_NAMES[UNIFORM_COLOR_SCALE]] = colorScaleUniform;
#ifdef DEVELOP_VERSION
    m_pTempTexture = CResourceManager::GetInstance()->GetResource<CTexture>("temp_texture.png", false);
#endif
    bool bRet = InitLineBuffer() && InitTriangleBuffer();
    BEATS_ASSERT(bRet, _T("Initialize render manager failed!"));
#ifndef DISABLE_RENDER
    SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("fullscreen.vs"));
    SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("fullscreen.ps"));
    BEATS_ASSERT(pVS && pPS, _T("Load full screen Shader Failed!"));
    m_pFullScreenProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
    BEATS_ASSERT(m_pFullScreenProgram != nullptr);
#endif
    return bRet;
}

void CRenderManager::Uninitialize()
{
    BEATS_SAFE_DELETE_COMPONENT(m_pCoordinateRenderObject);
    for (auto iter = m_vertexMap.begin(); iter != m_vertexMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            BEATS_SAFE_DELETE(subIter->second);
        }
    }
    m_vertexMap.clear();
    BEATS_SAFE_DELETE_VECTOR(m_shaderProgramPool);
    BEATS_SAFE_DELETE(m_pLineBatch);
    BEATS_SAFE_DELETE(m_pTriangleBatch);
    BEATS_SAFE_DELETE(m_pGlobalColorMultiFactor);
    BEATS_SAFE_DELETE(m_p2DCamera);
    BEATS_SAFE_DELETE(m_p3DCamera);
#ifdef DEVELOP_VERSION
    BEATS_SAFE_DELETE(m_pDebugTriangleBatch);
    BEATS_SAFE_DELETE(m_pDebugLineBatch);
#endif
}

void CRenderManager::Render()
{
    BEYONDENGINE_PERFORMDETECT_START(EPNT_SyncVertexDataBuffer)
    CRenderGroupManager::GetInstance()->SyncData();
#ifdef DEVELOP_VERSION
    m_pDebugTriangleBatch->SyncData();
    m_pDebugLineBatch->SyncData();
#endif
    m_pLineBatch->SyncData();
    m_pTriangleBatch->SyncData();

    CRenderer* pRenderer = CRenderer::GetInstance();
    // update vertex buffer.
    for (auto iter = m_vertexMap.begin(); iter != m_vertexMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            CVertexBufferContent* pBufferContent = subIter->second;
            BEATS_ASSERT(pBufferContent != NULL);
            if (pBufferContent->GetVertexBuffer()->GetWritePos() > 0)
            {
                if (pBufferContent->GetVBO() == 0)
                {
                    uint32_t uVBO = 0;
                    pRenderer->GenBuffers(1, &uVBO);
                    pBufferContent->SetVBO(uVBO);
                }
                if (pBufferContent->GetEBO() == 0)
                {
                    uint32_t uEBO = 0;
                    pRenderer->GenBuffers(1, &uEBO);
                    pBufferContent->SetEBO(uEBO);
                }
                if (CConfiguration::GetInstance()->SupportsShareableVAO())
                {
                    if (pBufferContent->GetArrayVAO() == 0)
                    {
                        uint32_t uArrayVAO = 0;
                        pRenderer->GenVertexArrays(1, &uArrayVAO);
                        pBufferContent->SetArrayVAO(uArrayVAO);
                        pRenderer->BindVertexArray(uArrayVAO);
                        const CVertexFormat* pVertexFormat = iter->first;
                        pVertexFormat->SetupAttribPointer(pBufferContent->GetVBO());
                        pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                        pRenderer->BindVertexArray(0);
                        pVertexFormat->DisableAttribPointer();
                        pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                        pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
                    }
                    if (pBufferContent->GetIndexVAO() == 0)
                    {
                        uint32_t uIndexVAO = 0;
                        pRenderer->GenVertexArrays(1, &uIndexVAO);
                        pBufferContent->SetIndexVAO(uIndexVAO);
                        pRenderer->BindVertexArray(uIndexVAO);
                        const CVertexFormat* pVertexFormat = iter->first;
                        pVertexFormat->SetupAttribPointer(pBufferContent->GetVBO());
                        pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBufferContent->GetEBO());
                        pRenderer->BindVertexArray(0);
                        pVertexFormat->DisableAttribPointer();
                        pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                        pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
                    }
                }
                pBufferContent->SyncData();
            }
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(EPNT_SyncVertexDataBuffer)

    //render line and triangle
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderLineAndTriangle)
    RenderLineImpl();
    RenderTriangleImpl();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderLineAndTriangle)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_GroupRender)
    CRenderGroupManager::GetInstance()->Render();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_GroupRender)
    Reset();
}

void CRenderManager::Reset()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderClear)
        m_pLineBatch->Clear();
    m_pTriangleBatch->Clear();
    CRenderGroupManager::GetInstance()->Clear();
    for (auto iter = m_vertexMap.begin(); iter != m_vertexMap.end(); ++iter)
    {
        for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
        {
            subIter->second->Reset();
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderClear)
}

CShaderProgram* CRenderManager::GetShaderProgram(GLuint uVertexShader, GLuint uPixelShader)
{
    CShaderProgram* pRet = NULL;
    for (uint32_t i = 0; i < m_shaderProgramPool.size(); ++i)
    {
        CShaderProgram* pProgram = m_shaderProgramPool[i];
        if (pProgram->GetPixelShader() == uPixelShader && pProgram->GetVertexShader() == uVertexShader)
        {
            pRet = pProgram;
            break;
        }
    }
    if (pRet == NULL)
    {
        pRet = new CShaderProgram(uVertexShader, uPixelShader);
        m_shaderProgramPool.push_back(pRet);
    }
    return pRet;
}

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
void CRenderManager::SwitchPolygonMode()
{
    if (m_uCurrPolygonMode == GL_FILL)
    {
        m_uCurrPolygonMode = GL_POINT;
    }
    else
    {
        ++m_uCurrPolygonMode;
    }
    CRenderer::GetInstance()->PolygonMode(m_uCurrPolygonMode, m_uCurrPolygonMode);
}

bool CRenderManager::SaveTextureToFile(GLuint uTextureId, const TCHAR* pszFilePath, bool bFlipY /* = false*/, int nPixelFormat/* = GL_RGBA*/)
{
    bool bRet = false;
    GLint nCurrentBindingTexture = 0;
    CRenderer::GetInstance()->GetIntegerV(GL_TEXTURE_BINDING_2D, &nCurrentBindingTexture);
    CRenderer::GetInstance()->BindTexture(GL_TEXTURE_2D, uTextureId);
    BEATS_ASSERT(uTextureId > 0, _T("Invalid texture id %d in SaveTextureToFile"), uTextureId);
    GLint nWidth, nHeight;
    // GL_TEXTURE_WIDTH and GL_TEXTURE_HEIGHT are not supported in opengl ES.
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &nWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &nHeight);
    if (nWidth > 0 && nHeight > 0)
    {
        unsigned char* pBuffer = new unsigned char[nWidth * nHeight * 4];
        if (nPixelFormat == GL_RGBA)
        {
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
        }
        else if (nPixelFormat == GL_LUMINANCE_ALPHA)
        {
            unsigned char* pRawBuffer = new unsigned char[nWidth * nHeight * 2];
            glGetTexImage(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pRawBuffer);
            // Fill as RGBA buffer.
            for (int i = 0; i < nWidth; ++i)
            {
                for (int j = 0; j < nHeight; ++j)
                {
                    int nCurPos = (j + i * nWidth) * 4;
                    unsigned char borderAlpha = pRawBuffer[nCurPos / 2];
                    unsigned char fontAlpha = pRawBuffer[nCurPos / 2 + 1];
                    pBuffer[nCurPos] = fontAlpha;
                    pBuffer[nCurPos + 1] = fontAlpha;
                    pBuffer[nCurPos + 2] = fontAlpha;
                    pBuffer[nCurPos + 3] = fontAlpha * borderAlpha > 0 ? 255 : fontAlpha + borderAlpha;
                }
            }
            BEATS_SAFE_DELETE_ARRAY(pRawBuffer);
        }
        else
        {
            BEATS_ASSERT(false, _T("Unsupport format %d can't be saved as file."), nPixelFormat);
        }
        //Flip Y because sometimes the opengl uv coordinate is inverse to most picture coordinate.
        if (bFlipY)
        {
            for (int j = 0; j < nWidth; ++j)
            {
                for (int i = 0; i < nHeight / 2; ++i)
                {
                    int nCurPos = (j + i * nWidth) * 4;
                    int nExchangePos = (j + (nHeight - i - 1) * nWidth) * 4;
                    for (int k = 0; k < 4; ++k)
                    {
                        char tmp = pBuffer[nCurPos + k];
                        pBuffer[nCurPos + k] = pBuffer[nExchangePos + k];
                        pBuffer[nExchangePos + k] = tmp;
                    }
                }
            }
        }
        CImage image;
        image.InitWithRawData(pBuffer, nWidth * nHeight * 4, nWidth, nHeight, 4, pszFilePath);
        bRet = image.SaveToFile(pszFilePath);
        BEATS_ASSERT(bRet, _T("Save texture to path %s failed! Note only *.png and *.jpg are support!"), pszFilePath);
        BEATS_SAFE_DELETE_ARRAY(pBuffer);
    }
    CRenderer::GetInstance()->BindTexture(GL_TEXTURE_2D, nCurrentBindingTexture);
    return bRet;
}
#endif

#ifdef DEVELOP_VERSION
void CRenderManager::GetSimulateSize(uint32_t& uWidth, uint32_t& uHeight) const
{
    uWidth = m_uSimulateWidth;
    uHeight = m_uSimulateHeight;
}

void CRenderManager::SetSimulateSize(uint32_t uWidth, uint32_t uHeight)
{
    m_uSimulateWidth = uWidth;
    m_uSimulateHeight = uHeight;
}
#endif

void CRenderManager::SendMVPToShader(CCamera::ECameraType type, const CMat4* pWorldMat, const CMat4* pViewMat, const CMat4* pProjectionMat)
{
    CRenderer *pRenderer = CRenderer::GetInstance();
    GLuint currShaderProgram = pRenderer->GetCurrentState()->GetShaderProgram();
    if (currShaderProgram != 0)
    {
        CCamera* pCamera = type == CCamera::eCT_2D ? m_p2DCamera : m_p3DCamera;
        if (pWorldMat == nullptr)
        {
            pWorldMat = &m_identityMatrix;
        }
#ifdef ENABLE_SINGLE_MVP_UNIFORM
        if (pViewMat == nullptr)
        {
            pViewMat = &pCamera->GetViewMatrix();
        }
        if (pProjectionMat == nullptr)
        {
            pProjectionMat = &pCamera->GetProjectionMatrix();
        }
#else
        BEYONDENGINE_UNUSED_PARAM(pProjectionMat);
        BEYONDENGINE_UNUSED_PARAM(pViewMat);
#endif
        CMat4 MVPmat;
        MVPmat = pCamera->GetViewProjectionMatrix(true) * (*pWorldMat);
        GLuint location = GetUniformLocation(currShaderProgram, COMMON_UNIFORM_NAMES[UNIFORM_MVP_MATRIX]);
        pRenderer->SetUniformMatrix4fv(location, MVPmat.Mat4ValuePtr(), 1);
#ifdef ENABLE_SINGLE_MVP_UNIFORM
        location = GetUniformLocation(currShaderProgram, COMMON_UNIFORM_NAMES[UNIFORM_M_MATRIX]);
        pRenderer->SetUniformMatrix4fv(location, pWorldMat->Mat4ValuePtr(), 1);
        location = GetUniformLocation(currShaderProgram, COMMON_UNIFORM_NAMES[UNIFORM_V_MATRIX]);
        pRenderer->SetUniformMatrix4fv(location, pViewMat->Mat4ValuePtr(), 1);
        location = GetUniformLocation(currShaderProgram, COMMON_UNIFORM_NAMES[UNIFORM_P_MATRIX]);
        pRenderer->SetUniformMatrix4fv(location, pProjectionMat->Mat4ValuePtr(), 1);
#endif
    }
}

bool CRenderManager::InitLineBuffer()
{
    m_pLineBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_LINES, true);
#ifdef DEVELOP_VERSION
    m_pDebugLineBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDebugPrimitiveMaterial, GL_LINES, true);
#endif
    return true;
}

bool CRenderManager::InitTriangleBuffer()
{
    m_pTriangleBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_TRIANGLES, true);
#ifdef DEVELOP_VERSION
    m_pDebugTriangleBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDebugPrimitiveMaterial, GL_TRIANGLES, true);
#endif
    return true;
}

void CRenderManager::RenderLineImpl()
{
    if(m_pLineBatch->GetDataSize() > 0)
    {
        CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_3D);
        pRenderGroup->AddRenderBatch(m_pLineBatch);
    }
#ifdef DEVELOP_VERSION
    if (m_pDebugLineBatch->GetDataSize() > 0)
    {
        CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_3D_DEBUG_PRIMITIVE);
        pRenderGroup->AddRenderBatch(m_pDebugLineBatch);
    }
#endif
}

void CRenderManager::RenderTriangleImpl()
{
    if (m_pTriangleBatch->GetDataSize() > 0)
    {
        CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_3D);
        pRenderGroup->AddRenderBatch(m_pTriangleBatch);
    }
#ifdef DEVELOP_VERSION
    if (m_pDebugTriangleBatch->GetDataSize() > 0)
    {
        CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_3D_DEBUG_PRIMITIVE);
        pRenderGroup->AddRenderBatch(m_pDebugTriangleBatch);
    }
#endif
}

void CRenderManager::RenderLine(const CVertexPC& start, const CVertexPC& end, float lineWidth, bool bDebug) const
{
    if (start.position != end.position)
    {
        static std::vector<CVertexPC> vertexData;
        if (vertexData.size() == 0)
        {
            vertexData.resize(2);
        }
        vertexData[0] = start;
        vertexData[1] = end;
        static std::vector<unsigned short> indicesData;
        if (indicesData.size() == 0)
        {
            indicesData.push_back(0);
            indicesData.push_back(1);
        }
        RenderLines(vertexData, indicesData, lineWidth, bDebug);
    }
}

void CRenderManager::RenderLines(const std::vector<CVertexPC>& points, const std::vector<unsigned short>& indices, float lineWidth, bool bDebug) const
{
#ifdef DEVELOP_VERSION
    if (bDebug)
    {
        if (m_pDebugPrimitiveMaterial->GetLineWidth() != lineWidth)
        {
            m_pDebugPrimitiveMaterial->SetLineWidth(lineWidth);
        }
        m_pDebugLineBatch->AddIndexedVertices(points.data(), points.size(), indices.data(), indices.size());
    }
    else
    {
#else
    BEYONDENGINE_UNUSED_PARAM(bDebug);
#endif
        if (m_pDefaultMaterial->GetLineWidth() != lineWidth)
        {
            m_pDefaultMaterial->SetLineWidth(lineWidth);
        }
        m_pLineBatch->AddIndexedVertices(points.data(), points.size(), indices.data(), indices.size());
#ifdef DEVELOP_VERSION
    }
#endif
}

void CRenderManager::Render2DLine(const CVertex2DPC& start, const CVertex2DPC& end)
{
    CVertexPC vertices[2];
    vertices[0].position.X() = start.position.X();
    vertices[0].position.Y() = start.position.Y();
    vertices[0].color = start.color;
    vertices[1].position.X() = end.position.X();
    vertices[1].position.Y() = end.position.Y();
    vertices[1].color = end.color;
    CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_2D)->GetRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_LINES, false)->AddVertices(vertices, 2);
}

void CRenderManager::Render2DLine(const CVertexPC& start, const CVertexPC& end)
{
    CRenderBatch *pBatch = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_2D)->GetRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_LINES, false);
    pBatch->AddVertices(&start, 1);
    pBatch->AddVertices(&end, 1);
}

void CRenderManager::RenderTriangle(const CVertexPC& pt1, const CVertexPC& pt2, const CVertexPC& pt3, bool bDebug)
{
    static std::vector<CVertexPC> vertexData;
    if (vertexData.size() == 0)
    {
        vertexData.resize(3);
    }
    vertexData[0] = pt1;
    vertexData[1] = pt2;
    vertexData[2] = pt3;
    static std::vector<unsigned short> indicesData;
    if (indicesData.size() == 0)
    {
        indicesData.push_back(0);
        indicesData.push_back(1);
        indicesData.push_back(2);
    }
#ifdef DEVELOP_VERSION
    if (bDebug)
    {
        m_pDebugTriangleBatch->AddIndexedVertices(vertexData.data(), vertexData.size(), indicesData.data(), indicesData.size());
    }
    else
    {
#else
    BEYONDENGINE_UNUSED_PARAM(bDebug);
#endif
        m_pTriangleBatch->AddIndexedVertices(vertexData.data(), vertexData.size(), indicesData.data(), indicesData.size());
#ifdef DEVELOP_VERSION
    }
#endif
}

void CRenderManager::RenderTriangle(const std::vector<CVertexPC>& pt, const std::vector<unsigned short>& indices, bool bDebug)
{
#ifdef DEVELOP_VERSION
    if (bDebug)
    {
        m_pDebugTriangleBatch->AddIndexedVertices(pt.data(), pt.size(), indices.data(), indices.size(), nullptr);
    }
    else
    {
#else
    BEYONDENGINE_UNUSED_PARAM(bDebug);
#endif
        m_pTriangleBatch->AddIndexedVertices(pt.data(), pt.size(), indices.data(), indices.size(), nullptr);
#ifdef DEVELOP_VERSION
    }
#endif
}

void CRenderManager::Render2DTriangle(const CVertex2DPC& pt1, const CVertex2DPC& pt2, const CVertex2DPC& pt3)
{
    CVertexPC vertices[3];
    vertices[0].position.X() = pt1.position.X();
    vertices[0].position.Y() = pt1.position.Y();
    vertices[0].color = pt1.color;
    vertices[1].position.X() = pt2.position.X();
    vertices[1].position.Y() = pt2.position.Y();
    vertices[1].color = pt2.color;
    vertices[2].position.X() = pt3.position.X();
    vertices[2].position.Y() = pt3.position.Y();
    vertices[2].color = pt3.color;

    CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_2D)->GetRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_TRIANGLES, false)->AddVertices(vertices, 3);
}

void CRenderManager::RenderPoints(const CVertexPC& pt, float fPointSize, bool bDebug/* = false*/)
{
    CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup();
    SharePtr<CMaterial> pMat = m_pDefaultMaterial;
#ifdef DEVELOP_VERSION
    if (bDebug)
    {
        pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_3D_DEBUG_PRIMITIVE);
        pMat = m_pDebugPrimitiveMaterial;
    }
#endif
    pMat->SetPointSize(fPointSize);
    CRenderBatch* pBatch = pRenderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPC), pMat, GL_POINTS, false);
    pBatch->AddVertices((const CVertexPC *)&pt, 1);
}

CCamera* CRenderManager::GetCamera(CCamera::ECameraType type) const
{
    return type == CCamera::eCT_2D ? m_p2DCamera : m_p3DCamera;
}

void CRenderManager::RenderTextureToFullScreen(GLuint texture)
{
    BEATS_ASSERT(texture != 0);
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->ScissorTest(false);
    pRenderer->DisableGL(CBoolRenderStateParam::eBSP_Blend);
    pRenderer->DisableGL(CBoolRenderStateParam::eBSP_CullFace);
    pRenderer->DepthMask(true);
    pRenderer->ClearColor(1.0f, 1.0f, 0, 1.0f);
    pRenderer->ClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    pRenderer->DepthFunc(CFunctionRenderStateParam::eFT_LESS);
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    CPolygonModeRenderStateParam::EPolygonModeType restoreFrontPolygonMode;
    CPolygonModeRenderStateParam::EPolygonModeType restoreBackPolygonMode;
    pRenderer->GetCurrentState()->GetPolygonMode(restoreFrontPolygonMode, restoreBackPolygonMode);
    pRenderer->PolygonMode(GL_FILL, GL_FILL);
#endif
    pRenderer->UseProgram( m_pFullScreenProgram->ID() );
    CRenderManager::GetInstance()->InitDefaultShaderUniform();
    pRenderer->ActiveTexture(GL_TEXTURE0);
    pRenderer->BindTexture( GL_TEXTURE_2D, texture );
    GLint textureLocation = GetUniformLocation(m_pFullScreenProgram->ID(), COMMON_UNIFORM_NAMES[UNIFORM_TEX0]);
    BEATS_ASSERT(textureLocation != -1);
    pRenderer->SetUniform1i( textureLocation, 0);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    pRenderer->BindVertexArray(0);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_POSITION);
    static float fullScreenQuadVB[8] =
    {
        -1.0F, -1.0F,
        -1.0F, +1.0F,
        +1.0F, +1.0F,
        +1.0F, -1.0F
    };
    static unsigned short fullScreenQuadIB[6] = { 2, 1, 0, 3, 2, 0 };
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_POSITION, 2, GL_FLOAT, GL_TRUE, 0, (const GLvoid *)fullScreenQuadVB);
    m_pGlobalColorMultiFactor->SendUniform();
    m_fGlobalColorFactor = m_pGlobalColorMultiFactor->GetData()[0]; //After we draw, sync the global factor for logic update.
    pRenderer->DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, fullScreenQuadIB);
    pRenderer->DisableVertexAttribArray(ATTRIB_INDEX_POSITION);
    m_pLastApplyMaterial = nullptr;// Because we have changed the opengl rendering context.
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    pRenderer->PolygonMode(restoreFrontPolygonMode, restoreBackPolygonMode);
#endif
}

uint32_t CRenderManager::GetDesignWidth()
{
    return DESIGN_WIDTH;
}

uint32_t CRenderManager::GetDesignHeight()
{
    return DESIGN_HEIGHT;
}

SharePtr<CMaterial> CRenderManager::GetDefaultMaterial() const
{
    return m_pDefaultMaterial;
}

SharePtr<CMaterial> CRenderManager::GetSpriteMaterial(bool bBlendAdd, bool bUseColorData) const
{
    SharePtr<CMaterial> pRet = nullptr;
    if (bUseColorData)
    {
        pRet = bBlendAdd ? m_pSpriteBlendAddMaterial : m_pSpriteMaterial;
    }
    else
    {
        pRet = bBlendAdd ? m_pSpriteBlendAddMaterialWithOutColor : m_pSpriteMaterialWithOutColor;
    }
    return pRet;
}

SharePtr<CMaterial> CRenderManager::GetSkinMaterial() const
{
    return m_pSkinMaterial;
}

SharePtr<CMaterial> CRenderManager::GetBackGroundMaterial() const
{
    return m_pBackGroundMaterial;
}

SharePtr<CMaterial> CRenderManager::GetParticleAlphaBlendMaterial(bool bDepthEnable) const
{
    return bDepthEnable ? m_pParticleAlphaBlendMaterial : m_pParticleNoDepthAlphaBlendMaterial;
}

SharePtr<CMaterial> CRenderManager::GetParticleAdditiveMaterial(bool bDepthEnable) const
{
    return bDepthEnable ? m_pParticleAdditiveMaterial : m_pParticleNoDepthAdditiveMaterial;
}

SharePtr<CMaterial> CRenderManager::GetAttackRangeMaterial() const
{
    return m_pAttackRangeMaterial;
}

SharePtr<CMaterial> CRenderManager::GetMapGridMaterial() const
{
    return m_pMapGridMaterial;
}

SharePtr<CMaterial> CRenderManager::GetFreeTypeFontMaterial() const
{
    BEATS_ASSERT(m_pFreetypeFontMaterial != nullptr);
    return m_pFreetypeFontMaterial;
}

SharePtr<CMaterial> CRenderManager::GetImageMaterial() const
{
    BEATS_ASSERT(m_pBitmapFontMaterial != nullptr);
    return m_pBitmapFontMaterial;
}

SharePtr<CMaterial> CRenderManager::GetMeshMaterial(bool bBlend, bool bBlendAdd) const
{
    if (bBlend)
    {
        return bBlendAdd ? m_pMeshMaterialBlendAdd : m_pMeshMaterialBlend;
    }
    return m_pMeshMaterial;
}

SharePtr<CMaterial> CRenderManager::GetMeshLightMapMaterial(bool bBlend) const
{
    return bBlend ? m_pMeshLightMapMaterialBlend : m_pMeshLightMapMaterial;
}

SharePtr<CMaterial> CRenderManager::GetWaterMaterial() const
{
    return m_pWaterMaterial;
}

SharePtr<CMaterial> CRenderManager::GetLastApplyMaterial() const
{
    return m_pLastApplyMaterial;
}

void CRenderManager::SetLastApplyMaterial(SharePtr<CMaterial> pMaterial)
{
    m_pLastApplyMaterial = pMaterial;
}

void CRenderManager::ApplyTextureMap(const std::map<unsigned char, SharePtr<CTexture> >& textureMap, bool bClampToEdgeOrRepeat)
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    bool bIsEtc = false;
    GLuint currProgram = 0;
    for (auto iter = textureMap.begin(); iter != textureMap.end(); ++iter)
    {
        unsigned char uChannel = iter->first;
        bool bCurrIsEtc = iter->second->GetFileFormat() == CImage::eF_ETC;
        if (bCurrIsEtc)
        {
            bIsEtc = true;
            uChannel *= 2;
        }
        else
        {
            BEATS_ASSERT(!bIsEtc, "in the texture map, all is etc or none is etc.");
        }
        const SharePtr<CTexture>& texture = iter->second;
        pRenderer->ActiveTexture(GL_TEXTURE0 + uChannel);
        pRenderer->BindTexture(GL_TEXTURE_2D, texture->ID());
        pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, bClampToEdgeOrRepeat ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, bClampToEdgeOrRepeat ? GL_CLAMP_TO_EDGE : GL_REPEAT);
#ifdef DEVELOP_VERSION
        if (iter->second->ID() == CRenderManager::GetInstance()->GetTempTexture()->ID())
        {
            pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
#endif
        if (bCurrIsEtc)
        {
            if (texture->GetAlphaTexture() != nullptr)
            {
                pRenderer->ActiveTexture(GL_TEXTURE0 + uChannel + 1);
                pRenderer->BindTexture(GL_TEXTURE_2D, texture->GetAlphaTexture()->ID());
                pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, bClampToEdgeOrRepeat ? GL_CLAMP_TO_EDGE : GL_REPEAT);
                pRenderer->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, bClampToEdgeOrRepeat ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            }
        }
        BEATS_ASSERT(bClampToEdgeOrRepeat || (IsPowerOf2(texture->Width()) && IsPowerOf2(texture->Height())),
            _T("Set GL_REPEAT to a non-POT texture may result black in OPENGLES, for details please check glTexParameteri of OPENGLES.\nTexture:%s width %d height %d"), texture->GetFilePath().c_str(), texture->Width(), texture->Height());
        currProgram = pRenderer->GetCurrentState()->GetShaderProgram();
        if (0 != currProgram)
        {
            GLint textureLocation = GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_TEX0 + uChannel]);
            pRenderer->SetUniform1i(textureLocation, uChannel);
            if (bCurrIsEtc)
            {
                // Only the first texture(also the color source) decide if this draw call has alpha channel for etc.
                // To avoid those batch with shadow map: the shadow map doesn't contain alpha, but the color contains.
                // So, we only decide has alpha from the color source.
                if (uChannel == 0)
                {
                    GLint hasAlphaLocation = GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_ETC_HAS_ALPHA]);
                    pRenderer->SetUniform1i(hasAlphaLocation, texture->GetAlphaTexture() != nullptr ? 1 : 0);

                    if (texture->GetAlphaTexture() != nullptr)
                    {
                        GLint alphaTextureLocation = GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_TEX0 + uChannel + 1]);
                        pRenderer->SetUniform1i(alphaTextureLocation, uChannel + 1);
                    }
                }
            }
        }
    }
    if (0 != currProgram)
    {
        GLint etcFlagLocation = GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_ISETC]);
        pRenderer->SetUniform1i(etcFlagLocation, bIsEtc ? 1 : 0);
    }
}

CVertexBufferContent* CRenderManager::GetVertexContent(const CVertexFormat* pFormat, GLenum primitiveType)
{
    CVertexBufferContent* pRet = nullptr;
    auto iter = m_vertexMap.find(pFormat);
    if (iter != m_vertexMap.end())
    {
        auto subIter = iter->second.find(primitiveType);
        if (subIter != iter->second.end())
        {
            pRet = subIter->second;
        }
    }
    if (pRet == NULL)
    {
        pRet = new CVertexBufferContent;
        m_vertexMap[pFormat][primitiveType] = pRet;
    }
    return pRet;
}

void CRenderManager::SetGlobalColorFactor(float fFactor)
{
    CRenderTarget* pCurrentTarget = GetCurrentRenderTarget();
    BEATS_ASSERT(pCurrentTarget);
    // If target is not rendering, we manually set the fFactor to m_fGlobalColorFactor
    if (pCurrentTarget->GetRenderSwitcher())
    {
        BEATS_ASSERT(!std::isnan(fFactor) && !std::isinf(fFactor) && fFactor >= 0);
        m_pGlobalColorMultiFactor->GetData()[0] = fFactor;
        m_pGlobalColorMultiFactor->GetData()[1] = fFactor;
        m_pGlobalColorMultiFactor->GetData()[2] = fFactor;
        m_pGlobalColorMultiFactor->GetData()[3] = 1.0f;
    }
    else
    {
        m_fGlobalColorFactor = fFactor;
    }
}

float CRenderManager::GetGlobalColorFactor() const
{
    BEATS_ASSERT(!std::isnan(m_fGlobalColorFactor) && !std::isinf(m_fGlobalColorFactor) && BEATS_FLOAT_GREATER_EQUAL(m_fGlobalColorFactor, 0));
    return m_fGlobalColorFactor;
}

CCoordinateRenderObject* CRenderManager::GetCoordinateRenderObject() const
{
    return m_pCoordinateRenderObject;
}

const std::map<TString, CShaderUniform>& CRenderManager::GetDefaultShaderUniformMap() const
{
    return m_defaultUniformMap;
}

void CRenderManager::InitDefaultShaderUniform()
{
    // Restore the uniform value for render environment
    const std::map<TString, CShaderUniform>& defaultUniformMap = CRenderManager::GetInstance()->GetDefaultShaderUniformMap();
    for (auto iter = defaultUniformMap.begin(); iter != defaultUniformMap.end(); ++iter)
    {
        iter->second.SendUniform();
    }
}

uint32_t CRenderManager::GetUniformLocation(uint32_t uProgramId, const TString& strName)
{
    bool bFind = false;
    uint32_t uRet = 0xFFFFFFFF;
    auto iter = m_uniformLocationCache.find(uProgramId);
    if (iter != m_uniformLocationCache.end())
    {
        auto subIter = iter->second.find(strName);
        if (subIter != iter->second.end())
        {
            uRet = subIter->second;
            bFind = true;
        }
    }
    if (!bFind)
    {
        uRet = CRenderer::GetInstance()->GetUniformLocation(uProgramId, strName.c_str());
        m_uniformLocationCache[uProgramId][strName] = uRet;
    }
    return uRet;
}

#ifdef DEVELOP_VERSION
SharePtr<CTexture> CRenderManager::GetTempTexture()
{
    return m_pTempTexture;
}
#endif
