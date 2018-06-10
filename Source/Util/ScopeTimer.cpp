#include "Core/Core.h"
#include "ScopeTimer.h"
#include <cstdio>

#ifdef _WIN32
#include <Windows.h>
ScopeTimer::ScopeTimer(const char* message) : _message(message)
{
    QueryPerformanceCounter(&_startTime);
}

ScopeTimer::~ScopeTimer()
{
    LARGE_INTEGER endTime;
    QueryPerformanceCounter(&endTime);
    printf("%s took %lld us\n", _message, endTime.QuadPart - _startTime.QuadPart);
}
#else
#error "Scope timer not implemented."
#endif