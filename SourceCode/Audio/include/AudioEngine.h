#include <mutex>
#ifndef BEYOND_ENGINE_AUDIOENGINE_H__INCLUDE
#define BEYOND_ENGINE_AUDIOENGINE_H__INCLUDE

#ifdef ERROR
#undef ERROR
#endif // ERROR
/**
* @class AudioProfile
*
* @brief
* @js NA
*/
class CAudioProfile
{
public:
    //Profile name can't be empty.
    std::string name;
    //The maximum number of simultaneous audio instance.
    unsigned int maxInstances;
    /* Minimum delay in between sounds */
    double minDelay;

    CAudioProfile()
        : maxInstances(0)
        , minDelay(0.0)
    {

    }
};

class CAudioEngineImpl;

/**
* @class AudioEngine
*
* @brief Offers a interface to play audio.
*
* @note Make sure to call AudioEngine::end() when the audio engine is not needed anymore to release resources.
* @js NA
*/
class CAudioEngineThreadPool;

class CAudioEngine
{
public:
    /** AudioState enum,all possible states of an audio instance.*/
    enum class AudioState
    {
        ERROR = -1,
        INITIALZING,
        PLAYING,
        PAUSED
    };

    static const int INVALID_AUDIO_ID;

    static const float TIME_UNKNOWN;

    static bool LazyInit();

    /**
    * Release objects relating to AudioEngine.
    *
    * @warning It must be called before the application exit.
    */
    static void End();

    /**
    * Gets the default profile of audio instances.
    *
    * @return The default profile of audio instances.
    */
    static CAudioProfile* GetDefaultProfile();

    /**
    * Play 2d sound.
    *
    * @param filePath The path of an audio file.
    * @param loop Whether audio instance loop or not.
    * @param volume Volume value (range from 0.0 to 1.0).
    * @param profile A profile for audio instance. When profile is not specified, default profile will be used.
    * @return An audio ID. It allows you to dynamically change the behavior of an audio instance on the fly.
    *
    * @see `AudioProfile`
    */
    static int Play2d(const std::string& filePath, bool loop = false, bool bAffectByPosition = false, float volume = 1.0f, const CAudioProfile *profile = nullptr);

    /**
    * Sets whether an audio instance loop or not.
    *
    * @param audioID An audioID returned by the play2d function.
    * @param loop Whether audio instance loop or not.
    */
    static void SetLoop(int audioID, bool loop);

    /**
    * Checks whether an audio instance is loop.
    *
    * @param audioID An audioID returned by the play2d function.
    * @return Whether or not an audio instance is loop.
    */
    static bool IsLoop(int audioID);

    /**
    * Sets volume for an audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    * @param volume Volume value (range from 0.0 to 1.0).
    */
    static void SetVolume(int audioID, float volume);

    /**
    * Sets volume percent for an audio instance.
    *
    * @param volumePercent Volume Percent value (range from 0.0 to 1.0).
    */
    static void SetVolumePercent(float volumePercent);

    /**
    * Gets the volume value of an audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    * @return Volume value (range from 0.0 to 1.0).
    */
    static float GetVolume(int audioID);

    /**
    * Pause an audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    */
    static void Pause(int audioID);

    /** Pause all playing audio instances. */
    static void PauseAll();

    /**
    * Resume an audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    */
    static void Resume(int audioID);

    /** Resume all suspended audio instances. */
    static void ResumeAll();

    /**
    * Stop an audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    */
    static void Stop(int audioID);

    /** Stop all audio instances. */
    static void StopAll();

    static std::vector<int> GetAudioList();

    /**
    * Gets the duration of an audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    * @return The duration of an audio instance.
    */
    static float GetDuration(int audioID);

    /**
    * Returns the state of an audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    * @return The status of an audio instance.
    */
    static AudioState GetState(int audioID);

    /**
    * Gets the maximum number of simultaneous audio instance of AudioEngine.
    */
    static int GetMaxAudioInstance() { return m_uMaxInstances; }

    /**
    * Sets the maximum number of simultaneous audio instance for AudioEngine.
    *
    * @param maxInstances The maximum number of simultaneous audio instance.
    */
    static bool SetMaxAudioInstance(int maxInstances);

    /**
    * Uncache the audio data from internal buffer.
    * AudioEngine cache audio data on ios,mac, and win32 platform.
    *
    * @warning This can lead to stop related audio first.
    * @param filePath Audio file path.
    */
    static void Uncache(const std::string& filePath);

    /**
    * Uncache all audio data from internal buffer.
    *
    * @warning All audio will be stopped first.
    */
    static void UncacheAll();

    /**
    * Gets the audio profile by id of audio instance.
    *
    * @param audioID An audioID returned by the play2d function.
    * @return The audio profile.
    */
    static CAudioProfile* GetProfile(int audioID);

    /**
    * Gets an audio profile by name.
    *
    * @param profileName A name of audio profile.
    * @return The audio profile.
    */
    static CAudioProfile* GetProfile(const std::string &profileName);

    /**
    * Preload audio file.
    * @param filePath The file path of an audio.
    */
    static void Preload(const std::string& filePath);

    /**
    * Preload audio file.
    * @param filePath The file path of an audio.
    * @param callback A callback which will be called after loading is finished.
    */

    static void Update(float dt);

    static bool CheckError(const TCHAR* pszFunction, uint32_t audioID);

protected:

    static void AddTask(const std::function<void()>& task);
    static void Remove(int nAudioID);

    struct SProfileHelper
    {
        CAudioProfile profile;

        std::list<int> audioIDs;

        double lastPlayTime;

        SProfileHelper()
            : lastPlayTime(0.0)
        {

        }
    };

    struct SAudioInfo
    {
        const std::string* pFilePath;
        SProfileHelper* pProfileHelper;

        float volume;
        bool loop;
        bool bAffectPercentByPersition;
        float duration;
        AudioState state;

        SAudioInfo()
            : pProfileHelper(nullptr)
            , pFilePath(nullptr)
            , duration(TIME_UNKNOWN)
            , state(AudioState::INITIALZING)
            , volume(1.0f)
            , loop(false)
            , bAffectPercentByPersition(false)
        {

        }
    };
    static float m_fVolumePercent;
    static std::mutex m_audioIdMutex;
    //audioID,audioAttribute
    static std::map<int, SAudioInfo> m_audioIDInfoMap;

    //audio file path,audio IDs
    static std::map<std::string, std::list<int>> m_audioPathIDMap;

    //profileName,ProfileHelper
    static std::map<std::string, SProfileHelper> m_audioPathProfileHelperMap;

    static unsigned int m_uMaxInstances;

    static SProfileHelper* m_pDefaultProfileHelper;

    static CAudioEngineImpl* m_pAudioEngineImpl;

    static CAudioEngineThreadPool* m_pThreadPool;

    friend class CAudioEngineImpl;
};

#ifdef DEVELOP_VERSION
#define CHECK_AUDIO_ERROR(id) CAudioEngine::CheckError(__FUNCTION__, id)
#else
#define CHECK_AUDIO_ERROR(id)
#endif
#endif // __AUDIO_ENGINE_H_
