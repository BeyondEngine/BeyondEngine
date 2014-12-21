#include "stdafx.h"
#include "Sprite.h"
#include "Texture.h"
#include "RenderManager.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "RenderGroup.h"
#include "RenderBatch.h"
#include "TextureAtlas.h"

CSprite::CSprite()
    : m_bBlendAdd(false)
    , m_bUseInheritColorScale(true)
    , m_pParentSprite(NULL)
{
    m_defaultGroupID = LAYER_Sprite;
}

CSprite::~CSprite()
{
    if (GetParentSprite() != NULL)
    {
        m_pParentSprite->RemoveChildSprite(this);
    }
    std::vector<CSprite*> childrenbak = m_childrenSprite;
    for (uint32_t i = 0; i < childrenbak.size(); ++i)
    {
        RemoveChildSprite(childrenbak[i]);
    }
}

void CSprite::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bBlendAdd, true, 0xFFFFFFFF, _T("颜色叠加"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fragInfo, true, 0xFFFFFFFF, _T("纹理"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_anchor, true, 0xFFFFFFFF, _T("锚点"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_size, true, 0xFFFFFFFF, _T("尺寸"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_userDefinePos, true, 0xFFFFFFFF, _T("用户定义点"), NULL, NULL, NULL);
    DECLARE_DEPENDENCY_LIST(serializer, m_childrenSpriteDependencyList, _T("子精灵"), eDT_Weak);
    HIDE_PROPERTY(m_userDefinePos3D);
}
#ifdef EDITOR_MODE
bool CSprite::OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent)
{
    bool bRet = super::OnDependencyListChange(pComponentAddr, action, pComponent);
    if (!bRet)
    {
        if (pComponentAddr == &m_childrenSpriteDependencyList)
        {
            auto iter = std::find(m_childrenSpriteDependencyList.begin(), m_childrenSpriteDependencyList.end(), pComponent);
            if (action == eDCA_Add)
            {
                CSprite* pChildSprite = down_cast<CSprite*>(pComponent);
                BEATS_ASSERT(iter == m_childrenSpriteDependencyList.end());
                m_childrenSpriteDependencyList.push_back(pChildSprite);
                AddChildSprite(pChildSprite);
            }
            else
            {
                BEATS_ASSERT(action == eDCA_Delete);
                CSprite* pChildSprite = static_cast<CSprite*>(pComponent);
                BEATS_ASSERT(iter != m_childrenSpriteDependencyList.end());
                m_childrenSpriteDependencyList.erase(iter);
                RemoveChildSprite(pChildSprite);
            }
            bRet = true;
        }
    }
    return bRet;
}

bool CSprite::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bHandled = super::OnPropertyChange(pVariableAddr, pSerializer);
    if(!bHandled)
    {
        if(pVariableAddr == &m_fragInfo)
        {
            DeserializeVariable(m_fragInfo, pSerializer, this);
            bHandled = true;
            UpdateRealSize();
        }
        else if (pVariableAddr == &m_size)
        {
            CVec2 size;
            DeserializeVariable(size, pSerializer, this);
            bHandled = true;
            SetSize(size.X(), size.Y());
        }
        else if (pVariableAddr == &m_anchor)
        {
            CVec2 anchor;
            DeserializeVariable(anchor, pSerializer, this);
            bHandled = true;
            SetAnchor(anchor.X(), anchor.Y());
        }
    }
    return bHandled;
}
#endif
bool CSprite::Load()
{
    UpdateRealSize();
    for (uint32_t i = 0; i < m_childrenSpriteDependencyList.size(); ++i)
    {
        AddChildSprite(m_childrenSpriteDependencyList[i]);
    }
	return super::Load();
}

const CQuadP &CSprite::GetQuadP() const
{
    return m_quad;
}

const CQuadT &CSprite::GetQuadT() const
{
    BEATS_ASSERT(m_fragInfo.GetTextureFrag());
    return m_fragInfo.GetTextureFrag()->GetQuadT();
}

SharePtr<CTextureFrag> CSprite::GetTextureFrag() const
{
    return m_fragInfo.GetTextureFrag();
}

CSprite* CSprite::CloneSprite(bool bResurcive, std::vector<CComponentInstance*>& newNodeList, std::vector<CComponentInstance*>& clonedNodeList)
{
    CSprite* pRet = down_cast<CSprite*>(CloneInstance());
    newNodeList.push_back(pRet);
    clonedNodeList.push_back(this);
    if (bResurcive)
    {
        for (uint32_t i = 0; i < m_childrenSpriteDependencyList.size(); ++i)
        {
            CSprite* pNewChildSprite = m_childrenSpriteDependencyList[i]->CloneSprite(true, newNodeList, clonedNodeList);
            pRet->m_childrenSpriteDependencyList.push_back(pNewChildSprite);
            pRet->RegisterReflectComponent(pNewChildSprite); // the pNewChildSprite clones from dependency, we register it as a reflect component, so we won't worry about it's destruct time.
        }
    }
#ifdef EDITOR_MODE
    pRet->SetSyncProxyComponent(GetProxyComponent());
#endif
    return pRet;
}

void CSprite::Play(bool /*bPause*/, bool /*bContinuePlay*/, bool /*bReversePlay*/, uint32_t /*uStartIndex*/, float /*fSpeed*/)
{
    // Do nothing.
}

bool CSprite::IsAnimationPlaying() const
{
    return false;
}

void CSprite::SetUseInheritColorScale(bool bUse)
{
    m_bUseInheritColorScale = bUse;
}

void CSprite::AddChildSprite(CSprite* pChild)
{
    BEATS_ASSERT(pChild != NULL);
    AddChild(pChild);
    m_childrenSprite.push_back(pChild);
    pChild->SetParentSprite(this);
}

void CSprite::RemoveChildSprite(CSprite* pChild)
{
    RemoveChild(pChild);
    auto iter = std::find(m_childrenSprite.begin(), m_childrenSprite.end(), pChild);
    BEATS_ASSERT(iter != m_childrenSprite.end());
    m_childrenSprite.erase(iter);
    pChild->SetParentSprite(NULL);
}

void CSprite::SetParentSprite(CSprite* pParentSprite)
{
    m_pParentSprite = pParentSprite;
}

CSprite* CSprite::GetParentSprite() const
{
    return m_pParentSprite;
}

CSprite* CSprite::GetUserDefinePos(const TString& strPointName, bool bWorldOrLocal, CVec3& outPos) const
{
    CSprite* pRet = nullptr;
    BEYONDENGINE_UNUSED_PARAM(strPointName);
    BEYONDENGINE_UNUSED_PARAM(bWorldOrLocal);
    BEYONDENGINE_UNUSED_PARAM(outPos);
    return pRet;
}

#ifdef DEVELOP_VERSION
void CSprite::RenderEdge() const
{
    SharePtr<CMaterial> pDefaultMaterial = CRenderManager::GetInstance()->GetDefaultMaterial();
    CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup();
    CRenderBatch* pBatch = pRenderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPC), pDefaultMaterial, GL_LINES, true);
    const CQuadP& quadp = GetQuadP();
    CQuadPC quadLine;
    quadLine.tl.position = quadp.tl;
    quadLine.tr.position = quadp.tr;
    quadLine.bl.position = quadp.bl;
    quadLine.br.position = quadp.br;
    CColor lineColor = 0xFFFFFF7F;
    quadLine.tl.color = lineColor;
    quadLine.tr.color = lineColor;
    quadLine.bl.color = lineColor;
    quadLine.br.color = lineColor;
    unsigned short indices[] = { 0, 1, 1, 3, 3, 2, 2, 0 };
    pBatch->AddIndexedVertices((const CVertexPC *)&quadLine, 4, indices, 8, &GetWorldTM());

#ifdef EDITOR_MODE
    if (m_fragInfo.GetTextureFrag() != nullptr && m_fragInfo.GetTextureFrag()->m_bTrimmed)
    {
        CVec2 originSize = m_fragInfo.GetTextureFrag()->m_originSize;
        quadLine.tl.position = CVec3(originSize.X() * -0.5f, 0, -originSize.Y());
        quadLine.tr.position = quadLine.tl.position + CVec3(originSize.X(), 0, 0);
        quadLine.bl.position = quadLine.tl.position + CVec3(0, 0, originSize.Y());
        quadLine.br.position = quadLine.tl.position + CVec3(originSize.X(), 0, originSize.Y());
        CColor lineColor = 0xFF0000FF;
        quadLine.tl.color = lineColor;
        quadLine.tr.color = lineColor;
        quadLine.bl.color = lineColor;
        quadLine.br.color = lineColor;
        CMat4 mat = GetWorldTM();
        mat.RemoveTranslate();
        pBatch->AddIndexedVertices((const CVertexPC *)&quadLine, 4, indices, 8, &mat);
    }
#endif
}

void CSprite::RenderUserDefinePos() const
{
    for (auto iter = m_userDefinePos.begin(); iter != m_userDefinePos.end(); ++iter)
    {
        CVertexPC point;
        point.position = iter->second * GetWorldTM();
        point.color = 0xFF0000FF;
        CRenderManager::GetInstance()->RenderPoints(point, 5.0f, true);
    }
}
#endif
#ifdef EDITOR_MODE
void CSprite::ClearPos()
{
    CVec3 emptyPos;
    UPDATE_PROXY_PROPERTY_BY_NAME(this, emptyPos, "m_pos");
    for (size_t i = 0; i < m_childrenSprite.size(); ++i)
    {
        m_childrenSprite[i]->ClearPos();
    }
}

void CSprite::ClearAnchor()
{
    CVec2 emptyAnchor;
    UPDATE_PROXY_PROPERTY_BY_NAME(this, emptyAnchor, "m_anchor");
    for (size_t i = 0; i < m_childrenSprite.size(); ++i)
    {
        m_childrenSprite[i]->ClearAnchor();
    }

}

void CSprite::ClearScale()
{
    CVec3 emptyScale(1.0f, 1.0f, 1.0f);
    UPDATE_PROXY_PROPERTY_BY_NAME(this, emptyScale, "m_scale");
    for (size_t i = 0; i < m_childrenSprite.size(); ++i)
    {
        m_childrenSprite[i]->ClearScale();
    }
}
#endif

void CSprite::DoRender()
{
#ifdef DEVELOP_VERSION
    if (!CRenderManager::GetInstance()->m_bRenderSprite)
    {
        return;
    }
#endif
    if (IsVisible())
    {
        SharePtr<CTextureFrag> pFrag = GetTextureFrag();
        if (pFrag && pFrag->GetTexture())
        {
            CRenderGroup *renderGroup = CRenderGroupManager::GetInstance()->GetRenderGroup();
            BEATS_ASSERT(renderGroup);
            CColor scaleColor = GetColorScale(m_bUseInheritColorScale);
            bool bUseColor = scaleColor.r != 100 || scaleColor.g != 100 || scaleColor.b != 100 || scaleColor.a != 100;
            SharePtr<CMaterial> pSpriteMaterial = CRenderManager::GetInstance()->GetSpriteMaterial(m_bBlendAdd, bUseColor);
            std::map<unsigned char, SharePtr<CTexture> > textureMap;
            textureMap[0] = pFrag->GetTexture();
            CRenderBatch *batch = renderGroup->GetRenderBatch(bUseColor ? VERTEX_FORMAT(CVertexPTC) : VERTEX_FORMAT(CVertexPT), pSpriteMaterial, GL_TRIANGLES, true, true, &textureMap);
            if (bUseColor)
            {
                batch->AddQuad(&m_quad, &GetQuadT(), scaleColor, &GetWorldTM());
            }
            else
            {
                batch->AddQuad(&m_quad, &GetQuadT(), &GetWorldTM());
            }
#ifdef DEVELOP_VERSION
            if (batch->m_usage == ERenderBatchUsage::eRBU_Count)
            {
                batch->m_usage = ERenderBatchUsage::eRBU_Sprite;
            }
            BEATS_ASSERT(batch->m_usage == ERenderBatchUsage::eRBU_Sprite);
#endif
        }
#ifdef DEVELOP_VERSION
        if (CRenderManager::GetInstance()->m_bRenderSpriteEdge)
        {
            RenderEdge();
        }
        if (CRenderManager::GetInstance()->m_bRenderSpriteUserDefinePos || m_bRenderUserdefinePos)
        {
            RenderUserDefinePos();
        }
#endif
    }
}

void CSprite::PostRender()
{
    for (uint32_t i = 0; i < m_childrenSprite.size(); ++i)
    {
        m_childrenSprite[i]->Render();
    }
    if (m_bNeedPopGroupID)
    {
        m_bNeedPopGroupID = false;
        CRenderGroupManager::GetInstance()->PopRenderGroupID();
    }
}

bool CSprite::HitTest(const CVec2& pt)
{
    bool bHasTexture = m_fragInfo.GetTextureFrag() != nullptr;
    CRay3 selectRay = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->GetRayFromScreenPos(pt.X(), pt.Y());
    CQuadP quad = GetQuadP();
    const CMat4& worldTM = GetWorldTM();
    quad.bl *= worldTM;
    quad.br *= worldTM;
    quad.tl *= worldTM;
    quad.tr *= worldTM;
    CVec3 hitRet;
    bool bRet = bHasTexture && IsVisible() && (selectRay.IntersectTriangleBothSide(quad.tl, quad.tr, quad.bl, hitRet) || selectRay.IntersectTriangleBothSide(quad.bl, quad.br, quad.tr, hitRet));
    if (!bRet)
    {
        for (size_t i = 0; i < m_childrenSprite.size(); ++i)
        {
            bRet = m_childrenSprite[i]->HitTest(pt);
            if (bRet)
            {
                break;
            }
        }
    }
    return bRet;
}

void CSprite::SetAnchor(float anchorX, float anchorY)
{
    if (!BEATS_FLOAT_EQUAL(anchorX, m_anchor.X()) ||
        !BEATS_FLOAT_EQUAL(anchorY, m_anchor.Y()))
    {
        m_anchor.X() = anchorX;
        m_anchor.Y() = anchorY;
        UpdateVertices();
    }
}

const CVec2 &CSprite::GetAnchor() const
{
    return m_anchor;
}

void CSprite::SetSize(float width, float height)
{
    m_size.X() = width;
    m_size.Y() = height;
    UpdateRealSize();
}

const CVec2 &CSprite::GetSize() const
{
    return m_size;
}

const CVec2& CSprite::GetRealSize() const
{
    return m_realSize;
}

void CSprite::UpdateVertices()
{
    CVec2 realAnchor = m_anchor * m_realSize;

    // Set vertex, lay on X-Z plane.
    m_quad.tl = CVec3(-realAnchor.X(), 0, -realAnchor.Y());
    CVec2 brPos = m_realSize - realAnchor;
    m_quad.br = CVec3(brPos.X(), 0, brPos.Y());
    m_quad.tr.X() = m_quad.br.X();
    m_quad.tr.Y() = 0;
    m_quad.tr.Z() = m_quad.tl.Z();
    m_quad.bl.X() = m_quad.tl.X();
    m_quad.bl.Y() = 0;
    m_quad.bl.Z() = m_quad.br.Z();
}

CRect CSprite::GetSelfRect() const
{
    CVec2 spriteSize = GetRealSize();
    const CMat4& worldTM = GetWorldTM();
    spriteSize.X() *= worldTM[0];
    spriteSize.Y() *= worldTM[10];
    CRect rect;
    if( spriteSize.X() > 0 && spriteSize.Y() > 0 )
    {
        const CVec2& selfAnchor = GetAnchor();
        const CVec3& position = GetWorldPosition();
        CVec2 offset(position.X(), position.Z());
        CVec2 positon = CVec2(-selfAnchor.X() * spriteSize.X() + offset.X(), -selfAnchor.Y() * spriteSize.Y() + offset.Y());
        rect = CRect(positon, spriteSize);
    }
    return rect;
}

CRect CSprite::GetBoundingRect() const
{
    CRect rect = GetSelfRect();
    std::vector< CSprite*> spriteList;
    GetCurrentRenderSpriteList(spriteList);
    for (uint32_t i = 0; i < spriteList.size(); ++i)
    {
        CSprite* pChildSprite = spriteList[i];
        if( rect.size.X() > 0 && rect.size.Y() > 0 )
        {
            rect = MerageRect(rect, pChildSprite->GetBoundingRect());
        }
        else
        {
            rect = pChildSprite->GetBoundingRect();
        }
    }
    return rect;
}

void CSprite::GetCurrentRenderSpriteList(std::vector<CSprite*>& spriteList) const
{
    for (auto child : m_childrenSprite)
    {
        if( child->IsVisible() )
        {
            spriteList.push_back(child);
        }
    }
}

void CSprite::UpdateRealSize()
{
    float fRealWidth = m_size.X();
    float fRealHeight = m_size.Y();
    if (BEATS_FLOAT_EQUAL(fRealHeight, 0))
    {
        SharePtr<CTextureFrag> pTextureFrag = GetTextureFrag();
        if (pTextureFrag && pTextureFrag->GetTexture())
        {
            const CVec2& textureSize = pTextureFrag->GetSize();
            if (BEATS_FLOAT_EQUAL(fRealWidth, 0))
            {
                fRealWidth = textureSize.X();
                fRealHeight = textureSize.Y();
            }
            else
            {
                if (textureSize.X() > textureSize.Y())
                {
                    fRealWidth = m_size.X();
                    fRealHeight = fRealWidth * textureSize.Y() / textureSize.X();
                }
                else
                {
                    fRealHeight = m_size.X();
                    fRealWidth = fRealHeight * textureSize.X() / textureSize.Y();
                }
            }
        }
    }
    if (!BEATS_FLOAT_EQUAL(m_realSize.X(), fRealWidth) ||
        !BEATS_FLOAT_EQUAL(m_realSize.Y(), fRealHeight))
    {
        m_realSize.X() = fRealWidth;
        m_realSize.Y() = fRealHeight;
        UpdateVertices();
    }
}

const std::vector<CSprite*>& CSprite::GetChildSpriteList() const
{
    return m_childrenSprite;
}

CSprite* CSprite::GetChildSpriteByName(const TString& name, bool bRecursion /*= true*/)
{
    CSprite* pRet = nullptr;
    for (auto sprite : m_childrenSprite)
    {
        if( sprite->GetName() == name )
        {
            pRet = sprite;
        }
        else
        {
            if( bRecursion )
            {
                pRet = sprite->GetChildSpriteByName(name, bRecursion);
            }
        }
        if( pRet )
        {
            break;
        }
    }
    return pRet;
}

void CSprite::SetTextureFragInfo(const SReflectTextureInfo& info)
{
    m_fragInfo = info;
    UpdateRealSize();
}

void CSprite::SetCurrFrame(uint32_t currFrame)
{
    for (auto iter : m_childrenSprite)
    {
        iter->SetCurrFrame(currFrame);
    }
}

uint32_t CSprite::GetFrameCount() const
{
    uint32_t uRet = 0;
    for (auto sprite : m_childrenSprite)
    {
        uint32_t uCount = sprite->GetFrameCount();
        if (uCount > uRet)
        {
            uRet = uCount;
        }
    }
    return uRet;
}
