#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTGRAPHIC_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTGRAPHIC_H__INCLUDE

static const int HEADER_HEIGHT = 2;
static const int DEPENDENCY_HEIGHT = 1;
static const int DEPENDENCY_WIDTH = 1;
static const int MIN_WIDTH = 6;
static const int CONNECTION_WIDTH = 1;
static const float DEPENDENCY_LINE_WIDTH = 2.5f;
static const float ARROW_SIZE = 15.0f;

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

class CComponentGraphic
{
public:
    CComponentGraphic();
    virtual ~CComponentGraphic();

    void OnRender(float gridSize, bool bSelected);
    void SetPosition(int x, int y);
    void GetPosition(int* pOutX, int* pOutY);
    size_t GetDependencyWidth();
    size_t GetDependencyHeight();
    size_t GetHeaderHeight();
    size_t GetConnectWidth();
    float GetDependencyLineWidth();
    float GetDependencyLineArrowSize();

    size_t GetWidth();
    size_t GetHeight();
    void SetOwner(CComponentProxy* pOwner);
    CComponentProxy* GetOwner() const;
    void CaculateSize();

    bool IsReference() const;
    void SetReferenceFlag(bool bReferenceFlag);

    virtual void GetDependencyPosition(size_t uDependencyIndex, int* pOutX, int* pOutY);
    virtual EComponentAeraRectType HitTestForAreaType(int x, int y, void** pReturnData);
    virtual CComponentGraphic* Clone() = 0;

protected:
    virtual void DrawHead(float cellSize) = 0;
    virtual void DrawDependencies(float cellSize) = 0;
    virtual void DrawDependencyLine(float cellSize, const CDependencyDescription* pDependency) = 0;
    virtual void DrawSelectedRect(float cellSize) = 0;

protected:
    bool m_bIsReference;
    CComponentProxy* m_pOwner;
    size_t m_width;
    size_t m_height;
    int m_gridPosX;
    int m_gridPosY;
    int m_gridPosZ;
};

#endif
