#ifndef UTIL_CPU_TIMING_H
#define UTIL_CPU_TIMING_H

const uint32_t PROF_COLOR_GRAPHICS = 0xFF1B00FF;

#if defined(NW_PROFILE) && defined(_WIN64)

#define USE_PIX
#include <pix3.h>

class ScopeCpuEvent
{
public:
    ScopeCpuEvent(uint32_t color, const char* name)
    {
        PIXBeginEvent(color, name);
    }
    ~ScopeCpuEvent()
    {
        PIXEndEvent();
    }
};

#else
class ScopeCpuEvent
{
public:
    ScopeCpuEvent(uint32_t, const char*) { }
    ~ScopeCpuEvent() { }
};
#endif

#define SCOPED_CPU_EVENT(name) ScopeCpuEvent name

#endif
