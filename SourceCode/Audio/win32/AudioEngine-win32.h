#ifndef BEYOND_ENGINE_AUDIOENGINE_WIN32_H__INCLUDE
#define BEYOND_ENGINE_AUDIOENGINE_WIN32_H__INCLUDE

#include "AudioCache.h"
#include "AudioPlayer.h"
#include "AL/al.h"

#define MAX_AUDIOINSTANCES 32

class CAudioEngineThreadPool;

class CAudioEngineImpl
{
public:
    CAudioEngineImpl();
    ~CAudioEngineImpl();
    
    bool Init();
    int Play2d(const std::string &fileFullPath ,bool loop ,float volume);
    void SetVolume(int audioID,float volume);
    void SetLoop(int audioID, bool loop);
    bool Pause(int audioID);
    bool Resume(int audioID);
    bool Stop(int audioID);
    void StopAll();
    float GetDuration(int audioID);
    void SetFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback);
    
    void Uncache(const std::string& filePath);
    void UncacheAll();
    CAudioCache* Preload(const std::string& filePath);

    void Update(float dt);
    
private:
    void _Play2d(CAudioCache *cache, int audioID);
    
    ALuint m_alSources[MAX_AUDIOINSTANCES];
    
    //source,used
    std::map<ALuint, bool> m_alSourceUsedMap;
    
    //filePath,bufferInfo
    std::map<std::string, CAudioCache> m_audioCachesMap;
    
    //audioID,AudioInfo
    std::map<int, CAudioPlayer> m_audioPlayers;
    
    std::mutex m_threadMutex;
    
    std::vector<CAudioCache*> m_toRemoveCaches;
    std::vector<int> m_toRemoveAudioIDs;
    
    int m_nCurrentAudioID;
    
};
#endif // __AUDIO_ENGINE_INL_H_

