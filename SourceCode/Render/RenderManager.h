#ifndef BEYOND_ENGINE_RENDER_RENDERMANAGER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERMANAGER_H__INCLUDE

#include "Event/EventDispatcher.h"
#include "RenderEventType.h"

class CRenderTarget;
class CRenderWindow;
class CShaderProgram;
class CCamera;
class CTexture;
class CMaterial;
class CRenderGroup;
class CRenderBatch;
class CVertexPC;
class CVertex2DPC;
class CColor;
struct GLFWwindow;
class CVertexPC;
class CVertex2DPC;
class CColor;
struct kmVec4;
struct kmVec2;

class CRenderManager : public CEventDispatcher
{
    BEATS_DECLARE_SINGLETON(CRenderManager);
public:
    bool Initialize();
    void SetCurrentRenderTarget(CRenderTarget *pRenderTarget);
    CRenderTarget *GetCurrentRenderTarget() const;
#ifdef USE_UBO
    void InitUBOList();
    void DeleteUBOList();
    GLuint GetUBO(ECommonUniformBlockType type) const;
    void UpdateUBO(ECommonUniformBlockType type, const GLvoid **data, const GLsizeiptr *size, size_t count);
#endif
    void RenderCoordinate(const kmMat4* pMatrix);
    void RenderLine(const CVertexPC& start,const CVertexPC& end);
    void Render2DLine(const CVertex2DPC& start, const CVertex2DPC& end);
    void Render2DLine(const CVertexPC& start,const CVertexPC& end);
    void RenderTriangle(const CVertexPC& pt1, const CVertexPC& pt2, const CVertexPC& pt3);
    void Render2DTriangle(const CVertex2DPC& pt1, const CVertex2DPC& pt2, const CVertex2DPC& pt3);
    void Render2DFan(const kmVec2& origin, const kmVec2& direction,
        float innerRadius, float outterRadius, float angle, CColor color,
        bool bRenderOutline = false, CColor outlineColor = 0);
    void RenderTextureToFullScreen(GLuint texture);

    void SetCurrrentCamera(CCamera* camera);
    void ApplyTexture( int index, GLuint texture );
    void SendMVPToShader(const kmMat4 &worldTransform);

    void Render();
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    void SwitchPolygonMode();
    bool SaveTextureToFile(GLuint uTextureId, const TCHAR* pszFilePath, bool bFlipY = false, int nPixelFormat = GL_RGBA);
#endif

    CShaderProgram* GetShaderProgram(GLuint uVertexShader, GLuint uPixelShader);

    GLuint GetMainFBOTexture() const;

    void GetSimulateSize(size_t& uWidth, size_t& uHeight) const;
    void SetSimulateSize(size_t uWidth, size_t uHeight);

    static size_t GetDesignWidth();
    static size_t GetDesignHeight();
    static float GetDesignRatio();

    size_t GetWidth() const;
    size_t GetHeight() const;
    SharePtr<CMaterial> GetDefaultMaterial() const;

private:
    bool InitLineBuffer();
    bool InitTriangleBuffer();
    void RenderLineImpl();
    void RenderTriangleImpl();
    void RecreateFBO();
private:
#ifdef USE_UBO
    typedef std::map<ECommonUniformBlockType, GLuint> UBOList;
    UBOList m_UBOList;
#endif
    GLuint m_uMainFBO;
    GLuint m_uMainFBOColorTexture;
    GLuint m_uMainFBODepthBuffer;
    CRenderTarget *m_pCurrRenderTarget;
    CRenderWindow *m_pMainRenderWindow;
    CCamera* m_pCurrCamera;
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    GLuint m_uCurrPolygonMode;
#endif

    size_t m_uSimulateWidth;
    size_t m_uSimulateHeight;
    static const size_t DESIGN_WIDTH = 960;
    static const size_t DESIGN_HEIGHT = 640;

    SharePtr<CMaterial> m_pDefaultMaterial;

    CRenderGroup *m_p2DRenderGroup;
    CRenderBatch *m_pLineBatch;
    CRenderBatch *m_pTriangleBatch;

    std::vector<CShaderProgram*> m_shaderProgramPool;
};

#endif