#ifndef BEYOND_ENGINE_NODEANIMATION_NODEANIMATION_H__INCLUDE
#define BEYOND_ENGINE_NODEANIMATION_NODEANIMATION_H__INCLUDE

#include "Event/EventType.h"
#include "Event/EventSubscription.h"

enum ENodeAnimationPlayType
{
    eNAPT_NONE,
    eNAPT_ONCE,
    eNAPT_LOOP,
    eNAPT_REVERSE,
    eNAPT_REVERSE_LOOP,
    eNAPT_BOUNCE,

    eNAPT_Count,
    eNAPT_Force32Bit = 0xFFFFFFFF
};


class CNodeAnimationData;
class CNode;
class CEventDispatcher;
class CScene;
class CNodeAnimation
{
private:
    CNodeAnimation();
    ~CNodeAnimation();

public:
    friend struct Details::Deallocator<CNodeAnimation, false>;
    friend class CNodeAnimationManager; // Only CNodeAnimationManager can create and delete CNodeAnimation.

    void Play(uint32_t uStartPos = 0);
    void Stop();
    void Pause();
    void Resume();
    bool IsPaused() const;
    bool IsPlaying() const;
    void SetPlayingFlag(bool bPlaying);

    void Update(float ddt);
    uint32_t GetFPS() const;
    void SetFPS(uint32_t uFPS);
    ENodeAnimationPlayType GetPlayType() const;
    void SetPlayType(ENodeAnimationPlayType type);

    void SetData(CNodeAnimationData* pData);
    CNodeAnimationData* GetData() const;
    void SetOwner(CNode* pOwner, bool bNeedStop = true);
    CNode* GetOwner() const;

    uint32_t GetCurrentFrame() const;
    void SetCurrentFrame(uint32_t uFrame);

    CEventDispatcher* GetEventDispatcher();
    void ResetNode();
    void Reset();

    // Event will only be trigger in this scene.
    void BindScene(CScene* pScene);

    void SetAutoDestroy(bool bAutoDestroy);
    bool IsAutoDestroy() const;
    void SetOnEndHandler(CEventSubscription::EventHandler pHandler);
    void TriggerEvent(EEventType eventType);

    void SetResetNodeWhenStop(bool bValue);

private:
    void InitStartPos(uint32_t uStartPos = 0);

private:
    bool m_bPause;
    bool m_bIsPlaying;
    bool m_bReversePlay;
    bool m_bAutoDestroy;
    bool m_bResetNodeWhenStop;
    bool m_bDeleteFlag;
    uint32_t m_uCurrFramePos;
    uint32_t m_uFPS;
    float m_fElapsedTime;
    float m_fTimeForAFrame;
    ENodeAnimationPlayType m_playType;
    CNodeAnimationData* m_pData;
    CNode* m_pOwner;
    CEventDispatcher* m_pEventDispatcher;
    CScene* m_pEventScene;
    bool m_bDispatcherBeginEvent;
};

#endif