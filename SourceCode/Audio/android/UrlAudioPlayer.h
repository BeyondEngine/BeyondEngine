
#pragma once

#include "audio/android/IAudioPlayer.h"
#include "audio/android/OpenSLHelper.h"
#include "audio/android/AssetFd.h"
#include <mutex>
#include <vector>
#include <memory>
#include <thread>

class AssetFd;

class UrlAudioPlayer : public IAudioPlayer
{
public:

    // Override Functions Begin
    virtual int getId() const override
    { return _id; };

    virtual void setId(int id) override
    { _id = id; };

    virtual std::string getUrl() const override
    { return _url; };

    virtual State getState() const override
    { return _state; };

    virtual void play() override;

    virtual void pause() override;

    virtual void resume() override;

    virtual void stop() override;

    virtual void rewind() override;

    virtual void setVolume(float volume) override;

    virtual float getVolume() const override;

    virtual void setLoop(bool isLoop) override;

    virtual bool isLoop() const override;

    virtual float getDuration() const override;

    virtual float getPosition() const override;

    virtual bool setPosition(float pos) override;

    virtual void setPlayEventCallback(const PlayEventCallback &playEventCallback) override;

    virtual bool delayDelete() override;
    // Override Functions EndOv
    virtual void destroy() override;

private:
    UrlAudioPlayer(SLEngineItf engineItf, SLObjectItf outputMixObject);
    virtual ~UrlAudioPlayer();

    bool prepare(const std::string &url, SLuint32 locatorType, std::shared_ptr<AssetFd> assetFd, int start, int length);

    static void stopAll();


    inline void setState(State state)
    { _state = state; };

    void playEventCallback(SLPlayItf caller, SLuint32 playEvent);

private:
    SLEngineItf _engineItf;
    SLObjectItf _outputMixObj;

    int _id;
    std::string _url;

    std::shared_ptr<AssetFd> _assetFd;

    SLObjectItf _playObj;
    SLPlayItf _playItf;
    SLSeekItf _seekItf;
    SLVolumeItf _volumeItf;

    float _volume;
    float _duration;
    bool _isLoop;
    State _state;

    PlayEventCallback _playEventCallback;

    bool _isDestroyed;

    friend class SLUrlAudioPlayerCallbackProxy;
    friend class AudioPlayerProvider;
};
