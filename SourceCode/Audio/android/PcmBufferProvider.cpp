
#define LOG_TAG "PcmBufferProvider"

#include "audio/android/cutils/log.h"
#include "audio/android/PcmBufferProvider.h"

//#define VERY_VERY_VERBOSE_LOGGING
#ifdef VERY_VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while (0)
#endif

PcmBufferProvider::PcmBufferProvider()
        : _addr(nullptr)
        , _numFrames(0)
        , _frameSize(0)
        , _nextFrame(0)
        , _unrel(0)
{
    
}

bool PcmBufferProvider::init(const void *addr, size_t frames, size_t frameSize)
{
    _addr = addr;
    _numFrames = frames;
    _frameSize = frameSize;
    _nextFrame = 0;
    _unrel = 0;
    return true;
}

status_t PcmBufferProvider::getNextBuffer(Buffer *buffer,
                                          int64_t pts/* = kInvalidPTS*/) {
    (void) pts; // suppress warning
    size_t requestedFrames = buffer->frameCount;
    if (requestedFrames > _numFrames - _nextFrame) {
        buffer->frameCount = _numFrames - _nextFrame;
    }

    ALOGVV("getNextBuffer() requested %zu frames out of %zu frames available,"
                  " and returned %zu frames",
              requestedFrames, (size_t) (_numFrames - _nextFrame), buffer->frameCount);

    _unrel = buffer->frameCount;
    if (buffer->frameCount > 0) {
        buffer->raw = (char *) _addr + _frameSize * _nextFrame;
        return NO_ERROR;
    } else {
        buffer->raw = NULL;
        return NOT_ENOUGH_DATA;
    }
}

void PcmBufferProvider::releaseBuffer(Buffer *buffer) {
    if (buffer->frameCount > _unrel) {
        ALOGVV("ERROR releaseBuffer() released %zu frames but only %zu available "
                "to release", buffer->frameCount, _unrel);
        _nextFrame += _unrel;
        _unrel = 0;
    } else {
        ALOGVV("releaseBuffer() released %zu frames out of %zu frames available "
                           "to release", buffer->frameCount, _unrel);
        _nextFrame += buffer->frameCount;
        _unrel -= buffer->frameCount;
    }
    buffer->frameCount = 0;
    buffer->raw = NULL;
}

void PcmBufferProvider::reset() {
    _nextFrame = 0;
}
