#ifndef BEYOND_ENGINE_TIMEMETER_H__INCLUDE
#define BEYOND_ENGINE_TIMEMETER_H__INCLUDE
#if BEYONDENGINE_PLATFORM == PLATFORM_IOS
#include <stdint.h>
#endif
#include <string>

class CTimeMeter
{
public:
    CTimeMeter();
    ~CTimeMeter();

    void Tick();
    float GetDeltaSec() const;
    void SetLastTime(uint64_t uLastTime);
    uint64_t GetCurrentFrameTimeUS() const;
    static uint64_t GetCurrUSec();

    // current system clock time in milliseconds
    static uint64_t GetCurrentClockTimeMS();
    static std::string GetCurrentClockTimeString();
private:
    uint64_t m_uLastTime;
    uint64_t m_uCurrentFrameTimeUS;
};

#endif