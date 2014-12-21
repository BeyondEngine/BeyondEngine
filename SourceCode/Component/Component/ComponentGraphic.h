#ifndef BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTGRAPHIC_H__INCLUDE
#define BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTGRAPHIC_H__INCLUDE

#include "Utility/MathExt/Vec2.h"
#include "Utility/MathExt/Vec3.h"

static const int HEADER_HEIGHT = 2;
static const int DEPENDENCY_HEIGHT = 1;
static const int DEPENDENCY_WIDTH = 1;
static const int MIN_WIDTH = 6;
static const int CONNECTION_WIDTH = 1;
static const float DEPENDENCY_LINE_WIDTH = 2.5f;
static const float ARROW_SIZE = 15.0f;

struct SVertex
{
    CVec3 m_position;
    CColor m_color;
    CVec2 m_uv;
    static const long VertexFormat/* = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1*/;

    SVertex() 
        : m_position(0,0,0)
        , m_color(0)
        , m_uv(0,0)
    {}

    SVertex(float x, float y, float z, unsigned long color, float u, float v) 
        : m_position(x,y,z)
        , m_color(color)
        , m_uv(u,v)
    {}
};

struct SVertex2
{
    CVec3 m_position;
    CColor m_color;
    static const long VertexFormat /*= D3DFVF_XYZ | D3DFVF_DIFFUSE*/;

    SVertex2() 
        : m_position(0,0,0)
        , m_color(0)
    {}

    SVertex2(float x, float y, float z, unsigned long color) 
        : m_position(x,y,z)
        , m_color(color)
    {}
};

enum EComponentAeraRectType
{
    eCART_Invalid,
    eCART_Body,
    eCART_Connection,
    eCART_Dependency,
    eCART_DependencyLine,

    eCART_Count,
    eCART_Force32Bit = 0xFFFFFFFF
};

enum EComponentTexture
{
    eCT_RectBG,
    eCT_RefRectBG,
    eCT_ConnectRect,
    eCT_SelectedRectBG,
    eCT_NormalLine,
    eCT_SelectedLine,
    eCT_NormalArrow,
    eCT_SelectedArrow,
    eCT_ConnectedDependency,
    eCT_ConnectedDependencyList,
    eCT_WeakDependency,
    eCT_WeakDependencyList,
    eCT_StrongDependency,
    eCT_StrongDependencyList,

    eCT_Count,
    eCT_Force32Bit = 0xFFFFFFFF
};
class CComponentProxy;
class CDependencyDescription;

class  CComponentGraphic
{
public:
    CComponentGraphic();
    virtual ~CComponentGraphic();

    void OnRender(float gridSize, bool bSelected);
    void SetPosition(int x, int y);
    void GetPosition(int* pOutX, int* pOutY);
    uint32_t GetDependencyWidth();
    uint32_t GetDependencyHeight();
    uint32_t GetHeaderHeight();
    uint32_t GetConnectWidth();
    float GetDependencyLineWidth();
    float GetDependencyLineArrowSize();

    uint32_t GetWidth();
    uint32_t GetHeight();
    void SetOwner(CComponentProxy* pOwner);
    CComponentProxy* GetOwner() const;
    void CaculateSize();

    virtual void GetDependencyPosition(uint32_t uDependencyIndex, int* pOutX, int* pOutY);
    virtual EComponentAeraRectType HitTestForAreaType(int x, int y, void** pReturnData);
    virtual CComponentGraphic* Clone() = 0;

protected:
    virtual void DrawHead(float cellSize) = 0;
    virtual void DrawDependencies(float cellSize) = 0;
    virtual void DrawDependencyLine(float cellSize, const CDependencyDescription* pDependency) = 0;
    virtual void DrawSelectedRect(float cellSize) = 0;

protected:
    CComponentProxy* m_pOwner;
    uint32_t m_width;
    uint32_t m_height;
    int m_gridPosX;
    int m_gridPosY;
    int m_gridPosZ;
};

#endif
