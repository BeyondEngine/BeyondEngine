#include "stdafx.h"
#include "TimeMeter.h"
#include <chrono>

#ifdef __linux__
#include <sys/time.h>

// Since Linux Kenel 2.6.39
// CLOCK_BOOTTIME lives in <linux/time.h> but we can't
//include that file because it conflicts with <time.h>. 
#ifndef CLOCK_BOOTTIME
# define CLOCK_BOOTTIME 7
#endif

#endif

#ifdef __MACH__
#include <mach/mach_time.h>
#define ACCESS_ONCE(type, var)  (*(volatile type*) &(var))
#endif


CTimeMeter::CTimeMeter()
    : m_uLastTime(GetCurrUSec())
    , m_uCurrentFrameTimeUS(GetCurrUSec())
{
}

CTimeMeter::~CTimeMeter()
{
}

float CTimeMeter::GetDeltaSec() const
{
#ifdef FIX_DELTA_TIME
    return 0.016f;
#else
    uint64_t deltaTime = m_uCurrentFrameTimeUS - m_uLastTime;
    return deltaTime * 0.000001f;
#endif
}

void CTimeMeter::SetLastTime(uint64_t uLastTime)
{
    m_uLastTime = uLastTime;
}

// taken from uv_hrtime(), see http://docs.libuv.org/en/v1.x/misc.html
uint64_t CTimeMeter::GetCurrUSec()
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    LARGE_INTEGER liCounter;
    QueryPerformanceCounter(&liCounter);
    LARGE_INTEGER liFreq;
    QueryPerformanceFrequency(&liFreq);
    return (uint64_t)((double)liCounter.QuadPart * 1000000 / liFreq.QuadPart);
#elif BEYONDENGINE_PLATFORM == PLATFORM_IOS
    // see https://github.com/libuv/libuv/blob/v1.x/src/unix/darwin.c
    static mach_timebase_info_data_t info;
    if ((ACCESS_ONCE(uint32_t, info.numer) == 0 || ACCESS_ONCE(uint32_t, info.denom) == 0))
    {
        int r = mach_timebase_info(&info);
        BEATS_ASSERT(r == KERN_SUCCESS, "mach_timebase_info() return %d", r);
    }
    return (mach_absolute_time() * info.numer / info.denom) / 1000;
#else
    // see https://github.com/libuv/libuv/blob/v1.x/src/unix/linux-core.c
    //
    // Try CLOCK_BOOTTIME first, fall back to CLOCK_MONOTONIC if not available (pre-2.6.39 kernels).
    // CLOCK_MONOTONIC doesn't increase when the system is suspended.
    static volatile int no_clock_boottime;
    struct timespec now = {};
    int r = 0;

    if (no_clock_boottime)
    {
    RETRY:
        r = clock_gettime(CLOCK_MONOTONIC, &now);
    }
    else if ((r = clock_gettime(CLOCK_BOOTTIME, &now)) && errno == EINVAL)
    {
        no_clock_boottime = 1;
        goto RETRY;
    }
    BEATS_ASSERT(r == 0, "clock_gettime() %d failed with %d", no_clock_boottime, r);
    return ((uint64_t)now.tv_sec * (uint64_t)1e9 + now.tv_nsec) / 1000;
#endif
}

uint64_t CTimeMeter::GetCurrentFrameTimeUS() const
{
    return m_uCurrentFrameTimeUS;
}

void CTimeMeter::Tick()
{
    uint64_t currTime = GetCurrUSec();
    m_uLastTime = m_uCurrentFrameTimeUS;
    m_uCurrentFrameTimeUS = currTime;
}

uint64_t CTimeMeter::GetCurrentClockTimeMS()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return (uint64_t)time.count();
}

TString CTimeMeter::GetCurrentClockTimeString()
{
    uint64_t time_point = GetCurrentClockTimeMS();
    time_t sec = time_point / 1000;
    struct tm* pinfo = localtime(&sec); // make sure localtime has thread-safety
    char buffer[MAX_PATH] = {};
    _sntprintf(buffer, MAX_PATH, _T("%d-%02d-%02d %02d:%02d:%02d.%03d"), 1900 + pinfo->tm_year, pinfo->tm_mon + 1,
        pinfo->tm_mday, pinfo->tm_hour, pinfo->tm_min, pinfo->tm_sec, int(time_point % 1000));
    return TString(buffer);
}