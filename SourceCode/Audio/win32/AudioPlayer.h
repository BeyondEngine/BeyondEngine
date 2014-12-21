#ifndef BEYOND_ENGINE_AUDIOPLAYER_H__INCLUDE
#define BEYOND_ENGINE_AUDIOPLAYER_H__INCLUDE

#include "AL/al.h"
#include "AudioCache.h"

class CAudioEngineImpl;

class CAudioPlayer
{
public:
    CAudioPlayer();
    CAudioPlayer(const CAudioPlayer&);
    ~CAudioPlayer();
    
    //queue buffer related stuff
    bool SetLoop(bool loop);
    void NotifyExitThread();
protected:
    void RotateBufferThread(int offsetFrame);
    bool Play2d(CAudioCache* cache);
    
    CAudioCache* m_pAudioCache;
    
    float m_fVolume;
    bool m_bLoop;

    bool m_bStoped = false;
    bool m_bReady;
    ALuint m_alSource;
    
    //play by circular buffer
    bool m_bStreamingSource;
    ALuint m_bufferIds[QUEUEBUFFER_NUM];
    std::thread m_rotateBufferThread;
    std::mutex m_sleepMutex;
    std::condition_variable m_sleepCondition;
    bool m_bExitThread; 
    bool m_bReadForRemove;

    friend class CAudioEngineImpl;
};

#endif // __AUDIO_PLAYER_H_

