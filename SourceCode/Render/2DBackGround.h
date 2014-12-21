#ifndef BEYOND_ENGINE_RENDER_2DBACKGROUND_H__INCLUDE
#define BEYOND_ENGINE_RENDER_2DBACKGROUND_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
#include "ReflectTextureInfo.h"
class CRenderBatch;
class C2DBackGround : public CComponentInstance
{
    DECLARE_REFLECT_GUID(C2DBackGround, 0x9F410BD1, CComponentInstance)
public:
    C2DBackGround();
    virtual ~C2DBackGround();

    virtual bool Load() override;
    virtual void ReflectData(CSerializer& serializer) override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    void Render();
    void SetProgress(const CVec2& progress);
    void SetUVSize(const CVec2& size);
    const CVec2& GetInitUVSize() const;
    const CVec2& GetSizeScaleForFov() const;
private:
    void UpdateQuad();

private:
    CVec2 m_initUVSize;
    CVec2 m_uvSize;
    CVec2 m_progressInCoord;
    CVec2 m_sizeScaleForFov;
    SReflectTextureInfo m_backgroundFrag;
    CVertexPT m_quadPT[4];
};

#endif