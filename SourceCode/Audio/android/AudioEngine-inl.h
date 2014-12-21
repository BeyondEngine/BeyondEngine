
#ifndef __AUDIO_ENGINE_INL_H_
#define __AUDIO_ENGINE_INL_H_

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <string>
#include <unordered_map>

#define MAX_AUDIOINSTANCES 24

#define ERRORLOG(msg) printf("fun:%s,line:%d,msg:%s",__func__,__LINE__,#msg)

class EventCustom;
class EventListener;

class IAudioPlayer;
class AudioPlayerProvider;

class CAudioEngineImpl;

class CAudioEngineImpl
{
public:
    CAudioEngineImpl();
    ~CAudioEngineImpl();

    bool Init();
    int Play2d(const std::string &fileFullPath ,bool loop ,float volume);
    void SetVolume(int audioID,float volume);
    void SetLoop(int audioID, bool loop);
    void Pause(int audioID);
    void Resume(int audioID);
    void Stop(int audioID);
    void StopAll();
    float GetDuration(int audioID);
    float GetCurrentTime(int audioID);
    bool SetCurrentTime(int audioID, float time);

    void Uncache(const std::string& filePath);
    void UncacheAll();
    void Preload(const std::string& filePath);
    void Update(float dt);

private:

    // engine interfaces
    SLObjectItf _engineObject;
    SLEngineItf _engineEngine;

    // output mix interfaces
    SLObjectItf _outputMixObject;

    //audioID,AudioInfo
    std::unordered_map<int, IAudioPlayer*>  _audioPlayers;

    AudioPlayerProvider* _audioPlayerProvider;
    EventListener* _onPauseListener;
    EventListener* _onResumeListener;
    std::set<IAudioPlayer*> _playerToDelete;
    int _audioIDIndex;
    
    bool _lazyInitLoop;
};

#endif