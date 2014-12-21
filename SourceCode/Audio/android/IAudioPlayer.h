
#pragma once

#include <functional>

class IAudioPlayer
{
public:
    enum class State
    {
        INVALID = 0,
        INITIALIZED,
        PLAYING,
        PAUSED,
        STOPPED,
        OVER
    };

    using PlayEventCallback = std::function<void(State)>;

    virtual ~IAudioPlayer()
    { };

    virtual int getId() const = 0;

    virtual void setId(int id) = 0;

    virtual std::string getUrl() const = 0;

    virtual State getState() const = 0;

    virtual void play() = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;

    virtual void stop() = 0;

    virtual void rewind() = 0;

    virtual void setVolume(float volume) = 0;

    virtual float getVolume() const = 0;

    virtual void setLoop(bool isLoop) = 0;

    virtual bool isLoop() const = 0;

    virtual float getDuration() const = 0;

    virtual float getPosition() const = 0;

    virtual bool setPosition(float pos) = 0;

    virtual bool delayDelete(){ return false;};

    virtual void destroy(){};
    // @note: STOPPED event is invoked in main thread
    //        OVER event is invoked in sub thread
    virtual void setPlayEventCallback(const PlayEventCallback &playEventCallback) = 0;
};

