#ifndef BEYOND_ENGINE_UTILITY_PERFORMDETECTOR_PERFORMDETECTOR_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_PERFORMDETECTOR_PERFORMDETECTOR_H__INCLUDE

#include <stack>

#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
#define BEYONDENGINE_PERFORMDETECT_STOP(type) {CPerformDetector::GetInstance()->StopDetectNode(type);};
#define BEYONDENGINE_PERFORMDETECT_START(type) {int eipValue=0; BEATS_ASSI_GET_EIP(eipValue);CPerformDetector::GetInstance()->StartDetectNode(type, eipValue);};
#define BEYONDENGINE_PERFORMDETECT_RESET() {CPerformDetector::GetInstance()->ResetFrameResult();};
#define BEYONDENGINE_PERFORMDETECT_SCOPE(type)\
    {\
        int _eip_value_ = 0;\
        BEATS_ASSI_GET_EIP(_eip_value_);\
        CAutoPerformDetector _auto_detector_(type, _eip_value_);\
    };
#else
#define BEYONDENGINE_PERFORMDETECT_STOP(type)
#define BEYONDENGINE_PERFORMDETECT_START(type)
#define BEYONDENGINE_PERFORMDETECT_RESET()
#define BEYONDENGINE_PERFORMDETECT_SCOPE(type)
#endif

#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
enum EPerformNodeType
{
    ePNT_Editor,
    ePNT_UpdateCamera,
    ePNT_Render,
    ePNT_EngineCenterUpdate,
    ePNT_AnimationManager,
    ePNT_AnimationManager_CalcMatrix,
    ePNT_AnimationManager_Blend,
    ePNT_AnimationManager_DeltaMatrix,
    ePNT_GUI_Update,
    ePNT_2DSkeleton_AnimationManager,
    ePNT_CSenceManager,
    ePNT_Scene2DNodeUpdate,
    ePNT_Scene3DNodeUpdate,

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
    ePNT_RenderBatchPreRender,
    ePNT_RenderBatchRender,
    ePNT_RenderBatchPostRender,
    ePNT_GroupRender,
    ePNT_RenderText,
    ePNT_RenderText1,
    ePNT_RenderText2,
    ePNT_RenderText3,
    ePNT_RenderText4,
    ePNT_RenderText5,
    ePNT_RenderText6,
    ePNT_RenderText7,
    ePNT_RenderText8,
    ePNT_RenderText9,
    ePNT_RenderSceneWindow,
    
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
    ePNT_ComponentWindow_InstanceHead_2_3,
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
    _T("Editor"),
    _T("UpdateCamera"),
    _T("Render"),
    _T("Engine Center Update"),
    _T("AnimationManager"),
    _T("AnimationManager_CalcMatrix"),
    _T("AnimationManager_Blend"),
    _T("AnimationManager_DeltaMatrix"),
    _T("GUI_Update"),
    _T("2DSkeleton_AnimationManager"),
    _T("CSceneManager"),
    _T("Scene2DNodeUpdate"),
    _T("Scene3DNodeUpdate"),

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
    _T("RenderBatch_PreRender"),
    _T("RenderBatch_Render"),
    _T("RenderBatch_PostRender"),
    _T("GroupRender"),
    _T("RenderText"),
    _T("RenderText1"),
    _T("RenderText2"),
    _T("RenderText3"),
    _T("RenderText4"),
    _T("RenderText5"),
    _T("RenderText6"),
    _T("RenderText7"),
    _T("RenderText8"),
    _T("RenderText9"),
    _T("RenderSceneWindow"),

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

struct SPerformanceResult
{
    size_t id;
    float result;
    LARGE_INTEGER startCount;
};

struct SPerformanceRecord
{
    int type;
    size_t id;//this will be an unique id for each record.
    size_t updateCount;
    float maxValue;
    float totalValue;
    SPerformanceRecord* pParent;
    const TCHAR* typeStr;
    std::vector<SPerformanceRecord*> children;

    SPerformanceRecord(size_t idParam = 0, int typeParam = 0)
        : id (idParam)
        , updateCount(0)
        , type(typeParam)
        , maxValue(0)
        , totalValue(0)
        , pParent(NULL)
        , typeStr(NULL)
    {

    }

    void Reset()
    {
        updateCount = 0;
        maxValue = 0;
        totalValue = 0;
        for (size_t i = 0; i < children.size(); ++i)
        {
            children[i]->Reset();
        }
    }
};

class CPerformDetector
{
    BEATS_DECLARE_SINGLETON(CPerformDetector);

public:
    void StartDetectNode(int type, size_t id);

    float StopDetectNode(int type);
    void ResetFrameResult();
    void ClearFrameResult();//Put this method at the end of the process.

    void SetTypeName(const TCHAR* typeStr[], size_t uCount);
    const TCHAR* GetTypeName(int type);

    SPerformanceRecord* GetRecord(size_t id);
    void GetResultThisFrame(std::vector<SPerformanceResult*>& outResult);

    bool PauseSwitcher();
    bool IsPaused();

    void ClearAllResult();

private:
    SPerformanceResult* GetResultFromPool();
    void IncreaseResultPool();
    void DestroyResultPool();
    void UpdateRecord(SPerformanceResult* pResult);

private:
    size_t m_resultPoolIndex;
    size_t m_lastResultPoolIndex;
    SPerformanceRecord* m_pCurRecord;
    bool m_bPause;
    bool m_bPauseRequest;    //We can only switch pause when a frame is end. Save the request.
    bool m_bClearAllRequest; //We can only clear result when a frame is end. Save the request.
    LARGE_INTEGER m_freq;

    std::vector<SPerformanceResult*> m_resultPool;//use a pool to avoid calling delete/new frequently.
    std::stack<SPerformanceResult*> m_resultOrder;
    std::vector<const TCHAR*> m_typeName;
    std::map<size_t, SPerformanceRecord*> m_recordMap;
    SPerformanceRecord m_rootRecord;
};

class CAutoPerformDetector
{
public:
    CAutoPerformDetector(int type, size_t id)
        : m_type(type)
    {
        CPerformDetector::GetInstance()->StartDetectNode(type, id);
    }
    ~CAutoPerformDetector()
    {
        CPerformDetector::GetInstance()->StopDetectNode(m_type);
    }
private:
    int m_type;
};
#endif

#endif