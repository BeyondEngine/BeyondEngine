#include "stdafx.h"
#include "RenderModule.h"
#include "Render/Material.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Render/RenderGroup.h"
#include "Render/VertexFormat.h"
#include "Render/Texture.h"
#include "ParticleEmitter.h"
#include "Render/RenderBatch.h"
#include "Render/ReflectTextureInfo.h"
#include "Render/TextureAtlas.h"

CRenderModule::CRenderModule()
{

}

CRenderModule::~CRenderModule()
{
}

void CRenderModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bBlendOrAdd, true, 0xFFFFFFFF, "混合或者叠加", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_mode, true, 0xFFFFFFFF, "渲染模式", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_fSpeedScale, true, 0xFFFFFFFF, "速度缩放", nullptr, nullptr, "VisibleWhen:m_mode==ePRM_StretchedBillboard");
    DECLARE_PROPERTY(serializer, m_fLengthScale, true, 0xFFFFFFFF, "尺寸缩放", nullptr, nullptr, "VisibleWhen:m_mode==ePRM_StretchedBillboard");
    DECLARE_PROPERTY(serializer, m_sortMode, true, 0xFFFFFFFF, "排序模式", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_texture, true, 0xFFFFFFFF, "纹理", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_tintColor, true, 0xFFFFFFFF, "tintColor", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_textureOffset, true, 0xFFFFFFFF, "纹理偏移", nullptr, "x,y代表Tiling;z,w代表Offset", "DefaultValue:1@1@0@0");
}

bool CRenderModule::Load()
{
    bool bRet = super::Load();
    if (m_texture.GetTextureFrag() != nullptr)
    {
        m_renderTextureMap[0] = m_texture.GetTextureFrag()->GetAtlas()->Texture();
#if defined(DEVELOP_VERSION)
        if (!CFilePathTool::GetInstance()->Exists(m_renderTextureMap[0]->GetFilePath().c_str()))
        {
            CParticleEmitter* pOwner = down_cast<CParticleEmitter*>(GetReflectOwner());
            CComponentInstance* pRootOwner = pOwner;
            while (pRootOwner->GetId() == 0xFFFFFFFF)
            {
                pRootOwner = pRootOwner->GetReflectOwner();
            }
        }
#endif
    }
    CShaderUniform& colorUniform = m_uniformMap["TintColor"];
    colorUniform.SetName("TintColor");
    colorUniform.SetType(eSUT_4f);
    std::vector<float>& colorData = colorUniform.GetData();
    BEATS_ASSERT(colorData.size() == 4);
    colorData[0] = m_tintColor.r / 255.0f;
    colorData[1] = m_tintColor.g / 255.0f;
    colorData[2] = m_tintColor.b / 255.0f;
    colorData[3] = m_tintColor.a / 255.0f;

    CShaderUniform& textureOffsetUniform = m_uniformMap["_MainTex_ST"];
    textureOffsetUniform.SetName("_MainTex_ST");
    textureOffsetUniform.SetType(eSUT_4f);
    std::vector<float>& textureOffsetData = textureOffsetUniform.GetData();
    BEATS_ASSERT(textureOffsetData.size() == 4);
    textureOffsetData[0] = m_textureOffset.X();
    textureOffsetData[1] = m_textureOffset.Y();
    textureOffsetData[2] = m_textureOffset.Z();
    textureOffsetData[3] = m_textureOffset.W();
    return bRet;
}
#ifdef EDITOR_MODE
bool CRenderModule::OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pNewValueToBeSet);
    if (!bRet)
    {
        if (pVariableAddr == &m_texture)
        {
            DeserializeVariable(m_texture, pNewValueToBeSet, this);
            m_renderTextureMap[0] = m_texture.GetTextureFrag()->GetAtlas()->Texture();
            bRet = true;
        }
        else if (pVariableAddr == &m_tintColor)
        {
            DeserializeVariable(m_tintColor, pNewValueToBeSet, this);
            CShaderUniform& colorUniform = m_uniformMap["TintColor"];
            colorUniform.SetName("TintColor");
            colorUniform.SetType(eSUT_4f);
            std::vector<float>& colorData = colorUniform.GetData();
            BEATS_ASSERT(colorData.size() == 4);
            colorData[0] = m_tintColor.r / 255.0f;
            colorData[1] = m_tintColor.g / 255.0f;
            colorData[2] = m_tintColor.b / 255.0f;
            colorData[3] = m_tintColor.a / 255.0f;
            bRet = true;
        }
        else if (pVariableAddr == &m_textureOffset)
        {
            DeserializeVariable(m_textureOffset, pNewValueToBeSet, this);
            CShaderUniform& textureOffsetUniform = m_uniformMap["_MainTex_ST"];
            textureOffsetUniform.SetName("_MainTex_ST");
            textureOffsetUniform.SetType(eSUT_4f);
            std::vector<float>& textureOffsetData = textureOffsetUniform.GetData();
            BEATS_ASSERT(textureOffsetData.size() == 4);
            textureOffsetData[0] = m_textureOffset.X();
            textureOffsetData[1] = m_textureOffset.Y();
            textureOffsetData[2] = m_textureOffset.Z();
            textureOffsetData[3] = m_textureOffset.W();
            bRet = true;
        }
    }
    return bRet;
}
#endif
EParticleRenderMode CRenderModule::GetRenderMode() const
{
    return m_mode;
}

EParticleSortMode CRenderModule::GetSortMode() const
{
    return m_sortMode;
}

SharePtr<CMaterial> CRenderModule::GetMaterial(bool bEnableDepthTest) const
{
    return m_bBlendOrAdd ? CRenderManager::GetInstance()->GetParticleAlphaBlendMaterial(bEnableDepthTest) : CRenderManager::GetInstance()->GetParticleAdditiveMaterial(bEnableDepthTest);
}

const CVec4& CRenderModule::GetTextureOffset() const
{
    return m_textureOffset;
}

float CRenderModule::GetLengthScale() const
{
    return m_fLengthScale;
}

float CRenderModule::GetSpeedScale() const
{
    return m_fSpeedScale;
}

CRenderBatch* CRenderModule::RequestRenderBatch(bool bEnableDepthTest, ERenderGroupID groupId)
{
    CRenderGroup *pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(groupId);
    CRenderBatch* pRenderBatch = pRenderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPTC), GetMaterial(bEnableDepthTest), GL_TRIANGLES, true, false, &m_renderTextureMap, &m_uniformMap);
    return pRenderBatch;
}

SharePtr<CTextureFrag> CRenderModule::GetTextureFrag()
{
    return m_texture.GetTextureFrag();
}