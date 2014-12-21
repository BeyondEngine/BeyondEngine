#include "stdafx.h"
#include "audio/android/AudioEngine-inl.h"

#include <unistd.h>
// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <unordered_map>
#include "Framework/android/JniHelper.h"
#include <android/log.h>
#include <jni.h>
#include "audio/include/AudioEngine.h"
#include "audio/android/IAudioPlayer.h"
#include "audio/android/AudioPlayerProvider.h"
#include "audio/android/cutils/log.h" 
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

#define DELAY_TIME_TO_REMOVE 0.5f
#define PRINT_AUDIO_INFO
static int fdGetter(const std::string& url, off_t* start, off_t* length)
{
    int fd = -1;
    auto asset = AAssetManager_open(CFilePathTool::GetInstance()->GetAssetManager(), url.c_str(), AASSET_MODE_UNKNOWN);
    // open asset as file descriptor
    fd = AAsset_openFileDescriptor(asset, start, length);
    AAsset_close(asset);
    if (fd <= 0)
    {
        ALOGE("Failed to open file descriptor for '%s'", url.c_str());
    }

    return fd;
};

CAudioEngineImpl::CAudioEngineImpl()
    : _audioIDIndex(0)
    , _engineObject(nullptr)
    , _engineEngine(nullptr)
    , _outputMixObject(nullptr)
    , _lazyInitLoop(true)
    , _audioPlayerProvider(nullptr)
    , _onPauseListener(nullptr)
    , _onResumeListener(nullptr)
{

}

CAudioEngineImpl::~CAudioEngineImpl()
{
    if (_audioPlayerProvider != nullptr)
    {
        delete _audioPlayerProvider;
        _audioPlayerProvider = nullptr;
    }

    if (_outputMixObject)
    {
        (*_outputMixObject)->Destroy(_outputMixObject);
    }
    if (_engineObject)
    {
        (*_engineObject)->Destroy(_engineObject);
    }
}

bool CAudioEngineImpl::Init()
{
    bool ret = false;
    do{

        // create engine
        auto result = slCreateEngine(&_engineObject, 0, nullptr, 0, nullptr, nullptr);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("create opensl engine fail"); break; }

        // realize the engine
        result = (*_engineObject)->Realize(_engineObject, SL_BOOLEAN_FALSE);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("realize the engine fail"); break; }

        // get the engine interface, which is needed in order to create other objects
        result = (*_engineObject)->GetInterface(_engineObject, SL_IID_ENGINE, &_engineEngine);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("get the engine interface fail"); break; }

        // create output mix
        const SLInterfaceID outputMixIIDs[] = {};
        const SLboolean outputMixReqs[] = {};
        result = (*_engineEngine)->CreateOutputMix(_engineEngine, &_outputMixObject, 0, outputMixIIDs, outputMixReqs);           
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("create output mix fail"); break; }

        // realize the output mix
        result = (*_outputMixObject)->Realize(_outputMixObject, SL_BOOLEAN_FALSE);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("realize the output mix fail"); break; }

        _audioPlayerProvider = new AudioPlayerProvider(_engineEngine, _outputMixObject, 44100, 192, fdGetter);

        ret = true;
    }while (false);

    return ret;
}

int CAudioEngineImpl::Play2d(const std::string &filePath ,bool loop ,float volume)
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("play2d, _audioPlayers.size=%d", (int)_audioPlayers.size());
#endif
    auto audioId = CAudioEngine::INVALID_AUDIO_ID;

    do 
    {
        if (_engineEngine == nullptr || _audioPlayerProvider == nullptr)
            break;

        audioId = _audioIDIndex++;

        auto player = _audioPlayerProvider->getAudioPlayer(filePath);
        if (player != nullptr)
        {
            player->setId(audioId);
            _audioPlayers.insert(std::make_pair(audioId, player));

            player->setPlayEventCallback([this, player](IAudioPlayer::State state){

                if (state != IAudioPlayer::State::OVER && state != IAudioPlayer::State::STOPPED)
                {
#ifdef PRINT_AUDIO_INFO
                    BEATS_PRINT("Ignore state: %d", static_cast<int>(state));
#endif
                    return;
                }

                int id = player->getId();
#ifdef PRINT_AUDIO_INFO
                BEATS_PRINT("Removing player id=%d, state:%d", id, (int)state);
#endif
                if (player->delayDelete())
                {
                    _playerToDelete.insert(player);
                }
                CAudioEngine::Remove(id);
                _audioPlayers.erase(id);
            });

            player->setLoop(loop);
            player->setVolume(volume);
            player->play();
        } 
        else
        {
#ifdef PRINT_AUDIO_INFO
            BEATS_PRINT("Oops, player is null ...");
#endif
            return CAudioEngine::INVALID_AUDIO_ID;
        }

        CAudioEngine::m_audioIDInfoMap[audioId].state = CAudioEngine::AudioState::PLAYING;
        
    } while (0);

    return audioId;
}

void CAudioEngineImpl::SetVolume(int audioID,float volume)
{
    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        player->setVolume(volume);
    }
}

void CAudioEngineImpl::SetLoop(int audioID, bool loop)
{
    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        player->setLoop(loop);
    }
}

void CAudioEngineImpl::Pause(int audioID)
{
    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        player->pause();
    }
}

void CAudioEngineImpl::Resume(int audioID)
{
    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        player->resume();
    }
}

void CAudioEngineImpl::Stop(int audioID)
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("Stop audio %d\n", audioID);
#endif

    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        player->stop();
    }
}

void CAudioEngineImpl::StopAll()
{
    if (_audioPlayers.empty())
    {
        return;
    }

    // Create a temporary vector for storing all players since
    // p->stop() will trigger _audioPlayers.erase, 
    // and it will cause a crash as it's already in for loop
    std::vector<IAudioPlayer*> players;
    players.reserve(_audioPlayers.size());

    for (const auto& e : _audioPlayers)
    {
        players.push_back(e.second);
    }

    for (auto p : players)
    {
        p->stop();
    }
}

float CAudioEngineImpl::GetDuration(int audioID)
{
    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        return player->getDuration();
    }
    return 0.0f;
}

float CAudioEngineImpl::GetCurrentTime(int audioID)
{
    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        return player->getPosition();
    }
    return 0.0f;
}

bool CAudioEngineImpl::SetCurrentTime(int audioID, float time)
{
    auto iter = _audioPlayers.find(audioID);
    if (iter != _audioPlayers.end())
    {
        auto player = iter->second;
        return player->setPosition(time);
    }
    return false;
}

void CAudioEngineImpl::Preload(const std::string& filePath)
{
    if (_audioPlayerProvider != nullptr)
    {
        std::string fullPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Audio);
        fullPath.append(_T("/")).append(filePath);
        _audioPlayerProvider->preloadEffect(fullPath, nullptr);
    }
}

void CAudioEngineImpl::Uncache(const std::string& filePath)
{
    if (_audioPlayerProvider != nullptr)
    {
        std::string fullPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Audio);
        fullPath.append(_T("/")).append(filePath);
        _audioPlayerProvider->clearPcmCache(fullPath);
    }
}

void CAudioEngineImpl::UncacheAll()
{
    if (_audioPlayerProvider != nullptr)
    {
        _audioPlayerProvider->clearAllPcmCaches();
    }
}

void CAudioEngineImpl::Update(float )
{
    if (_playerToDelete.size() > 0)
    {
        for (auto iter : _playerToDelete)
        {
            iter->destroy();
            delete iter;
        }
        _playerToDelete.clear();
    }
}