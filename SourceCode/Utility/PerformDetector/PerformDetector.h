#ifndef BEYOND_ENGINE_UTILITY_PERFORMDETECTOR_PERFORMDETECTOR_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_PERFORMDETECTOR_PERFORMDETECTOR_H__INCLUDE

#include <stack>

#define BEYONDENGINE_ENABLE_PERFORMANCE 0

#if defined(EDITOR_MODE) && !defined(_WIN64) && BEYONDENGINE_ENABLE_PERFORMANCE
#define BEYONDENGINE_PERFORMDETECT_STOP(type) {CPerformDetector::GetInstance()->StopDetectNode(type);};
#define BEYONDENGINE_PERFORMDETECT_START(type) {int eipValue=0; BEATS_ASSI_GET_EIP(eipValue);CPerformDetector::GetInstance()->StartDetectNode(type, eipValue);};
#define BEYONDENGINE_PERFORMDETECT_RESET() {CPerformDetector::GetInstance()->ResetFrameResult();};
#else
#ifdef DEVELOP_VERSION
#define BEYONDENGINE_PERFORMDETECT_STOP(type)\
    {\
        if (CEngineCenter::GetInstance()->m_uNestPerformCounter == 1)\
        {\
            uint32_t uCurrTimeMs = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000); \
            auto iter = CEngineCenter::GetInstance()->m_performMap.find((uint32_t)type); \
            BEATS_ASSERT(iter != CEngineCenter::GetInstance()->m_performMap.end()); \
            iter->second.second += (uCurrTimeMs - iter->second.first); \
        }\
        CEngineCenter::GetInstance()->m_uNestPerformCounter--;\
    }
#define BEYONDENGINE_PERFORMDETECT_START(type)\
    {\
if (CEngineCenter::GetInstance()->m_uNestPerformCounter == 0)\
        {\
        uint32_t uCurrTimeMs = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000); \
        CEngineCenter::GetInstance()->m_performMap[(uint32_t)type].first = uCurrTimeMs; \
        }\
        CEngineCenter::GetInstance()->m_uNestPerformCounter++; \
    }
#define BEYONDENGINE_PERFORMDETECT_RESET() \
    {\
        if (CEngineCenter::GetInstance()->m_bClearPerformRequest)\
        {\
            CEngineCenter::GetInstance()->m_bClearPerformRequest = false;\
            CEngineCenter::GetInstance()->m_performMap.clear();\
            CEngineCenter::GetInstance()->m_uTotalRunningTimeMS = 100;\
            BEATS_ASSERT(CEngineCenter::GetInstance()->m_uNestPerformCounter == 0);\
        }\
    }
#else
#define BEYONDENGINE_PERFORMDETECT_STOP(type)
#define BEYONDENGINE_PERFORMDETECT_START(type)
#define BEYONDENGINE_PERFORMDETECT_RESET()
#endif
#endif

#ifdef DEVELOP_VERSION
enum EPerformNodeType
{
    ePNT_Test1,
    ePNT_Test2,
    ePNT_Test3,
    ePNT_Test4,
    ePNT_Test5,
    ePNT_Update,
    ePNT_Render,
    ePNT_Editor,
    ePNT_UpdateCamera,
    ePNT_ApplyViewPort,
    ePNT_RenderScene,
    ePNT_UISystemManagerRender,
    ePNT_RenderManagerRender,
    ePNT_EngineCenterUpdate,
    ePNT_AnimationManager,
    ePNT_AnimationManager_CalcMatrix,
    ePNT_AnimationManager_Blend,
    ePNT_AnimationManager_DeltaMatrix,
    ePNT_GUI_Update,
    ePNT_2DSkeleton_AnimationManager,
    ePNT_CSenceManager,
    ePNT_ParticleUpdate,
    ePNT_NodeAnimation,
    ePNT_Scene2DNodeUpdate,
    ePNT_Scene3DNodeUpdate,
    ePNT_SceneUpdateAction,
    ePNT_BattleManager,
    ePNT_StarRaidersUpdate,

    ePNT_ObjectRender,
    ePNT_GUI,
    ePNT_ParticleRender,
    ePNT_3D,
    ePNT_2D,
    ePNT_GUIRender,
    ePNT_SetGrid9VerticesAndIndices,
    ePNT_CreateRenderBatch,
    ePNT_RenderBatchUpdateVBO,
    ePNT_RenderBatchUpdateVBO0,
    ePNT_RenderBatchUpdateVBO1,
    ePNT_RenderBatchRender,
    ePNT_GroupRender,
    EPNT_SyncVertexDataBuffer,
    ePNT_RenderLineAndTriangle,
    ePNT_RenderClear,
    ePNT_RenderText,
    ePNT_RenderText1,
    ePNT_RenderText2,
    ePNT_RenderText3,
    ePNT_RenderText4,
    ePNT_RenderText5,
    ePNT_RenderSceneWindow,
    ePNT_RenderLogManager,
    
    ePNT_RenderComponentWindow,
    ePNT_ComponentWindow_Prepare,
    ePNT_ComponentWindow_Grid,
    ePNT_ComponentWindow_DraggingLine,
    ePNT_ComponentWindow_Components,
    ePNT_ComponentWindow_ComponnetInstance,
    ePNT_ComponentWindow_InstanceHead,
    ePNT_ComponentWindow_InstanceHead_1,
    ePNT_ComponentWindow_InstanceHead_2,
    ePNT_ComponentWindow_InstanceHead_2_1,
    ePNT_ComponentWindow_InstanceHead_2_2,
    ePNT_ComponentWindow_InstanceHead_3,
    ePNT_ComponentWindow_InstanceHead_4,
    ePNT_ComponentWindow_InstanceHead_5,

    ePNT_ComponentWindow_Dependency,
    ePNT_ComponentWindow_DependencyLine,
    ePNT_ComponentWindow_SelectRect,
    ePNT_ComponentWindow_RenderGroup,

    ePNT_Animation2D,
    ePNT_MaterialCompare,
    ePNT_MaterialUse,
    ePNT_MaterialUse_SetRenderState,
    ePNT_MaterialUse_ApplyGPU,

    ePNT_PerformDetector,

    ePNT_Count,
    ePNT_Force32Bit = 0xFFFFFFFF
};

static const TCHAR* pszPerformNodeStr[] = 
{
    _T("Test1"),
    _T("Test2"),
    _T("Test3"),
    _T("Test4"),
    _T("Test5"),
    _T("Update"),
    _T("Render"),
    _T("Editor"),
    _T("UpdateCamera"),
    _T("ApplyViewPort"),
    _T("RenderScene"),
    _T("UISystemManagerRender"),
    _T("RenderManagerRender"),
    _T("Engine Center Update"),
    _T("AnimationManager"),
    _T("AnimationManager_CalcMatrix"),
    _T("AnimationManager_Blend"),
    _T("AnimationManager_DeltaMatrix"),
    _T("GUI_Update"),
    _T("2DSkeleton_AnimationManager"),
    _T("CSceneManager"),
    _T("ParticleUpdate"),
    _T("NodeAnimation"),
    _T("Scene2DNodeUpdate"),
    _T("Scene3DNodeUpdate"),
    _T("SceneUpdateAction"),
    _T("BattleManager"),
    _T("StarRaidersUpdate"),

    _T("RenderGrid"),
    _T("ObjectRender"),
    _T("GUI"),
    _T("ParticleRender"),
    _T("3D"),
    _T("2D"),
    _T("GUIRender"),
    _T("CreateRenderBatch"),
    _T("RenderBatchUpdateVBO"),
    _T("RenderBatchUpdateVBO0"),
    _T("RenderBatchUpdateVBO1"),
    _T("RenderBatch_Render"),
    _T("GroupRender"),
    _T("SyncVertexDataBuffer"),
    _T("RenderLineAndTriangle"),
    _T("RenderClear"),
    _T("RenderText"),
    _T("RenderText1"),
    _T("RenderText2"),
    _T("RenderText3"),
    _T("RenderText4"),
    _T("RenderText5"),
    _T("RenderSceneWindow"),
    _T("RenderLogManager"),

    _T("RenderComponentWindow"),
    _T("CompWnd_Prepare"),
    _T("CompWnd_Grid"),
    _T("CompWnd_DraggingLine"),
    _T("CompWnd_Components"),
    _T("CompWnd_ComponnetInstance"),
    _T("CompWnd_InstanceHead"),
    _T("CompWnd_InstanceHead_1"),
    _T("CompWnd_InstanceHead_2"),
    _T("CompWnd_InstanceHead_2_1"),
    _T("CompWnd_InstanceHead_2_2"),
    _T("CompWnd_InstanceHead_2_3"),
    _T("CompWnd_InstanceHead_3"),
    _T("CompWnd_InstanceHead_4"),
    _T("CompWnd_InstanceHead_5"),

    _T("CompWnd_Dependency"),
    _T("CompWnd_DependencyLine"),
    _T("CompWnd_SelectRect"),
    _T("CompWnd_RenderGroup"),

    _T("Animation2D"),
    _T("MaterialCompare"),
    _T("MaterialUse"),
    _T("MaterialUse_SetRenderState"),
    _T("MaterialUse_ApplyGPU"),

    _T("PerformDetector"),
};

#endif

#ifdef EDITOR_MODE
struct SPerformanceResult
{
    uint32_t id;
    float resultMS;
    LARGE_INTEGER startCount;
};

struct SPerformanceRecord
{
    int type;
    uint32_t id;//this will be an unique id for each record.
    uint32_t updateCount;
    float maxValueMS;
    float totalValueMS;
    SPerformanceRecord* pParent;
    const TCHAR* typeStr;
    std::vector<SPerformanceRecord*> children;

    SPerformanceRecord(uint32_t idParam = 0, int typeParam = 0)
        : id (idParam)
        , updateCount(0)
        , type(typeParam)
        , maxValueMS(0)
        , totalValueMS(0)
        , pParent(NULL)
        , typeStr(NULL)
    {

    }

    void Reset()
    {
        updateCount = 0;
        maxValueMS = 0;
        totalValueMS = 0;
        for (uint32_t i = 0; i < children.size(); ++i)
        {
            children[i]->Reset();
        }
    }
};

class CPerformDetector
{
    BEATS_DECLARE_SINGLETON(CPerformDetector);

public:
    void StartDetectNode(int type, uint32_t id);

    float StopDetectNode(int type);
    void ResetFrameResult();
    void ClearFrameResult();//Put this method at the end of the process.

    void SetTypeName(const TCHAR* typeStr[], uint32_t uCount);
    const TCHAR* GetTypeName(int type);

    SPerformanceRecord* GetRecord(uint32_t id);
    void GetResultThisFrame(std::vector<SPerformanceResult*>& outResult);

    void SetPauseFlag(bool bValue);
    bool PauseSwitcher();
    bool IsPaused();

    void ClearAllResult();
    const std::map<uint32_t, SPerformanceRecord*>& GetRecordMap() const;

private:
    SPerformanceResult* GetResultFromPool();
    void DestroyResultPool();
    void UpdateRecord(SPerformanceResult* pResult);

private:
    SPerformanceRecord* m_pCurRecord;
    bool m_bPause;
    bool m_bPauseRequest;    //We can only switch pause when a frame is end. Save the request.
    bool m_bClearAllRequest; //We can only clear result when a frame is end. Save the request.
    LARGE_INTEGER m_freq;

    std::vector<SPerformanceResult*> m_resultThisFrame;
    std::vector<SPerformanceResult*> m_resultPool;//use a pool to avoid calling delete/new frequently.
    std::stack<SPerformanceResult*> m_resultOrder;
    std::vector<const TCHAR*> m_typeName;
    std::map<uint32_t, SPerformanceRecord*> m_recordMap;
    SPerformanceRecord m_rootRecord;
};

#endif

#endif