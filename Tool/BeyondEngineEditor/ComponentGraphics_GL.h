#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_COMPONENTGRAPHICS_GL_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_COMPONENTGRAPHICS_GL_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"

class CFontFace;
class CMaterial;
class CTextureAtlas;
class CTextureFrag;
class CRenderBatch;
class CComponentGraphic_GL : public CComponentGraphic
{
    typedef CComponentGraphic super;
public:
    CComponentGraphic_GL();
    virtual ~CComponentGraphic_GL();

    void CreateMaterials();
    SharePtr<CMaterial> CreateMaterial(const TString &textureFileName);
    virtual void GetDependencyPosition(size_t uDependencyIndex, int* pOutX, int* pOutY);
    virtual EComponentAeraRectType HitTestForAreaType( int x, int y, void** pReturnData);
    virtual CComponentGraphic* Clone();

    static CFontFace* GetFontFace();

private:
    virtual void DrawHead(float cellSize);
    virtual void DrawDependencies(float cellSize);
    virtual void DrawDependencyLine(float cellSize, const CDependencyDescription* pDependency);
    virtual void DrawSelectedRect(float cellSize);

private:
    static CFontFace* m_pFont;
    static SharePtr<CMaterial> m_pMaterial;
    static SharePtr<CTextureAtlas> m_pAtlas;
    static CTextureFrag *m_pFrags[eCT_Count];

public:
    static CRenderBatch* m_pRenderBatch;
};

#endif