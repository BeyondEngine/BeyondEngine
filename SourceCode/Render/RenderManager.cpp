#include "stdafx.h"
#include "RenderManager.h"
#include "ShaderProgram.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/PerformDetector/PerformDetector.h"
#include "external/Configuration.h"
#include "Camera.h"
#include "Texture.h"
#include "Resource/ResourceManager.h"
#include "Spline/Curve.h"
#include "Render/Material.h"
#include "Render/RenderState.h"
#include "Render/Shader.h"
#include "Render/RenderTarget.h"
#include "Render/RenderWindow.h"
#include "renderer.h"
#include "Event/EventType.h"
#include "Event/KeyboardEvent.h"


#include "RenderGroupManager.h"
#include "Render/RenderGroup.h"
#include "RenderBatch.h"

#include "Scene/SceneManager.h"

#include "Event/TouchDelegate.h"
#include "Framework/Application.h"
#include "GUI/WindowManager.h"

CRenderManager* CRenderManager::m_pInstance = NULL;

CRenderManager::CRenderManager()
    : m_pCurrRenderTarget(nullptr)
    , m_pMainRenderWindow(nullptr)
    , m_pCurrCamera(NULL)
#ifndef GL_ES_VERSION_2_0
    , m_uCurrPolygonMode(GL_FILL)
#endif
    , m_p2DRenderGroup(nullptr)
    , m_pLineBatch(nullptr)
    , m_pTriangleBatch(nullptr)
    , m_uMainFBO(0)
    , m_uMainFBOColorTexture(0)
    , m_uSimulateWidth(1920)
    , m_uSimulateHeight(1080)
{
}

CRenderManager::~CRenderManager()
{
#ifdef USE_UBO
    DeleteUBOList();
#endif
    BEATS_SAFE_DELETE_VECTOR(m_shaderProgramPool);
    BEATS_SAFE_DELETE(m_pLineBatch);
    BEATS_SAFE_DELETE(m_pTriangleBatch);
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->DeleteFramebuffer(1, &m_uMainFBO);
    pRenderer->DeleteTexture(1, &m_uMainFBOColorTexture);
    pRenderer->DeleteRenderbuffer(1, &m_uMainFBODepthBuffer);
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
    // check OpenGL version at first
#ifdef _DEBUG
    const GLubyte* glVersion = glGetString(GL_VERSION);
    BEATS_ASSERT(glVersion != NULL, _T("Get Opengl version failed!"));
    TCHAR szVersionTCHAR[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR((const char*)glVersion, szVersionTCHAR, MAX_PATH);
#if(GL_ES_VERSION_2_0)
    BEATS_ASSERT(strncmp((const char*)glVersion, "OpenGL ES ", 10) == 0 && atof((const char*)glVersion+10) >= 2.0, _T("OpenGL ES 2.0 or higher is required (your version is %s)."), szVersionTCHAR);
#else
    BEATS_ASSERT( atof((const char*)glVersion) >= 1.5, _T("OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card."), szVersionTCHAR);
#endif  //GL_ES_VERSION_2_0
#endif  //def _DEBUG

    // Configuration. Gather GPU info
    CConfiguration *conf = CConfiguration::GetInstance();
    conf->GatherGPUInfo();
    BEATS_PRINT(_T("%s\n"),conf->GetInfo().c_str());

    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();

#ifdef USE_UBO
    InitUBOList();
#endif

    m_pDefaultMaterial = new CMaterial();
    m_pDefaultMaterial->SetSharders( _T("PointColorShader.vs"), _T("PointColorShader.ps"));
    m_pDefaultMaterial->SetDepthTest(true);
    m_pDefaultMaterial->SetBlendEnable(true);
    m_pDefaultMaterial->SetBlendSource(GL_SRC_ALPHA);
    m_pDefaultMaterial->SetBlendDest(GL_ONE_MINUS_SRC_ALPHA);
    m_pDefaultMaterial->SetBlendEquation(GL_FUNC_ADD);
    m_pDefaultMaterial->GetRenderState()->SetLineWidth(1.f);
    m_pDefaultMaterial->GetRenderState()->SetPointSize(5.f);
    m_pDefaultMaterial->SetTexture(0, nullptr );

    m_p2DRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(CRenderGroupManager::LAYER_2D);

    bool bRet = InitLineBuffer() && InitTriangleBuffer();
    BEATS_ASSERT(bRet, _T("Initialize render manager failed!"));

    RecreateFBO();

    return bRet;
}

#ifdef USE_UBO
void CRenderManager::InitUBOList()
{
    for(int i = 0; i < UNIFORM_BLOCK_COUNT; ++i)
    {
        ECommonUniformBlockType type = static_cast<ECommonUniformBlockType>(i);
        GLuint ubo;
        CRenderer::GetInstance()->GenBuffers(1, &ubo);
        m_UBOList[type] = ubo;
    }
}

void CRenderManager::DeleteUBOList()
{
    for(auto ubopair : m_UBOList)
    {
        CRenderer::GetInstance()->DeleteBuffers(1, &ubopair.second);
    }
    m_UBOList.clear();
}

GLuint CRenderManager::GetUBO(ECommonUniformBlockType type) const
{
    GLuint uRet = 0;
    auto itr = m_UBOList.find(type);
    if(itr != m_UBOList.end())
    {
        uRet = itr->second;
    }
    return uRet;
}

void CRenderManager::UpdateUBO(ECommonUniformBlockType type, const GLvoid **data, const GLsizeiptr *size, size_t count)
{
    GLuint ubo = GetUBO(type);
    if(ubo >= 0)
    {
        GLsizeiptr totalsize = 0;
        for(size_t i = 0; i < count; ++i)
        {
            totalsize += size[i];
        }
        CRenderer::GetInstance()->BindBuffer(GL_UNIFORM_BUFFER, ubo);
        CRenderer::GetInstance()->BufferData(GL_UNIFORM_BUFFER, totalsize, 0, GL_DYNAMIC_DRAW);

        GLsizeiptr offset = 0;
        for(size_t i = 0; i < count; ++i)
        {
            CRenderer::GetInstance()->BufferSubData(GL_UNIFORM_BUFFER, offset, size[i], data[i]);
            offset += size[i];
        }
    }
}
#endif
void CRenderManager::Render()
{
    //render line and triangle
    RenderLineImpl();
    RenderTriangleImpl();

    BEYONDENGINE_PERFORMDETECT_START(ePNT_GroupRender)
    CRenderGroupManager::GetInstance()->Render();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_GroupRender)
    m_pLineBatch->Clear();
    m_pTriangleBatch->Clear();
    CRenderGroupManager::GetInstance()->Clear();
}

#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
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
    CRenderer::GetInstance()->PolygonMode(GL_FRONT_AND_BACK, m_uCurrPolygonMode);
}
#endif
CShaderProgram* CRenderManager::GetShaderProgram(GLuint uVertexShader, GLuint uPixelShader)
{
    CShaderProgram* pRet = NULL;
    for (size_t i = 0; i < m_shaderProgramPool.size(); ++i)
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
        image.InitWithRawData(pBuffer, nWidth * nHeight * 4, nWidth, nHeight, 4);
        bRet = image.SaveToFile(pszFilePath, false);
        BEATS_ASSERT(bRet, _T("Save texture to path %s failed! Note only *.png and *.jpg are support!"), pszFilePath);
        BEATS_SAFE_DELETE_ARRAY(pBuffer);
    }
    CRenderer::GetInstance()->BindTexture(GL_TEXTURE_2D, nCurrentBindingTexture);
    return bRet;
}
#endif

GLuint CRenderManager::GetMainFBOTexture() const
{
    return m_uMainFBOColorTexture;
}

void CRenderManager::GetSimulateSize(size_t& uWidth, size_t& uHeight) const
{
    uWidth = m_uSimulateWidth;
    uHeight = m_uSimulateHeight;
}

void CRenderManager::SetSimulateSize(size_t uWidth, size_t uHeight)
{
    m_uSimulateWidth = uWidth;
    m_uSimulateHeight = uHeight;
    RecreateFBO();
}

void CRenderManager::ApplyTexture( int index, GLuint texture )
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->ActiveTexture(GL_TEXTURE0 + index);
    pRenderer->BindTexture(GL_TEXTURE_2D, texture);
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE );
    pRenderer->TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE );

    GLuint currProgram = pRenderer->GetCurrentState()->GetShaderProgram();
    BEATS_ASSERT(currProgram != 0);
    GLint textureLocation = pRenderer->GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_TEX0 + index]);
    pRenderer->SetUniform1i( textureLocation, index);
}

void CRenderManager::SetCurrrentCamera(CCamera* camera)
{
    m_pCurrCamera = camera;
}

void CRenderManager::SendMVPToShader(const kmMat4 &worldTransform)
{
    BEATS_ASSERT(m_pCurrCamera != NULL);
    CRenderer *pRenderer = CRenderer::GetInstance();
    GLuint currShaderProgram = pRenderer->GetCurrentState()->GetShaderProgram();
    if (currShaderProgram != 0)
    {
        const kmMat4& projMat = m_pCurrCamera->GetProjectionMatrix();
        const kmMat4& cameraMat = m_pCurrCamera->GetViewMatrix();
        kmMat4 MVPmat;
        kmMat4Multiply(&MVPmat, &cameraMat, &worldTransform);
        kmMat4Multiply(&MVPmat, &projMat, &MVPmat);
        GLuint location = pRenderer->GetUniformLocation(currShaderProgram, COMMON_UNIFORM_NAMES[UNIFORM_MVP_MATRIX]);
        pRenderer->SetUniformMatrix4fv(location, (const GLfloat *)MVPmat.mat, 1);
    }
}

bool CRenderManager::InitLineBuffer()
{
    m_pLineBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_LINES, false);
    return true;
}

bool CRenderManager::InitTriangleBuffer()
{
    m_pTriangleBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_TRIANGLES, false);
    return true;
}

void CRenderManager::RenderLineImpl()
{
    if(m_pLineBatch->GetVertexCount() > 0)
    {
        CRenderGroup *pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(
            CRenderGroupManager::LAYER_3D);
        pRenderGroup->AddRenderBatch(m_pLineBatch);
    }
}

void CRenderManager::RenderTriangleImpl()
{
    if(m_pTriangleBatch->GetVertexCount() > 0)
    {
        CRenderGroup *pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup(
            CRenderGroupManager::LAYER_3D);
        pRenderGroup->AddRenderBatch(m_pTriangleBatch);
    }
}

void CRenderManager::RecreateFBO()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    if (m_uMainFBO != 0)
    {
        pRenderer->DeleteFramebuffer(1, &m_uMainFBO);
        m_uMainFBO = 0;
        pRenderer->DeleteTexture(1, &m_uMainFBOColorTexture);
        m_uMainFBOColorTexture = 0;
        pRenderer->DeleteRenderbuffer(1, &m_uMainFBODepthBuffer);
        m_uMainFBODepthBuffer = 0;
    }
    //FBO stuff
    pRenderer->GenFramebuffers(1, &m_uMainFBO);
    pRenderer->BindFramebuffer(GL_FRAMEBUFFER, m_uMainFBO);
    pRenderer->GenTextures(1, &m_uMainFBOColorTexture);
    pRenderer->BindTexture(GL_TEXTURE_2D, m_uMainFBOColorTexture);
    pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    pRenderer->TextureImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_uSimulateWidth, m_uSimulateHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    pRenderer->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_uMainFBOColorTexture, 0);

    pRenderer->GenRenderbuffers(1, &m_uMainFBODepthBuffer);
    pRenderer->BindRenderbuffer(GL_RENDERBUFFER, m_uMainFBODepthBuffer);
    pRenderer->RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_uSimulateWidth, m_uSimulateHeight);
    pRenderer->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER ,m_uMainFBODepthBuffer);
    BEATS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, _T("FBO initialize failed!"));
    pRenderer->BindTexture(GL_TEXTURE_2D, 0);
    pRenderer->BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CRenderManager::RenderCoordinate(const kmMat4* pMatrix)
{
    static const size_t X_AIXS_COLOR = 0xFF0000FF;
    static const size_t Y_AIXS_COLOR = 0x00FF00FF;
    static const size_t Z_AIXS_COLOR = 0x0000FFFF;
    kmVec3 upVec, rightVec, forwardVec;

    kmMat4GetUpVec3(&upVec, pMatrix);
    kmMat4GetRightVec3(&rightVec, pMatrix);
    kmMat4GetForwardVec3(&forwardVec, pMatrix);
    kmVec3Scale(&forwardVec, &forwardVec, -100);
    kmVec3Scale(&upVec, &upVec, 100);
    kmVec3Scale(&rightVec, &rightVec, 100);

    float fPosW = (*pMatrix).mat[15];
    kmVec3 pos;
    kmVec3Fill(&pos,(*pMatrix).mat[12] / fPosW, (*pMatrix).mat[13] / fPosW, (*pMatrix).mat[14] / fPosW);

    kmVec3Add(&rightVec,&rightVec,&pos);
    kmVec3Add(&upVec,&upVec,&pos);
    kmVec3Add(&forwardVec,&forwardVec,&pos);

    CVertexPC originPos, xPos, yPos, zPos;
    originPos.position.x = pos.x;
    originPos.position.y = pos.y;
    originPos.position.z = pos.z;
    originPos.color = X_AIXS_COLOR;

    xPos.position.x = rightVec.x;
    xPos.position.y = rightVec.y;
    xPos.position.z = rightVec.z;
    xPos.color = X_AIXS_COLOR;
    RenderLine(originPos,xPos);

    yPos.position.x = upVec.x;
    yPos.position.y = upVec.y;
    yPos.position.z = upVec.z;
    yPos.color = Y_AIXS_COLOR;
    originPos.color = Y_AIXS_COLOR;
    RenderLine(originPos,yPos);

    zPos.position.x = forwardVec.x;
    zPos.position.y = forwardVec.y;
    zPos.position.z = forwardVec.z;
    zPos.color = Z_AIXS_COLOR;
    originPos.color = Z_AIXS_COLOR;
    RenderLine(originPos,zPos);
}

void CRenderManager::RenderLine(const CVertexPC& start,const CVertexPC& end)
{
    m_pLineBatch->AddVertices(&start, 1);
    m_pLineBatch->AddVertices(&end, 1);
}

void CRenderManager::Render2DLine(const CVertex2DPC& start, const CVertex2DPC& end)
{
    CVertexPC vertices[2];
    vertices[0].position.x = start.position.x;
    vertices[0].position.y = start.position.y;
    vertices[0].color = start.color;
    vertices[1].position.x = end.position.x;
    vertices[1].position.y = end.position.y;
    vertices[1].color = end.color;

    m_p2DRenderGroup->GetRenderBatch(
        VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_LINES, false)->AddVertices(vertices, 2);
}

void CRenderManager::Render2DLine(const CVertexPC& start, const CVertexPC& end)
{
    CRenderBatch *pBatch = m_p2DRenderGroup->GetRenderBatch(
        VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_LINES, false);
    pBatch->AddVertices(&start, 1);
    pBatch->AddVertices(&end, 1);
}

void CRenderManager::RenderTriangle( const CVertexPC& pt1, const CVertexPC& pt2, const CVertexPC& pt3 )
{
    m_pTriangleBatch->AddVertices(&pt1, 1);
    m_pTriangleBatch->AddVertices(&pt2, 1);
    m_pTriangleBatch->AddVertices(&pt3, 1);
}

void CRenderManager::Render2DTriangle(const CVertex2DPC& pt1, const CVertex2DPC& pt2, const CVertex2DPC& pt3)
{
    CVertexPC vertices[3];
    vertices[0].position.x = pt1.position.x;
    vertices[0].position.y = pt1.position.y;
    vertices[0].color = pt1.color;
    vertices[1].position.x = pt2.position.x;
    vertices[1].position.y = pt2.position.y;
    vertices[1].color = pt2.color;
    vertices[2].position.x = pt3.position.x;
    vertices[2].position.y = pt3.position.y;
    vertices[2].color = pt3.color;

    m_p2DRenderGroup->GetRenderBatch(
        VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_TRIANGLES, false)->AddVertices(vertices, 3);
}

void CRenderManager::Render2DFan(const kmVec2& origin, const kmVec2& direction,
                                 float innerRadius, float outterRadius, float angle, CColor color,
                                 bool bRenderOutline, CColor outlineColor)
{
    bool isHalfFan = innerRadius > 0.f;

    CRenderBatch *batch = m_p2DRenderGroup->GetRenderBatch(
        VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, isHalfFan ? GL_TRIANGLE_STRIP : GL_TRIANGLE_FAN, false);
    CRenderBatch *outlineBatch = nullptr;
    if(bRenderOutline)
    {
        outlineBatch = m_p2DRenderGroup->GetRenderBatch(
            VERTEX_FORMAT(CVertexPC), m_pDefaultMaterial, GL_LINE_LOOP, false);
    }

    if(!isHalfFan)
    {
        CVertexPC verOrigin;
        verOrigin.position.x = origin.x;
        verOrigin.position.y = origin.y;
        verOrigin.color = color;
        batch->AddVertices(&verOrigin, 1);
        if(bRenderOutline)
        {
            verOrigin.color = outlineColor;
            outlineBatch->AddVertices(&verOrigin, 1);
        }
    }

    kmVec2 dir;
    kmVec2Normalize(&dir, &direction);

    std::vector<CVertexPC> buffer;

    static const int FRAG_COUNT = 9;
    if(bRenderOutline && isHalfFan)
        buffer.resize(FRAG_COUNT + 1);
    float fragAngle = angle / FRAG_COUNT;
    for(int i = 0; i <= FRAG_COUNT; ++i)
    {
        float currAngle = ((float)FRAG_COUNT / 2 - i) * fragAngle;
        float sine = sin(currAngle);
        float cosine = cos(currAngle);
        kmVec2 currdir;
        currdir.x = dir.x * cosine - dir.y * sine;
        currdir.y = dir.x * sine + dir.y * cosine;
        if(isHalfFan)
        {
            CVertexPC vertexInner;
            vertexInner.position.x = origin.x + currdir.x * innerRadius;
            vertexInner.position.y = origin.y + currdir.y * innerRadius;
            vertexInner.color = color;
            batch->AddVertices(&vertexInner, 1);
            if(bRenderOutline)
            {
                vertexInner.color = outlineColor;
                buffer[FRAG_COUNT - i] = vertexInner;
            }
        }
        CVertexPC vertexOutter;
        vertexOutter.position.x = origin.x + currdir.x * outterRadius;
        vertexOutter.position.y = origin.y + currdir.y * outterRadius;
        vertexOutter.color = color;
        batch->AddVertices(&vertexOutter, 1);
        if(bRenderOutline)
        {
            vertexOutter.color = outlineColor;
            outlineBatch->AddVertices(&vertexOutter, 1);
        }
    }
    if(isHalfFan && bRenderOutline)
        outlineBatch->AddVertices(&buffer[0], buffer.size());

    batch->Complete();
    if(bRenderOutline)
        outlineBatch->Complete();
}

void CRenderManager::RenderTextureToFullScreen(GLuint texture)
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->DisableGL(CBoolRenderStateParam::eBSP_ScissorTest);
    pRenderer->DisableGL(CBoolRenderStateParam::eBSP_Blend);
    pRenderer->DisableGL(CBoolRenderStateParam::eBSP_CullFace);
    CRenderer::GetInstance()->ClearColor( 1.0f, 1.0f, 0, 1.0f);
    CRenderer::GetInstance()->ClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static float fullScreenQuadVB[8] = 
    {
        -1.0F, -1.0F,
        -1.0F, +1.0F,
        +1.0F, +1.0F,
        +1.0F, -1.0F
    };
    static unsigned short fullScreenQuadIB[6] = {2, 1, 0, 3, 2, 0};
    SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("fullScreen.vs"));
    SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("fullScreen.ps"));
    BEATS_ASSERT(pVS && pPS, _T("Load Shader Failed!"));
    CShaderProgram* pProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
    pRenderer->UseProgram( pProgram->ID() );
    pRenderer->ActiveTexture(GL_TEXTURE0);
    pRenderer->BindTexture( GL_TEXTURE_2D, texture );

    GLint textureLocation = pRenderer->GetUniformLocation(pProgram->ID(), COMMON_UNIFORM_NAMES[UNIFORM_TEX0]);
    pRenderer->SetUniform1i( textureLocation, 0);
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, 0);
    pRenderer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    pRenderer->BindVertexArray(0);
    pRenderer->EnableVertexAttribArray(ATTRIB_INDEX_POSITION);
    pRenderer->VertexAttribPointer(ATTRIB_INDEX_POSITION,
        2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)fullScreenQuadVB);
    pRenderer->DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, fullScreenQuadIB);
    pRenderer->DisableVertexAttribArray(ATTRIB_INDEX_POSITION);
}

size_t CRenderManager::GetDesignWidth()
{
    return DESIGN_WIDTH;
}

size_t CRenderManager::GetDesignHeight()
{
    return DESIGN_HEIGHT;
}

float CRenderManager::GetDesignRatio()
{
    return (float)DESIGN_WIDTH / DESIGN_HEIGHT;
}

size_t CRenderManager::GetWidth() const
{
    return m_pCurrRenderTarget->GetWidth();
}

size_t CRenderManager::GetHeight() const
{
    return m_pCurrRenderTarget->GetHeight();
}

SharePtr<CMaterial> CRenderManager::GetDefaultMaterial() const
{
    return m_pDefaultMaterial;
}
