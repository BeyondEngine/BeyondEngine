#include "stdafx.h"
#include "AudioEngine-win32.h"
#include "Audio/include/AudioEngine.h"
#include "AL/alc.h"
#include "mpg123.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

static ALCdevice *s_ALDevice = nullptr;
static ALCcontext *s_ALContext = nullptr;
static bool MPG123_LAZYINIT = true;

CAudioEngineImpl::CAudioEngineImpl()
: m_nCurrentAudioID(0)
{
    memset(&m_alSources, 0, sizeof(m_alSources));
}

CAudioEngineImpl::~CAudioEngineImpl()
{
    if (s_ALContext)
    {
        alDeleteSources(MAX_AUDIOINSTANCES, m_alSources);

        m_audioCachesMap.clear();

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(s_ALContext);
        s_ALContext = nullptr;
    }
    if (s_ALDevice)
    {
        alcCloseDevice(s_ALDevice);
        s_ALDevice = nullptr;
    }

    mpg123_exit();
    MPG123_LAZYINIT = true;
}

bool CAudioEngineImpl::Init()
{
    bool ret = false;
    s_ALDevice = alcOpenDevice(NULL);

    if (s_ALDevice)
    {
        auto alError = alGetError();
        s_ALContext = alcCreateContext(s_ALDevice, NULL);
        alcMakeContextCurrent(s_ALContext);
        alGenSources(MAX_AUDIOINSTANCES, m_alSources);
        alError = alGetError();
        BEATS_ASSERT(alError == AL_NO_ERROR, "%s:generating sources fail! error = %x\n", __FUNCTION__, alError);
        if (alError == AL_NO_ERROR)
        {
            for (int i = 0; i < MAX_AUDIOINSTANCES; ++i)
            {
                m_alSourceUsedMap[m_alSources[i]] = false;
            }
            ret = true;
        }
    }

    return ret;
}

CAudioCache* CAudioEngineImpl::Preload(const std::string& filePath)
{
    CAudioCache* pAudioCache = nullptr;
    do
    {
        auto it = m_audioCachesMap.find(filePath);
        if (it != m_audioCachesMap.end())
        {
            pAudioCache = &it->second;
            break;
        }

        CAudioCache::EFileFormat fileFormat = CAudioCache::EFileFormat::UNKNOWN;
        auto fileExtension = filePath.substr(filePath.rfind('.'));
        transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);
        if (fileExtension == ".ogg")
        {
            fileFormat = CAudioCache::EFileFormat::OGG;
        }
        else if (fileExtension == ".mp3")
        {
            fileFormat = CAudioCache::EFileFormat::MP3;

            if (MPG123_LAZYINIT)
            {
                auto error = mpg123_init();
                if (error == MPG123_OK)
                {
                    MPG123_LAZYINIT = false;
                }
                else
                {
                    BEATS_ASSERT(false, _T("Basic setup goes wrong: %s"), mpg123_plain_strerror(error));
                    break;
                }
            }
        }
        else
        {
            BEATS_ASSERT(false, _T("Unsupported media type file: %s\n"), filePath.c_str());
            break;
        }

        std::string strPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Audio);
        pAudioCache = &m_audioCachesMap[filePath];
        pAudioCache->m_eFileFormat = fileFormat;
        pAudioCache->m_strFileFullPath = strPath.append(_T("/")).append(filePath);
        BEATS_ASSERT(CFilePathTool::GetInstance()->Exists(pAudioCache->m_strFileFullPath.c_str()), "audio file %s is not found!", pAudioCache->m_strFileFullPath.c_str());
        CAudioEngine::AddTask(std::bind(&CAudioCache::ReadDataTask, pAudioCache));
    } while (false);

    return pAudioCache;
}

int CAudioEngineImpl::Play2d(const std::string &filePath, bool loop, float volume)
{
    int nRet = CAudioEngine::INVALID_AUDIO_ID;
    bool availableSourceExist = false;
    ALuint alSource = 0;
    for (int i = 0; i < MAX_AUDIOINSTANCES; ++i)
    {
        alSource = m_alSources[i];
        if (!m_alSourceUsedMap[alSource])
        {
            availableSourceExist = true;
            break;
        }
    }
    if (availableSourceExist)
    {
        m_alSourceUsedMap[alSource] = true;
        auto player = &m_audioPlayers[m_nCurrentAudioID];
        player->m_alSource = alSource;
        player->m_bLoop = loop;
        player->m_fVolume = volume;
        CAudioCache* audioCache = Preload(filePath);
        BEATS_ASSERT(audioCache != nullptr);
        if (audioCache->m_bAlBufferReady)
        {
            _Play2d(audioCache, m_nCurrentAudioID);
        }
        else
        {
            audioCache->AddPlayCallback(std::bind(&CAudioEngineImpl::_Play2d, this, audioCache, m_nCurrentAudioID));
        }
        nRet = m_nCurrentAudioID++;
    }
    return nRet;
}

void CAudioEngineImpl::_Play2d(CAudioCache *cache, int audioID)
{
    m_threadMutex.lock();
    BEATS_ASSERT(cache->m_bAlBufferReady);
    if (!cache->m_bLoadFail)
    {
        BEATS_ASSERT(cache->m_bAlBufferReady);
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
        CHECK_AUDIO_ERROR(audioID);
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
            CHECK_AUDIO_ERROR(audioID);
        }
    }
    else
    {
        player.m_bLoop = loop;
    }
}

bool CAudioEngineImpl::Pause(int audioID)
{
    alSourcePause(m_audioPlayers[audioID].m_alSource);
    CHECK_AUDIO_ERROR(audioID);
    return alGetError() == AL_NO_ERROR;
}

bool CAudioEngineImpl::Resume(int audioID)
{
    alSourcePlay(m_audioPlayers[audioID].m_alSource);
    CHECK_AUDIO_ERROR(audioID);
    return alGetError() == AL_NO_ERROR;
}

bool CAudioEngineImpl::Stop(int audioID)
{
    bool ret = true;
    auto& player = m_audioPlayers[audioID];
    player.m_bStoped = true;
    if (player.m_bReady)
    {
        alSourceStop(player.m_alSource);
        CHECK_AUDIO_ERROR(audioID);
        ret = alGetError() == AL_NO_ERROR;
    }

    alSourcei(player.m_alSource, AL_BUFFER, NULL);
    return ret;
}

void CAudioEngineImpl::StopAll()
{
    for (int index = 0; index < MAX_AUDIOINSTANCES; ++index)
    {
        alSourceStop(m_alSources[index]);
        alSourcei(m_alSources[index], AL_BUFFER, NULL);
        m_alSourceUsedMap[m_alSources[index]] = false;
    }

    for (auto it = m_audioPlayers.begin(); it != m_audioPlayers.end();)
    {
        auto& player = it->second;
        if (player.m_bStreamingSource)
        {
            player.NotifyExitThread();
            ++it;
        }
        else
        {
            it = m_audioPlayers.erase(it);
        }
    }
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

void CAudioEngineImpl::Update(float /*dt*/)
{
    ALint sourceState;
    int audioID;

    if (m_threadMutex.try_lock())
    {
        size_t removeAudioCount = m_toRemoveAudioIDs.size();
        for (size_t index = 0; index < removeAudioCount; ++index)
        {
            audioID = m_toRemoveAudioIDs[index];
            auto playerIt = m_audioPlayers.find(audioID);
            if (playerIt != m_audioPlayers.end())
            {
                m_alSourceUsedMap[playerIt->second.m_alSource] = false;
                m_audioPlayers.erase(audioID);
                CAudioEngine::Remove(audioID);
            }
        }
        m_toRemoveAudioIDs.clear();
        size_t removeCacheCount = m_toRemoveCaches.size();
        for (size_t index = 0; index < removeCacheCount; ++index)
        {
            auto itEnd = m_audioCachesMap.end();
            for (auto it = m_audioCachesMap.begin(); it != itEnd; ++it)
            {
                if (&it->second == m_toRemoveCaches[index])
                {
                    m_audioCachesMap.erase(it);
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
            m_alSourceUsedMap[player.m_alSource] = false;

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
    m_audioCachesMap.erase(filePath);
}

void CAudioEngineImpl::UncacheAll()
{
    m_audioCachesMap.clear();
}

