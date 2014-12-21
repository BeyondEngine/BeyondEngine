#ifndef BEYOND_ENGINE_RENDER_VERTEXBUFFERCONTENT_H__INCLUDE
#define BEYOND_ENGINE_RENDER_VERTEXBUFFERCONTENT_H__INCLUDE

class CSerializer;
class CRenderBatch;

class CVertexBufferContent
{
public:
    CVertexBufferContent();
    ~CVertexBufferContent();

    CSerializer* GetVertexBuffer();
    CSerializer* GetIndexBuffer();
    uint32_t GetArrayVAO() const;
    void SetArrayVAO(uint32_t uArrayVAO);
    uint32_t GetIndexVAO() const;
    void SetIndexVAO(uint32_t uIndexVAO);
    uint32_t GetVBO() const;
    void SetVBO(uint32_t uVBO);
    uint32_t GetEBO() const;
    void SetEBO(uint32_t uEBO);
    void SyncData();
    void Reset();
private:
    uint32_t m_uArrayVAO = 0;
    uint32_t m_uIndexVAO = 0;
    uint32_t m_uVBO = 0;
    uint32_t m_uEBO = 0;
    CSerializer* m_pVertexBuffer = nullptr;
    CSerializer* m_pIndexBuffer = nullptr;
};

#endif