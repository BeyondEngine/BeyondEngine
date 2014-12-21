#ifndef BEYOND_ENGINE_AUDIOENGINE_INL_H__INCLUDE
#define BEYOND_ENGINE_AUDIOENGINE_INL_H__INCLUDE

#include <unordered_map>

#include "AudioCache.h"
#include "AudioPlayer.h"

#define MAX_AUDIOINSTANCES 32

class CAudioEngineThreadPool;

class CAudioEngineImpl
{
public:
    CAudioEngineImpl();
    ~CAudioEngineImpl();

    bool Init();
    int Play2d(const std::string &fileFullPath, bool loop, float volume);
    void SetVolume(int audioID, float volume);
    void SetLoop(int audioID, bool loop);
    bool Pause(int audioID);
    bool Resume(int audioID);
    bool Stop(int audioID);
    void StopAll();
    float GetDuration(int audioID);
    float GetCurTime(int audioID);
    bool SetCurTime(int audioID, float time);

    void Uncache(const std::string& filePath);
    void UncacheAll();

    void Update(float dt);
    CAudioCache* Preload(const std::string& filePath);
private:
    void _Play2d(CAudioCache *cache, int audioID);

    ALuint m_alSources[MAX_AUDIOINSTANCES];

    //source,used
    std::unordered_map<ALuint, bool> m_alSourceUsed;

    //filePath,bufferInfo
    std::unordered_map<std::string, CAudioCache> m_pAudioCaches;

    //audioID,AudioInfo
    std::unordered_map<int, CAudioPlayer>  m_audioPlayers;

    std::mutex m_threadMutex;

    std::vector<int> m_toRemoveAudioIDs;
    std::vector<CAudioCache*> m_toRemoveCaches;

    int m_nCurrentAudioID;

};

#endif

