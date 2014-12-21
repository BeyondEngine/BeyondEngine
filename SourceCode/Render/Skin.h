#ifndef BEYOND_ENGINE_RENDER_SKIN_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SKIN_H__INCLUDE

struct SSkinBatchInfo
{
    SSkinBatchInfo()
    {
    }
    ~SSkinBatchInfo()
    {
    }
    uint32_t m_uStartPos = 0xFFFFFFFF;
    uint32_t m_uDataSize = 0;
};

class CSkin
{
public:
    CSkin();
    virtual ~CSkin();

    void Initialize();
    void Uninitialize();
    uint32_t GetVAO() const;
    uint32_t GetVBO() const;
    uint32_t GetEBO() const;
    const std::map<std::string, SSkinBatchInfo>& GetBatchInfoMap() const;

    void Decode();

    bool Load(CSerializer& serializer);

    bool IsInitialized() const;
#ifdef DEVELOP_VERSION
    uint32_t m_uVertexCount = 0;
#endif
private:
    std::map<std::string, SSkinBatchInfo> m_batchInfoMap;
    CSerializer* m_pVertexData;
    CSerializer* m_pIndexData;
    uint32_t m_uVAO;
    uint32_t m_uVBO;
    uint32_t m_uEBO;

    bool m_bIsInitialized;
};

#endif