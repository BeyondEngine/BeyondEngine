#include "stdafx.h"
#include "TimeMeter.h"

CTimeMeter::CTimeMeter()
    : m_uLastTime(GetCurrUSec())
{
}

CTimeMeter::~CTimeMeter()
{
}

float CTimeMeter::GetDeltaSec()
{
    unsigned long long currTime = GetCurrUSec();
    unsigned long long deltaTime = currTime - m_uLastTime;
    m_uLastTime = currTime;
    return (float)((double)deltaTime / 1000000);
}

unsigned long long CTimeMeter::GetCurrUSec()
{
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    LARGE_INTEGER liCounter;
    QueryPerformanceCounter(&liCounter);
    LARGE_INTEGER liFreq;
    QueryPerformanceFrequency(&liFreq);
    return (unsigned long long)((double)liCounter.QuadPart / liFreq.QuadPart * 1000000);
#else
    timeval now;
    gettimeofday(&now, nullptr);
    return (unsigned long long)now.tv_sec * 1000000 + now.tv_usec;
#endif
}
