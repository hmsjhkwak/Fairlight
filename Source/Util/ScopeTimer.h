#ifndef UTIL_SCOPE_TIMER_H
#define UTIL_SCOPE_TIMER_H

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

class ScopeTimer
{
private:
    const char* _message;
#ifdef _WIN32
    LARGE_INTEGER _startTime;
#endif

public:
    ScopeTimer(const char* message);
    ~ScopeTimer();
};

#endif
