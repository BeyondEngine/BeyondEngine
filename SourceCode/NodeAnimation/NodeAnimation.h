#ifndef BEYOND_ENGINE_NODEANIMATION_NODEANIMATION_H__INCLUDE
#define BEYOND_ENGINE_NODEANIMATION_NODEANIMATION_H__INCLUDE

#include "Event/EventType.h"

enum ENodeAnimationPlayType
{
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
class CNodeAnimation
{
public:
    CNodeAnimation();
    ~CNodeAnimation();

    void Play();
    void Stop();
    void Pause();
    void Resume();
    bool IsPaused() const;
    bool IsPlaying() const;

    void Update(float ddt);
    size_t GetFPS() const;
    void SetFPS(size_t uFPS);
    ENodeAnimationPlayType GetPlayType() const;
    void SetPlayType(ENodeAnimationPlayType type);

    void SetData(CNodeAnimationData* pData);
    CNodeAnimationData* GetAnimationData() const;
    void SetOwner(CNode* pOwner);
    CNode* GetOwner() const;

    size_t GetCurrentFrame() const;
    void SetCurrentFrame(size_t uFrame);

    CEventDispatcher* GetEventDispatcher();
    void ResetNode();

private:
    void InitStartPos();
    void TriggerEvent(EEventType eventType);
private:
    bool m_bPause;
    bool m_bIsPlaying;
    bool m_bReversePlay;
    size_t m_uCurrFramePos;
    size_t m_uFPS;
    float m_fElapsedTime;
    float m_fTimeForAFrame;
    ENodeAnimationPlayType m_playType;
    CNodeAnimationData* m_pData;
    CNode* m_pOwner;
    CEventDispatcher* m_pEventDispatcher;
};

#endif