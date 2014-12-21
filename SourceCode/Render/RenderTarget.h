#ifndef BEYOND_ENGINE_RENDER_RENDERTARGET_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERTARGET_H__INCLUDE

class CViewport;
class CRenderTarget
{
public:
    CRenderTarget(uint32_t uDeviceWidth, uint32_t uDeviceHeight);
    virtual ~CRenderTarget();

    virtual void Uninitialize();
    void SetDeviceSize(uint32_t uDeviceWidth, uint32_t uDeviceHeight, bool bUpdateLogicPos = true, bool bUpdateWindowManager = true);
    uint32_t GetDeviceWidth() const;
    uint32_t GetDeviceHeight() const;
    uint32_t GetLogicWidth() const;
    uint32_t GetLogicHeight() const;

    void SetScaleFactor(float fScaleFactor);
    float GetScaleFactor() const;

    void SetViewport(CViewport *pViewport);
    CViewport *GetViewport() const;

    virtual void SetToCurrent();
    virtual void Render();
    void RecreateFBO(uint32_t uWidth, uint32_t uHeight);
    void SetUsePostProcess(bool bUsePostProcess);
    bool IsUsePostProcess() const;
    uint32_t GetFBOTexture() const;
    void SetFBOTexture(uint32_t uId);
    void SetRenderSwitcher(bool bRenderSwitcher);
    bool GetRenderSwitcher() const;
protected:
    void DeleteFBO();

private:
    void SetLogicSize(uint32_t uLogicWidth, uint32_t uLogicHeight);

private:
    bool m_bRenderSwitch = true;
    bool m_bUsePostProcess;
    uint32_t m_uLogicWidth;
    uint32_t m_uLogicHeight;
    uint32_t m_uDeviceWidth;
    uint32_t m_uDeviceHeight;
    uint32_t m_uPostProcessFBO;
    uint32_t m_uPostProcessTexture;
    uint32_t m_uPostProcessDepthBuffer;
    float m_fScaleFactor;
    CViewport* m_pViewport;
};

#endif