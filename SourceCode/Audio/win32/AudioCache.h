#ifndef BEYOND_ENGINE_AUDIOCACHE_H__INCLUDE
#define BEYOND_ENGINE_AUDIOCACHE_H__INCLUDE

#include "AL/al.h"

#define QUEUEBUFFER_NUM 5
#define QUEUEBUFFER_TIME_STEP 0.1f

class CAudioEngineImpl;
class CAudioPlayer;

class CAudioCache{
public:
    enum class EFileFormat
    {
        UNKNOWN,
        OGG,
        MP3
    };

    CAudioCache();
    CAudioCache(const CAudioCache&);
    ~CAudioCache();

    void AddPlayCallback(const std::function<void()>& callback);

protected:
    void ReadDataTask();  
    void InvokingPlayCallbacks();

    std::string m_strFileFullPath;
    EFileFormat m_eFileFormat;
    //pcm data related stuff
    size_t m_uPcmDataSize;
    ALenum m_alBufferFormat;

    int m_nChannels;
    ALuint m_sampleRate;
    size_t m_uBytesPerFrame;
    float m_fDuration;
    
    /*Cache related stuff;
     * Cache pcm data when sizeInBytes less than PCMDATA_CACHEMAXSIZE
     */
    ALuint m_alBufferId;
    void* m_pPcmData;
    size_t m_uBytesOfRead;

    /*Queue buffer related stuff
     *  Streaming in OpenAL when sizeInBytes greater then PCMDATA_CACHEMAXSIZE
     */
    char* m_queBuffers[QUEUEBUFFER_NUM];
    ALsizei m_queBufferSize[QUEUEBUFFER_NUM];
    int m_nQueBufferFrames;
    int m_nQueBufferBytes;

    bool m_bLoadFail;
    bool m_bAlBufferReady;
    std::mutex m_callbackMutex; 
    std::vector< std::function<void()> > m_callbacks;
    std::mutex m_readDataTaskMutex;

    int m_nMp3Encoding;

    friend class CAudioEngineImpl;
    friend class CAudioPlayer;
} ;

#endif // __AUDIO_CACHE_H_

