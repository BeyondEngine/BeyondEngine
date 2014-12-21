#include "stdafx.h"

#import <OpenAL/alc.h>
#import <AVFoundation/AVFoundation.h>
#include "Resource/ResourceManager.h"
#include "AudioEngine-inl.h"
#include "audio/include/AudioEngine.h"
#include <UIKit/UIKit.h>

static ALCdevice *s_ALDevice = nullptr;
static ALCcontext *s_ALContext = nullptr;

@interface AudioEngineSessionHandler : NSObject
{
}

-(id)init;
-(void)handleInterruption:(NSNotification*)notification;

@end

@implementation AudioEngineSessionHandler

void AudioEngineInterruptionListenerCallback(void* user_data, UInt32 interruption_state)
{
    if (kAudioSessionBeginInterruption == interruption_state)
    {
        alcMakeContextCurrent(nullptr);
    }
    else if (kAudioSessionEndInterruption == interruption_state)
    {
        OSStatus result = AudioSessionSetActive(true);
        if (result) NSLog(@"Error setting audio session active! %d\n", (int)result);

        alcMakeContextCurrent(s_ALContext);
    }
}

-(id)init
{
    //for play background music begin
    if (![[AVAudioSession sharedInstance] isOtherAudioPlaying]) {
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategorySoloAmbient error:nil];
        [[AVAudioSession sharedInstance] setActive:YES error:nil];
    } else {
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback withOptions:AVAudioSessionCategoryOptionMixWithOthers error:nil];
        [[AVAudioSession sharedInstance] setActive:YES error:nil];
    }
    //end
    
    if (self = [super init])
    {
        if ([[[UIDevice currentDevice] systemVersion] intValue] > 5) {
            [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleInterruption:) name:AVAudioSessionInterruptionNotification object:[AVAudioSession sharedInstance]];
            [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleInterruption:) name:UIApplicationDidBecomeActiveNotification object:nil];
        }
        else {
            AudioSessionInitialize(NULL, NULL, AudioEngineInterruptionListenerCallback, self);
        }
    }
    return self;
}

-(void)handleInterruption:(NSNotification*)notification
{
    static bool resumeOnBecomingActive = false;
    
    if ([notification.name isEqualToString:AVAudioSessionInterruptionNotification]) {
        NSInteger reason = [[[notification userInfo] objectForKey:AVAudioSessionInterruptionTypeKey] integerValue];
        if (reason == AVAudioSessionInterruptionTypeBegan) {
            alcMakeContextCurrent(NULL);
        }
        
        if (reason == AVAudioSessionInterruptionTypeEnded) {
            if ([UIApplication sharedApplication].applicationState == UIApplicationStateActive) {
                NSError *error = nil;
                [[AVAudioSession sharedInstance] setActive:YES error:&error];
                alcMakeContextCurrent(s_ALContext);
            } else {
                resumeOnBecomingActive = true;
            }
        }
    }
    
    if ([notification.name isEqualToString:UIApplicationDidBecomeActiveNotification] && resumeOnBecomingActive) {
        resumeOnBecomingActive = false;
        NSError *error = nil;
        BOOL success = [[AVAudioSession sharedInstance]
                        setCategory: AVAudioSessionCategoryAmbient
                        error: &error];
        if (!success) {
            BEATS_PRINT("Fail to set audio session.\n");
            return;
        }
        [[AVAudioSession sharedInstance] setActive:YES error:&error];
        alcMakeContextCurrent(s_ALContext);
    }
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:AVAudioSessionInterruptionNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidBecomeActiveNotification object:nil];
    
    [super dealloc];
}
@end

static id s_AudioEngineSessionHandler = nullptr;

CAudioEngineImpl::CAudioEngineImpl()
: m_nCurrentAudioID(0)
{

}

CAudioEngineImpl::~CAudioEngineImpl()
{
    if (s_ALContext)
    {
        alDeleteSources(MAX_AUDIOINSTANCES, m_alSources);

        m_pAudioCaches.clear();

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(s_ALContext);
    }
    if (s_ALDevice)
    {
        alcCloseDevice(s_ALDevice);
    }
    [s_AudioEngineSessionHandler release];
}

bool CAudioEngineImpl::Init()
{
    bool ret = false;
    do{
        s_AudioEngineSessionHandler = [[AudioEngineSessionHandler alloc] init];
        s_ALDevice = alcOpenDevice(nullptr);

        if (s_ALDevice)
        {
            auto alError = alGetError();
            s_ALContext = alcCreateContext(s_ALDevice, nullptr);
            alcMakeContextCurrent(s_ALContext);

            alGenSources(MAX_AUDIOINSTANCES, m_alSources);
            alError = alGetError();
            if (alError != AL_NO_ERROR)
            {
                BEATS_PRINT("%s:generating sources fail! error = %x\n", __PRETTY_FUNCTION__, alError);
                break;
            }

            for (int i = 0; i < MAX_AUDIOINSTANCES; ++i)
            {
                m_alSourceUsed[m_alSources[i]] = false;
            }
            //to make openal initialized successfully
            ALuint unusedAlBufferId = 0;
            alGenBuffers(1, &unusedAlBufferId);
            alDeleteBuffers(1, &unusedAlBufferId);
            ret = true;
        }
    } while (false);

    return ret;
}

CAudioCache* CAudioEngineImpl::Preload(const std::string& filePath)
{
    CAudioCache* audioCache = nullptr;
    auto it = m_pAudioCaches.find(filePath);
    if (it == m_pAudioCaches.end())
    {
        audioCache = &m_pAudioCaches[filePath];
        auto fileExtension = filePath.substr(filePath.rfind('.'));
        transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);
        if (fileExtension == ".ogg")
        {
            audioCache->m_eFileFormat = CAudioCache::EFileFormat::OGG;
        }
        else
        {
            audioCache->m_eFileFormat = CAudioCache::EFileFormat::UNKNOWN;
        }
        std::string strPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Audio);
        audioCache->m_strFileFullPath = strPath.append(_T("/")).append(filePath);
        BEATS_ASSERT(CFilePathTool::GetInstance()->Exists(audioCache->m_strFileFullPath.c_str()), "audio file %s is not found!", audioCache->m_strFileFullPath.c_str());
        CAudioEngine::AddTask(std::bind(&CAudioCache::ReadDataTask, audioCache));
    }
    else
    {
        audioCache = &it->second;
    }
    return audioCache;
}

int CAudioEngineImpl::Play2d(const std::string &filePath, bool loop, float volume)
{
    int nRet = CAudioEngine::INVALID_AUDIO_ID;
    if (s_ALDevice != nullptr)
    {
        bool sourceFlag = false;
        ALuint alSource = 0;
        for (int i = 0; i < MAX_AUDIOINSTANCES; ++i)
        {
            alSource = m_alSources[i];
            if (!m_alSourceUsed[alSource])
            {
                sourceFlag = true;
                break;
            }
        }
        if (sourceFlag)
        {
            m_alSourceUsed[alSource] = true;
            auto player = &m_audioPlayers[m_nCurrentAudioID];
            player->m_alSource = alSource;
            player->m_bLoop = loop;
            player->m_fVolume = volume;
            nRet = m_nCurrentAudioID++;
            BEATS_PRINT("CAudioEngineImpl::Play2d: %d\n", nRet);
            CAudioCache* audioCache = Preload(filePath);
            BEATS_ASSERT(audioCache != nullptr);
            if (audioCache->m_bAlBufferReady)
            {
                _Play2d(audioCache, nRet);
            }
            else
            {
                BEATS_PRINT("Preload\n");
                audioCache->AddPlayCallback(std::bind(&CAudioEngineImpl::_Play2d, this, audioCache, nRet));
            }
        }
        else
        {
            BEATS_PRINT("%s: Not enough source\n", __PRETTY_FUNCTION__);
        }
    }
    return nRet;
}

void CAudioEngineImpl::_Play2d(CAudioCache *cache, int audioID)
{
    m_threadMutex.lock();
    BEATS_ASSERT(cache->m_bAlBufferReady);
    if (!cache->m_bLoadFail)
    {
        auto playerIt = m_audioPlayers.find(audioID);
        if (playerIt != m_audioPlayers.end())
        {
            if (!m_audioPlayers[audioID].m_bStoped && playerIt->second.Play2d(cache))
            {
                CAudioEngine::m_audioIDInfoMap[audioID].state = CAudioEngine::AudioState::PLAYING;
            }
            else
            {
                m_toRemoveAudioIDs.push_back(audioID);
            }
        }
    }
    else
    {
        m_toRemoveCaches.push_back(cache);
        m_toRemoveAudioIDs.push_back(audioID);
    }
    m_threadMutex.unlock();
}

void CAudioEngineImpl::SetVolume(int audioID, float volume)
{
    auto& player = m_audioPlayers[audioID];
    player.m_fVolume = volume;

    if (player.m_bReady)
    {
        alSourcef(m_audioPlayers[audioID].m_alSource, AL_GAIN, volume);

        auto error = alGetError();
        if (error != AL_NO_ERROR)
        {
            BEATS_PRINT("%s: audio id = %d, error = %x\n", __PRETTY_FUNCTION__, audioID, error);
        }
    }
}

void CAudioEngineImpl::SetLoop(int audioID, bool loop)
{
    auto& player = m_audioPlayers[audioID];

    if (player.m_bReady)
    {
        if (player.m_bStreamingSource)
        {
            player.SetLoop(loop);
        }
        else
        {
            alSourcei(player.m_alSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
            auto error = alGetError();
            if (error != AL_NO_ERROR)
            {
                BEATS_PRINT("%s: audio id = %d, error = %x\n", __PRETTY_FUNCTION__, audioID, error);
            }
        }
    }
    else
    {
        player.m_bLoop = loop;
    }
}

bool CAudioEngineImpl::Pause(int audioID)
{
    BEATS_PRINT("%s: audio id = %d\n", __PRETTY_FUNCTION__, audioID);
    alSourcePause(m_audioPlayers[audioID].m_alSource);
    auto error = alGetError();
    bool ret = error == AL_NO_ERROR;
    if (!ret)
    {
        BEATS_PRINT("%s: audio id = %d, error = %x\n", __PRETTY_FUNCTION__, audioID, error);
    }

    return ret;
}

bool CAudioEngineImpl::Resume(int audioID)
{
    BEATS_PRINT("%s: audio id = %d\n", __PRETTY_FUNCTION__, audioID);
    alSourcePlay(m_audioPlayers[audioID].m_alSource);

    auto error = alGetError();
    bool ret = error == AL_NO_ERROR;

    if (!ret)
    {
        BEATS_PRINT("%s: audio id = %d, error = %x\n", __PRETTY_FUNCTION__, audioID, error);
    }
    return ret;
}

bool CAudioEngineImpl::Stop(int audioID)
{
    BEATS_PRINT("%s: audio id = %d\n", __PRETTY_FUNCTION__, audioID);
    bool ret = true;
    auto& player = m_audioPlayers[audioID];
    player.m_bStoped = true;
    if (player.m_bReady)
    {
        alSourceStop(player.m_alSource);

        auto error = alGetError();
        ret = error == AL_NO_ERROR;
        if (!ret)
        {
            BEATS_PRINT("%s: audio id = %d, error = %x\n", __PRETTY_FUNCTION__, audioID, error);
        }
    }

    alSourcei(player.m_alSource, AL_BUFFER, 0);
    return ret;
}

void CAudioEngineImpl::StopAll()
{
    BEATS_PRINT("%s\n", __PRETTY_FUNCTION__);
    for (int index = 0; index < MAX_AUDIOINSTANCES; ++index)
    {
        alSourceStop(m_alSources[index]);
        alSourcei(m_alSources[index], AL_BUFFER, 0);
        m_alSourceUsed[m_alSources[index]] = false;
    }

    m_audioPlayers.clear();
}

float CAudioEngineImpl::GetDuration(int audioID)
{
    float fRet = CAudioEngine::TIME_UNKNOWN;
    auto& player = m_audioPlayers[audioID];
    if (player.m_bReady)
    {
        fRet = player.m_pAudioCache->m_fDuration;
    }
    return fRet;
}

float CAudioEngineImpl::GetCurTime(int audioID)
{
    float ret = 0.0f;
    auto& player = m_audioPlayers[audioID];
    if (player.m_bReady)
    {
        if (player.m_bStreamingSource)
        {
            ret = player.GetTime();
        }
        else
        {
            alGetSourcef(player.m_alSource, AL_SEC_OFFSET, &ret);
            auto error = alGetError();
            if (error != AL_NO_ERROR)
            {
                BEATS_PRINT("%s, audio id:%d,error code:%x\n", __PRETTY_FUNCTION__, audioID, error);
            }
        }
    }

    return ret;
}

bool CAudioEngineImpl::SetCurTime(int audioID, float time)
{
    bool ret = false;
    auto& player = m_audioPlayers[audioID];
    if (player.m_bReady)
    {
        if (player.m_bStreamingSource)
        {
            ret = player.SetTime(time);
        }
        else
        {
            if (player.m_pAudioCache->m_bytesOfRead != player.m_pAudioCache->m_dataSize &&
                (time * player.m_pAudioCache->m_sampleRate * player.m_pAudioCache->m_nBytesPerFrame) > player.m_pAudioCache->m_bytesOfRead)
            {
                BEATS_PRINT("%s: audio id = %d\n", __PRETTY_FUNCTION__, audioID);
            }
            else
            {
                alSourcef(player.m_alSource, AL_SEC_OFFSET, time);
                auto error = alGetError();
                if (error != AL_NO_ERROR)
                {
                    BEATS_PRINT("%s: audio id = %d, error = %x\n", __PRETTY_FUNCTION__, audioID, error);
                }
                ret = true;
            }
        }
    }
    return ret;
}

void CAudioEngineImpl::Update(float dt)
{
    ALint sourceState;
    int audioID = 0;

    if (m_threadMutex.try_lock())
    {
        size_t removeAudioCount = m_toRemoveAudioIDs.size();
        for (size_t index = 0; index < removeAudioCount; ++index)
        {
            audioID = m_toRemoveAudioIDs[index];
            auto playerIt = m_audioPlayers.find(audioID);
            if (playerIt != m_audioPlayers.end())
            {
                m_alSourceUsed[playerIt->second.m_alSource] = false;
                m_audioPlayers.erase(audioID);
                CAudioEngine::Remove(audioID);
            }
        }
        m_toRemoveAudioIDs.clear();
        size_t removeCacheCount = m_toRemoveCaches.size();
        for (size_t index = 0; index < removeCacheCount; ++index)
        {
            auto itEnd = m_pAudioCaches.end();
            for (auto it = m_pAudioCaches.begin(); it != itEnd; ++it)
            {
                if (&it->second == m_toRemoveCaches[index])
                {
                    m_pAudioCaches.erase(it);
                    break;
                }
            }
        }
        m_threadMutex.unlock();
    }

    for (auto it = m_audioPlayers.begin(); it != m_audioPlayers.end();)
    {
        audioID = it->first;
        auto& player = it->second;
        alGetSourcei(player.m_alSource, AL_SOURCE_STATE, &sourceState);

        if (player.m_bReadForRemove)
        {
            it = m_audioPlayers.erase(it);
            CAudioEngine::Remove(audioID);
        }
        else if (player.m_bReady && sourceState == AL_STOPPED)
        {
            m_alSourceUsed[player.m_alSource] = false;

            if (player.m_bStreamingSource)
            {
                player.NotifyExitThread();
                ++it;
            }
            else
            {
                it = m_audioPlayers.erase(it);
                CAudioEngine::Remove(audioID);
            }
        }
        else
        {
            ++it;
        }
    }
}

void CAudioEngineImpl::Uncache(const std::string &filePath)
{
    BEATS_PRINT("%s: file = %s\n", __PRETTY_FUNCTION__, filePath.c_str());
    m_pAudioCaches.erase(filePath);
}

void CAudioEngineImpl::UncacheAll()
{
    BEATS_PRINT("CAudioEngineImpl::UncacheAll\n");
    m_pAudioCaches.clear();
}
