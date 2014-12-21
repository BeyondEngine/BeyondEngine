
#pragma once

#include "audio/android/PcmData.h"
#include "audio/android/IVolumeProvider.h"
#include "audio/android/PcmBufferProvider.h"

#include <functional>
#include <mutex>

class Track : public PcmBufferProvider, public IVolumeProvider
{
public:
    enum class State
    {
        IDLE,
        PLAYING,
        RESUMED,
        PAUSED,
        STOPPED,
        OVER,
        DESTROYED
    };

    Track(const PcmData &pcmData);
    virtual ~Track();

    inline State getState() const { return _state; };
    void setState(State state);

    inline State getPrevState() const { return _prevState; };

    inline bool isPlayOver() const { return _state == State::PLAYING && _nextFrame >= _numFrames;};
    inline void setName(int name) { _name = name; };
    inline int getName() const { return _name; };

    void setVolume(float volume);
    float getVolume() const;

    bool setPosition(float pos);
    float getPosition() const;

    virtual gain_minifloat_packed_t getVolumeLR() override ;

    inline void setLoop(bool isLoop) { _isLoop = isLoop; };
    inline bool isLoop() const { return _isLoop; };

    std::function<void(State)> onStateChanged;

private:
    inline bool isVolumeDirty() const
    { return _isVolumeDirty; };

    inline void setVolumeDirty(bool isDirty)
    { _isVolumeDirty = isDirty; };

    inline bool isInitialized() const
    { return _isInitialized; };

    inline void setInitialized(bool isInitialized)
    { _isInitialized = isInitialized; };

private:
    PcmData _pcmData;
    State _prevState;
    State _state;
    std::mutex _stateMutex;
    int _name;
    float _volume;
    bool _isVolumeDirty;
    std::mutex _volumeDirtyMutex;
    bool _isLoop;
    bool _isInitialized;

    friend class AudioMixerController;
};
