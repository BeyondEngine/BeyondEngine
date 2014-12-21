#ifndef BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONELEMENT_H__INCLUDE
#define BEYOND_ENGINE_NODEANIMATION_NODEANIMATIONELEMENT_H__INCLUDE

#include "Component/Component/ComponentInstance.h"

enum ENodeAnimationElementType
{
    eNAET_Pos,
    eNAET_Rotation,
    eNAET_Scale,
    eNAET_UIColor,
    eNAET_UIAlpha,
    eNAET_ColorScale,
    eNAET_UV,
    eNAET_Anchor,
    eNAET_AlphaScale,
    eNAET_contentSize,

    eNAET_Count,
    eNAET_Force32Bit = 0xFFFFFFFF
};

static const TCHAR* strNodeAnimationElementType[] =
{
    _T("位置"),
    _T("旋转"),
    _T("缩放"),
    _T("UI颜色"),
    _T("UI透明度"),
    _T("颜色比例"),
    _T("纹理坐标"),
    _T("锚点"),
    _T("透明度比例"),
    _T("内部尺寸")
};

class CNode;
class CNodeAnimationElement : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CNodeAnimationElement, 0x7B4AA1D7, CComponentInstance)
public:
    CNodeAnimationElement();
    virtual ~CNodeAnimationElement();

    virtual void ReflectData(CSerializer& serializer) override;
    ENodeAnimationElementType GetType() const;
    void SetType(ENodeAnimationElementType type);
    bool GetValue(uint32_t uFrame, CVec3& outValue);
    bool SetValue(uint32_t uFrame, CVec3& value);
    void Apply(CNode* pNode, uint32_t uCurrFrame);
    void AddKeyFrame(uint32_t uFramePos, const CVec3& data);
    void RemoveKeyFrame(uint32_t uFramePos);
    const std::map<uint32_t, CVec3>& GetKeyFrames() const;

private:
    ENodeAnimationElementType m_type;
    std::map<uint32_t, CVec3> m_keyFrames;
};

#endif