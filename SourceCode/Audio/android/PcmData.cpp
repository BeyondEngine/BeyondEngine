
#define LOG_TAG "PcmData"

#include "audio/android/OpenSLHelper.h"
#include "audio/android/PcmData.h"

PcmData::PcmData()
{
//    ALOGV("In the constructor of PcmData (%p)", this);
    reset();
}

PcmData::~PcmData()
{
//    ALOGV("In the destructor of PcmData (%p)", this);
}

PcmData::PcmData(const PcmData &o)
{
//    ALOGV("In the copy constructor of PcmData (%p)", this);
    numChannels = o.numChannels;
    sampleRate = o.sampleRate;
    bitsPerSample = o.bitsPerSample;
    containerSize = o.containerSize;
    channelMask = o.channelMask;
    endianness = o.endianness;
    numFrames = o.numFrames;
    duration = o.duration;
    pcmBuffer = std::move(o.pcmBuffer);
}

PcmData::PcmData(PcmData &&o)
{
//    ALOGV("In the move constructor of PcmData (%p)", this);
    numChannels = o.numChannels;
    sampleRate = o.sampleRate;
    bitsPerSample = o.bitsPerSample;
    containerSize = o.containerSize;
    channelMask = o.channelMask;
    endianness = o.endianness;
    numFrames = o.numFrames;
    duration = o.duration;
    pcmBuffer = std::move(o.pcmBuffer);
    o.reset();
}

PcmData &PcmData::operator=(const PcmData &o)
{
//    ALOGV("In the copy assignment of PcmData");
    numChannels = o.numChannels;
    sampleRate = o.sampleRate;
    bitsPerSample = o.bitsPerSample;
    containerSize = o.containerSize;
    channelMask = o.channelMask;
    endianness = o.endianness;
    numFrames = o.numFrames;
    duration = o.duration;
    pcmBuffer = o.pcmBuffer;
    return *this;
}

PcmData &PcmData::operator=(PcmData &&o)
{
//    ALOGV("In the move assignment of PcmData");
    numChannels = o.numChannels;
    sampleRate = o.sampleRate;
    bitsPerSample = o.bitsPerSample;
    containerSize = o.containerSize;
    channelMask = o.channelMask;
    endianness = o.endianness;
    numFrames = o.numFrames;
    duration = o.duration;
    pcmBuffer = std::move(o.pcmBuffer);
    o.reset();
    return *this;
}

void PcmData::reset()
{
    numChannels = -1;
    sampleRate = -1;
    bitsPerSample = -1;
    containerSize = -1;
    channelMask = -1;
    endianness = -1;
    numFrames = -1;
    duration = -1.0f;
    pcmBuffer = nullptr;
}

bool PcmData::isValid() const
{
    return numChannels > 0 && sampleRate > 0 && bitsPerSample > 0 && containerSize > 0
           && numFrames > 0 && duration > 0 && pcmBuffer != nullptr;
}

std::string PcmData::toString() const
{
    std::string ret;
    char buf[256] = {0};

    snprintf(buf, sizeof(buf),
             "numChannels: %d, sampleRate: %d, bitPerSample: %d, containerSize: %d, "
                     "channelMask: %d, endianness: %d, numFrames: %d, duration: %f",
             numChannels, sampleRate, bitsPerSample, containerSize, channelMask, endianness,
             numFrames, duration
    );

    ret = buf;
    return ret;
}

