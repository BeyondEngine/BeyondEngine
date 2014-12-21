#ifndef BEYONDENGINE_COMPONENT_DEPENDENCYDESCRIPTIONLINE_H__INCLUDE
#define BEYONDENGINE_COMPONENT_DEPENDENCYDESCRIPTIONLINE_H__INCLUDE

class CDependencyDescription;
class CComponentProxy;
struct SVertex;

class CDependencyDescriptionLine
{
public:
    CDependencyDescriptionLine(CDependencyDescription* pOwner, uint32_t uIndex, CComponentProxy* pTo);
    ~CDependencyDescriptionLine();

    void UpdateRect(float cellSize, bool bInverseY = false);
    const SVertex* GetRectArray();
    const SVertex* GetArrowRectArray();
    CDependencyDescription* GetOwnerDependency();
    CComponentProxy* GetConnectedComponent();
    void SetConnectComponent(CComponentProxy* pComponentTo);
    bool IsSelected();
    void SetSelected(bool bFlag);
    uint32_t GetIndex();
    void SetIndex(uint32_t uIndex);
    bool HitTest(float x, float y);

private:
    bool m_bIsSelected;
    uint32_t m_uIndex;
    CDependencyDescription* m_pOwner;
    CComponentProxy* m_pConnectedComponent;
    SVertex* m_pRect; // 0 = left top, 1 = left down, 2 = right top, 3 = right down.
    SVertex* m_pArrowRect;
};
#endif