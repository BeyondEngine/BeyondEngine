
#define LOG_TAG "Track"

#include "audio/android/cutils/log.h"
#include "audio/android/Track.h"

#include <math.h>

Track::Track(const PcmData &pcmData)
        : onStateChanged(nullptr)
        , _pcmData(pcmData)
        , _prevState(State::IDLE)
        , _state(State::IDLE)
        , _name(-1)
        , _volume(1.0f)
        , _isVolumeDirty(true)
        , _isLoop(false)
        , _isInitialized(false)
{
    init(_pcmData.pcmBuffer->data(), _pcmData.numFrames, _pcmData.bitsPerSample / 8 * _pcmData.numChannels);
}

Track::~Track()
{
    ALOGV("~Track(): %p", this);
}

gain_minifloat_packed_t Track::getVolumeLR()
{
    gain_minifloat_t v = gain_from_float(_volume);
    return gain_minifloat_pack(v, v);
}

bool Track::setPosition(float pos)
{
    _nextFrame = (size_t) (pos * _numFrames / _pcmData.duration);
    _unrel = 0;
    return true;
}

float Track::getPosition() const
{
    return _nextFrame * _pcmData.duration / _numFrames;
}

void Track::setVolume(float volume)
{
    std::lock_guard<std::mutex> lk(_volumeDirtyMutex);
    if (fabs(_volume - volume) > 0.00001)
    {
        _volume = volume;
        setVolumeDirty(true);
    }
}

float Track::getVolume() const
{
    return _volume;
}

void Track::setState(State state)
{
    std::lock_guard<std::mutex> lk(_stateMutex);
    if (_state != state)
    {
        _prevState = _state;
        _state = state;
        onStateChanged(_state);
    }
};
