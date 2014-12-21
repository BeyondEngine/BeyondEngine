#ifndef BEYOND_ENGINE_RENDER_RENDERMANAGER_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERMANAGER_H__INCLUDE

#include "RenderGroupManager.h"
#include "Camera.h"

class CRenderTarget;
class CShaderProgram;
class CTexture;
class CMaterial;
class CRenderGroup;
class CRenderBatch;
class CVertexPC;
class CVertex2DPC;
class CColor;
class CVertexFormat;
class CSerializer;
class CVertexBufferContent;
class CRenderState;
class CShaderUniform;
class CNode;
class CVec4;
class CVec2;
struct GLFWwindow;
class CCoordinateRenderObject;

class CRenderManager
{
    BEATS_DECLARE_SINGLETON(CRenderManager);
public:
    typedef std::map<GLenum, CVertexBufferContent*> TPrimitiveVertexBufferMap;
    typedef std::map<const CVertexFormat*, TPrimitiveVertexBufferMap> TFormatBufferMap;

    bool Initialize();
    void Uninitialize();
    void SetCurrentRenderTarget(CRenderTarget *pRenderTarget);
    CRenderTarget *GetCurrentRenderTarget() const;
    void RenderLine(const CVertexPC& start,const CVertexPC& end, float lineWidth = 1.0f, bool bDebug = false) const;
    void RenderLines(const std::vector<CVertexPC>& points, const std::vector<unsigned short>& indices, float lineWidth = 1.0f, bool bDebug = false) const;
    void Render2DLine(const CVertex2DPC& start, const CVertex2DPC& end);
    void Render2DLine(const CVertexPC& start,const CVertexPC& end);
    void RenderTriangle(const CVertexPC& pt1, const CVertexPC& pt2, const CVertexPC& pt3, bool bDebug = false);
    void RenderTriangle(const std::vector<CVertexPC>& pt, const std::vector<unsigned short>& indices, bool bDebug = false);
    void Render2DTriangle(const CVertex2DPC& pt1, const CVertex2DPC& pt2, const CVertex2DPC& pt3);
    void RenderPoints(const CVertexPC& pt, float fPointSize, bool bDebug = false);
    void RenderTextureToFullScreen(GLuint texture);
    CCamera* GetCamera(CCamera::ECameraType type) const;
    void SendMVPToShader(CCamera::ECameraType type, const CMat4* pWorldMat, const CMat4* pViewMat, const CMat4* pProjectionMat);
    void Render();
    void Reset();
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    void SwitchPolygonMode();
    bool SaveTextureToFile(GLuint uTextureId, const TCHAR* pszFilePath, bool bFlipY = false, int nPixelFormat = GL_RGBA);
#endif
    CShaderProgram* GetShaderProgram(GLuint uVertexShader, GLuint uPixelShader);
#ifdef DEVELOP_VERSION
    void GetSimulateSize(uint32_t& uWidth, uint32_t& uHeight) const;
    void SetSimulateSize(uint32_t uWidth, uint32_t uHeight);
    bool m_bRenderSpriteUserDefinePos = false;
    bool m_bRenderSpriteEdge = false;
    bool m_bRenderSkeletonDbgLine = false;
    bool m_bRenderPathFindingGrid = false;
    bool m_bRenderBuilding = true;
    bool m_bRenderSprite = true;
    bool m_bRenderMapGrid = false;
    bool m_bRenderSelectArea = false;
    bool m_bRenderWater = true;
    bool m_bRenderMesh = true;
    bool m_bRenderModel = true;
    bool m_bRenderAABB = false;
    bool m_bRenderHPNumber = false;
#endif

    static uint32_t GetDesignWidth();
    static uint32_t GetDesignHeight();

    SharePtr<CMaterial> GetDefaultMaterial() const;
    SharePtr<CMaterial> GetSpriteMaterial(bool bBlendAdd, bool bUseColorData = true) const;
    SharePtr<CMaterial> GetSkinMaterial() const;
    SharePtr<CMaterial> GetBackGroundMaterial() const;
    SharePtr<CMaterial> GetParticleAlphaBlendMaterial(bool bDepthEnable) const;
    SharePtr<CMaterial> GetParticleAdditiveMaterial(bool bDepthEnable) const;
    SharePtr<CMaterial> GetAttackRangeMaterial() const;
    SharePtr<CMaterial> GetMapGridMaterial() const;
    SharePtr<CMaterial> GetLastApplyMaterial() const;
    SharePtr<CMaterial> GetFreeTypeFontMaterial() const;
    SharePtr<CMaterial> GetImageMaterial() const;
    SharePtr<CMaterial> GetMeshMaterial(bool bBlend, bool bBlendAdd) const;
    SharePtr<CMaterial> GetMeshLightMapMaterial(bool bBlend) const;
    SharePtr<CMaterial> GetWaterMaterial() const;
    void SetLastApplyMaterial(SharePtr<CMaterial> pMaterial);

    void ApplyTextureMap(const std::map<unsigned char, SharePtr<CTexture> >& textureMap, bool bClampToEdgeOrRepeat);
    CVertexBufferContent* GetVertexContent(const CVertexFormat* pFormat, GLenum primitiveType);

    //Post Process Function
    void SetGlobalColorFactor(float fFactor);
    float GetGlobalColorFactor() const;

    CCoordinateRenderObject* GetCoordinateRenderObject() const;
    const std::map<TString, CShaderUniform>& GetDefaultShaderUniformMap() const;
    void InitDefaultShaderUniform();
    uint32_t GetUniformLocation(uint32_t uProgramId, const TString& strName);

#ifdef DEVELOP_VERSION
    SharePtr<CTexture> GetTempTexture();
#endif

private:
    bool InitLineBuffer();
    bool InitTriangleBuffer();
    void RenderLineImpl();
    void RenderTriangleImpl();
private:
    CRenderTarget *m_pCurrRenderTarget;
    CCamera* m_p2DCamera;
    CCamera* m_p3DCamera;
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    GLuint m_uCurrPolygonMode;
#endif
#ifdef DEVELOP_VERSION
    uint32_t m_uSimulateWidth = 1920;
    uint32_t m_uSimulateHeight = 1080;
#endif
    static const uint32_t DESIGN_WIDTH = 1024;
    static const uint32_t DESIGN_HEIGHT = 768;

    SharePtr<CMaterial> m_pDefaultMaterial;
    SharePtr<CMaterial> m_pSpriteMaterial;
    SharePtr<CMaterial> m_pSpriteMaterialWithOutColor;
    SharePtr<CMaterial> m_pSpriteBlendAddMaterial;
    SharePtr<CMaterial> m_pSpriteBlendAddMaterialWithOutColor;
    SharePtr<CMaterial> m_pSkinMaterial;
    SharePtr<CMaterial> m_pBackGroundMaterial;
    SharePtr<CMaterial> m_pParticleAlphaBlendMaterial;
    SharePtr<CMaterial> m_pParticleAdditiveMaterial;
    SharePtr<CMaterial> m_pParticleNoDepthAlphaBlendMaterial;
    SharePtr<CMaterial> m_pParticleNoDepthAdditiveMaterial;
    SharePtr<CMaterial> m_pAttackRangeMaterial;
    SharePtr<CMaterial> m_pMapGridMaterial;
    SharePtr<CMaterial> m_pFreetypeFontMaterial;
    SharePtr<CMaterial> m_pBitmapFontMaterial;
    SharePtr<CMaterial> m_pWaterMaterial;
    SharePtr<CMaterial> m_pMeshMaterial;
    SharePtr<CMaterial> m_pMeshMaterialBlend;
    SharePtr<CMaterial> m_pMeshMaterialBlendAdd;
    SharePtr<CMaterial> m_pMeshLightMapMaterial;
    SharePtr<CMaterial> m_pMeshLightMapMaterialBlend;
    SharePtr<CMaterial> m_pLastApplyMaterial;
    // Post Process Param
    CShaderUniform* m_pGlobalColorMultiFactor;
    //When we set global factor in update, this value may not take effect immediately unless after we draw one frame.
    //So we use a cache variable to record what the real color factor is(after draw)
    float m_fGlobalColorFactor = 1.f;
    CShaderProgram* m_pFullScreenProgram = nullptr;
    CRenderBatch* m_pLineBatch;
    CRenderBatch* m_pTriangleBatch;
    TFormatBufferMap m_vertexMap;
    std::vector<CShaderProgram*> m_shaderProgramPool;
    CMat4 m_identityMatrix;
    CCoordinateRenderObject* m_pCoordinateRenderObject = nullptr;
    std::map<TString, CShaderUniform> m_defaultUniformMap;
    std::map<uint32_t, std::map<TString, uint32_t> > m_uniformLocationCache;//<programId, <name, location> >
#ifdef DEVELOP_VERSION
    // For those loading failed texture, use this temp texture instead.
    SharePtr<CTexture> m_pTempTexture;
    CRenderBatch* m_pDebugTriangleBatch = nullptr;
    CRenderBatch* m_pDebugLineBatch = nullptr;
    SharePtr<CMaterial> m_pDebugPrimitiveMaterial;
#endif
};

#endif