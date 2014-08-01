#ifndef BEYOND_ENGINE_AUDIO_SIMPLE_AUDIO_ENGINE_H__INCLUDE
#define BEYOND_ENGINE_AUDIO_SIMPLE_AUDIO_ENGINE_H__INCLUDE

class SimpleAudioEngine
{
    BEATS_DECLARE_SINGLETON(SimpleAudioEngine);

public:
    void end();

public:

    /**
     @brief Preload background music
     @param pszFilePath The path of the background music file.
     * @js preloadMusic
     * @lua preloadMusic
     */
     void preloadBackgroundMusic(const char* pszFilePath);
    
    /**
     @brief Play background music
     @param pszFilePath The path of the background music file,or the FileName of T_SoundResInfo
     @param bLoop Whether the background music loop or not
     * @js playMusic
     * @lua playMusic
     */
     void playBackgroundMusic(const char* pszFilePath, bool bLoop = false);

    /**
     @brief Stop playing background music
     @param bReleaseData If release the background music data or not.As default value is false
     * @js stopMusic
     * @lua stopMusic
     */
     void stopBackgroundMusic(bool bReleaseData = false);

    /**
     @brief Pause playing background music
     * @js pauseMusic
     * @lua pauseMusic
     */
     void pauseBackgroundMusic();

    /**
     @brief Resume playing background music
     * @js resumeMusic
     * @lua resumeMusic
     */
     void resumeBackgroundMusic();

    /**
     @brief Rewind playing background music
     * @js rewindMusic
     * @lua rewindMusic
     */
     void rewindBackgroundMusic();

    /**
     @brief Indicates whether any background music can be played or not.
     @return <i>true</i> if background music can be played, otherwise <i>false</i>.
     * @js willPlayMusic
     * @lua willPlayMusic
     */
     bool willPlayBackgroundMusic();

    /**
     @brief Indicates whether the background music is playing
     @return <i>true</i> if the background music is playing, otherwise <i>false</i>
     * @js isMusicPlaying
     * @lua isMusicPlaying
     */
     bool isBackgroundMusicPlaying();

    // 
    // properties
    //

    /**
     @brief The volume of the background music within the range of 0.0 as the minimum and 1.0 as the maximum.
     * @js getMusicVolume
     * @lua getMusicVolume
     */
     float getBackgroundMusicVolume();

    /**
     @brief Set the volume of background music
     @param volume must be within the range of 0.0 as the minimum and 1.0 as the maximum.
     * @js setMusicVolume
     * @lua setMusicVolume
     */
     void setBackgroundMusicVolume(float volume);

    /**
    @brief The volume of the effects within the range of 0.0 as the minimum and 1.0 as the maximum.
    */
     float getEffectsVolume();

    /**
    @brief Set the volume of sound effects
    @param volume must be within the range of 0.0 as the minimum and 1.0 as the maximum.
    */
     void setEffectsVolume(float volume);

    // 
    // for sound effects

    /**
    @brief Play sound effect with a file path, pitch, pan and gain
    @param pszFilePath The path of the effect file.
    @param bLoop Determines whether to loop the effect playing or not. The default value is false.
    @param pitch Frequency, normal value is 1.0. Will also change effect play time.
    @param pan   Stereo effect, in the range of [-1..1] where -1 enables only left channel.
    @param gain  Volume, in the range of [0..1]. The normal value is 1.
    @return the OpenAL source id

    @note Full support is under development, now there are limitations:
        - no pitch effect on Samsung Galaxy S2 with OpenSL backend enabled;
        - no pitch/pan/gain on emscrippten, win32, marmalade.
    */
     unsigned int playEffect(const char* pszFilePath, bool bLoop = false,
                                    float pitch = 1.0f, float pan = 0.0f, float gain = 1.0f);

    /**
    @brief Pause playing sound effect
    @param nSoundId The return value of function playEffect
    */
     void pauseEffect(unsigned int nSoundId);

    /**
    @brief Pause all playing sound effect
    */
     void pauseAllEffects();

    /**
    @brief Resume playing sound effect
    @param nSoundId The return value of function playEffect
    */
     void resumeEffect(unsigned int nSoundId);

    /**
    @brief Resume all playing sound effect
    */
     void resumeAllEffects();

    /**
    @brief Stop playing sound effect
    @param nSoundId The return value of function playEffect
    */
     void stopEffect(unsigned int nSoundId);

    /**
    @brief Stop all playing sound effects
    */
     void stopAllEffects();

    /**
    @brief          preload a compressed audio file
    @details        the compressed audio will be decoded to wave, then written into an internal buffer in SimpleAudioEngine
    @param pszFilePath The path of the effect file
    */
     void preloadEffect(const char* pszFilePath);

    /**
    @brief          unload the preloaded effect from internal buffer
    @param pszFilePath        The path of the effect file
    */
     void unloadEffect(const char* pszFilePath);
};

#endif // _SIMPLE_AUDIO_ENGINE_H_
