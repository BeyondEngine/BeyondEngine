#ifndef BEYOND_ENGINE_AUDIOCACHE_INL_H__INCLUDE
#define BEYOND_ENGINE_AUDIOCACHE_INL_H__INCLUDE

#if BEYONDENGINE_PLATFORM == PLATFORM_IOS

#import <OpenAL/al.h>
#import <AudioToolbox/AudioToolbox.h>

#include <string>
#include <mutex>
#include <vector>

#define QUEUEBUFFER_NUM 3
#define QUEUEBUFFER_TIME_STEP 0.1

class CAudioEngineImpl;
class CAudioPlayer;

class CAudioCache{
    enum class EFileFormat
    {
        UNKNOWN,
        OGG
    };
public:
    CAudioCache();
    ~CAudioCache();

    void AddPlayCallback(const std::function<void()>& callback);

protected:
    void ReadDataTask();
    void InvokingPlayCallbacks();

    std::string m_strFileFullPath;
    EFileFormat m_eFileFormat;
    
    //pcm data related stuff
    ALsizei m_dataSize;
    ALenum m_format;
    
    AudioStreamBasicDescription m_outputFormat;
    ALsizei m_sampleRate;
    int m_nBytesPerFrame;
    float m_fDuration;
    
    /*Cache related stuff;
    * Cache pcm data when sizeInBytes less than PCMDATA_CACHEMAXSIZE
    */
    ALuint m_alBufferId;
    char* m_pPcmData;
    SInt64 m_bytesOfRead;

    /*Queue buffer related stuff
    *  Streaming in openal when sizeInBytes greater then PCMDATA_CACHEMAXSIZE
    */
    char* m_queBuffers[QUEUEBUFFER_NUM];
    ALsizei m_queBufferSize[QUEUEBUFFER_NUM];
    UInt32 m_uQueBufferFrames;
    UInt32 m_uQueBufferBytes;

    bool m_bLoadFail;
    bool m_bAlBufferReady;
    std::mutex m_callbackMutex;
    std::vector< std::function<void()> > m_callbacks;
    std::mutex m_readDataTaskMutex;

    bool m_bExitReadDataTask;

    friend class CAudioEngineImpl;
    friend class CAudioPlayer;
    
private:
    void ReadOthers();
    void ReadOgg();
};

#endif // __AUDIO_CACHE_H_
#endif

