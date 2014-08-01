#ifndef BEYOND_ENGINE_TIMEMETER_H__INCLUDE
#define BEYOND_ENGINE_TIMEMETER_H__INCLUDE

class CTimeMeter
{
public:
    CTimeMeter();
    ~CTimeMeter();

    float GetDeltaSec();
    static unsigned long long GetCurrUSec();

private:
    unsigned long long m_uLastTime;
};

#endif