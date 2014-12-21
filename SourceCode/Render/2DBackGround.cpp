#include "stdafx.h"
#include "2DBackGround.h"
#include "RenderGroupManager.h"
#include "RenderManager.h"
#include "RenderGroup.h"
#include "VertexFormat.h"
#include "RenderBatch.h"
#include "Material.h"
#include "RenderTarget.h"
#include "Texture.h"
C2DBackGround::C2DBackGround()
{
    m_uvSize.Fill(1.0f, 1.0f);
    m_sizeScaleForFov.Fill(1.0, 1.0f);
}

C2DBackGround::~C2DBackGround()
{

}

bool C2DBackGround::Load()
{
    m_uvSize = m_initUVSize;
    return super::Load();
}

void C2DBackGround::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_initUVSize, true, 0xFFFFFFFF, "uv尺寸", NULL, "指整屏所占的UV长度，不得低于0或者大于1，如果值为1,1，表示该背景图恰好填满整个屏幕", "MinValue:0, MaxValue:1");
    DECLARE_PROPERTY(serializer, m_sizeScaleForFov, true, 0xFFFFFFFF, "Fov缩放因子", NULL, "x为fov为最小值时size的缩放，y为fov为最大值时size的缩放", NULL);
    DECLARE_PROPERTY(serializer, m_backgroundFrag, true, 0xFFFFFFFF, "背景图", NULL, NULL, NULL);
}
#ifdef EDITOR_MODE
bool C2DBackGround::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bRet)
    {
        if (pVariableAddr == &m_initUVSize)
        {
            DeserializeVariable(m_initUVSize, pSerializer, this);
            m_uvSize = m_initUVSize;
            UpdateQuad();
            bRet = true;
        }
        else if (pVariableAddr == &m_sizeScaleForFov)
        {
            DeserializeVariable(m_sizeScaleForFov, pSerializer, this);
            UpdateQuad();
            bRet = true;
        }
        else if (pVariableAddr == &m_backgroundFrag)
        {
            DeserializeVariable(m_backgroundFrag, pSerializer, this);
            UpdateQuad();
            bRet = true;
        }
    }
    return bRet;
}
#endif
void C2DBackGround::Render()
{
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_backgroundFrag.GetTextureFrag())
        CRenderGroup* pBackGroundGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(LAYER_BACKGROUND);
        SharePtr<CMaterial> pBackGroundMaterial = CRenderManager::GetInstance()->GetBackGroundMaterial();
        CRenderBatch* pBackGroundBatch = pBackGroundGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPT), pBackGroundMaterial, GL_TRIANGLES, true);
        std::map<unsigned char, SharePtr<CTexture> > textureMap;
        textureMap[0] = m_backgroundFrag.GetTextureFrag()->GetTexture();
        pBackGroundBatch->SetTextureMap(textureMap);
        static unsigned short triangleIndex[6] = { 0, 1, 2, 2, 3, 0 };
        pBackGroundBatch->AddIndexedVertices(m_quadPT, 4, triangleIndex, 6, nullptr);
#ifdef DEVELOP_VERSION
        pBackGroundBatch->m_usage = ERenderBatchUsage::eRBU_BackGround;
#endif
    ASSUME_VARIABLE_IN_EDITOR_END
}

void C2DBackGround::SetProgress(const CVec2& progress)
{
    BEATS_ASSERT(BEATS_FLOAT_LESS_EQUAL(progress.X(), 1.0f) && BEATS_FLOAT_GREATER_EQUAL(progress.X(), 0));
    BEATS_ASSERT(BEATS_FLOAT_LESS_EQUAL(progress.Y(), 1.0f) && BEATS_FLOAT_GREATER_EQUAL(progress.Y(), 0));
    m_progressInCoord = progress;
    UpdateQuad();
}

void C2DBackGround::SetUVSize(const CVec2& size)
{
    m_uvSize = size;
    UpdateQuad();
}

const CVec2& C2DBackGround::GetInitUVSize() const
{
    return m_initUVSize;
}

const CVec2& C2DBackGround::GetSizeScaleForFov() const
{
    return m_sizeScaleForFov;
}

void C2DBackGround::UpdateQuad()
{
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_backgroundFrag.GetTextureFrag() != nullptr)
        CRenderTarget* pCurrRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
        BEATS_ASSERT(pCurrRenderTarget != nullptr);
        float fWidth = (float)pCurrRenderTarget->GetDeviceWidth();
        float fHeight = (float)pCurrRenderTarget->GetDeviceHeight();
        BEATS_ASSERT(m_uvSize.X() >= 0 && m_uvSize.X() <= 1.0f);
        BEATS_ASSERT(m_uvSize.Y() >= 0 && m_uvSize.Y() <= 1.0f);
        const CVec2& textureSize = m_backgroundFrag.GetTextureFrag()->GetSize();
        BEATS_ASSERT(textureSize.X() > textureSize.Y(), "This is a art design requirement!");
        CVec2 uvSize = m_uvSize;
        float fMaxUStartValue = 1.0f - uvSize.X();
        float fMaxVStartValue = 1.0f - uvSize.Y();
        float fScaleFactor = fHeight / textureSize.Y();
        float fXOffset = (textureSize.X() * fScaleFactor - fWidth) * 0.5f;
        CVec2 leftUpUV(m_progressInCoord.X() * fMaxUStartValue, m_progressInCoord.Y() * fMaxVStartValue + uvSize.Y());
        leftUpUV = m_backgroundFrag.GetTextureFrag()->ConvertToRealUV(leftUpUV);
        m_quadPT[0].position = CVec3(-fXOffset, 0, 0);
        m_quadPT[0].tex.u = leftUpUV.X();
        m_quadPT[0].tex.v = leftUpUV.Y();

        CVec2 leftDownUV(m_progressInCoord.X() * fMaxUStartValue, m_progressInCoord.Y() * fMaxVStartValue);
        leftDownUV = m_backgroundFrag.GetTextureFrag()->ConvertToRealUV(leftDownUV);
        m_quadPT[1].position = CVec3(-fXOffset, fHeight, 0);
        m_quadPT[1].tex.u = leftDownUV.X();
        m_quadPT[1].tex.v = leftDownUV.Y();

        CVec2 rightDownUV(m_progressInCoord.X() * fMaxUStartValue + uvSize.X(), m_progressInCoord.Y() * fMaxVStartValue);
        rightDownUV = m_backgroundFrag.GetTextureFrag()->ConvertToRealUV(rightDownUV);
        m_quadPT[2].position = CVec3(fWidth + fXOffset, fHeight, 0);
        m_quadPT[2].tex.u = rightDownUV.X();
        m_quadPT[2].tex.v = rightDownUV.Y();

        CVec2 rightUpUV(m_progressInCoord.X() * fMaxUStartValue + uvSize.X(), m_progressInCoord.Y() * fMaxVStartValue + uvSize.Y());
        rightUpUV = m_backgroundFrag.GetTextureFrag()->ConvertToRealUV(rightUpUV);
        m_quadPT[3].position = CVec3(fWidth + fXOffset, 0, 0);
        m_quadPT[3].tex.u = rightUpUV.X();
        m_quadPT[3].tex.v = rightUpUV.Y();
    ASSUME_VARIABLE_IN_EDITOR_END
}
