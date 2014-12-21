#ifndef BEYOND_ENGINE_AUDIOPLAYER_INL_H__INCLUDE
#define BEYOND_ENGINE_AUDIOPLAYER_INL_H__INCLUDE

#if BEYONDENGINE_PLATFORM == PLATFORM_IOS

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#import <OpenAL/al.h>

class CAudioCache;
class CAudioEngineImpl;

class CAudioPlayer
{
public:
    CAudioPlayer();
    ~CAudioPlayer();
    
    //queue buffer related stuff
    bool SetTime(float time);
    float GetTime() { return m_fCurrTime;}
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
    float m_fCurrTime;
    bool m_bStreamingSource;
    ALuint m_bufferIds[3];
    std::thread m_rotateBufferThread;
    std::condition_variable m_sleepCondition;
    std::mutex m_sleepMutex;
    bool m_bExitThread;
    bool m_bReadForRemove;

    friend class CAudioEngineImpl;
};

#endif // __AUDIO_PLAYER_H_
#endif

