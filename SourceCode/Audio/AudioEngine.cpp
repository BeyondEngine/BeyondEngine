#include "stdafx.h"
#include "Audio/include/AudioEngine.h"

#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#include "android/AudioEngine-inl.h"
#elif (BEYONDENGINE_PLATFORM == PLATFORM_IOS || BEYONDENGINE_PLATFORM == PLATFORM_MAC)
#include "ios/AudioEngine-inl.h"
#elif (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
#include "win32/AudioEngine-win32.h"
#endif
#include "Framework/Application.h"

#if (BEYONDENGINE_PLATFORM != PLATFORM_LINUX)
#include "AL/al.h"
#endif
#define PRINT_AUDIO_INFO
#define TIME_DELAY_PRECISION 0.0001

#ifdef ERROR
#undef ERROR
#endif // ERROR

const int CAudioEngine::INVALID_AUDIO_ID = -1;
const float CAudioEngine::TIME_UNKNOWN = -1.0f;
CAudioEngine::SProfileHelper* CAudioEngine::m_pDefaultProfileHelper = nullptr;

std::mutex CAudioEngine::m_audioIdMutex;
//audio file path,audio IDs
std::map<std::string, std::list<int>> CAudioEngine::m_audioPathIDMap;
//profileName,ProfileHelper
std::map<std::string, CAudioEngine::SProfileHelper> CAudioEngine::m_audioPathProfileHelperMap;
unsigned int CAudioEngine::m_uMaxInstances = MAX_AUDIOINSTANCES;

std::map<int, CAudioEngine::SAudioInfo> CAudioEngine::m_audioIDInfoMap;
CAudioEngineImpl* CAudioEngine::m_pAudioEngineImpl = nullptr;
CAudioEngineThreadPool* CAudioEngine::m_pThreadPool = nullptr;
float CAudioEngine::m_fVolumePercent = 1.0f;

class CAudioEngineThreadPool
{
public:
    CAudioEngineThreadPool(int threads = 4)
        : m_bStop(false)
    {
        for (int index = 0; index < threads; ++index)
        {
            m_workers.emplace_back(std::thread(std::bind(&CAudioEngineThreadPool::threadFunc, this)));
        }
    }

    void AddTask(const std::function<void()> &task)
    {
        std::unique_lock<std::mutex> lk(m_queueMutex);
        m_taskQueue.emplace(task);
        m_taskCondition.notify_one();
    }

    ~CAudioEngineThreadPool()
    {
        {
            std::unique_lock<std::mutex> lk(m_queueMutex);
            m_bStop = true;
            m_taskCondition.notify_all();
        }

        for (auto&& worker : m_workers)
        {
            worker.join();
        }
    }

private:
    void threadFunc()
    {
        while (true)
        {
            std::function<void()> task = nullptr;
            {
                std::unique_lock<std::mutex> lk(m_queueMutex);
                if (m_bStop)
                {
                    break;
                }
                if (!m_taskQueue.empty())
                {
                    task = std::move(m_taskQueue.front());
                    m_taskQueue.pop();
                }
                else
                {
                    m_taskCondition.wait(lk);
                    continue;
                }
            }
            task();
        }
    }

    std::vector<std::thread>  m_workers;
    std::queue< std::function<void()> > m_taskQueue;

    std::mutex m_queueMutex;
    std::condition_variable m_taskCondition;
    bool m_bStop;
};

void CAudioEngine::End()
{
    BEATS_SAFE_DELETE(m_pThreadPool);
    BEATS_SAFE_DELETE(m_pAudioEngineImpl);
    BEATS_SAFE_DELETE(m_pDefaultProfileHelper);
}

bool CAudioEngine::LazyInit()
{
    bool bRet = true;
    if (m_pAudioEngineImpl == nullptr)
    {
        m_pAudioEngineImpl = new (std::nothrow) CAudioEngineImpl();
        if (!m_pAudioEngineImpl || !m_pAudioEngineImpl->Init()){
            delete m_pAudioEngineImpl;
            m_pAudioEngineImpl = nullptr;
            bRet = false;
        }
    }

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    if (m_pAudioEngineImpl && m_pThreadPool == nullptr)
    {
        m_pThreadPool = new (std::nothrow) CAudioEngineThreadPool();
    }
#elif BEYONDENGINE_PLATFORM != PLATFORM_ANDROID
    if (m_pAudioEngineImpl && m_pThreadPool == nullptr)
    {
        m_pThreadPool = new (std::nothrow) CAudioEngineThreadPool();
    }
#endif
    return bRet;
}

int CAudioEngine::Play2d(const std::string& filePath, bool loop, bool bAffectByPosition, float volume, const CAudioProfile *pProfile)
{
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::Play2d can only be invoked in main thread!");
    int ret = 0;
    BEATS_ASSERT(!filePath.empty());
    ret = CAudioEngine::INVALID_AUDIO_ID;
    if (LazyInit())
    {
        GetDefaultProfile();//Create default profile helper
        auto profileHelper = m_pDefaultProfileHelper;
        if (pProfile && pProfile != &profileHelper->profile)
        {
            BEATS_ASSERT(!pProfile->name.empty());
            profileHelper = &m_audioPathProfileHelperMap[pProfile->name];
            profileHelper->profile = *pProfile;
        }
        if (m_audioIDInfoMap.size() < m_uMaxInstances)
        {
            bool bCheckProfileHelper = profileHelper == nullptr;
            if (!bCheckProfileHelper)
            {
                bCheckProfileHelper = profileHelper->profile.maxInstances == 0 || profileHelper->audioIDs.size() < profileHelper->profile.maxInstances;
                if (bCheckProfileHelper && profileHelper->profile.minDelay > TIME_DELAY_PRECISION)
                {
                    auto currTime = CApplication::GetInstance()->GetFrameTimeMS();
                    bCheckProfileHelper = profileHelper->lastPlayTime <= TIME_DELAY_PRECISION || currTime - profileHelper->lastPlayTime > profileHelper->profile.minDelay;
                }
            }
            if (bCheckProfileHelper)
            {
                BEATS_CLIP_VALUE(volume, 0.0f, 1.0f);
                float fRealVolume = bAffectByPosition ? m_fVolumePercent * volume : volume;
#ifdef PRINT_AUDIO_INFO
                BEATS_PRINT("CAudioEngine::Play2d: Volume:%f file:%s\n", fRealVolume, filePath.c_str());
#endif
                ret = m_pAudioEngineImpl->Play2d(filePath, loop, fRealVolume);
                if (ret != INVALID_AUDIO_ID)
                {
                    m_audioPathIDMap[filePath].push_back(ret);
                    auto it = m_audioPathIDMap.find(filePath);

                    auto& audioRef = m_audioIDInfoMap[ret];
                    audioRef.volume = volume;
                    audioRef.loop = loop;
                    audioRef.bAffectPercentByPersition = bAffectByPosition;
                    audioRef.pFilePath = &it->first;
                    BEATS_ASSERT(audioRef.pFilePath != nullptr);
                    profileHelper->lastPlayTime = CApplication::GetInstance()->GetFrameTimeMS();
                    profileHelper->audioIDs.push_back(ret);
                    audioRef.pProfileHelper = profileHelper;
                }
            }
        }
    }
    return ret;
}

void CAudioEngine::SetLoop(int audioID, bool loop)
{
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end() && it->second.loop != loop)
    {
        m_pAudioEngineImpl->SetLoop(audioID, loop);
        it->second.loop = loop;
    }
}

void CAudioEngine::SetVolume(int audioID, float volume)
{
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::SetVolume can only be invoked in main thread!");
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end())
    {
        BEATS_CLIP_VALUE(volume, 0, 1.0f);
        if (it->second.volume != volume)
        {
            float fVolumePercent = it->second.bAffectPercentByPersition ? m_fVolumePercent : 1.0f;
            m_pAudioEngineImpl->SetVolume(audioID, volume * fVolumePercent);
            it->second.volume = volume;
        }
    }
}

void CAudioEngine::SetVolumePercent(float volumePercent)
{
    BEATS_CLIP_VALUE(volumePercent, 0, 1.0f);
    if (m_fVolumePercent != volumePercent)
    {
        m_fVolumePercent = volumePercent;
        for (auto iter : m_audioIDInfoMap)
        {
            if (iter.second.bAffectPercentByPersition)
            {
                m_pAudioEngineImpl->SetVolume(iter.first, iter.second.volume * volumePercent);
            }
        }
    }
}

void CAudioEngine::Pause(int audioID)
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::Pause\n");
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::Pause can only be invoked in main thread!");
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end() && it->second.state == AudioState::PLAYING)
    {
        m_pAudioEngineImpl->Pause(audioID);
        it->second.state = AudioState::PAUSED;
    }
}

void CAudioEngine::PauseAll()
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::PauseAll\n");
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::PauseAll can only be invoked in main thread!");
    auto itEnd = m_audioIDInfoMap.end();
    for (auto it = m_audioIDInfoMap.begin(); it != itEnd; ++it)
    {
        if (it->second.state == AudioState::PLAYING)
        {
            m_pAudioEngineImpl->Pause(it->first);
            it->second.state = AudioState::PAUSED;
        }
    }
}

void CAudioEngine::Resume(int audioID)
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::Resume %d\n", audioID);
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::Resume can only be invoked in main thread!");
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end() && it->second.state == AudioState::PAUSED)
    {
        m_pAudioEngineImpl->Resume(audioID);
        it->second.state = AudioState::PLAYING;
    }
}

void CAudioEngine::ResumeAll()
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::ResumeAll\n");
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::ResumeAll can only be invoked in main thread!");
    auto itEnd = m_audioIDInfoMap.end();
    for (auto it = m_audioIDInfoMap.begin(); it != itEnd; ++it)
    {
        if (it->second.state == AudioState::PAUSED)
        {
            m_pAudioEngineImpl->Resume(it->first);
            it->second.state = AudioState::PLAYING;
        }
    }
}

void CAudioEngine::Stop(int audioID)
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::Stop %d\n", audioID);
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::Stop can only be invoked in main thread!");
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end())
    {
        m_pAudioEngineImpl->Stop(audioID);
    }
}

void CAudioEngine::Remove(int audioID)
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::Remove %d\n", audioID);
#endif
    std::unique_lock<std::mutex> lck(m_audioIdMutex);
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end())
    {
        if (it->second.pProfileHelper)
        {
            it->second.pProfileHelper->audioIDs.remove(audioID);
        }
        BEATS_ASSERT(it->second.pFilePath != nullptr);
        m_audioPathIDMap[*it->second.pFilePath].remove(audioID);
        m_audioIDInfoMap.erase(audioID);
    }
}

void CAudioEngine::StopAll()
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::StopAll\n");
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::StopAll can only be invoked in main thread!");
    if (m_pAudioEngineImpl)
    {
        m_pAudioEngineImpl->StopAll();
        std::vector<int> idList;
        auto itEnd = m_audioIDInfoMap.end();
        for (auto it = m_audioIDInfoMap.begin(); it != itEnd; ++it)
        {
            idList.push_back(it->first);
        }
        for (auto iter : idList)
        {
            Remove(iter);
        }
    }
}

std::vector<int> CAudioEngine::GetAudioList()
{
    std::vector<int> audioList;
    for (auto& iter : m_audioIDInfoMap)
    {
        audioList.push_back(iter.first);
    }
    return audioList;
}

void CAudioEngine::Uncache(const std::string &filePath)
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::Uncache %s\n", filePath.c_str());
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::Uncache can only be invoked in main thread!");
    if (m_audioPathIDMap.find(filePath) != m_audioPathIDMap.end())
    {
        auto itEnd = m_audioPathIDMap[filePath].end();
        for (auto it = m_audioPathIDMap[filePath].begin(); it != itEnd; ++it)
        {
            auto audioID = *it;
            m_pAudioEngineImpl->Stop(audioID);

            auto itInfo = m_audioIDInfoMap.find(audioID);
            if (itInfo != m_audioIDInfoMap.end())
            {
                if (itInfo->second.pProfileHelper)
                {
                    itInfo->second.pProfileHelper->audioIDs.remove(audioID);
                }
                m_audioIDInfoMap.erase(audioID);
            }
        }
        m_audioPathIDMap.erase(filePath);
    }

    if (m_pAudioEngineImpl)
    {
        m_pAudioEngineImpl->Uncache(filePath);
    }
}

void CAudioEngine::UncacheAll()
{
#ifdef PRINT_AUDIO_INFO
    BEATS_PRINT("CAudioEngine::UncacheAll\n");
#endif
    BEATS_ASSERT(CEngineCenter::GetInstance()->GetMainThreadId() == std::this_thread::get_id(), "CAudioEngine::UncacheAll can only be invoked in main thread!");
    if (m_pAudioEngineImpl)
    {
        StopAll();
        m_pAudioEngineImpl->UncacheAll();
    }
}

float CAudioEngine::GetDuration(int audioID)
{
    float fRet = TIME_UNKNOWN;
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end() && it->second.state != AudioState::INITIALZING)
    {
        if (it->second.duration == TIME_UNKNOWN)
        {
            it->second.duration = m_pAudioEngineImpl->GetDuration(audioID);
        }
        fRet = it->second.duration;
    }
    return fRet;
}

bool CAudioEngine::SetMaxAudioInstance(int maxInstances)
{
    bool bRet = maxInstances > 0 && maxInstances <= MAX_AUDIOINSTANCES;
    if (bRet)
    {
        m_uMaxInstances = maxInstances;
    }
    return bRet;
}

bool CAudioEngine::IsLoop(int audioID)
{
    bool bRet = false;
    auto tmpIterator = m_audioIDInfoMap.find(audioID);
    BEATS_ASSERT(tmpIterator != m_audioIDInfoMap.end(), "AudioEngine::isLoop-->The audio instance %d is non-existent", audioID);
    if (tmpIterator != m_audioIDInfoMap.end())
    {
        bRet = tmpIterator->second.loop;
    }
    return bRet;
}

float CAudioEngine::GetVolume(int audioID)
{
    float fRet = 0;
    auto tmpIterator = m_audioIDInfoMap.find(audioID);
    BEATS_ASSERT(tmpIterator != m_audioIDInfoMap.end(), "AudioEngine::getVolume-->The audio instance %d is non-existent", audioID);
    if (tmpIterator != m_audioIDInfoMap.end())
    {
        fRet = tmpIterator->second.volume;
    }
    return fRet;
}

CAudioEngine::AudioState CAudioEngine::GetState(int audioID)
{
    CAudioEngine::AudioState ret = AudioState::ERROR;
    auto tmpIterator = m_audioIDInfoMap.find(audioID);
    if (tmpIterator != m_audioIDInfoMap.end())
    {
        ret = tmpIterator->second.state;
    }
    return ret;
}

CAudioProfile* CAudioEngine::GetProfile(int audioID)
{
    CAudioProfile* pRet = nullptr;
    auto it = m_audioIDInfoMap.find(audioID);
    if (it != m_audioIDInfoMap.end())
    {
        pRet = &it->second.pProfileHelper->profile;
    }
    return pRet;
}

CAudioProfile* CAudioEngine::GetDefaultProfile()
{
    if (m_pDefaultProfileHelper == nullptr)
    {
        m_pDefaultProfileHelper = new (std::nothrow) SProfileHelper();
    }
    return &m_pDefaultProfileHelper->profile;
}

CAudioProfile* CAudioEngine::GetProfile(const std::string &name)
{
    CAudioProfile* pRet = nullptr;
    auto it = m_audioPathProfileHelperMap.find(name);
    if (it != m_audioPathProfileHelperMap.end())
    {
        pRet = &it->second.profile;
    }
    return pRet;
}

void CAudioEngine::Preload(const std::string& filePath)
{
    LazyInit();
    if (m_pAudioEngineImpl)
    {
        BEATS_ASSERT(!filePath.empty());
        m_pAudioEngineImpl->Preload(filePath);
    }
}

void CAudioEngine::AddTask(const std::function<void()>& task)
{
    LazyInit();
    if (m_pAudioEngineImpl && m_pThreadPool)
    {
        m_pThreadPool->AddTask(task);
    }
}

void CAudioEngine::Update(float dt)
{
    if (m_pAudioEngineImpl)
    {
        m_pAudioEngineImpl->Update(dt);
    }
}

bool CAudioEngine::CheckError(const TCHAR* pszFunction, uint32_t audioID)
{
#if (BEYONDENGINE_PLATFORM != PLATFORM_ANDROID)
    auto error = alGetError();
    if (error == AL_INVALID_NAME)
    {
        BEATS_ASSERT(false, "%s: audio id = %d, error = %s\n", pszFunction, audioID, "AL_INVALID_NAME");
    }
    else if (error == AL_INVALID_ENUM)
    {
        BEATS_ASSERT(false, "%s: audio id = %d, error = %s\n", pszFunction, audioID, "AL_INVALID_ENUM");
    }
    else if (error == AL_INVALID_VALUE)
    {
        BEATS_ASSERT(false, "%s: audio id = %d, error = %s\n", pszFunction, audioID, "AL_INVALID_VALUE");
    }
    else if (error == AL_INVALID_OPERATION)
    {
        BEATS_ASSERT(false, "%s: audio id = %d, error = %s\n", pszFunction, audioID, "AL_INVALID_OPERATION");
    }
    else if (error == AL_OUT_OF_MEMORY)
    {
        BEATS_ASSERT(false, "%s: audio id = %d, error = %s\n", pszFunction, audioID, "AL_OUT_OF_MEMORY");
    }
    else if (AL_NO_ERROR != error)
    {
        BEATS_ASSERT(false, "%s: audio id = %d, error = %s\n", pszFunction, audioID, "Unknown error!");
    }
    return AL_NO_ERROR == error;
#else
    return true;
#endif
}
